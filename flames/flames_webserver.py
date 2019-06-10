from flask import Flask
from flask import request
from flask import Response
from flask import abort
import json
import logging
import requests
import urllib

import flames_controller
import poofermapping
import pattern_manager
import triggers

PORT = 5000
HYDRAULICS_PORT = 9000
hydraulics_addr = "noetica-hydraulics.local"

#logging.basicConfig()
logger = logging.getLogger("flames")

app = Flask("flg", static_url_path="", static_folder="/home/flaming/Noetica/Flames/static")
#app = Flask("flg", static_url_path="")

hydraulics_port = HYDRAULICS_PORT

# XXX TODO - function to set the log level

def serve_forever(httpPort=PORT, hydraulicsAddr=hydraulics_addr, hydraulicsPort=HYDRAULICS_PORT ):
    logger.info("FLAMES WebServer: port {}, hydraulics addr {}, \
                                      hydraulics port {}".format(httpPort,hydraulicsAddr, hydraulicsPort))
    global hydraulics_port
    global hydraulics_addr
    hydraulics_port = hydraulicsPort
    hydraulics_addr = hydraulicsAddr
    app.run(host="0.0.0.0", port=httpPort, threaded=True) ## XXX - FIXME - got a broken pipe on the socket that terminated the application (uncaught exception) supposedly this is fixed in flask 0.12

