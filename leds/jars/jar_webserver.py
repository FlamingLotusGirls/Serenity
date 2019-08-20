from builtins import str
from flask import Flask
from flask import request
from flask import Response
import json
import logging
import queue
from threading import Thread
import time

from flask_utils import CORSResponse, JSONResponse
''' 
    REST service for controlling firefly jar leds
'''

PORT = 8000

IMAGE_DIR="/images"

logger = logging.getLogger("firefly_jar_leds")
# XXX TODO - Use the logger or get rid of it

app = Flask("flg", static_url_path=IMAGE_DIR, static_folder="images")

def init(cmd_queue, resp_queue, jarId):
    global command_queue
    global response_queue
    global jar_id
    global command_id

    command_queue = cmd_queue
    response_queue = resp_queue
    jar_id = jarId
    command_id = 0

def serve_forever(httpPort=PORT):
    print("Fireflies Jar LED WebServer: port {}".format(httpPort))
    app.run(host="0.0.0.0", port=httpPort, threaded=False)
    print("Serve forever returns...")

def my_rpc(function_name, params):
    ''' Generic RPC for communicating with the led driver 
    '''

    global command_id
    command = {'cmd':function_name, 'params':params, 'id':command_id}
    command_queue.put(command)
    timeout = time.time() + 0.1
    response = None
    while(time.time() < timeout and response is None):
        try:
            resp = response_queue.get(timeout=timeout - time.time())
            if (resp is not None \
                and 'id' in resp \
                and resp['id'] == command_id):
                response = resp
            else:
                print(f"Warning - received unexpected response {resp}")
        except queue.Empty:
            pass

    command_id += 1
    
    if response is None:
        print(f"Error - timeout - rpc call to function {function_name}")
        raise ValueError("RPC call returns None")
   
    if 'error' in response['response']:
        raise ValueError(response['response']['error'])

    return response['response']

@app.route("/jar_leds", methods=['GET', 'POST'])
def jar_status():
    ''' GET  Get current foreground, background patterns and intensity 
    '''
    if request.method == 'GET':
        try: 
            values = my_rpc("get_current_status", None)
            if values is None:
                raise ValueError("No response")
            if "foreground" not in values \
                or "background" not in values \
                or "intensity" not in values:
                raise ValueError("Bad response")
            response = {
                "foreground":values['foreground'], 
                "background":values['background'], 
                "intensity":values['intensity']
            }
            return JSONResponse(json.dumps(response))
        except Exception as e:
            print(f"Warning Exception {e} in GET")
            return CORSResponse("Internal Error", 500)
    elif request.method == 'POST':
        try:
            params = {}
            params['foreground'] = request.values['foreground'] if 'foreground' in request.values else None
            params['background'] = request.values['background'] if 'background' in request.values else None
            params['intensity']  = request.values['intensity']  if 'intensity'  in request.values else None
            resp = my_rpc("set_current_status", params)
            return CORSResponse("Success", 200)
        except Exception as e:
            print(f"jar status post has exception {e}")
            if e.args[0] == "Invalid State":
                return CORSResponse("Pattern In Transition", 409)
            else:
                return CORSResponse("Internal Error", 500)
    else:
        return CORSResponse("Method Not Allowed", 405)

@app.route("/jar_leds/patterns", methods=['GET'])
def get_available_patterns():
    ''' Get available foreground and background patterns
    '''
    if request.method == 'GET':
        try: 
            values = my_rpc("get_patterns", None)
            if values is None:
                raise ValueError("No response")
            if "foregrounds" not in values \
                or "backgrounds" not in values:
                raise ValueError("Bad response")

            response = {'backgrounds':[], 'foregrounds':[]}
            response['backgrounds'] = [{
                    'name':background_name, 
                    'url':IMAGE_DIR + "/" + background_name
                } 
                for background_name in values['backgrounds']
            ]
            response['foregrounds'] = values['foregrounds']
            return JSONResponse(json.dumps(response))
        except:
            return CORSResponse("Internal Error", 500)
    else:
        return CORSResponse("Method Not Allowed", 405)
    

def makeJsonResponse(jsonString, respStatus=200):
    return  CORSResponse(jsonString, status=respStatus, mimetype='application/json')


if __name__ == "__main__":

    logging.basicConfig(format='%(asctime)-15s %(levelname)s %(module)s %(lineno)d: %(message)s', level=logging.DEBUG)
    
    serve_forever()

    flaskThread = Thread(target=serve_forever, args=[6000])
    flaskThread.start()

    print("About to make request!")

    baseURL = 'http://localhost:' + str(PORT) + "/"
