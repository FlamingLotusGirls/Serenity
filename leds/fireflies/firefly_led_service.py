from builtins import str
from flask import Flask
from flask import request
from flask import Response
from flask import abort
import json
import logging
import requests
import urllib.request, urllib.parse, urllib.error
from threading import Thread

import led_controller

''' 
    REST service for controlling firefly leds
'''

PORT = 7000

logger = logging.getLogger("firefly_leds")

app = Flask("flg", static_url_path="", static_folder="/home/flaming/Serenity/leds/fireflies/static")
#app = Flask("flg", static_url_path="")

def serve_forever(httpPort=PORT):
    logger.info("Fireflies LED WebServer: port {}".format(httpPort))
    app.run(host="0.0.0.0", port=httpPort, threaded=True) ## XXX - FIXME - got a broken pipe on the socket that terminated the application (uncaught exception) supposedly this is fixed in flask 0.12

g_led_state = [ 
  {'swarm':0, 'red':255, 'green':255, 'blue':255, 'speed':1, 'sequence':'0000011111'},
  {'swarm':1, 'red':255, 'green':255, 'blue':255, 'speed':1, 'sequence':'0000011111'},
  {'swarm':2, 'red':255, 'green':255, 'blue':255, 'speed':1, 'sequence':'0000011111'},
  {'swarm':3, 'red':255, 'green':255, 'blue':255, 'speed':1, 'sequence':'0000011111'},
]

@app.route("/firefly_leds", methods=['GET', 'POST'])
def firefly_status():
    ''' GET  Get current firefly led patterns for all swarms. Patterns
        consist of a color (red, green, and blue values), a speed, and 
        a sequence of 1's and 0's
        POST swarm=[swarm_id] red=, green=, blue=, speed=, sequence=
        Set a pattern for a swarm. Swarm id is necessary; all others 
        can be set independently.
    '''
    if request.method == 'POST':
        if not 'swarm' in request.values:
            return Response("Must have 'swarm' value", 400)
        try:
            swarm_id = int(request.values['swarm'])
            if swarm_id < 0 or swarm_id > 3:
                raise Exception
            current_leds = led_controller.get_led_patterns()
        except:
            return Response("Invalid swarm value", 400)

        if 'color' in request.values:
            try:
                red, green, blue = parse_color(request.values['color'])
            except:
                return Response("Invalid color values", 400)
        else:
            red = current_leds['red']
            green = current_leds['green']
            blue = current_leds['blue'] 
    
        if 'speed' in request.values:
            try:
                speed = parse_speed(request.values['speed'])
            except:
                return Response("Invalid speed values", 400) 
        else:
            speed = current_leds['speed']
    
        if 'sequence' in request.values:
            try:
                sequence = parse_sequence(request.values['sequence'])
            except:
                return Response("Invalid sequence values", 400)
        else:
            sequence = current_leds['sequence']
            
        led_controller.send_pattern(swarm_id, red, green, blue, speed, sequence)

        return Response("", 200)

    else:
        return makeJsonResponse(json.dumps(led_controller.get_led_patterns()))

def makeJsonResponse(jsonString, VrespStatus=200):
    resp = Response(jsonString, status=respStatus, mimetype='application/json')
    resp.headers['Access-Control-Allow-Origin'] = '*'
    return resp


if __name__ == "__main__":

    logging.basicConfig(format='%(asctime)-15s %(levelname)s %(module)s %(lineno)d: %(message)s', level=logging.DEBUG)

    flaskThread = Thread(target=serve_forever, args=[7000])
    flaskThread.start()

    print("About to make request!")

    baseURL = 'http://localhost:' + str(PORT) + "/"
