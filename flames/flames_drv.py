#!/usr/bin/python
##########  Overview ########################
#
# This module translates a firing sequence to low-level ! (bang) code and 
# sends it to the poofer controller boards.
# 
# The code runs on a single thread- the PooferControlThread.
# 
# Thisthread listens for new commands - run a sequence, stop a
# sequence, disable/enable a poofer, disable/enable all firing. When it gets
# such a command the thread translates it into a series of low-level
# wire-protocol ('! protocol') command messages, and places them into a queue
# ordered by the time that the command will take effect. When time time arrives
# for the command to be executed, the thread pops it off the queue
# and writes the command to via the serial port to the RS-485 wire 
#
# See poofer_sequence.py for information about the format of the poofer
# sequences themselves.
#
########## '!' (Bang) Protocol
#
# The bang protocol defines the form of the code used to command the poofer controller boards.
# Bang protocol defined here: http://flg.waywardengineer.com/index.php?title=Bang_(!)_Protocol
#
##########
'''
poofer sequence - let's define it as:
pooferid, on time, delaytime
[{"id":"NW", duration:1000, "startTime":1200} That's an event. So a sequence is
{"name":<name>, [{"id":"NW" etc... Let's use the tool to generate these.
That's an eve self.pooferMapping as easily human understandable, but it works
'''

import os
import sys                    # system functions
import traceback            # exceptions and tracebacks
import time                    # system time
import re                     # regex
from threading import Thread
import Queue
import json
import logging
import event_manager
import pattern_manager
from poofermapping import mappings as pooferMapping
from collections import defaultdict
import serial
from operator import itemgetter

logger = logging.getLogger("flames_drv")
POOFER_MAPPINGS_FILE = "./poofer_mappings.json"

### PARAMETERS - DO NOT CHANGE ###
#these parameters may need to be tweaked during early testing
minPooferCycleTime           = 50      # milliseconds, this is the poofer 
                                       # off-to-on-to-off cycle time, dictated 
                                       # by the switching speed of the DMI-SH-112L
                                       #  relay on the poofer controller board
maxFiringSequenceSteps       = 50      # some upper limit to firing sequence, for sanity checks
minFiringRestTime            = 100     # milliseconds, this is the minimum time
                                       # we want between two sequential poofer firing steps
maxNonfiringRestTime         = 9999    # milliseconds, dictates the maximum 
                                       # time for a firing sequence rest event
maxCommandsInAFiringSequence = 50      # integer, needs to be tested
BAUDRATE                     = 19200


### VARIABLES ###
pooferFiringThread = None

### CODE ###
def init(cmdQueue, homeDir):
    global pooferFiringThread
    pooferFiringThread = PooferFiringThread(cmdQueue, homeDir)
    pooferFiringThread.start()

def shutdown():
    global pooferFiringThread
    logger.info("Flame driver shutdown()")
    if pooferFiringThread != None:
        logger.info("...Joining flame driver thread")
        pooferFiringThread.shutdown()
        pooferFiringThread.join()
        pooferFiringThread = None


class PooferFiringThread(Thread): # comment out for unit testing
# class PooferFiringThread():
    TIMEOUT = 1 # 1 second timeout, even if no events

    # NB - there ought to be a global configuration manager rather than having to pass around
    # the home directory and handling configuration files per module, but I don't have time to do that
    def __init__(self, cmdQueue, homeDir):
        Thread.__init__(self) # comment out for unit testing
        logger.info("Init Poofer Firing Thread")
        self.cmdQueue = cmdQueue
        self.running = False
        self.isFiringDisabled = False
        self.pooferEvents = list() # time-ordered list of poofer events
