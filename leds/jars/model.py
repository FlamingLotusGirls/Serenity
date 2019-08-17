import json
import numpy as np

class Model(object):
    """
    Minimal base class for a model describing the layout of the LEDs in physical space. 
    """
    def __init__(self, num_leds):
        self.numLEDs = num_leds


class JarModel(Model):
    """
    The model has the following members, derived from the 'points.json' file
    (or whatever file you decide to load):
    - rawPoints. Array of raw positions, unscaled.
    - nodes. Array of scaled positions - coordinates have been scaled to [0,1]
    """

    def __init__(self, jar_id, points_filename='jars.json'):

        # pull raw positions from JSON
        json_data = json.load(open(points_filename))
        
        jar_data = json_data[str(jar_id)]
            
        self.rawPoints = self._nodesFromJSON(jar_data)

        # Scaled Nodes: It's easier to work with coordinates in the range
        # [0, 1], so find the bounding box and scale the coordinates
        # accordingly

        # The minimum and maximum are 3-vectors in the same coordinate space as self.nodes.
        self.minAABB = [ min(v[i] for v in self.rawPoints) for i in range(3) ]
        self.maxAABB = [ max(v[i] for v in self.rawPoints) for i in range(3) ]

        self.nodes = np.array(
            [
                [ 
                    (v[i] - self.minAABB[i]) / (self.maxAABB[i] - self.minAABB[i])
                    for i in range(3)
                ] 
                for v in self.rawPoints
            ]
        )

        # Init base model
        Model.__init__(self, len(self.rawPoints))

    def _nodesFromJSON(self, jar_data):
        points = []
        for val in jar_data:
            points.append(val['point'])
        return np.array(points)

