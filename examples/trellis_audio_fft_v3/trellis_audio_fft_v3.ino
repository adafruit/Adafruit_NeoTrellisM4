/* This example will calculate an FFT of incoming audio through the microphone
 *  input and display a visualizer on the trellis pixels.
 *  The FFT bins will also be printed to the serial console and
 *  the microphone audio will be fed through to the headphone output.
 */

#include <Audio.h>
#include "Adafruit_NeoTrellisM4.h"

#define BIN_MAX 0.06 //adjust this value to change sensitivity
#define NEO_PIN 10

#define MAX_BRIGHTNESS 128

// Create the Audio components.  These should be created in the
// order data flows, inputs/sources -> processing -> outputs
//
AudioInputAnalogStereo  audioInput(PIN_LINE_LEFT, PIN_LINE_RIGHT);
AudioOutputAnalogStereo      audioOutput;
AudioAnalyzeFFT1024    myFFT;

// Connect either the live input or synthesized sine wave
AudioConnection patchCord1(audioInput, 0, myFFT, 0);
AudioConnection patchCord2(audioInput, 0, audioOutput, 0);
AudioConnection patchCord3(audioInput, 0, audioOutput, 1);

// The NeoTrellisM4 object is a keypad and neopixel strip subclass
// that does things like auto-update the NeoPixels and stuff!
Adafruit_NeoTrellisM4 trellis = Adafruit_NeoTrellisM4();

extern const float _mel_8_256[8][256];

void setup() {
  Serial.begin(115200);

  trellis.begin();
  trellis.show(); // Initialize all pixels to 'off'
  trellis.setBrightness(MAX_BRIGHTNESS);
  
  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(12);

  // Configure the window algorithm to use
  myFFT.windowFunction(AudioWindowHanning1024);
}

uint32_t colors[4][3] = { 
                          {255, 69, 0},
                          {255, 215, 0},
                          {0, 191, 255},
                          {0, 0, 255},
};

float bins[8] = {0, 0, 0, 0, 0, 0, 0, 0};
float binsLast[8] = {0, 0, 0, 0, 0, 0, 0, 0};
float binsLastLast[8] = {0, 0, 0, 0, 0, 0, 0, 0};

void loop() {
  float n;
  int i;
  
  if (myFFT.available()) {
    for(i=0; i<8; i++) bins[i] = 0;

    // bin the first 256 values into 8 bins
    // exclude the first 2 with the DC offset and below-audible freqs
    for(i=2; i<256+2; i++){
      n = myFFT.read(i);
      // dot product with a matrix of mel bands
      for(int j=0; j<8; j++){
        bins[j] += _mel_8_256[j][i-2] * n;
      }
    }
    
    // each time new FFT data is available
    // print it all to the Arduino Serial Monitor
    for (i=0; i<8; i++) { // Column
      //moving average across the past 3 windows
      bins[i] = (binsLastLast[i] + binsLast[i] + bins[i])/3;
      n = bins[i];
      binsLastLast[i] = binsLast[i];
      binsLast[i] = bins[i];

      for(int j=3; j>=0; j--){ // Row
        int pixnum = i + 8*j;

        float cell_start = (BIN_MAX/4)*(4-j);
        float cell_end = cell_start+(BIN_MAX/4);
        if (j == 0 && n > cell_end) {
          trellis.setPixelColor(pixnum, trellis.Color(255, 0, 0));
        } else if (n > cell_start && n < cell_end ) {
          float percent = (n-cell_start)/(BIN_MAX/4);
          trellis.setPixelColor(pixnum, trellis.Color(colors[j][0]*percent, colors[j][1]*percent, colors[j][2]*percent));
        }
        else if (n > cell_start) {
          trellis.setPixelColor(pixnum, trellis.Color(colors[j][0], colors[j][1], colors[j][2]));
        }
        else {
          trellis.setPixelColor(pixnum, 0);
        }
      }
    }
    trellis.show();
  }
  delay(0.1);
}
