#!/usr/bin/env python
from model import JarModel 
from renderer import Renderer
from controller import AnimationController
from effectlayer import EffectParameters
from effectlayer import IntensityLayer
from effects.color_cycle import ColorCycleLayer
from effects.photo_colors import PhotoColorsLayer 
from effects.repair import Repair
from playlist import Playlist

import os
import queue
import sys
from threading import Thread

DEFAULT_PHOTO = "fire_image.png"

class JarLedDriver():
    ''' Manage LEDs patterns running on the jar.'''

    def __init__(self, cmd_queue, resp_queue, jar_id):
        self.cmd_queue = cmd_queue
        self.resp_queue = resp_queue
        self.jar_id = jar_id
        self.driver_thread = Thread(target=self.run_led_driver)
        self.driver_thread.start()

    def stop(self):
        # send stop on the cmd queue
        self.cmd_queue.put({'cmd':'stop'})
        self.driver_thread.join()    

    def run_led_driver(self):
        # master parameters, used in rendering and updated by playlist 
        # advancer thread. Time, requested framerate, button state
        masterParams = EffectParameters()
    
        # Get the appropriate piece of the jar
        model = JarModel(self.jar_id)
    
        # the controller manages the animation loop - creates frames, calls into the renderer
        # at appropriate intervals, updates the time stored in master params, and sends frames
        # out over OPC
        controller = AnimationController(
            model,
            self.cmd_queue,
            self.resp_queue,
            (DEFAULT_PHOTO, 'colorcycle', 1.0),
            masterParams,
        )
        controller.start() 
    
