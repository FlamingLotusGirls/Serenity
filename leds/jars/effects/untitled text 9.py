import math
import random
import numpy
from effectlayer import *

class MonoLayer(EffectLayer):


    self.hue = 16
    def __init__(self, hue):
        self.hue = hue

    def render(self, model, params, frames):
        for frame in frames:
            hsv = numpy.array(colorsys.rgb_to_hsv(frame[0], frame[1], frame[2]))
            frame = numpy.array(colorsys.hsv_to_rgb(self.hue, hsv[1], hsv[2])

