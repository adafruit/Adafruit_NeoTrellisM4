#include <Audio.h>

#include "sampler.h"
#include "controls.h"
#include "filters.h"
#include "sequencer.h"
#include "recorder.h"

Sampler sampler;
Controls controls;
Sequencer sequencer;
Recorder recorder;

AudioOutputAnalogStereo  audioOut;

AudioFilterFIR      myFilterL;
AudioFilterFIR      myFilterR;

AudioEffectBitcrusher   bitCrushL;
AudioEffectBitcrusher   bitCrushR;

// overall volume mixer
AudioMixer4        volOutL;
AudioMixer4        volOutR;

// Create Audio connections between the components
AudioConnection cSampFiltL(Sampler::mix5, 0, myFilterL, 0);
AudioConnection cSampFiltR(Sampler::mix5, 0, myFilterR, 0);

AudioConnection cBitcrushInputL(myFilterL, 0, bitCrushL, 0);
AudioConnection cBitcrushInputR(myFilterR, 0, bitCrushR, 0);

AudioConnection cVolOutL(bitCrushL, 0, volOutL, 0);
AudioConnection cVolOutR(bitCrushR, 0, volOutR, 0);

AudioConnection cOutL(volOutL, 0, audioOut, 0);
AudioConnection cOutR(volOutR, 0, audioOut, 1);

// index of current filter. Start with the low pass.
struct fir_filter fir_list[] = {
  {low_pass , 100},    // low pass with cutoff at 1kHz and -60dB at 2kHz
  {band_pass, 100},    // bandpass 1200Hz - 1700Hz
  {NULL,      0}
};
int filterActive = -1;

void setup() {
  Serial.begin(115200);
  //while (!Serial);
  delay(100);
  
  sampler.begin();
  controls.begin();
  sequencer.begin();
  recorder.begin();

  myFilterL.begin(FIR_PASSTHRU, 0);
  myFilterR.begin(FIR_PASSTHRU, 0);

  bitCrushL.bits(16); //set the crusher to defaults. This will passthrough clean at 16,44100
  bitCrushL.sampleRate(44100); //set the crusher to defaults. This will passthrough clean at 16,44100
  bitCrushR.bits(16); //set the crusher to defaults. This will passthrough clean at 16,44100
  bitCrushR.sampleRate(44100);

  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(120);

}

void loop() {
  controls.run();
  delay(10);
}
