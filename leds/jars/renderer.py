#!/usr/bin/env python

import time
import numpy
from effectlayer import GammaLayer
from effectlayer import IntensityLayer
from effects.photo_colors import PhotoColorsLayer
from playlist import Playlist
import fade
class Renderer:
    """
    Renders the selected routine in the currently active playlist. 
    (A "routine" is an effect layer or list of effect layers.)
    Performs smooth transitions when the active routine changes (either due to swapping 
    playlists or to advancing the selection in the current playlist).
    
    Also applies a gamma correction layer after everything else is rendered.
    """
    def __init__(self, playlists, activePlaylist=None, useFastFades=False, gamma=2.2):
        """
        playlists argument should be dictionary of playlist names : playlists.
        useFastFades uses fast fades instead of linear fades when advancing/swapping playlists
         (see fade class comments below)
        """        
        if not playlists:
            raise Exception("Can't define a renderer without any playlists")
        self.playlists = playlists
        
        # activePlaylist is the index of the current playlist 
        if activePlaylist is not None:
            self.activePlaylist = activePlaylist if activePlaylist in range(len(playlists)) else 0
        else:
            self.activePlaylist = list(playlists.keys())[0]
            
        # used when fading between playlists, to know what to return to when the fade is done
        self.nextPlaylist = None 
        
        self.useFastFades = useFastFades
        self.fade = None
        #self.gammaLayer = GammaLayer(gamma)

    def getActivePlaylist(self):
        return self._active()

    def _active(self):
        if self.activePlaylist is None:
            return None
        else:
            return self.playlists[self.activePlaylist]
        
    def _next(self):
        if self.nextPlaylist is None:
            return None
        else:
            return self.playlists[self.nextPlaylist]
       
    # XXX note here that fades are defined between playlists. A playlist has
    # a set of 'songs', each 'song' has a number of 'tracks' (aka effects) and
    # there can be multiple playlists.
    # In this particular simple implementation I'm using for Serenity, there
    # is one playlist with one song. The song has two tracks - foreground and
    # background. And that's it. 
    def render(self, model, params, frame):
        if self.fade:
            self.fade.render(model, params, frame)
            if self.fade.done:
                # If the fade was to a new playlist, set that one to active
                if self.nextPlaylist is not None:
                    # Special hack for Serenity, where I'm creating playlists
                    # on the fly, and only ever really have one
                    if self.nextPlaylist == 'incoming':
                        self.playlists[self.activePlaylist] = self.playlists[self.nextPlaylist]
                        self.playlists.pop('incoming')
                    self.nextPlaylist = None
                self.fade = None
        elif self.activePlaylist is not None:
            for layer in self._active().selection():
                layer.render(model, params, frame)
                # layer.safely_render(model, params, frame)
        #self.gammaLayer.render(model, params, frame)
        
    def advanceCurrentPlaylist(self, fadeTime=1):
        """ Advance selection within current playlist
        """
        active = self._active()
        if active is not None:
            selection = active.selection()
            active.advance()
            self.fade = fade.LinearFade(selection, active.selection(), fadeTime)
        else:
            raise Exception("Can't advance playlist - no playlist is currently active")

    def _fadeTimeForTransition(self, playlist):
        return max([effect.transitionFadeTime for effect in playlist.selection()])

    def changeIntensity(self, newIntensity):
        intensityLayer = self._active().selection()[2]
        intensityLayer.set_intensity(newIntensity)

    def changePlaylist(self, newPlaylist):
        if self.fade is not None:
            print("Warning. Already in transition")
            raise Exception("Invalid State")
        self.playlists['incoming'] = newPlaylist
        self.swapPlaylists('incoming')
    
    def swapPlaylists(self, nextPlaylist, intermediatePlaylist=None, advanceAfterFadeOut=False, fadeTime=1):
        """Swap to a new playlist, either directly or by doing a two-step fade to an intermediate one first."""
        
        active = self._active()
        self.nextPlaylist = nextPlaylist
        
        if self.useFastFades:
            self.fade = fade.FastFade(active.selection(), self._next().selection(), fadeTime)
        else:
            if intermediatePlaylist:
                middle = self._get(intermediatePlaylist)
                self.fade = fade.TwoStepLinearFade(active.selection(), middle.selection(), self._next().selection(), 0.25, self._fadeTimeForTransition(middle))
                if advanceAfterFadeOut:
                    middle.advance()
            else:
                self.fade = fade.LinearFade(active.selection(), self._next().selection(), fadeTime)
        if advanceAfterFadeOut:
            active.advance()



