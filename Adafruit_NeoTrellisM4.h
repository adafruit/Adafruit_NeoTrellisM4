#ifndef _ADAFRUIT_NEOTRELLIS_M4_H_
#define _ADAFRUIT_NEOTRELLIS_M4_H_

#include <Arduino.h>
#include <Adafruit_Keypad.h>
#include <Adafruit_NeoPixel.h>

#ifndef ADAFRUIT_TRELLIS_M4_EXPRESS
#error "This library is only for the Adafruit NeoTrellis M4!!"
#endif



class Adafruit_NeoTrellisM4 : public Adafruit_Keypad, public Adafruit_NeoPixel {

 public:
  Adafruit_NeoTrellisM4();
  void begin(void);
  void tick(void);
  void setPixelColor(uint32_t pixel, uint32_t color);
  void fill(uint32_t color);

  uint8_t num_keys(void) {return _num_keys; }

 private:
  int _num_keys, _rows, _cols;
};

#endif
