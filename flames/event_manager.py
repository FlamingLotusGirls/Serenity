import json
import logging
from threading import Thread
import Queue

logger = logging.getLogger("flames")

'''
 {"event":{"msgType": ["poofer_on"|
                     "poofer_off"|
                     "sequence_start"|
                     "sequence_stop"|
                     "poofer_enabled"|
                     "poofer_disabled"|
                     "sequence_enabled"|
                     "sequence_disabled"|
                     "trigger_enabled" |
                     "trigger_disabled"
                     "global_pause"
                     "global_resume"
                     "hydraulics_mode_change"
                     "playback_start"
                     "playback_stop"], "id": [pooferId | patternName | triggerName | playbackName]}} '''
                     
''' Listens on the event queue, and publishes events to listeners'''
  
eventThread = None
eventQueue  = None
eventHandlers = list()

def init():
    global eventThread
    global eventQueue
    logger.info("Event Manager Init")
    if eventThread == None:
        eventQueue = Queue.Queue()
        eventThread = EventManagerThread(eventQueue)
        eventThread.start()
    
def shutdown():
    global eventThread
    global eventHandlers
    logger.info("Event Manager Shutdown")
    if eventThread != None:
        logger.info("...Joining Event Manager thread")
        eventThread.shutdown()
        eventThread.join()
        eventThread = None
    eventHandlers = list()
    
def postEvent(event):
    eventQueue.put(event)
    
def addListener(eventHandler, msgType=None):
    if not callable(eventHandler):
        logger.warning("Add listener called with invalid eventHandler")
        return
    eventHandlers.append({"handler": eventHandler, "msgType":msgType})
    
def removeListener(eventHandler):
    for handler in eventHandlers:
        if handler["handler"] == eventHandler:
            eventHandlers.remove(handler)

class EventManagerThread(Thread):
    def __init__(self, eventQueue):
        Thread.__init__(self)
        self.eventQueue = eventQueue
        
    def run(self):
        self.running = True
        while (self.running):
            try:
                msg = self.eventQueue.get(True, 1) # 1 second timeout
                logger.debug("received event {}".format(msg))
                msgType = msg["msgType"]
                for eventHandler in eventHandlers:
                    if (eventHandler["msgType"] == None or 
                        eventHandler["msgType"] == msgType or 
                        msgType in eventHandler["msgType"]):
                        eventHandler["handler"](msg)
            except Queue.Empty:
                # just timeout, completely expected
                pass
            except Exception:
                logger.exception("Unexpected exception in event manager thread")
                        
    def shutdown(self):
        self.running = False
