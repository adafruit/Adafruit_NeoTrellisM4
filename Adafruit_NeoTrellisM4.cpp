#include "Adafruit_NeoTrellisM4.h"


static const byte NEO_PIN = 10;

static const byte ROWS = 4; // four rows
static const byte COLS = 8; // eight columns

//define the symbols on the buttons of the keypads
static byte trellisKeys[ROWS][COLS] = {
  {0, 1,  2,  3,  4,  5,  6,  7},
  {8, 9,  10, 11, 12, 13, 14, 15},
  {16, 17, 18, 19, 20, 21, 22, 23},
  {24, 25, 26, 27, 28, 29, 30, 31}
};
static byte rowPins[ROWS] = {14, 15, 16, 17}; //connect to the row pinouts of the keypad
static byte colPins[COLS] = {2, 3, 4, 5, 6, 7, 8, 9}; //connect to the column pinouts of the keypad

Adafruit_NeoTrellisM4::Adafruit_NeoTrellisM4(void) :
  Adafruit_Keypad(makeKeymap(trellisKeys), rowPins, colPins, ROWS, COLS),
  Adafruit_NeoPixel(ROWS*COLS, NEO_PIN, NEO_GRB + NEO_KHZ800)
{
  _num_keys = ROWS * COLS;
  _rows = ROWS;
  _cols = COLS;
}

void Adafruit_NeoTrellisM4::begin(void) {
  Adafruit_Keypad::begin();

  // Initialize all pixels to 'off'
  Adafruit_NeoPixel::begin();
  fill(0x0);
  Adafruit_NeoPixel::show();
  Adafruit_NeoPixel::setBrightness(255);
}

void Adafruit_NeoTrellisM4::setPixelColor(uint32_t pixel, uint32_t color) {
  Adafruit_NeoPixel::setPixelColor(pixel, color);
  Adafruit_NeoPixel::show();
}

void Adafruit_NeoTrellisM4::fill(uint32_t color) {
  for (int i=0; i<ROWS*COLS; i++) {
    Adafruit_NeoPixel::setPixelColor(i, color);
  }
  Adafruit_NeoPixel::show();
}

void Adafruit_NeoTrellisM4::tick(void)
{
  Adafruit_Keypad::tick();
  
  // look for an entire column being pressed at once and if it was, clear the whole buffer
  uint8_t rcount[] = {0, 0, 0, 0, 0, 0, 0, 0};
  for(int i=0; i<(COLS*ROWS)-1; i++){
    if (Adafruit_Keypad::justPressed(i+1, false))
      rcount[i%COLS]++;
  }
  for (int i=0; i<COLS; i++){
    if (rcount[i] >= ROWS){
      Adafruit_Keypad::clear();
      break;
    }
  }
}
