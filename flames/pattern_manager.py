import copy
import json
import logging
from operator import itemgetter
import os
import sys
from threading import Lock
from shutil import copyfile
import unittest

import poofermapping

''' Handles creation-deletion-modification-retrieval (and permanent save) of 
    flame effect sequences, aka patterns. A pattern is defined to have the
    following format:
      {
       "name":<patternName>,
       "modifyable":[True|False],
       "events":[eventList]
      }
    where each event has the format:
      {
       "duration":<duration, int milliseconds>, 
       "startTime":<startTime, in ms from start of sequence>
       "ids:[poofer_ids]
      }
    
    poofer_ids are strings that name the unique poofers. These strings can be
    found in poofermapping.py
'''

gPatterns = list()
patternLock = Lock()
patternFileName = None

logger = logging.getLogger('flames')

def init(flameEffectsFile="./sequences.json"):
    global patternFileName

    logger.info("Pattern Manager Init, sequence file {}".format(flameEffectsFile))
    try:
        _loadPatternFile(flameEffectsFile)
    except Exception:
        logger.exception("Unexpected error initializing pattern manager")

def _loadPatternFile(flameEffectsFile):
    global patternFileName
    global gPatterns
    gPatterns = list()
    patternFileName = flameEffectsFile
    patternNames = list()
    try:
        with open(flameEffectsFile) as f:
            savedPatterns = json.load(f)
            for pattern in savedPatterns:
                if not (pattern['name'] in patternNames):
                    if not _validatePattern(pattern):
                        logger.warn("Pattern {} does not validate, rejecting".format(pattern['name']))
                        continue
                    patternNames.append(pattern['name'])
                    gPatterns.append(pattern)
                else:
                    logger.warn("Pattern name {} used twice".format(pattern['name']))
    except ValueError:
        logger.exception("Bad JSON in pattern file")

def shutdown():
    logger.info("Pattern Manager shut down")

def _validatePattern(pattern):
    if not "name" in pattern:
        logger.warn("Pattern has no name")
        return False

    if not "events" in pattern:
        logger.warn("Pattern {} has no events".format(pattern["name"]))
        return False

    if not "modifiable" in pattern:
        logger.warn("Pattern modifiable flag not set, defaulting to True")
        pattern["modifiable"] = True

    for event in pattern["events"]:
        if not "ids" in event:
            logger.warn("Pattern {} has no ids".format(pattern["name"]))
            return False
        for id in event["ids"]:
            if not id in poofermapping.mappings.keys():
                logger.warn("Pattern {} contains invalid id {}".format(pattern["name"], id))
                return False
        if not "duration" in event:
            logger.warn("Pattern {} has no duration".format(pattern["name"]))
            return False
        if not "startTime" in event:
            logger.warn("Pattern {} has no startTime".format(pattern["name"]))
            return False
    return True

def getPattern(patternName):
    returnPattern = None
    patternLock.acquire()
    for pattern in gPatterns:
        if pattern["name"] == patternName:
            returnPattern = pattern
    patternLock.release()
    return returnPattern

def getAllPatterns():
    patternLock.acquire()
    returnPattern = gPatterns
    patternLock.release()
    return returnPattern

def getPatternNames():
    patternNames = list()
    patternLock.acquire()
    for pattern in gPatterns:
        patternNames.append(pattern['name'])
    patternLock.release()
    return patternNames

def addOrModifyPattern(newPattern):
    patternName = newPattern['name']
    bFoundPattern = False
    for pattern in gPatterns:
        if pattern['name'] == patternName:
            bFoundPattern = True
            break
    if bFoundPattern:
        modifyPattern(newPattern)
    else:
        addPattern(newPattern)


def addPattern(newPattern):
    if not _validatePattern(newPattern):
        log.warn("Pattern {} does not validate, will not add".format(pattern['name']))
        return

    patternLock.acquire()
    patternExists = False
    for pattern in gPatterns:
        if pattern["name"] == newPattern["name"]:
            patternExists = True
            break
    if patternExists:
        logger.warning("Cannot add pattern {}, pattern already exists".format(newPattern["name"]))
    else:
        gPatterns.append(newPattern)
    patternLock.release()

