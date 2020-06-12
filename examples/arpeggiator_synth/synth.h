#include <Audio.h>
#include <Wire.h>
//#include "presets.h"

AudioSynthWaveform sine0, sine1, sine2, sine3, sine4, sine5, sine6, sine7,
    sine8, sine9, sine10, sine11, sine12, sine13, sine14, sine15, sine16,
    sine17, sine18, sine19, sine20, sine21, sine22, sine23, sine24, sine25,
    sine26, sine27, sine28, sine29, sine30, sine31;

AudioSynthWaveform *waves[32] = {
    &sine0,  &sine1,  &sine2,  &sine3,  &sine4,  &sine5,  &sine6,  &sine7,
    &sine8,  &sine9,  &sine10, &sine11, &sine12, &sine13, &sine14, &sine15,
    &sine16, &sine17, &sine18, &sine19, &sine20, &sine21, &sine22, &sine23,
    &sine24, &sine25, &sine26, &sine27, &sine28, &sine29, &sine30, &sine31,
};

// boolean playing[32] = {true};        // Pressed state for each button

short wave_type[4] = {
    WAVEFORM_SINE,
    WAVEFORM_SQUARE,
    WAVEFORM_SAWTOOTH,
    WAVEFORM_TRIANGLE,
};

float midiMap[127]; // array mapping note numbers to frequencies

AudioEffectEnvelope env0, env1, env2, env3, env4, env5, env6, env7, env8, env9,
    env10, env11, env12, env13, env14, env15, env16, env17, env18, env19, env20,
    env21, env22, env23, env24, env25, env26, env27, env28, env29, env30, env31;
AudioEffectEnvelope *envs[32] = {
    &env0,  &env1,  &env2,  &env3,  &env4,  &env5,  &env6,  &env7,
    &env8,  &env9,  &env10, &env11, &env12, &env13, &env14, &env15,
    &env16, &env17, &env18, &env19, &env20, &env21, &env22, &env23,
    &env24, &env25, &env26, &env27, &env28, &env29, &env30, &env31,
};

AudioConnection patchCord01(sine0, env0);
AudioConnection patchCord02(sine1, env1);
AudioConnection patchCord03(sine2, env2);
AudioConnection patchCord04(sine3, env3);
AudioConnection patchCord05(sine4, env4);
AudioConnection patchCord06(sine5, env5);
AudioConnection patchCord07(sine6, env6);
AudioConnection patchCord08(sine7, env7);
AudioConnection patchCord09(sine8, env8);
AudioConnection patchCord10(sine9, env9);
AudioConnection patchCord11(sine10, env10);
AudioConnection patchCord12(sine11, env11);
AudioConnection patchCord13(sine12, env12);
AudioConnection patchCord14(sine13, env13);
AudioConnection patchCord15(sine14, env14);
AudioConnection patchCord16(sine15, env15);
AudioConnection patchCord17(sine16, env16);
AudioConnection patchCord18(sine17, env17);
AudioConnection patchCord19(sine18, env18);
AudioConnection patchCord20(sine19, env19);
AudioConnection patchCord21(sine20, env20);
AudioConnection patchCord22(sine21, env21);
AudioConnection patchCord23(sine22, env22);
AudioConnection patchCord24(sine23, env23);
AudioConnection patchCord25(sine24, env24);
AudioConnection patchCord26(sine25, env25);
AudioConnection patchCord27(sine26, env26);
AudioConnection patchCord28(sine27, env27);
AudioConnection patchCord29(sine28, env28);
AudioConnection patchCord30(sine29, env29);
AudioConnection patchCord31(sine30, env30);
AudioConnection patchCord32(sine31, env31);

AudioMixer4 mixer1;

AudioConnection patchCord33(env0, 0, mixer1, 0);
AudioConnection patchCord34(env1, 0, mixer1, 1);
AudioConnection patchCord35(env2, 0, mixer1, 2);
AudioConnection patchCord36(env3, 0, mixer1, 3);

AudioMixer4 mixer2;

AudioConnection patchCord37(env4, 0, mixer2, 0);
AudioConnection patchCord38(env5, 0, mixer2, 1);
AudioConnection patchCord39(env6, 0, mixer2, 2);
AudioConnection patchCord40(env7, 0, mixer2, 3);

AudioMixer4 mixer3;

AudioConnection patchCord41(env8, 0, mixer3, 0);
AudioConnection patchCord42(env9, 0, mixer3, 1);
AudioConnection patchCord43(env10, 0, mixer3, 2);
AudioConnection patchCord44(env11, 0, mixer3, 3);

AudioMixer4 mixer4;

