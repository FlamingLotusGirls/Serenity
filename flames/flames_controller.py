'''Flame controller. Responsible for high-level management of flame effects. All objects
or modules wanting to know the status of poofers or sequences should call into this module.
Similarly, all objects or modules wanting to change the status of poofers or sequences -
including running a sequence - should call into this module.

Mediates with the low-level flames_drv via a message Queue (flameQueue, for pushing
commands to the low level code) and event listener (for receiving events created by the
flames driver)

Note that at the moment, many event types are not getting created. A more solid architecture
would use the event queue to set state rather than setting state off of the command. But
that seems like a nicety that I can ignore for now.'''

import Queue
import json
import logging
from threading import Thread
from threading import Lock
from websocket_server import WebsocketServer
import mock_event_producer as mockDriver
import event_manager
import pattern_manager

#logging.basicConfig()
logger = logging.getLogger("flames")

cmdQueue   = None       # requests from upper level
disabledPoofers = list()
activePoofers = list()
globalEnable = True
disabledFlameEffects = list()
activeFlameEffects = list()
gUseDriver = False


def init(flameQueue, useDriver=True):
    global cmdQueue
    global gUseDriver
    logger.info("Flame Controller Init")
    cmdQueue = flameQueue
    gUseDriver = useDriver

    event_manager.addListener(eventHandler)


def shutdown():
    logger.info("Flame Controller Shutdown")


def doFlameEffect(flameEffectName):
    if not flameEffectName in disabledFlameEffects:
        flameEffectMsg = {"cmdType":"flameEffectStart", "name":flameEffectName}
        cmdQueue.put(json.dumps(flameEffectMsg))

def stopFlameEffect(flameEffectName):
    flameEffectMsg = {"cmdType":"flameEffectStop", "name":flameEffectName}
    cmdQueue.put(json.dumps(flameEffectMsg))

def disableFlameEffect(flameEffectName):
    if not flameEffectName in disabledFlameEffects:
        disabledFlameEffects.append(flameEffectName)
    stopFlameEffect(flameEffectName)

def enableFlameEffect(flameEffectName):
    if flameEffectName in disabledFlameEffects:
        disabledFlameEffects.remove(flameEffectName)

def isFlameEffectActive(flameEffectName):
    return flameEffectName in activeFlameEffects

def isFlameEffectEnabled(flameEffectName):
    return not flameEffectName in disabledFlameEffects

def disablePoofer(pooferId):
    if not pooferId in disabledPoofers:
        disabledPoofers.append(pooferId)
        if gUseDriver:
            flameEffectMsg = {"cmdType":"pooferDisable", "name":pooferId}
            cmdQueue.put(json.dumps(flameEffectMsg))
        else:
            mockDriver.disablePoofer(pooferId)

def enablePoofer(pooferId):
    if pooferId in disabledPoofers:
        disabledPoofers.remove(pooferId)
        if gUseDriver:
            flameEffectMsg = {"cmdType":"pooferEnable", "name":pooferId}
            cmdQueue.put(json.dumps(flameEffectMsg))
        else:
            mockDriver.enablePoofer(pooferId)

def isPooferEnabled(pooferId):
    return not (pooferId in disabledPoofers)

def isPooferActive(pooferId):
    return pooferId in activePoofers

def globalPause():
    global globalEnable
    flameEffectMsg = {"cmdType":"stop"}
    globalEnable = False
    cmdQueue.put(json.dumps(flameEffectMsg))

def globalRelease():
    global globalEnable
    globalEnable = True
    flameEffectMsg = {"cmdType":"resume"}
    cmdQueue.put(json.dumps(flameEffectMsg))

def isStopped():
    return not globalEnable

def getDisabledPoofers():
    return disabledPoofers

def getDisabledFlameEffects():
    return disabledFlameEffects

def eventHandler(msg):
    msgType = msg["msgType"]
    id = msg["id"]
    if (msgType == "poofer_on"):
        if not id in activePoofers:
            activePoofers.append(id)
    elif (msgType == "poofer_off"):
        try:
            activePoofers.remove(id)
        except:
            pass

if __name__ == "__main__":
    import mock_event_producer
    import time
    import Queue

    logging.basicConfig(format='%(asctime)-15s %(levelname)s %(module)s %(lineno)d:  %(message)s', level=logging.DEBUG)

    try:

        event_manager.init()
        mock_event_producer.init()
        init(Queue.Queue())

        while(True):
            time.sleep(10)

    except Exception as e:
        print "Exception occurs!", e
    except KeyboardInterrupt:
        print "Keyboard Interrupt!"

    event_manager.shutdown()
    mock_event_producer.shutdown()
    shutdown()
