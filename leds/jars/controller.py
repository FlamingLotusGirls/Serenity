from model import Model
from effectlayer import EffectParameters
from effectlayer import IntensityLayer
from renderer import Renderer
from playlist import Playlist
from effects.firefly_swarm import FireflySwarmLayer
from effects.photo_colors import PhotoColorsLayer
from effects.colorwiper import ColorWiper
from effects.color_cycle import ColorCycleLayer
from effects.colorwave import ColorWave
import os
import socket
import time
import sys
import numpy
import struct
import logging
import traceback

effects_map = {
                "fireflies" : {'cls': FireflySwarmLayer, 'params': []},
                "colorcycle" : {'cls': ColorCycleLayer, 'params' : []},
                "colorwiper" : {'cls': ColorWiper, 'params': ["$MODEL"]},
                "colorwave" : {'cls': ColorWave, 'params': ["$MODEL"]},
              }

def fixup_effects_map(model):
    ''' Because some of the effects need the model as a parameter... 
    '''    
    for key, value in effects_map.items():
        try:
            params = value['params']
            params[params.index('$MODEL')] = model
        except ValueError:
            pass

logging.basicConfig()
logger = logging.getLogger()
   
class AnimationController(object):
    """Manages the main animation loop. Calls into the renderer to populate
       a frame of LED data, then sends it to the OPC server. This class manages frame
       rate control, and handles the advancement of time in EffectParameters.

       This code is taken from the Soma codebase, but I've hacked it quite severely
       for Serenity. You have a background pattern (taken from an image) and a 
       foreground effectr (one of a small list), and an overall intensity. If 
       we continue to show Serenity it might be nice to restore some of the Soma
       functionality - playlists and interactivity.  CSW 8/2019 
       """

    def __init__(self, model, cmd_queue, resp_queue, playlist_params, params=None, server=None):
        self.opc = FastOPC(server)
        self.model = model
        self.params = params or EffectParameters()
        self.cmd_queue = cmd_queue
        self.resp_queue = resp_queue
        self.running = True
        self.image_dir = 'images/'  # XXX pass this in from above...
      
        # Set up the first playlist. The architecture here is sort of fucked
        # for what I'm trying to do, but ... 
        fixup_effects_map(model)

        first_playlist = self.create_first_playlist(*playlist_params) 
        # the renderer manages a playlist (or dict of multiple playlists), as well as transitions 
        # and gamma correction 
        self.renderer = Renderer(playlists={'primary':first_playlist}, gamma=2.2)
    
        self._fpsFrames = 0
        self._fpsTime = 0
        self._fpsLogPeriod = 1    # How often to log frame rate

    def advanceTime(self):
        """Update the timestep in EffectParameters.

           This is where we enforce our target frame rate, by sleeping until the minimum amount
           of time has elapsed since the previous frame. We try to synchronize our actual frame
           rate with the target frame rate in a slightly loose way which allows some jitter in
           our clock, but which keeps the frame rate centered around our ideal rate if we can keep up.

           This is also where we log the actual frame rate to the console periodically, so we can
           tell how well we're doing.
           """

        now = time.time()
        dt = now - self.params.time
        dtIdeal = 1.0 / self.params.targetFrameRate

        if dt > dtIdeal * 2:
            # Big jump forward. This may mean we're just starting out, or maybe our animation is
            # skipping badly. Jump immediately to the current time and don't look back.

            self.params.time = now

        else:
            # We're approximately keeping up with our ideal frame rate. Advance our animation
            # clock by the ideal amount, and insert delays where necessary so we line up the
            # animation clock with the real-time clock.

            self.params.time += dtIdeal
            if dt < dtIdeal:
                time.sleep(dtIdeal - dt)

        # Log frame rate

        self._fpsFrames += 1
        if now > self._fpsTime + self._fpsLogPeriod:
            fps = self._fpsFrames / (now - self._fpsTime)
            self._fpsTime = now
            self._fpsFrames = 0
            logger.debug("%7.2f FPS" % fps)

    def renderLayers(self):
        """Generate a complete frame of LED data by rendering each layer."""

        # Note: You'd think it would be faster to use float32 on the rPI, but
        #       32-bit floats take a slower path in NumPy sadly.
        # Also: Numpy makes float64s by default
        frame = numpy.zeros((self.model.numLEDs, 3))

        self.renderer.render(self.model, self.params, frame)
        return frame

    def frameToHardwareFormat(self, frame):
        """Convert a frame in our abstract floating-point format to the specific format used
           by the OPC server. Does not clip to the range [0,255], this is handled by FastOPC.

           Modifies 'frame' in-place.
           """
        numpy.multiply(frame, 255, frame)

    def drawFrame(self):
        """Render a frame and send it to the OPC server"""
        self.advanceTime()
        pixels = self.renderLayers()
        self.frameToHardwareFormat(pixels)
        self.opc.putPixels(0, pixels)

    def _get_current_status_response(self, command_id):
        resp = {
                'id': command_id, 
                'response': {'foreground':self.foreground,
                             'background':self.background,
                             'intensity' :self.intensity
                            }
               }
        return resp
    
    def get_available_backgrounds(self):
        return [f for f in os.listdir(self.image_dir) if os.path.isfile(os.path.join(self.image_dir, f))]
   
    def create_first_playlist(self, background, foreground, intensity): 
        self.background = background
        self.foreground = foreground
        self.intensity = intensity 
        
        foreground_effect = effects_map[foreground]['cls']
        foreground_params = effects_map[foreground]['params']
        foreground_layer = foreground_effect(*foreground_params)
        background_layer = PhotoColorsLayer(self.model, self.image_dir + background)
        intensity_layer  = IntensityLayer(float(intensity))
       
        routines = [[background_layer, foreground_layer, intensity_layer]]
        return Playlist(routines)
 
    def set_current_playlist(self, background, foreground, intensity):
        errStr = None
        foreground_effect = None
        background_photo = None
        intensity_val = None
        
        if foreground is not None:
            try:
                foreground_effect = effects_map[foreground]['cls']
                foreground_params  = effects_map[foreground]['params']
            except KeyError:
                errStr = f"Warning: effect {foreground} not found"
        if background is not None:
            background_photo = self.image_dir + background
            if not os.path.isfile(background_photo):
                errStr = f"Warning. Photo {background_photo} not found"
        if intensity is not None:
            try:
                intensity_val = float(intensity)
                if (intensity_val < 0 or intensity_val > 1.0):
                    raise Exception
            except Exception:
                errStr = f"Warning. Intensity {intensity} not legal value"
        
        if errStr is not None:
            raise ValueError(errStr) 
         
        new_routines = self.renderer.getActivePlaylist().routines.copy()
        if background_photo is not None:
            new_routines[0][0] = PhotoColorsLayer(self.model, background_photo)
            self.background = background
        if foreground_effect is not None:
            new_routines[0][1] = foreground_effect(*foreground_params)
            self.foreground = foreground
        if intensity_val is not None:
            new_routines[0][2] = IntensityLayer(intensity_val)
            self.intensity = intensity_val

        new_playlist = Playlist(new_routines)
        self.renderer.changePlaylist(new_playlist)

    def process_commands(self):
        while not self.cmd_queue.empty():
            command = self.cmd_queue.get()
            try: 
                cmd = command['cmd']
                if cmd == 'stop':
                    self.running = False
                    return
                elif cmd == 'set_current_status':
                    self.set_current_playlist(
                        command['params']['background'], 
                        command['params']['foreground'],
                        command['params']['intensity']
                    )
                    resp = self._get_current_status_response(command['id'])
                elif cmd == 'get_current_status':
                    resp = self._get_current_status_response(command['id'])
                elif cmd == 'get_patterns':
                    resp = {
                            'id' : command['id'],
                            'response': {
                                         'foregrounds': list(effects_map.keys()),
                                         'backgrounds': self.get_available_backgrounds(),
                                        }
                            }
                else:
                    resp = {
                            'id' : command['id'],
                            'response' : {
                                          'error' : 'unknown command'
                                         }
                            }
            except Exception as e:
                traceback.print_exc()
                resp = {
                        'id' : command['id'],
                        'response' : {
                                      'error' : str(e)
                                     }
                        }
                
            self.resp_queue.put(resp)

    def start(self):
        self.drawingLoop()

    def drawingLoop(self):
        """Render frames forever or until keyboard interrupt"""
        try:
            while self.running:
                # check for commands
                self.process_commands()
                self.drawFrame()
        except KeyboardInterrupt:
            pass
        
        
class FastOPC(object):
    """High-performance Open Pixel Control client, using Numeric Python.
       By default, assumes the OPC server is running on localhost. This may be overridden
       with the OPC_SERVER environment variable, or the 'server' keyword argument.
       """

    def __init__(self, server=None):
        self.server = server or os.getenv('OPC_SERVER') or '127.0.0.1:7890'
        self.host, port = self.server.split(':')
        self.port = int(port)
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.connect((self.host, self.port))

    def putPixels(self, channel, pixels):
        """Send a list of 8-bit colors to the indicated channel. (OPC command 0x00).
           'Pixels' is an array of any shape, in RGB order. Pixels range from 0 to 255.

           They need not already be clipped to this range; that's taken care of here.
           'pixels' is clipped in-place. If any values are out of range, the array is modified.
           """

        numpy.clip(pixels, 0, 255, pixels)
        packedPixels = pixels.astype('B').tostring()
        header = struct.pack('>BBH',
            channel,
            0x00,  # Command
            len(packedPixels))
        self.socket.send(header + packedPixels)
