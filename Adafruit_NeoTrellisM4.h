#ifndef _ADAFRUIT_NEOTRELLIS_M4_H_
#define _ADAFRUIT_NEOTRELLIS_M4_H_

#include <Arduino.h>
#include <Adafruit_Keypad.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_NeoPixel_ZeroDMA.h>
#include "MIDIUSB.h"

#ifndef ADAFRUIT_TRELLIS_M4_EXPRESS
#error "This library is only for the Adafruit NeoTrellis M4!!"
#endif



class Adafruit_NeoTrellisM4 : public Adafruit_Keypad, public Adafruit_NeoPixel_ZeroDMA {

 public:
  Adafruit_NeoTrellisM4();
  void begin(void);
  void tick(void);

  void autoUpdateNeoPixels(boolean flag);
  void setPixelColor(uint32_t pixel, uint32_t color);
  void fill(uint32_t color);

  uint8_t num_keys(void) {return _num_keys; }

  void enableUSBMIDI(boolean f);
  void enableUARTMIDI(boolean f);
  void setUARTMIDIchannel(uint8_t c);
  void setUSBMIDIchannel(uint8_t c);
  void noteOn(byte pitch, byte velocity);
  void noteOff(byte pitch, byte velocity);
  void pitchBend(int value);
  void controlChange(byte control, byte value);
  void sendMIDI(void);

 private:
  int _num_keys, _rows, _cols;
  boolean _pending_midi, _auto_update, _midi_usb, _midi_uart;
  int _midi_channel_usb, _midi_channel_uart;
};

#endif
