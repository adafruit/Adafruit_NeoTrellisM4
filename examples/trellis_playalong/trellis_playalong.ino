/* this is the same as the trellis synth example except it will also
 * mix in external audio from the line input pins. You can play along
 * to whatever music you plug in.
 *
 */

#include <Audio.h>
#include <Adafruit_NeoTrellisM4.h>
#include <Adafruit_ADXL343.h>

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

AudioInputAnalogStereo      audioInput(PIN_LINE_LEFT, PIN_LINE_RIGHT);
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
AudioConnection patchCord50(audioInput, 0, mixerLeft, 3);
AudioConnection patchCord37(mixer1, 0, mixerRight, 0);
AudioConnection patchCord40(delay1, 2, mixerRight, 1);
AudioConnection patchCord43(delay1, 3, mixerRight, 2);
AudioConnection patchCord51(audioInput, 1, mixerRight, 3);
AudioConnection patchCord41(mixerLeft, 0, audioOut, 0);
AudioConnection patchCord42(mixerRight, 0, audioOut, 1);

Adafruit_NeoTrellisM4 trellis = Adafruit_NeoTrellisM4();
Adafruit_ADXL343 accel = Adafruit_ADXL343(123, &Wire1);

void setup(){
  Serial.begin(115200);
  //while (!Serial);

  trellis.begin();
  trellis.setBrightness(255);  

  AudioMemory(120);

  mixerLeft.gain(0, 0.36);
  mixerLeft.gain(1, 0.36);
  mixerLeft.gain(2, 0.36);
  mixerRight.gain(0, 0.36);
  mixerRight.gain(1, 0.36);
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
  trellis.tick();
  
  while(trellis.available())
  {
    keypadEvent e = trellis.read();
    int keyindex = e.bit.KEY;
    if(e.bit.EVENT == KEY_JUST_PRESSED){
        //trellis.setPixelColor(keyindex, 0xFFFFFF); // plain white
        trellis.setPixelColor(keyindex, Wheel(keyindex * 255 / 32)); // rainbow!
        noteOn(keyindex);
      }
    else if(e.bit.EVENT == KEY_JUST_RELEASED){
        noteOff(keyindex);
        trellis.setPixelColor(keyindex, 0);
      }
   }
  delay(10);
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return Adafruit_NeoPixel::Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return Adafruit_NeoPixel::Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return Adafruit_NeoPixel::Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}