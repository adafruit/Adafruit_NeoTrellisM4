/* This example will calculate an FFT of incoming audio through the
 * trellis M4 audio input circuit on the STEMMA port or the Micrpohone:
 *
 *  https://learn.adafruit.com/trellis-m4-audio-visualizer-and-filter/audio-input-circuit
 *
 *  Displays a visualizer on the trellis pixels.
 *  The audio will be fed through to the headphone output.
 *
 * Configurable:
 * - Can include a red peak dot (or line).
 * - Can have serial FFT output enabled/disabled.
 * - Can be configured to show partial cells by varying intensity.
 * - Colors can be changed by changing the color table. 
 */

#include <Audio.h>
#include "Adafruit_NeoTrellisM4.h"

#define BIN_MAX 0.06 // Adjust this value to change sensitivity
                     // 0.05 to 0.07 works well with line-level audio (eg from a mixer)
                     // 0.1 will be good with louder sources or on the Mic port
#define NEO_PIN 10

#define COLUMNS 8   // You must provide an updated Mel table (mel_8_256.c) if you change this.
#define ROWS    4

// Peak modes
#define PEAK_MODE_NONE 0
#define PEAK_MODE_LINE 1
#define PEAK_MODE_DOT 2


// Choose your peak mode here
#define PEAK_MODE PEAK_MODE_DOT
// Whether to vary the brightness for partial values at the top of a bar
#define VARY_BAR_TOP_BRIGHTNESS 1
// Change to 0 to disbale printing the FFT to serial, 1 to enable.
#define PRINT_FFT 0
// Pause between loops.  Good values seem to be between 5 and 15 ms.  Low values may appear to flicker too much)
#define DELAY 5

// Choose an audio input
//AudioInputAnalogStereo  audioInput(PIN_MIC, 0);
AudioInputAnalogStereo  audioInput(PIN_LINE_LEFT, PIN_LINE_RIGHT);


// Create the Audio components.  These should be created in the
// order data flows, inputs/sources -> processing -> outputs

AudioAnalyzeFFT1024     myFFT;
AudioOutputAnalogStereo audioOutput;

// Connect either the live input or synthesized sine wave
AudioConnection patchCord1(audioInput, 0, myFFT, 0);
AudioConnection patchCord2(audioInput, 0, audioOutput, 0);
AudioConnection patchCord3(audioInput, 0, audioOutput, 1);

// The NeoTrellisM4 object is a keypad and neopixel strip subclass
// that does things like auto-update the NeoPixels and stuff!
Adafruit_NeoTrellisM4 trellis = Adafruit_NeoTrellisM4();

// Mel scale bands
// https://en.wikipedia.org/wiki/Mel_scale
extern const float _mel_8_256[8][256];

#if (PRINT_FFT == 1)
  #define DO_PRINT(x) Serial.print(x)
  #define DO_PRINTLN() Serial.println()
#else
  #define DO_PRINT(x)
  #define DO_PRINTLN()
#endif

void setup() {
  Serial.begin(115200);

  trellis.begin();
  trellis.show(); // Initialize all pixels to 'off'
  trellis.setBrightness(128);
  
  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(12);

  // Configure the window algorithm to use
  myFFT.windowFunction(AudioWindowHanning1024);
}

// Color Table
uint32_t colors[4][3] = { 
  {255, 69, 0},   // Orange
  {255, 215, 0},  // Gold/Yellow
  {0, 191, 255},  // Teal
  {128, 170, 160},    // White (adjusted for slight purple tint)
};
uint32_t peak_color = trellis.Color(255, 0, 0);

float bins[8] = {0, 0, 0, 0, 0, 0, 0, 0};
float binsLast[8] = {0, 0, 0, 0, 0, 0, 0, 0};
float binsLastLast[8] = {0, 0, 0, 0, 0, 0, 0, 0};

#define BIN_SIZE (BIN_MAX/4)
#define TOP_ROW (ROWS-1)
#define PEAK (BIN_MAX + BIN_SIZE)

void loop() {
  float n;
  int i;
  
  if (myFFT.available()) {
    for(i=0; i<COLUMNS; i++) bins[i] = 0;

    // bin the first 256 values into 8 bins
    // exclude the first 2 with the DC offset and below-audible freqs
    for (i=2; i<256+2; i++) {
      n = myFFT.read(i);
      // dot product with a matrix of mel bands
      for(int j=0; j<COLUMNS; j++){
        bins[j] += _mel_8_256[j][i-2] * n;
      }
    }
    
    // each time new FFT data is available
    DO_PRINT("FFT: ");
    //moving average across the past 3 windows
    for (i=0; i<8; i++) { // Column
      bins[i] = (binsLastLast[i] + binsLast[i] + bins[i])/3;
      n = bins[i];
      binsLastLast[i] = binsLast[i];
      binsLast[i] = bins[i];
  
      for(int j=TOP_ROW; j>=0; j--) { // Row
        int pixnum = i + (COLUMNS * j);
  
        float cell_start = BIN_SIZE * (4-j);
        float cell_end = cell_start + BIN_SIZE;
        #if (PEAK_MODE != PEAK_MODE_NONE) 
          if (n > PEAK) {
            #if (PEAK_MODE == PEAK_MODE_DOT)
              if (j == 0)
            #endif
            trellis.setPixelColor(pixnum, peak_color);
          } else
        #endif
        #if (VARY_BAR_TOP_BRIGHTNESS)
          if (n > cell_start && n < cell_end) {
            float percent = (n-cell_start)/BIN_SIZE;
            trellis.setPixelColor(pixnum, trellis.Color(colors[j][0]*percent, colors[j][1]*percent, colors[j][2]*percent));
          }
          else 
        #endif
        if (n > cell_start) {
          trellis.setPixelColor(pixnum, trellis.Color(colors[j][0], colors[j][1], colors[j][2]));
        }
        else {
          trellis.setPixelColor(pixnum, 0);
        }
  
        if (n >= 0.01) {
          DO_PRINT(n);
          DO_PRINT(" ");
        } else {
          DO_PRINT("  -  "); // don't print "0.00"
        }
      }
    }
    DO_PRINTLN();
  }
  trellis.show();
  delay(DELAY);
}