AudioConnection patchCord45(env12, 0, mixer4, 0);
AudioConnection patchCord46(env13, 0, mixer4, 1);
AudioConnection patchCord47(env14, 0, mixer4, 2);
AudioConnection patchCord48(env15, 0, mixer4, 3);

AudioMixer4 mixer5;

AudioConnection patchCord49(env16, 0, mixer5, 0);
AudioConnection patchCord50(env17, 0, mixer5, 1);
AudioConnection patchCord51(env18, 0, mixer5, 2);
AudioConnection patchCord52(env19, 0, mixer5, 3);

AudioMixer4 mixer6;

AudioConnection patchCord53(env20, 0, mixer6, 0);
AudioConnection patchCord54(env21, 0, mixer6, 1);
AudioConnection patchCord55(env22, 0, mixer6, 2);
AudioConnection patchCord56(env23, 0, mixer6, 3);

AudioMixer4 mixer7;

AudioConnection patchCord57(env24, 0, mixer7, 0);
AudioConnection patchCord58(env25, 0, mixer7, 1);
AudioConnection patchCord59(env26, 0, mixer7, 2);
AudioConnection patchCord60(env27, 0, mixer7, 3);

AudioMixer4 mixer8;

AudioConnection patchCord61(env28, 0, mixer8, 0);
AudioConnection patchCord62(env29, 0, mixer8, 1);
AudioConnection patchCord63(env30, 0, mixer8, 2);
AudioConnection patchCord64(env31, 0, mixer8, 3);

AudioMixer4 mixer9;  // combines mixers 1-4
AudioMixer4 mixer10; // combines mixers 5-8
AudioMixer4 mixer11; // combines mixers 9 & 10

AudioOutputAnalogStereo audioOut;

AudioConnection patchCord65(mixer1, 0, mixer9, 0);
AudioConnection patchCord66(mixer2, 0, mixer9, 1);
AudioConnection patchCord67(mixer3, 0, mixer9, 2);
AudioConnection patchCord68(mixer4, 0, mixer9, 3);

AudioConnection patchCord69(mixer5, 0, mixer10, 0);
AudioConnection patchCord70(mixer6, 0, mixer10, 1);
AudioConnection patchCord71(mixer7, 0, mixer10, 2);
AudioConnection patchCord72(mixer8, 0, mixer10, 3);

AudioConnection patchCord73(mixer9, 0, mixer11, 0);
AudioConnection patchCord74(mixer10, 0, mixer11, 1);

AudioFilterBiquad filter;

AudioConnection patchCord75(mixer11, 0, filter, 0);

AudioConnection patchCord76(filter, 0, audioOut, 0);
AudioConnection patchCord77(filter, 0, audioOut, 1);

void audioSetup() {

  AudioMemory(120);

  // reduce the gain on some channels, so half of the channels
  // are "positioned" to the left, half to the right, but all
  // are heard at least partially on both ears
  mixer11.gain(0, 0.75);
  mixer11.gain(1, 0.75);

  filter.setLowpass(0, 1500, 0.707);

  // set envelope parameters, for pleasing sound :-)
  for (int i = 0; i < 32; i++) {
    envs[i]->attack(9.2);
    envs[i]->hold(2.1);
    envs[i]->decay(31.4);
    envs[i]->sustain(0.0);
    envs[i]->release(0.0);
  }

  // Initialize processor and memory measurements
  AudioProcessorUsageMaxReset();
  AudioMemoryUsageMaxReset();

  // generate frequency array
  int a = 440; // a is 440 hz
  for (int x = 0; x < 127; ++x) {
    midiMap[x] = (a / 32.0) * pow(2.0, ((x - 9.0) / 12.0));
  }
}

void noteOn(int note, int index) {

  // check if note is playing, prevent note retriggering
  //  bool active = playing[index];
  //  if (active) {
  //    debugLed(true);
  //    return;
  //  }
  //  else playing[index] = true;

  AudioNoInterrupts();
  waves[index]->begin(.5, midiMap[note], wave_type[wave]);
  envs[index]->noteOn();
  AudioInterrupts();
}

void freqOn(float freq, int index) {

  // check if note is playing, prevent note retriggering
  //  bool active = playing[index];
  //  if (active) {
  //    debugLed(true);
  //    return;
  //  }
  //  else playing[index] = true;

  AudioNoInterrupts();
  waves[index]->begin(.5, freq, wave_type[wave]);
  envs[index]->noteOn();
  AudioInterrupts();
}

void noteOff(int note, int index) {

  //  playing[index] = false;

  envs[index]->noteOff();
}