#        self.disabled_poofers = set()
        self.disabled_poofers = [] # NB - set() is a better construct, but I need to load from a json file. Without internet access, the only way I know to quickly do that is to use a list
        self.initSerial()
        self.disabledFile = homeDir + "disabled_poofers.json"
        # with open(pooferMappingPath) as data_file:
        #     self.pooferMapping = json.load(data_file)
        self.disableAllPoofersCommand = self.generateDisableAllString()
        self.readDisabledPoofers()


    def shutdown(self):
        self.running = False

    def initSerial(self):
        self.ser = serial.Serial()
        self.ser.baudrate = BAUDRATE
        port = False
        for filename in os.listdir("/dev"):
            if filename.startswith("tty.usbserial"):  # this is the ftdi usb cable on the Mac
                port = "/dev/" + filename
                logger.info("Found usb serial at " + port)
                break;
            elif filename.startswith("ttyUSB0"):      # this is the ftdi usb cable on the Pi (Linux Debian)
                port = "/dev/" + filename
                logger.info("Found usb serial at " + port)
                break;

        if not port:
            logger.exception("No usb serial connected")
            return None

        self.ser.port = port
        self.ser.timeout = 0
        self.ser.stopbits = serial.STOPBITS_ONE
        self.ser.bytesize = 8
        self.ser.parity   = serial.PARITY_NONE
        self.ser.rtscts   = 0
        self.ser.open() # if serial open fails... XXX

    def generateDisableAllString(self):
        self.disableAllPoofersCommand = ""
        controllerDict = defaultdict(list)
        for attribute, value in pooferMapping.iteritems():
            controllerDict[value[:2]].append(value[2])  # value[:2] is the id of the flame driver board. value[2] is the id of the poofer on that board

        for i in controllerDict.keys(): # ie, for all flame driver boards
            self.disableAllPoofersCommand += "!" + i + "~".join(map(lambda x: x+"0", controllerDict[i])) + "."

    def run(self):
        self.running = True
        while(self.running):
            if len(self.pooferEvents) > 0: # there are poofer events
                # pop events off of the list. If the current time is greater than
                # the time associated with the event, set up for serial

                event = self.pooferEvents.pop(0)
                currentTime = time.time()
                firingTime = event["time"]
                if firingTime < currentTime:
                    if not currentTime - firingTime > 2: #If it's more than two seconds in the past, ignore it
                        self.firePoofers(event["bangCommandList"])
                else:
                    self.pooferEvents.insert(0, event)

            if len(self.pooferEvents) > 0: # there are poofer events in the future
                waitTime = self.pooferEvents[0]["time"] - time.time()

            else:
                waitTime = PooferFiringThread.TIMEOUT

            try:
                if waitTime < 0:
                    waitTime = 0  
                cmd = self.cmdQueue.get(True, waitTime)
                # parse message. If this is a request to do a flame sequence,
                # set up poofer events, ordered by time. Event["time"] attribute
                # should be current time (time.time()) plus the relative time from
                # the start of the sequence
                msgObj = json.loads(cmd)
                type = msgObj["cmdType"]
                if type == "stop":
                    self.stopAll()

                elif type == "resume":
                    self.resumeAll()

                elif type == "pooferDisable":
                    # TODO: does this need to persist?
                    self.disablePoofer(msgObj)

                elif not self.isFiringDisabled:
                    if type == "flameEffectStop":
                        self.stopFlameEffect(msgObj)

                    elif type == "pooferEnable":
                        self.enablePoofer(msgObj)

                    elif type == "flameEffectStart":
                        self.startFlameEffect(msgObj)
                        # else - whatever other type of event you want to process ...
                    elif type == "flameEffectStop":
                        self.stopFlameEffect(msgObj)

            except Queue.Empty:
                # this is just a timeout - completely expected. Run the loop
                pass
            except Exception:
                logger.exception("Unexpected exception processing command queue!")

    def checkSequence(self, firingSequence):
        try:
            events = firingSequence["events"]

            if len(events) > maxFiringSequenceSteps:
                raise Exception ("Error: maxFiringSequenceSteps < len(firingSequence) = ", len(firingSequence))

            totalDuration = 0
            for e in events:
                totalDuration += e["duration"]
            if totalDuration > 60:
                raise Exception ("Error: duration", len(firingSequence))

            return True

        except Exception as e:
            logger.exception("firingSequence is malformed or out of bounds" + str(e))
            return False

    ## send bangCommandList to the poofer controller boards
    def firePoofers(self, bangCommandList):
        try:
            if not self.running or self.isFiringDisabled:
                return 1

            if not self.ser:
                self.initSerial()

            for command in bangCommandList:
                self.ser.write(command.encode())

        except Exception as e:
            self.ser.close()
            self.ser = None
            logger.exception("Error sending bangCommandSequence to poofer controller boards: %s", str(e))

    def disablePoofer(self, msgObj):
        logger.info("Disabling poofer {}".format(msgObj["name"]))
        if not msgObj["name"] in self.disabled_poofers:
            self.disabled_poofers.append(msgObj["name"])
            self.writeDisabledPoofers()
            # XXX Rip this poofer out of the command  list
            # XXX self.pooferEvents
            event_manager.postEvent({"msgType":"poofer_disabled", "id":msgObj["name"]})

    def enablePoofer(self, msgObj):
        try:
            logger.info("Disabling poofer {}".format(msgObj["name"]))
            self.disabled_poofers.remove(msgObj["name"])
            self.writeDisabledPoofers()
            event_manager.postEvent({"msgType":"poofer_enabled", "id":msgObj["name"]})
        except KeyError as e:
            pass

    def readDisabledPoofers(self):
        try :
            with open(self.disabledFile, 'r') as f:
                self.disabled_poofers = json.load(f)

        except Exception:
            logger.exception("Trouble reading disabled poofers")

    def writeDisabledPoofers(self):
        try :
            with open(self.disabledFile, 'w') as f: # open write
                json.dump(self.disabled_poofers, f)
        except Exception:
            logger.exception("Trouble reading disabled poofers")

    def resumeAll(self):
        self.isFiringDisabled = False
        event_manager.postEvent({"msgType":"global_resume", "id":"all?"})

    def stopAll(self):
        try:
            if not self.ser:
                self.ser.initSerial()
            if disableAllPoofersCommand == "":
                self.generateDisableAllString()
            self.ser.write(disableAllPoofersCommand.encode())

            self.isFiringDisabled = True
            self.pooferEvents = list() # reset all pooferEvents
            event_manager.postEvent({"msgType":"global_pause", "id":"all?"})

        except Exception as e:
            logger.exception("Error stopping all poofers: %s", str(e))


    def startFlameEffect(self, msgObj):
        try:
            sequenceName = msgObj["name"]
            sequence = pattern_manager.getPattern(sequenceName)
            if self.checkSequence(sequence):
                self.setUpEvent(sequence)
                event_manager.postEvent({"msgType":"sequence_start", "id":msgObj["name"]})

        except Exception as e:
            logger.exception("Failed to fetch or set up sequence.%s", str(e))

    def stopFlameEffect(self, msgObj):
        event_manager.postEvent({"msgType":"sequence_stop", "id":msgObj["name"]})
        filter(lambda p: p["sequence"] != msgObj["name"], self.pooferEvents)

    def setUpEvent(self, sequence):
        # Takes a sequence object, and add to self.pooferEvents the bang commands
        # to turn on and to turn off the specified poofers.
        # The obect added to self.pooferEvents is of format:
        # # { "sequence":"sequenceName", "time":"1502068215.5",
        # "bangCommandList":["!0011~21.", "!0021~21."] }

        sequenceName = sequence["name"]

        events = sequence["events"]
        firstFiringTime = time.time()

        if not self.isFiringDisabled:
            for event in events:
                ids = event["ids"]
                startTime = firstFiringTime + event["startTime"]
                endTime = startTime + event["duration"]

                # CSW DO NOT ADD DISABLED POOFERS!!!
                for poofer in self.disabled_poofers:
                    if poofer in ids:
                        ids.remove(poofer)

                addresses = [pooferMapping[a] for a in ids]
                bangCommandList = self.makeBangCommandList(addresses)

                pooferEvent = {}
                pooferEvent["sequence"] = sequenceName
                pooferEvent["time"] = startTime
                pooferEvent["bangCommandList"] = bangCommandList["on"]

                endPooferEvent = {}
                endPooferEvent["sequence"] = sequenceName
                endPooferEvent["time"] = endTime
                endPooferEvent["bangCommandList"] = bangCommandList["off"]

                # TODO: need to figure out best way to sort this thing
                self.pooferEvents.append(pooferEvent)
                self.pooferEvents.append(endPooferEvent)
                self.pooferEvents.sort(key=itemgetter("time"))

    def makeBangCommandList(self, addresses):
        # creates a dictionary with the key being a controller ID (two digits),
        # and values being all the channels for a given controller.
        # returns an object with bang commands to turn poofers both on and off

        onBangCommands = []
        offBangCommands = []

        try:
            controllerDict = defaultdict(list)
            for controllerId in addresses:
                controllerDict[controllerId[:2]].append(controllerId[2])

            for i in controllerDict.keys():
                onBangCommands.append(
                    "!" + i + "~".join(map(lambda x: x+"1", controllerDict[i])) + ".")
                offBangCommands.append(
                    "!" + i + "~".join(map(lambda x: x+"0", controllerDict[i])) + ".")

        except Exception as e:
            logger.exception("Error generating bang code: %s", str(e))
            raise Exception(str(e))

        return {"on":onBangCommands, "off":offBangCommands}


'''
## proof of concept: fire a single poofer ##

def singlePooferProofOfConcept():

#debugging
boardID="01"
boardChannel="1"
print boardID
print boardChannel

#command components: write, boardID, boardChannel, on
bangString="!"+ boardID + boardChannel + "1"
# command terminator
bangString=poofString + "."

print bangString
'''
