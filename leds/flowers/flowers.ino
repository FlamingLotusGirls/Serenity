// NeoPixel test program showing use of the WHITE channel for RGBW
// pixels only (won't look correct on regular RGB NeoPixel strips).

#include <Adafruit_NeoPixel.h>


// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN     6



// NeoPixel brightness, 0 (min) to 255 (max)
#define BRIGHTNESS 50
const int max_num_pulses = 15;
const int num_rings = 3;
const int pixels_per_ring = 24;
const int num_pixels = num_rings * pixels_per_ring;
// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(num_pixels, LED_PIN, NEO_GRBW + NEO_KHZ800);


uint8_t ringBaseR[num_rings] = {100, 150, 200};
uint8_t ringBaseG[num_rings] = {0, 0, 0};
uint8_t ringBaseB[num_rings] = {255, 255, 255};
uint8_t ringBaseW[num_rings] = {0, 0, 0};
int pulseLocations[max_num_pulses];
int pulseRings[max_num_pulses];
uint8_t pulseIntensities[max_num_pulses][4];
bool pulseDirections[max_num_pulses];
int pulseStepDelay[max_num_pulses];
uint8_t pulseStepsLeft[max_num_pulses];
uint8_t pulseWidths[max_num_pulses];
uint8_t rgbwColors[num_pixels][4];
long nextPulseCheckTime[max_num_pulses];

void setup() {
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(255); // Set BRIGHTNESS to about 1/5 (max = 255)
  for (int i=0; i < max_num_pulses; i++){
    pulseStepsLeft[i] = 0;
  }
  //Serial.begin(115200);

}

void loop() {
  for (int i=0; i < num_pixels; i++){
      int ringIndex = i / pixels_per_ring;
      rgbwColors[i][0] = ringBaseR[ringIndex];
      rgbwColors[i][1] = ringBaseG[ringIndex];
      rgbwColors[i][2] = ringBaseB[ringIndex];
      rgbwColors[i][3] = ringBaseW[ringIndex];
  }
  for (int i=0; i < max_num_pulses; i++){
    if (pulseStepsLeft[i] > 0){ //add each active pulse to the pixels
      if (millis() > nextPulseCheckTime[i]){
        pulseStepsLeft[i] -=1;
        nextPulseCheckTime[i] = millis() + pulseStepDelay[i];
        pulseLocations[i] += pulseDirections[i] ? 1: -1;
        pulseLocations[i] = (pixels_per_ring + pulseLocations[i]) % pixels_per_ring;
      }
      for (int j=0; j<pulseWidths[i]; j++){
        int upLocation = ((pulseLocations[i] + j) % pixels_per_ring) + (pulseRings[i] * pixels_per_ring);
        int dnLocation = ((pixels_per_ring + pulseLocations[i] - j) % pixels_per_ring) + (pulseRings[i] * pixels_per_ring);
        for (int k=0; k<3; k++){
          int colorAddition = (pulseIntensities[i][k] * (pulseWidths[i] - j));
          rgbwColors[upLocation][k] -= min(colorAddition, rgbwColors[upLocation][k]);
          rgbwColors[dnLocation][k] -= min(colorAddition, rgbwColors[dnLocation][k]);
        }
        int colorAddition = (pulseIntensities[i][3] * (pulseWidths[i] - 3));
        rgbwColors[upLocation][3] -= min(colorAddition, rgbwColors[upLocation][3]);
        rgbwColors[dnLocation][3] -= min(colorAddition, rgbwColors[dnLocation][3]);
      }
    }
    else { // decide if we should initialize another
      if (random(1000) == 1){
        //Serial.println(i);
        pulseStepsLeft[i] = random(100) + 5;
        nextPulseCheckTime[i] = 0;
        pulseLocations[i] = random(pixels_per_ring);
        pulseStepDelay[i] = 50 + random(250);
        pulseRings[i] = random(num_rings);
        pulseWidths[i] =  random(3) + 1;
        pulseIntensities[i][0] = random(50);
        pulseIntensities[i][1] = random(20);
        pulseIntensities[i][2] = random(50);
        pulseIntensities[i][3] = random(150); 
        pulseDirections[i] = random(2) == 1;                       
      }
    }
  }
  for (int i=0; i < num_pixels; i++){
    strip.setPixelColor(i, strip.Color(rgbwColors[i][0], rgbwColors[i][1], rgbwColors[i][2], rgbwColors[i][3])); 
    
  }
  strip.show();
  //Serial.println('L');
}


