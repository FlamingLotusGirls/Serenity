from flask import Flask
from flask import request
from flask import Response
from flask import abort
import json
import logging
import requests
import time
import urllib

import flames_controller
import poofermapping
import pattern_manager

''' 
    Webserver for the flame effect controller. In this variant, we're mostly
    interested in sequences - we have CRUD endpoints for sequences, as well
    as commands to run (or stop) a particular sequence. Other endpoints will
    globally disable (or re-enable) particular poofers; note, however that
    the buttons on the sculpture bypass any software control, so if you 
    really want to turn a poofer off, you're going to have to go put pink
    tape on the button
'''

PORT = 5000

logger = logging.getLogger("flames")

app = Flask("flg", static_url_path="", static_folder="/home/flaming/Serenity/Flames/static")
#app = Flask("flg", static_url_path="")


# XXX TODO - function to set the log level

def serve_forever(httpPort=PORT):
    logger.info("FLAMES WebServer: port {}".format(httpPort))
    app.run(host="0.0.0.0", port=httpPort, threaded=True) ## XXX - FIXME - got a broken pipe on the socket that terminated the application (uncaught exception) supposedly this is fixed in flask 0.12

@app.route("/flame", methods=['GET', 'POST'])
def flame_status():
    ''' GET /flame. Get status of all poofers, any active patterns. (Poofer status 
          is [on|off], [enabled|disabled].)
        POST /flame playState=[pause|play]. Whole sculpture gross control. 
          Pause/Play: Pause all poofing and flame effects (should terminate any 
          current patterns, prevent any poofing until Play is called]
    '''
    if request.method == 'POST':
        if "playState" in request.values:
            playState = request.values["playState"].lower()
            if playState == "pause":
                flames_controller.globalPause()
            elif playState == "play":
                flames_controller.globalRelease()
            else:
                return Response("Invalid 'playState' value", 400)
        else:
            return Response("Must have 'playState' value", 400)

        return Response("", 200)

    else:
        return makeJsonResponse(json.dumps(get_status()))

def makeJsonResponse(jsonString, respStatus=200):
    resp = Response(jsonString, status=respStatus, mimetype='application/json')
    resp.headers['Access-Control-Allow-Origin'] = '*'
    return resp


@app.route("/flame/poofers/<poofer_id>", methods=['GET', 'POST'])
def specific_flame_status(poofer_id):
    ''' GET /flame/poofers/<poofername>. Get status of particular poofer.
        POST /flame/poofers/<poofername> enabled=[true|false]. Set enabled
         state for individual poofers.
    '''
    if not poofer_id_valid(poofer_id):
        abort(400)
    if request.method == 'POST':
        if not "enabled" in request.values:
            return Response("'enabled' must be present", 400)

        enabled = request.values["enabled"].lower()
        if enabled == 'true':
            flames_controller.enablePoofer(poofer_id)
        elif enabled == 'false':
            flames_controller.disablePoofer(poofer_id)
        else:
            return Response("Invalid 'enabled' value", 400)

        return "" # XXX check for errors as a matter of course
    else:
        return makeJsonResponse(json.dumps(get_poofer_status(poofer_id)))
        
@app.route("/flame/patterns", methods=['GET','POST'])
def flame_patterns():
    ''' GET /flame/patterns: Get list of all flame patterns, whether active
         or not
        POST /flame/patterns: Creates a new flame pattern from json patterndata
    '''
    if request.method == 'GET':
        return makeJsonResponse(json.dumps(get_flame_patterns()))
    else:
        if not "patternData" in request.values:
            return Response("'patternData' must be present", 400)
        else:
            set_flame_pattern(request.values["patternData"])
            return Response("", 200)


@app.route("/flame/patterns/<patternName>", methods=['GET', 'POST', 'DELETE'])
def flame_pattern(patternName):
    ''' POST /flame/patterns/<patternName> active=[true|false] enabled=[true|false] pattern=[pattern]
          active - Start an individual pattern (or stop it if it is currently running).
          enabled - enable/disable a pattern.
          pattern - pattern data, modify existing pattern
    '''
    includesPattern = "pattern" in request.values
    includesEnabled = "enabled" in request.values
    includesActive  = "active"  in request.values

    if request.method == 'POST':
        # pattern create - pattern data included, but pattern name not in system
        if  (not includesPattern) and (not patternName_valid(patternName)):
            return Response("Must have valid 'patternName'", 400)

        if includesPattern:
            patternData = json.loads(request.values["pattern"])
            oldPatternData = None
            for pattern in patternList:
                if pattern["name"] == patternData["name"]:
                    oldPatternData = pattern
                    break;
            if oldPatternData == None:
                patternList.append(patternData)
            else:
                oldPatternData["events"] = patternData["events"]
            savePatternData()

        if includesEnabled:
            enabled = request.values["enabled"].lower()
            enabledValid = param_valid(enabled, ["true", "false"])
        else:
            enabledValid = False
        if includesActive:
            active = request.values["active"].lower()
            activeValid = param_valid(active, ["true", "false"])
        else:
            activeValid = False

        if (not enabledValid and not activeValid):
            abort(400)

        if enabledValid:
            if (enabled == "true"):
                flames_controller.enableFlameEffect(patternName)
            elif (enabled == "false"):
                flames_controller.disableFlameEffect(patternName)
        if activeValid:
            if (active == "true"):
                flames_controller.doFlameEffect(patternName)
            elif (active == "false"):
                flames_controller.stopFlameEffect(patternName)


        return ""
    elif request.method == "DELETE":
        pattern_manager.deletePattern(patternName)
        pattern_manager.savePatterns()
        return ""

    else:
        if (not patternName_valid(patternName)):
            return Response("Must have valid 'patternName'", 400)
        else:
            return makeJsonResponse(json.dumps(get_pattern_status(patternName)))


