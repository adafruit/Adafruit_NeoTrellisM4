// Talk into the mic and hear it through your headphones or play through the line inputs.

#include <Audio.h>

//uncomment one of these:

#define USE_MIC
//#define USE_LINE_IN

#if defined(USE_MIC)
AudioInputAnalogStereo  audioIn(PIN_MIC, 0);
#elif defined(USE_LINE_IN)
AudioInputAnalogStereo  audioIn(PIN_LINE_LEFT, PIN_LINE_RIGHT);
#endif
AudioOutputAnalogStereo  audioOut;

#if defined(USE_MIC)
AudioConnection patchCord41(audioIn, 0, audioOut, 0);
AudioConnection patchCord42(audioIn, 0, audioOut, 1);
#elif defined(USE_LINE_IN)
AudioConnection patchCord41(audioIn, 0, audioOut, 0);
AudioConnection patchCord42(audioIn, 1, audioOut, 1);
#endif

void setup() {
  AudioMemory(12);
}

void loop() {

}
