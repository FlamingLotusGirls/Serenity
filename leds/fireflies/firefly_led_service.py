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

from led_controller import FireflyLedController

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


class CORSResponse(Response):
    def __init__(self, status_str, status_code, mimetype="text/plain"):
        super().__init__(status_str, status_code, mimetype=mimetype)
        self.headers['Access-Control-Allow-Origin'] = '*'


class JSONResponse(CORSResponse):
    def __init__(self, status_str, status_code=200):
        super().__init__(status_str, status_code, "application/json")


def parse_colors(colors):
    colorarray = colors.split(",")
    if len(colorarray) != 3:
        raise ValueError
    colorvals = []
    for color in colorarray:
        colorval = int(color)
        if colorval < 0 or colorval > 255:
            raise ValueError
        colorvals.append(int(colorval))
        
    return colorvals[0], colorvals[1], colorvals[2]


def parse_speed(speed):
    speedval = int(speed)
    if speedval < 1 or speedval > 10:
        raise ValueError
    return speedval


def parse_sequence(sequence):
    for character in sequence:
        if character != '0' and character != '1':
            raise ValueError
    return sequence
    

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
            return CORSResponse("Must have 'swarm' value", 400)
        try:
            swarm_id = int(request.values['swarm'])
            if swarm_id < 0 or swarm_id > 3:
                raise Exception
            current_leds = controller.get_led_patterns()
            for leds in current_leds:
                if leds['board_id'] == swarm_id:
                    led_settings = leds
                    break
        except:
            return CORSResponse("Invalid swarm value", 400)

        if 'color' in request.values:
            try:
                red, green, blue = parse_colors(request.values['color'])
            except:
                return CORSResponse("Invalid color values", 400)
            if red < 0 \
                or red > 255 \
                or green < 0 \
                or green > 255 \
                or blue < 0 \
                or blue > 255:
                return CORSResponse("Invalid color values", 400)
            red = red/255
            green = green/255
            blue = blue/255  
        else:
            red = led_settings['color'][0]
            green = led_settings['color'][1]
            blue = led_settings['color'][2] 
    
        if 'speed' in request.values:
            try:
                speed = parse_speed(request.values['speed'])
            except:
                return CORSResponse("Invalid speed values", 400) 
        else:
            speed = led_settings['speed']
    
        if 'sequence' in request.values:
            try:
                sequence = parse_sequence(request.values['sequence'])
            except:
                return CORSResponse("Invalid sequence values", 400)
        else:
            sequence = led_settings['pattern']
            
        controller.set_led_pattern(swarm_id, red, green, blue, speed, sequence)

        return CORSResponse("", 200)

    else:
        foo = controller.get_led_patterns()
        return JSONResponse(json.dumps(controller.get_led_patterns()))



if __name__ == "__main__":

    logging.basicConfig(format='%(asctime)-15s %(levelname)s %(module)s %(lineno)d: %(message)s', level=logging.DEBUG)

    controller = FireflyLedController() 
    flaskThread = Thread(target=serve_forever, args=[7000])
    flaskThread.start()

    print("About to make request!")

    baseURL = 'http://localhost:' + str(PORT) + "/"
