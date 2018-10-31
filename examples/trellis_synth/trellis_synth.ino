#include <Audio.h>
#include <Wire.h>

#include <Adafruit_Keypad.h>
#include <Adafruit_NeoPixel.h>

#define NEO_PIN 10
#define NUM_KEYS 32

AudioSynthWaveform sine0, sine1, sine2, sine3;
AudioSynthWaveform *waves[4] = {
  &sine0, &sine1, &sine2, &sine3,
};

short wave_type[4] = {
  WAVEFORM_SINE,
  WAVEFORM_SQUARE,
  WAVEFORM_SAWTOOTH,
  WAVEFORM_TRIANGLE,
};

float cmaj_low[8] = { 130.81, 146.83, 164.81, 174.61, 196.00, 220.00, 246.94, 261.63 };
float cmaj_high[8] = { 261.6, 293.7, 329.6, 349.2, 392.0, 440.0, 493.9, 523.3 };

AudioEffectEnvelope env0, env1, env2, env3;
AudioEffectEnvelope *envs[4] = {
  &env0, &env1, &env2, &env3,
};

AudioEffectDelay         delay1; 

AudioConnection patchCord01(sine0, env0);
AudioConnection patchCord02(sine1, env1);
AudioConnection patchCord03(sine2, env2);
AudioConnection patchCord04(sine3, env3);
AudioConnection patchCord08(env3, delay1);

AudioMixer4     mixer1;

AudioConnection patchCord17(env0, 0, mixer1, 0);
AudioConnection patchCord18(env1, 0, mixer1, 1);
AudioConnection patchCord19(env2, 0, mixer1, 2);
AudioConnection patchCord20(env3, 0, mixer1, 3);

AudioMixer4     mixerLeft;
AudioMixer4     mixerRight;

AudioOutputAnalogStereo  audioOut;

AudioConnection patchCord33(mixer1, 0, mixerLeft, 0);
AudioConnection patchCord38(delay1, 0, mixerLeft, 1);
AudioConnection patchCord39(delay1, 1, mixerLeft, 2);
AudioConnection patchCord37(mixer1, 0, mixerRight, 0);
AudioConnection patchCord40(delay1, 2, mixerRight, 1);
AudioConnection patchCord43(delay1, 3, mixerRight, 2);
AudioConnection patchCord41(mixerLeft, 0, audioOut, 0);
AudioConnection patchCord42(mixerRight, 0, audioOut, 1);

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_KEYS, NEO_PIN, NEO_GRB + NEO_KHZ800);

const byte ROWS = 4; // four rows
const byte COLS = 8; // eight columns
//define the symbols on the buttons of the keypads
byte trellisKeys[ROWS][COLS] = {
  {1,  2,  3,  4,  5,  6,  7,  8},
  {9,  10, 11, 12, 13, 14, 15, 16},
  {17, 18, 19, 20, 21, 22, 23, 24},
  {25, 26, 27, 28, 29, 30, 31, 32}
};
byte rowPins[ROWS] = {14, 15, 16, 17}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {2, 3, 4, 5, 6, 7, 8, 9}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Adafruit_Keypad customKeypad = Adafruit_Keypad( makeKeymap(trellisKeys), rowPins, colPins, ROWS, COLS); 

void setup(){
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  customKeypad.begin();
  
  Serial.begin(9600);
  //while (!Serial);

  customKeypad.tick();

  strip.setBrightness(255);
  
  AudioMemory(120);

  // reduce the gain on some channels, so half of the channels
  // are "positioned" to the left, half to the right, but all
  // are heard at least partially on both ears
  mixerLeft.gain(1, 0.36);
  mixerLeft.gain(3, 0.36);
  mixerRight.gain(0, 0.36);
  mixerRight.gain(2, 0.36);

  // set envelope parameters, for pleasing sound :-)
  for (int i=0; i<4; i++) {
    envs[i]->attack(9.2);
    envs[i]->hold(2.1);
    envs[i]->decay(31.4);
    envs[i]->sustain(0.6);
    envs[i]->release(84.5);
    // uncomment these to hear without envelope effects
    //envs[i]->attack(0.0);
    //envs[i]->hold(0.0);
    //envs[i]->decay(0.0);
    //envs[i]->release(0.0);
  }

  delay1.delay(0, 110);
  delay1.delay(1, 220);
  delay1.delay(2, 660);
  
  Serial.println("setup done");
  
  // Initialize processor and memory measurements
  AudioProcessorUsageMaxReset();
  AudioMemoryUsageMaxReset();
}

void noteOn(int num){
  int voice = num/8;
  float *scale;
  if(voice == 0 || voice == 1) scale = cmaj_low;
  else scale = cmaj_high;
  AudioNoInterrupts();
  waves[voice]->begin(.5, scale[num%8], wave_type[voice]);
  envs[voice]->noteOn();
  AudioInterrupts();
}

void noteOff(int num){
  int voice = num/8;
  envs[voice]->noteOff();
}
 
void loop() {
  customKeypad.tick();
  
  while(customKeypad.available())
  {
    keypadEvent e = customKeypad.read();
    int keyindex = e.bit.KEY - 1;
    if(e.bit.EVENT == KEY_JUST_PRESSED){
        strip.setPixelColor(keyindex, 0xFFFFFF);
        noteOn(keyindex);
      }
    else if(e.bit.EVENT == KEY_JUST_RELEASED){
        noteOff(keyindex);
        strip.setPixelColor(keyindex, 0);
      }
   }
  strip.show();
  delay(10);
}


// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}



// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