def modifyPattern(newPattern):
    if not _validatePattern(newPattern):
        log.warn("Pattern {} does not validate, will not modify".format(pattern['name']))
        return

    patternLock.acquire()
    patternName = newPattern['name']
    foundPattern = None
    for pattern in gPatterns:
        if pattern['name'] == patternName:
            foundPattern = pattern
            break

    if not foundPattern:
        logger.warning("Could not find existing pattern {}, will not modify".format(patternName))
    elif foundPattern["modifiable"] != True:
        logger.warning("Pattern {} is not modifiable, will not modify".format(patternName))
    else:
        foundPattern["events"] = newPattern["events"]
        
    patternLock.release()

def deletePattern(patternName):
    foundPattern = None
    for pattern in gPatterns:
        if pattern['name'] == patternName:
            foundPattern = pattern
            break
    if not foundPattern:
        logger.warning("Could not find pattern {}, will not delete".format(patternName))
    else:
        gPatterns.remove(pattern)


def savePatterns(filename=None):
    if filename == None:
        filename = patternFileName
    with open(filename, 'w') as f: # open write
        json.dump(gPatterns, f)

def patternsEqual(pat1, pat2):
    if pat1["name"] != pat2["name"]:
        return False
    if pat1["modifiable"] != pat2["modifiable"]:
        return False

    if len(pat1["events"]) != len(pat2["events"]):
        return False

    pat1["events"].sort(key=itemgetter("startTime"))
    pat2["events"].sort(key=itemgetter("startTime"))
    for i in range(len(pat1["events"])):
        event1 = pat1["events"][i]
        event2 = pat2["events"][i]
        if (event1["startTime"] != event2["startTime"]):
            return False
        if (event1["duration"] != event2["duration"]):
            return False
        for id in event1["ids"]:
            if not id in event2["ids"]:
                return False

        if len(event1["ids"]) != len(event2["ids"]):
            return False

    return True

class PatternTests(unittest.TestCase):
    def setUp(self):
        logging.basicConfig(format='%(asctime)-15s %(levelname)s %(module)s %(lineno)d: %(message)s', level=logging.DEBUG)
        self.pattern_file = "pattern_test.json"
        self.temp_file = "pattern_test_tmp.json"
        _loadPatternFile(self.pattern_file)

    def test_getPattern(self):
        pattern = getPattern("Top")
        self.assertTrue(pattern != None)

    def test_modifyPattern(self):
        pattern = getPattern("Bottom")

        modifiedPattern = copy.deepcopy(pattern)
        savedPattern    = copy.deepcopy(pattern)
        modifiedPattern["events"][0]["duration"] = 4000
        modifyPattern(modifiedPattern)


        modifiedPattern = getPattern("Bottom")
        self.assertEquals(modifiedPattern["name"], savedPattern["name"])
        self.assertEquals(len(modifiedPattern["events"]), len(savedPattern["events"]))
        self.assertEquals(modifiedPattern["events"][0]["duration"], 4000)
        self.assertEquals(modifiedPattern["events"][0]["startTime"], savedPattern["events"][0]["startTime"])

    def test_addPattern(self):
        pattern = getPattern("Custom")
        self.assertEquals(pattern, None)
        pattern = {"modifiable": True, "name": "Custom",
                    "events": [{"duration": 2000, "ids": ["NE", "NW"], "startTime": 0},
                               {"duration": 2000, "ids": ["NE", "NW"], "startTime": 4000}]}
        addPattern(pattern)
        addedPattern = getPattern("Custom")

        self.assertTrue(patternsEqual(pattern, addedPattern))

    def test_deletePattern(self):
        pattern = getPattern("Top")
        self.assertTrue(pattern != None)
        deletePattern("Top")
        pattern = getPattern("Top")
        self.assertTrue(pattern == None)

    def test_savePattern(self):
        copyfile(self.pattern_file, self.temp_file)
        _loadPatternFile(self.temp_file)
        newPattern = {"modifiable": True, "name": "New Pattern",
                "events": [{"duration": 2000, "ids": ["NE", "NW"], "startTime": 0},
                           {"duration": 2000, "ids": ["NE", "NW"], "startTime": 4000}]}
        addPattern(newPattern)
        savePatterns()
        _loadPatternFile(self.temp_file)
        restoredPattern = getPattern("New Pattern")
        self.assertFalse(restoredPattern == None)
        self.assertTrue(patternsEqual(restoredPattern, newPattern))
        os.remove(self.temp_file)


    def tearDown(self):
        pass

if __name__ == '__main__':
    unittest.main()
