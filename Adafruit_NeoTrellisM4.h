#ifndef _ADAFRUIT_NEOTRELLIS_M4_H_
#define _ADAFRUIT_NEOTRELLIS_M4_H_

#include "Arduino.h"
#include <Adafruit_Keypad.h>

#ifndef ADAFRUIT_TRELLIS_M4_EXPRESS
#error "This library is only for the Adafruit NeoTrellis M4!!"
#endif

const byte ROWS = 4; // four rows
const byte COLS = 8; // eight columns

extern byte trellisKeys[ROWS][COLS];
extern byte rowPins[ROWS];
extern byte colPins[COLS];

extern Adafruit_Keypad trellisKeypad;

extern void tick_trellis();

#endif
