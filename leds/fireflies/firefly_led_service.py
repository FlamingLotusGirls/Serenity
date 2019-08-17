from builtins import str
from flask import Flask
from flask import request
from flask import Response
from flask import abort
from flask import send_file
from flask import make_response
import netifaces as ni
import json
import io
import logging
import os
import requests
import urllib.request, urllib.parse, urllib.error
from threading import Thread

from led_controller import FireflyLedController
from pattern_manager import PatternManager

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
        colorval = float(color)
        if colorval < 0 or colorval > 1.0:
            print(f"color {colorval}")
            raise ValueError
        colorvals.append(colorval)
        
    return colorvals[0], colorvals[1], colorvals[2]


def parse_speed(speed):
    speedval = int(speed)
    if speedval < 1 or speedval > 10:
        print(f"speedval {speedval}")
        raise ValueError
    return speedval


def parse_sequence(sequence):
    for character in sequence:
        if character != '0' and character != '1':
            print(f"sequnceval {sequence}")
            raise ValueError
    return sequence
    
# 
# Calls:
# /firefly_leds 
# Get/Set the current firefly led patterns being shown
#  on the swarms
# /firefly_leds/patterns
# CRUD for the available patterns. Backed by a json file
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
        except Exception as e:
            print(e)
            return CORSResponse("Invalid swarm value", 400)

        if 'pattern_name' in request.values:
            pattern = pm.get_pattern(request.values['pattern_name'])
            if pattern == None:
                return CORSResponse("Invalid pattern name", 400)
            red,green,blue = pattern['color']
            sequence = pattern['pattern']
            speed = pattern['speed']
            pattern_name = request.values['pattern_name']
        else:
            pattern_name = None  
            if 'color' in request.values:
                try:
                    red, green, blue = parse_colors(request.values['color'])
                except:
                    return CORSResponse("Invalid color values", 400)
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
          
        controller.set_led_pattern(
            swarm_id,
            red,
            green,
            blue,
            speed,
            sequence,
            pattern_name
        )

        return CORSResponse("", 200)

    else:
        return JSONResponse(json.dumps(controller.get_led_patterns()))

@app.route("/firefly_leds/patterns/<pattern_name>", methods=['GET', 'POST', 'DELETE'])
def firefly_single_pattern(pattern_name):
    print("SINGLE PATTERN")
    pattern = pm.get_pattern(pattern_name)
    if pattern is None:
        return CORSResponse(f"pattern {pattern_name} not found", 404)
    
    if request.method == 'GET':
        return JSONResponse(json.dumps(pattern))
    elif request.method == 'POST':
        try: 
            if 'color' in request.values:
                red, green, blue  = parse_colors(request.values['color'])
            else:
                red, green, blue  = pattern['color']
            if 'sequence' in request.values:
                sequence = parse_sequence(request.values['sequence'])
            else:
                sequence = pattern['pattern']
            if 'speed' in request.values:
                speed = parse_speed(request.values['speed'])
            else:
                speed = pattern['speed']

        except Exception as e:
            print(f"failed! {e}")
            return CORSResponse("Invalid values", 400)

        pattern = FireflyLedController.FireflyPattern(
            None,
            red,
            green,
            blue,
            speed,
            sequence,
            pattern_name
        ).get_minimal()
        pm.set_pattern(pattern_name, pattern)

        return CORSResponse("Success", 200)        
    elif request.method == 'DELETE':
        pm.delete_pattern(pattern_name)
        return CORSResponse("Success", 200)        
        
@app.route("/firefly_leds/patterns", methods=['GET', 'POST'])
def firefly_patterns():
    if request.method == 'GET':
        return JSONResponse(json.dumps(pm.get_patterns()))
    else: # POST
        if not 'pattern_name' in request.values:
            return CORSResponse("Must have name for new pattern", 400)
        if not 'color' in request.values:
            return CORSResponse("Must have color for new pattern", 400)
        if not 'speed' in request.values:
            return CORSResponse("Must have speed for new pattern", 400)
        if not 'sequence' in request.values:
            return CORSResponse("Must have sequence in new pattern", 400)

        name = request.values['pattern_name']
        try: 
            red, green, blue = parse_colors(request.values['color'])
            speed = parse_speed(request.values['speed'])
            sequence = parse_sequence(request.values['sequence'])
        except Exception as e:
            print(f"failed {e}")
            return CORSResponse("Invalid values", 400)

        pattern = FireflyLedController.FireflyPattern(
            None,
            red,
            green,
            blue,
            speed,
            sequence,
            name
        ).get_minimal()
        pm.set_pattern(name, pattern)

        print("RETURN SUCCESS")
        return CORSResponse("Success", 200)

@app.route("/firefly_leds/firmware/<md5>", methods=['GET', 'POST'])
def do_firmware(md5):
    print("Received request for firmware")
    if request.method == 'GET':
        if md5 != controller.get_firmware_hash():
            print("Invalid MD5 has received")
            return CORSResponse("Invalid MD5 hash", 400)

        # fetch current firmware
        print("Fetching firmware")
        with open('current_fw.bin', 'rb') as fw:
            print("Attempting to read file")
#            response = make_response(
            response = send_file(
                    io.BytesIO(fw.read()),
                    attachment_filename="firmware.bin",
                    mimetype="application/octet-stream"
            )
            #    200    
#            )
            print("About to send")
            print(response)
            response.headers['Content-Length'] =  os.path.getsize('current_fw.bin')
            response.headers['x-MD5'] = md5
            #response.headers['Content-Type', 'application/octet-stream']
            #response.headers[
            print("Sending firmware")
            return response

    if request.method == 'POST':
        print("Reloading firmware")
        # trigger reload of firmware in driver
        controller.update_firmware()

#@app.route("/firefly_leds/swarm/<swarm_id>", methods=['GET', 'POST'])
#def firefly_board_pattern(swarm_id):
#    if swarm_id < 0 or swarm_id > 3: # define 3
#        return CORSResponse("Invalid Swarm Id", 400)
#    if request.method == 'GET':
#        current_patterns = led_controller.get_patterns()
#        for pattern in current_patterns:
#            if pattern['board_id'] == swarm_id:
#                break
#        return JSONResponse(json.dumps(pattern))
#    elif request.method == 'POST':
#        if 'pattern_name' not in request.values:
#            return CORSResponse("No pattern name supplied", 400)
#        pattern_name = request.values['pattern_name']
#        pattern = pm.get_pattern(pattern)
#        if pattern is None:
#            return CORSResponse("Invalid pattern name", 400)
#        led_controller.set_pattern(swarm_id, pattern_name, pattern)
#        return CORSResponse("Success", 200)
              
if __name__ == "__main__":

    logging.basicConfig(format='%(asctime)-15s %(levelname)s %(module)s %(lineno)d: %(message)s', level=logging.DEBUG)

    controller = FireflyLedController()
    interfaces = ni.interfaces()
    for interface in interfaces:
        addr = ni.ifaddresses(interface)
        if ni.AF_INET in addr:
            local_addr = addr[ni.AF_INET][0]['addr']
            if local_addr.startswith("127"):
                continue
            print(f"Local address is {addr[ni.AF_INET][0]['addr']}")
            controller.set_service_addr(addr[ni.AF_INET][0]['addr'], PORT) 
            break
    pm = PatternManager()
    flaskThread = Thread(target=serve_forever, args=[PORT])
    flaskThread.start()

    print("About to make request!")

    baseURL = 'http://localhost:' + str(PORT) + "/"
