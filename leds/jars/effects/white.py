from effectlayer import EffectLayer 


class WhiteLayer(EffectLayer):
    """
    This layer makes the whole thing light up white.
    """
    def __init__(self):
        pass

    def render(self, model, params, frame):
        for i in range(len(frame)):
            frame[i] = 1.0




