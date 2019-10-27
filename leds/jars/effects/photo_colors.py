import math
import random
import numpy
from effectlayer import *
import png
import glob

class PhotoColorsLayer(EffectLayer):

    def __init__(self, model, image_path):
        #images = glob.glob("images/*.png")
        #self.image = random.choice(images)
        self.image = image_path
        self.file = None
        print(f"image file is {self.image}")
        #self.file = open(self.image, 'rb')
        self.lastFrame = None
        self.pixelIter = self._pixelIter()
    
    def load_image(self):
        if self.file is not None:
            self.file.close()
        self.file = open(self.image, "rb")
        reader = png.Reader(self.file)
        photo = reader.read()
        print(self.image)
        print(f"width {photo[0]}")
        print(f"height {photo[1]}")
        print(f"metadata {photo[3]}")
        metadata = photo[3]
        self.alpha = metadata['alpha'] # Boolean indicates the presense of an alpha channel.
        self.photoSize = photo[0] * photo[1]
        self.rows = photo[2]
        self.offset = 0
        self.photo = photo
        
        
    def get_photo(self):
        return self.image

    def _pixelIter(self):
        """
        Infinitely iterate over pixels in the image.
        """
        while True:
            self.load_image()
            idx = 0
            # Generate a new row iterator
            rowIter = self.photo[2]  # photo[2] is list of rows
            for row in rowIter:
                width = len(row)
                idx += 1
                i = 0
                try:
                    while i < width:
                        r = row[i]
                        g = row[i+1]
                        b = row[i+2]
                        i += 3
                        if self.alpha:
                            i += 1
                        pixel = numpy.array((r/255.,g/255.,b/255.))
                        yield pixel
                except IndexError:
                    continue

    def render(self, model, params, frame):
        if self.lastFrame is None:
            for i in range(len(frame)):
               frame[i] = next(self.pixelIter)
            self.lastFrame = frame
        else:
            # add a new pixel from the stream
            frame[0] = next(self.pixelIter)
            # Shift the last frame over by one
            for i in range(1, len(frame)):
                frame[i] = self.lastFrame[i-1]
            self.lastFrame = numpy.copy(frame)
