''' mock flame event producer '''

import event_manager
import logging
from operator import itemgetter
import random
from threading import Thread
import time

import poofermapping

gFiringThread = None

logger = logging.getLogger("flames")

def init():
    fireRandomPoofers()
    
def shutdown():
    stopFiringRandomPoofers()


def turnOnPoofer(pooferName):
    event_manager.postEvent({"msgType":"poofer_on", "id":pooferName})

def turnOffPoofer(pooferName):
    event_manager.postEvent({"msgType":"poofer_off", "id":pooferName})
    
def enablePoofer(pooferName):
    event_manager.postEvent({"msgType":"poofer_enabled", "id":pooferName})

def disablePoofer(pooferName):
    event_manager.postEvent({"msgType":"poofer_disabled", "id":pooferName})
    
def sequenceStart(sequenceName):
    event_manager.postEvent({"msgType":"sequence_start", "id":sequenceName})

def sequenceStop(sequenceName):
    event_manager.postEvent({"msgType":"sequence_stop", "id":sequenceName})

def sequenceEnabled(sequenceName):
    event_manager.postEvent({"msgType":"sequence_enabled", "id":sequenceName})

def sequenceDisabled(sequenceName):
    event_manager.postEvent({"msgType":"sequence_disabled", "id":sequenceName})
    
def fireRandomPoofers():
    global gFiringThread
    gFiringThread = RandomPooferFiringThread()
    gFiringThread.start()
    
def stopFiringRandomPoofers():
    global gFiringThread
    if gFiringThread != None:
        gFiringThread.stop()
        gFiringThread.join()
    gFiringThread = None    
    
class RandomPooferFiringThread(Thread):

    def __init__(self):
        Thread.__init__(self)
        self.availablePoofers = poofermapping.mappings.keys()
        self.poofersActive = list()
        
    def run(self):
        self.isRunning = True
        nextFiringTime = 0
        sleepTime = 0
        while (self.isRunning):
            time.sleep(sleepTime)
            curTime = time.time()
    
            # Set up some new poofers to fire
            nPoofers = random.randrange(0, min(4, len(self.availablePoofers)))
            firingPoofers = list()
            if (curTime > nextFiringTime): 
                while (len(firingPoofers) < nPoofers):
                    pooferIdx = random.randrange(0, nPoofers - len(firingPoofers))
                    poofer = {"id":self.availablePoofers[pooferIdx], "timeStop": curTime + (float(100*random.randrange(3, 30))/1000)}
                    turnOnPoofer(poofer["id"])
                    firingPoofers.append(poofer)
                    del self.availablePoofers[pooferIdx]
                    
                logger.info("Turning on poofers: {}".format(firingPoofers))
            
                self.poofersActive += firingPoofers
                logger.debug("Poofers active are: {}".format(self.poofersActive))
                self.poofersActive.sort(key=itemgetter("timeStop"))
                nextFiringTime = curTime + random.uniform(1.0, 5.0)
            
            # See if any have stopped firing
            lastExpiredPooferIdx = -1
            for i in range(0, len(self.poofersActive)):
                poofer = self.poofersActive[i]
                bDeleted = False
                if poofer["timeStop"] < curTime:
                    logger.info("Turning off poofer : {}".format(poofer["id"]))
                    turnOffPoofer(poofer["id"])
                    self.availablePoofers.append(poofer["id"])
                    lastExpiredPooferIdx = i
                
            if lastExpiredPooferIdx >= 0:
                if lastExpiredPooferIdx + 1 == len(self.poofersActive):
                    self.poofersActive = list()
                else:
                    self.poofersActive = self.poofersActive[lastExpiredPooferIdx+1:]
                print "New poofers active are", self.poofersActive
                    

            # calculate sleep time
            if len(self.poofersActive) > 0:
                offTime = self.poofersActive[0]["timeStop"] - curTime
            else:
                offTime = 5
                
            sleepTime = min(offTime, nextFiringTime - curTime)
            sleepTime = max(0.1, sleepTime)
            print "sleeptime is", sleepTime
            
    
    def stop(self):
        self.isRunning = False

if __name__ == "__main__":

    logging.basicConfig(format='%(asctime)-15s %(levelname)s %(module)s %(lineno)d:  %(message)s', level=logging.DEBUG)

    try: 
        event_manager.init()
        init()
        time.sleep(10)
        print "Stop now!"
        stopFiringRandomPoofers()
        event_manager.shutdown()
    except Exception as e:
        print "Exception occurs!", e
        shutdown()
        event_manager.shutdown()
    except KeyboardInterrupt:
        print "Keyboard Interrupt!"
        shutdown()
        event_manager.shutdown()
    
        
