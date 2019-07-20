import json
import os
import copy


class PatternManager():
    def __init__(self, pattern_filename="./firefly_led_patterns.json"):
        try:
            with open(pattern_filename) as f:
                self.patterns = json.load(f)
        except:
            self.patterns = {}

        self.pattern_filename = pattern_filename

    def get_patterns(self):
        return self.patterns

    def get_pattern(self, pattern_name):
        if pattern_name not in self.patterns:
            return None
        return self.patterns[pattern_name]

    def set_pattern(self, pattern_name, pattern_data):
        self.patterns[pattern_name] = pattern_data
        self.write_patterns()

    def delete_pattern(self, pattern_name):
        if pattern_name in self.patterns:
            del self.patterns[pattern_name]
        self.write_patterns()

    def write_patterns(self):
        with open(self.pattern_filename, "w+") as fp:
            json.dump(self.patterns, fp)
        
              