# GET /flame. Get status of all poofers, any active patterns. (Poofer status is [on|off], [enabled|disabled].)
# POST /flame playState=[pause|play]. Whole sculpture gross control. Pause/Play: Pause all poofing and flame effects (should terminate any current patterns, prevent any poofing until Play is called]
@app.route("/flame", methods=['GET', 'POST'])
def flame_status():
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
        POST /flame/poofers/<poofername> enabled=[true|false]. Set enabled state for
        individual poofers.'''
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
        
@app.route("/flame/triggers", methods=['GET','POST'])
def flame_triggers():
    ''' GET /flame/triggers: Get list of all flame triggers, whether active or not
        POST /flame/triggers: Creates a new flame trigger from json patterndata'''
    if request.method == 'GET':
        return makeJsonResponse(json.dumps(get_triggers()))
    else:
        if not "triggerData" in request.values: 
            return Response("'triggerData' must be present", 400)
        set_trigger(urllib.unquote(request.values["triggerData"]))
        return Response("", 200)    
        
@app.route("/flame/triggers/<triggerId>", methods=['DELETE', 'POST'])
def flame_trigger(triggerId):    
    if request.method == 'DELETE':
        delete_trigger(triggerId)
        return Response("", 200)
    if request.method == 'POST':
        if not "newName" in request.values:
            return Response("'newName' must be present", 400)
        rename_trigger(triggerId, request.values["newName"])
        return Response("", 200)
            
@app.route("/flame/patterns", methods=['GET','POST'])
def flame_patterns():
    ''' GET /flame/patterns: Get list of all flame patterns, whether active or not
        POST /flame/patterns: Creates a new flame pattern from json patterndata'''
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
    ''' POST /flame/patterns/<patternName> active=[true|false] enabled=[true|false]. Start an
    individual pattern (or stop it if it is currently running). Enable/disable a pattern.
    Also, create or modify an existing pattern'''
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

@app.route("/hydraulics", methods=['GET', 'POST'])
@app.route("/hydraulics/playbacks", methods=['GET', 'POST'])
@app.route("/hydraulics/playbacks/<path:path>", methods=['GET', 'POST', 'DELETE'])
@app.route("/hydraulics/position", methods=['GET'])
def remote_hydraulics(path=None):
#    status, response = hydraulics_passthrough(request.script_root + request.path, request.method, request.values)
    return hydraulics_passthrough(request.script_root + request.path, request.method, request.values)
#    return Response(response, status)

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
    
def get_triggers():
    return triggers.getTriggers()
    
def rename_trigger(oldName, newName):
    triggers.renameTrigger(oldName, newName)
    triggers.saveTriggers()
    
def set_trigger(triggerData):
    print ("*** trigger data is {}".format(triggerData))
    triggers.addOrModifyTrigger(json.loads(triggerData))
    triggers.saveTriggers()
    
def delete_trigger(triggerName):
    triggers.deleteTrigger(triggerName)
    triggers.saveTriggers()

def poofer_id_valid(id):
    return id in poofermapping.mappings

def patternName_valid(patternName):
    return patternName in pattern_manager.getPatternNames()

def param_valid(value, validValues):
    return value != None and (value.lower() in validValues)

def hydraulics_passthrough(request, method, params):
    hydraulicsBaseURL = "http://" + hydraulics_addr + ":" + str(hydraulics_port)
    if method == "POST":
        r = requests.post(hydraulicsBaseURL + request, data=params)
    elif method == "GET":
        r = requests.get(hydraulicsBaseURL + request, data=params)
    elif method == "DELETE":
        r = requests.delete(hydraulicsBaseURL + request, data=params)
    else:
        return Response("Endpoing unknown", 404)
    
    print r.headers["content-type"]

    logger.debug("About to return response {} {}".format(r.text, r.status_code))
    
    return Response(r.text, r.status_code) 
    
#     print r
#     print r.text
#     print r.json
#     print r.content-type
# 
#     return (r.status_code, r.text) # XXX FIXME - check what r.json does on a non-json return.


if __name__ == "__main__":
    from threading import Thread
    import event_manager
    import Queue
    import flames_drv
    print "flame api!!"

    logging.basicConfig(format='%(asctime)-15s %(levelname)s %(module)s %(lineno)d: %(message)s', level=logging.DEBUG)

    pattern_manager.init("./pattern_test_2.json")
    event_manager.init()

    commandQueue = Queue.Queue()
    flames_drv.init(commandQueue)
    flames_controller.init(commandQueue)

    flaskThread = Thread(target=serve_forever, args=[5000, "localhost", 9000])
    flaskThread.start()
    serve_forever

    print "About to make request!"

    baseURL = 'http://localhost:' + str(PORT) + "/"

    print "Setting playstate to Pause"
    r = requests.post(baseURL + "flame", data={"playState":"pause"})
    print r.status_code

    r = requests.get(baseURL + "flame")
    print r.status_code
    print r.json()

    print "Setting playstate to Play"
    r = requests.post(baseURL + "flame", data={"playState":"play"})
    print r.status_code

    r = requests.get(baseURL + "flame")
    print r.status_code
    print r.json()

    print "Get poofers"
    r = requests.get(baseURL + "flame/poofers/NW")
    print r.status_code
    print r.json()

    print "Set poofer enabled/disabled"
    r = requests.post(baseURL + "flame/poofers/NW", data={"enabled":"false"})

    r = requests.get(baseURL + "flame/poofers/NW")
    print r.status_code
    print r.json()

    r = requests.post(baseURL + "flame/poofers/NW", data={"enabled":"true"})

    r = requests.get(baseURL + "flame/poofers/NW")
    print r.status_code
    print r.json()

    print "Set pattern enabled/disabled"
    r = requests.post(baseURL + "flame/patterns/Top", data={"enabled":"false"})

    r = requests.get(baseURL + "flame/patterns/Top")
    print r.status_code
    print r.json()

    r = requests.post(baseURL + "flame/patterns/Top", data={"enabled":"true"})

    r = requests.get(baseURL + "flame/patterns/Top")
    print r.status_code
    print r.json()

    print "Set pattern active"
    r = requests.post(baseURL + "flame/patterns/Top", data={"active":"true"})

    print "Set pattern inactive"
    r = requests.post(baseURL + "flame/patterns/Top", data={"active":"false"})

    print "Try hydraulics request"
    r = requests.get(baseURL + "hydraulics")
    print r.status_code
    print r.json()

    r = requests.get(baseURL + "hydraulics/playbacks")
    print r.status_code
    print r.json()

    r = requests.get(baseURL + "hydraulics/position")
    print r.status_code
    print r.json()

    r = requests.post(baseURL + "hydraulics", data={"state":"nomove"})
    print r.status_code

    flames_drv.shutdown()
    flames_controller.shutdown()
    websocket.shutdown()
    event_manager.shutdown()
    pattern_manager.shutdown()
