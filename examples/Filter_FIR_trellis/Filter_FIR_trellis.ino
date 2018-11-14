/*
c
  - released
b
- Use FIR filters with fast_fft option

Press key 1 on the trellis to engage the filter
Press key 2 to change the filter type
*/

//#include <arm_math.h>
#include <Audio.h>
#include "filters.h"
#include <Adafruit_NeoPixel_ZeroDMA.h>
#include "Adafruit_NeoTrellisM4.h"

#define BIN_MAX 0.1 //adjust this value to change sensitivity
#define NEO_PIN 11
#define NUM_KEYS 32

// If this key is pressed FIR filter is turned off
// which just passes the audio sraight through
#define PASSTHRU_KEY 1
// If this key is pressedthe next FIR filter in the list
// is switched in.
#define FILTER_KEY 2

AudioInputAnalogStereo  audioInput(PIN_LINE_LEFT, PIN_LINE_RIGHT);
AudioFilterFIR      myFilterL;
AudioFilterFIR      myFilterR;
AudioOutputAnalogStereo  audioOutput;
AudioAnalyzeFFT1024    myFFT;

// Create Audio connections between the components
// Route audio into the left and right filters
AudioConnection c1(audioInput, 0, myFilterL, 0);
AudioConnection c2(audioInput, 1, myFilterR, 0);
AudioConnection patchCord1(myFilterL, 0, myFFT, 0);
// Route the output of the filters to their respective channels
AudioConnection c3(myFilterL, 0, audioOutput, 0);
AudioConnection c4(myFilterR, 0, audioOutput, 1);

Adafruit_NeoPixel_ZeroDMA strip(NUM_KEYS, NEO_PIN, NEO_GRB);

extern const float _mel_8_256[8][256];

struct fir_filter {
  short *coeffs;
  short num_coeffs;    // num_coeffs must be an even number, 4 or higher
};

// index of current filter. Start with the low pass.
int fir_idx = 0;
struct fir_filter fir_list[] = {
  {low_pass , 100},    // low pass with cutoff at 1kHz and -60dB at 2kHz
  {band_pass, 100},    // bandpass 1200Hz - 1700Hz
  {NULL,      0}
};


void setup() {
  Serial.begin(9600);
  delay(300);

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  strip.setBrightness(255);

  trellisKeypad.begin();

  // allocate memory for the audio library
  AudioMemory(20);
  
  // Initialize the filter
  myFilterL.begin(fir_list[0].coeffs, fir_list[0].num_coeffs);
  myFilterR.begin(fir_list[0].coeffs, fir_list[0].num_coeffs);

  // Configure the window algorithm to use
  myFFT.windowFunction(AudioWindowHanning1024);
  
  Serial.println("setup done");
}

// Input a value 0 to 255 to get a color value.
// The colors are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  return 0;
}

float bins[8] = {0, 0, 0, 0, 0, 0, 0, 0};
float binsLast[8] = {0, 0, 0, 0, 0, 0, 0, 0};
float binsLastLast[8] = {0, 0, 0, 0, 0, 0, 0, 0};

// index of current filter when passthrough is selected
int old_idx = -1;

void loop()
{
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
    
    for (i=0; i<8; i++) {
      //moving average across the past 3 windows
      bins[i] = (binsLastLast[i] + binsLast[i] + bins[i])/3;
      n = bins[i];
      binsLastLast[i] = binsLast[i];
      binsLast[i] = bins[i];

      for(int j=3; j>=0; j--){
        int pixnum = i + 8*j;
        if(n > (BIN_MAX/4)*(4-j))
          strip.setPixelColor(pixnum, Wheel((255>>2)*(j+1)));
        else
          strip.setPixelColor(pixnum, 0);
      }
    }
    strip.show();
  }
  
  tick_trellis();
  
  while(trellisKeypad.available())
  {
    keypadEvent e = trellisKeypad.read();
    if(e.bit.KEY == PASSTHRU_KEY){
      if(e.bit.EVENT == KEY_JUST_PRESSED){
        // If the passthru button is pushed, save the current
        // filter index and then switch the filter to passthru
        old_idx = fir_idx;
        myFilterL.begin(FIR_PASSTHRU, 0);
        myFilterR.begin(FIR_PASSTHRU, 0);
      }
      else if(e.bit.EVENT == KEY_JUST_RELEASED){
        // If passthru button is released, restore previous filter
        if(old_idx != -1) {
          myFilterL.begin(fir_list[fir_idx].coeffs, fir_list[fir_idx].num_coeffs);
          myFilterR.begin(fir_list[fir_idx].coeffs, fir_list[fir_idx].num_coeffs);
        }
        old_idx = -1;
      }
    }
    else if(e.bit.KEY == FILTER_KEY){
      if(e.bit.EVENT == KEY_JUST_PRESSED){
        fir_idx++;
        if (fir_list[fir_idx].num_coeffs == 0) fir_idx = 0;
        myFilterL.begin(fir_list[fir_idx].coeffs, fir_list[fir_idx].num_coeffs);
        myFilterR.begin(fir_list[fir_idx].coeffs, fir_list[fir_idx].num_coeffs);
      }
    }
  }
  delay(10);
}