def get_status():
    pooferList = list()
    patternList = list()
    for pooferId in poofermapping.mappings:
        pooferList.append({"id" : pooferId,
                           "enabled": flames_controller.isPooferEnabled(pooferId),
                           "active" : flames_controller.isPooferActive(pooferId)})
    for patternName in pattern_manager.getPatternNames():
        patternList.append({"name" : patternName,
                            "enabled": flames_controller.isFlameEffectEnabled(patternName),
                            "active" : flames_controller.isFlameEffectActive(patternName)})
    return {"globalState": (not flames_controller.isStopped()),
            "poofers":pooferList,
            "patterns":patternList }



def get_poofer_status(poofer_id):
    # there's enabled, and there's active (whether it's currently on)
    pooferStatus = {"enabled": flames_controller.isPooferEnabled(poofer_id),
                    "active" : flames_controller.isPooferActive(poofer_id)}
    return pooferStatus

def get_pattern_status(patternName):
    patternStatus = {"enabled": flames_controller.isFlameEffectEnabled(patternName),
                     "active" : flames_controller.isFlameEffectActive(patternName)}
    return patternStatus

def get_flame_patterns():
    return pattern_manager.getAllPatterns()

# abort 500 in general? how are errors expected to be propagated in this framework?s
def set_flame_pattern(pattern):
    pattern_manager.addOrModifyPattern(json.loads(pattern))
    pattern_manager.savePatterns()
    
def poofer_id_valid(id):
    return id in poofermapping.mappings

def patternName_valid(patternName):
    return patternName in pattern_manager.getPatternNames()

def param_valid(value, validValues):
    return value != None and (value.lower() in validValues)

def shutdown():
    flames_drv.shutdown()
    flames_controller.shutdown()
    event_manager.shutdown()
    pattern_manager.shutdown()

production = True

if __name__ == "__main__":
    from threading import Thread
    import event_manager
    import queue
    import flames_drv
    print("flame api!!")

    logging.basicConfig(format='%(asctime)-15s %(levelname)s %(module)s %(lineno)d: %(message)s', level=logging.DEBUG)

    pattern_manager.init("./pattern_test_2.json")
    event_manager.init()

    commandQueue = queue.Queue()
    flames_drv.init(commandQueue, ".") # XXX FIXME. Homedir may not be "." Take from args?:
    flames_controller.init(commandQueue)

    if production:
        try:
            serve_forever()
        except Exception:
            shutdown()
    else:
        flaskThread = Thread(target=serve_forever) #, args=[5000, "localhost", 9000])
        flaskThread.start()

        time.sleep(5)
        print("About to make request!")

        baseURL = 'http://localhost:' + str(PORT) + "/"

        print("Setting playstate to Pause")
        r = requests.post(baseURL + "flame", data={"playState":"pause"})
        print(r.status_code)

        r = requests.get(baseURL + "flame")
        print(r.status_code)
        print(r.json())

        print("Setting playstate to Play")
        r = requests.post(baseURL + "flame", data={"playState":"play"})
        print(r.status_code)

        r = requests.get(baseURL + "flame")
        print(r.status_code)
        print(r.json())

        print("Get poofers")
        r = requests.get(baseURL + "flame/poofers/1_T1")
        print(r.status_code)
        print(r.json())

        print("Set poofer enabled/disabled")
        r = requests.post(baseURL + "flame/poofers/1_T1", data={"enabled":"false"})

        r = requests.get(baseURL + "flame/poofers/1_T1")
        print(r.status_code)
        print(r.json())

        r = requests.post(baseURL + "flame/poofers/1_T1", data={"enabled":"true"})

        r = requests.get(baseURL + "flame/poofers/1_T1")
        print(r.status_code)
        print(r.json())

        print("Set pattern enabled/disabled")
        r = requests.post(baseURL + "flame/patterns/Firefly_3_chase", data={"enabled":"false"})

        r = requests.get(baseURL + "flame/patterns/Firefly_3_chase")
        print(r.status_code)
        print(r.json())

        r = requests.post(baseURL + "flame/patterns/Firefly_3_chase", data={"enabled":"true"})

        r = requests.get(baseURL + "flame/patterns/Firefly_3_chase")
        print(r.status_code)
        print(r.json())

        print("Set pattern active")
        r = requests.post(baseURL + "flame/patterns/Firefly_3_chase", data={"active":"true"})

        print("Set pattern inactive")
        r = requests.post(baseURL + "flame/patterns/Firefly_3_chase", data={"active":"false"})

        shutdown()
