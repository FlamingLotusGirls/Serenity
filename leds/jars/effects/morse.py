import math
import random
import numpy
import time

from effectlayer import *

class MorseLayer(EffectLayer):
    UNIT = 500
    INTER_LETTER_TIME = UNIT*3
    SPACE_TIME = UNIT*7
    INTERSTITIAL_TIME = UNIT
    DOT_TIME = UNIT
    DASH_TIME = UNIT*3
    REFRESH_TIME = UNIT*9

    def __init__(self, theString1, theString2):
        self.target_time = 0
        self.prev_time = 0
        self.string = theString1
        self.major_index = 0  # start at the beginning
        self.minor_index = 0  # the index in the morse dot-dash series
        self.mode = 0 # 0 is off, 1 is on...
        self.iteration = 0
        self.mapping = {'a':[0,1], 
                        'b':[1,0,0,0],
                        'c':[1,0,1,0],
                        'd':[1,0,0],
                        'e':[0],
                        'f':[0,0,1,0],
                        'g':[1,1,0],
                        'h':[0,0,0,0],
                        'i':[0,0],
                        'j':[0,1,1,1],
                        'k':[1,0,1],
                        'l':[0,1,0,0],
                        'm':[1,1],
                        'n':[1,0],
                        'o':[1,1,1],
                        'p':[0,1,1,0],
                        'q':[1,1,0,1],
                        'r':[0,1,0],
                        's':[0,0,0],
                        't':[1],
                        'u':[0,0,1],
                        'v':[0,0,0,1],
                        'w':[0,1,1],
                        'x':[1,0,0,1],
                        'y':[1,0,1,1],
                        'z':[1,1,0,0],
                        'A':[0,1], 
                        'B':[1,0,0,0],
                        'C':[1,0,1,0],
                        'D':[1,0,0],
                        'E':[0],
                        'F':[0,0,1,0],
                        'G':[1,1,0],
                        'H':[0,0,0,0],
                        'I':[0,0],
                        'J':[0,1,1,1],
                        'K':[1,0,1],
                        'L':[0,1,0,0],
                        'M':[1,1],
                        'N':[1,0],
                        'O':[1,1,1],
                        'P':[0,1,1,0],
                        'Q':[1,1,0,1],
                        'R':[0,1,0],
                        'S':[0,0,0],
                        'T':[1],
                        'U':[0,0,1],
                        'V':[0,0,0,1],
                        'W':[0,1,1],
                        'X':[1,0,0,1],
                        'Y':[1,0,1,1],
                        'Z':[1,1,0,0],
                        '1':[0,1,1,1,1],
                        '2':[0,0,1,1,1],
                        '3':[0,0,0,1,1],
                        '4':[0,0,0,0,1],
                        '5':[0,0,0,0,0],
                        '6':[1,0,0,0,0],
                        '7':[1,1,0,0,0],
                        '8':[1,1,1,0,0],
                        '9':[1,1,1,1,0],
                        '0':[0,0,0,0,0]}
        self.getNextPrintable()
        

    # Morse timing:  ON: DOT_TIME, DASH_TIME
    #                OFF: INTERSTITIAL_TIME, INTRA_LETTER_TIME, SPACE_TIME, REFRESH_TIME
    # Pointers into string - major_index -> pointer to char in string
    #                      - minor_index -> pointer to dot/dash in char, if appropriate
    # The pointers are pre-incremented, that is, we increment the pointers *before* we hit
    # the target time, not before
    def render(self, model, params, frame):
        # check whether we should restart the pattern - more than a second has passed since
        # the last time we've rendered. XXX TODO
        # look at current time... decide if we need to do the next item...
        current_time = int(time.time() *1000)
        # print "target", self.target_time
        # print current_time
        print frame
        if current_time >= self.target_time:
            self.prev_time = current_time
            # print "morse turn on off"
            if self.mode == 0: # if we're currently off, turn on XXX - we start off, but we haven't called get next printable at that point.
                if self.major_index == -1:
                    # invalid string. Blink.
                    self.target_time = current_time + self.INTERSTITIAL_TIME
                else:
                    letter = self.string[self.major_index]
                
                    if letter in self.mapping: # valid letter
                        #print "minor index is ", self.minor_index
                        dotdash = self.mapping[letter][self.minor_index] # get current dot-dash
                        if dotdash == 0:  # dot!
                            self.target_time = current_time + self.DOT_TIME
                        else: # dash!
                            self.target_time = current_time + self.DASH_TIME
                    else: # invalid string. Simply blink
                        self.target_time = current_time + self.INTERSTITIAL_TIME
                
                self.mode = 1 # we're now on
                    
            else: # if we're currently on, turn off
                self.mode = 0
                dark_time = 0
                next_printable = self.getNextPrintable()
                # check - are there no printable characters?
                if next_printable == -1:
                    dark_time = self.INTERSTITIAL_TIME
                # have we gone past the end of the string?
                elif next_printable < self.major_index:
                    dark_time = self.REFRESH_TIME
                    self.iteration += 1
                    print "iteration is", self.iteration
                # have we moved past some non-printables?
                elif next_printable > self.major_index + 1:
                    dark_time = self.SPACE_TIME
                # going to the next letter
                elif next_printable == self.major_index + 1:
                    dark_time = self.INTER_LETTER_TIME
                # still in the middle of the letter
                else:
                    dark_time = self.INTERSTITIAL_TIME
                
                # update pointers...
                if next_printable != self.major_index:
                    self.major_index = next_printable
                    self.minor_index = 0
                else:
                    self.minor_index = self.minor_index + 1
                
                self.target_time = current_time + dark_time
            
        len = frame.size/3
        # iterate through pixels...
        # frame lower does the first 
        for i in range(0,len):
            if i in model.lowerIndices:
                frame[i] = self.mode
                
    def getNextPrintable(self):
        cur_char = self.string[self.major_index]
        # if we're in the middle of a letter, the next printable is the current letter
        # print "cur char is ", cur_char
        # print "minor index is ", self.minor_index
        # print "mapping length is ", len(self.mapping[cur_char])
        if cur_char in self.mapping and self.minor_index+1 < len(self.mapping[cur_char]):
            return self.major_index
        
        # other wise - figure out where the next printable character is
        start_idx = self.major_index
        str_len = len(self.string)
        for i in range(str_len):
            cur_idx = (i + 1 + start_idx) % str_len
            cur_char = self.string[cur_idx]
            if cur_char in self.mapping:
                break
        if i==str_len:
            return -1
        else:
            return (i + 1 + start_idx) % str_len
            
        # okay - so what should be happening? A slow pulse with some variation on one side, becomes coherent
        # and then goes to the morse message that then moves over to the other side?
                
