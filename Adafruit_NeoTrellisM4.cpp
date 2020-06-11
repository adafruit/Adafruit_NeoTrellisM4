/*!
 * @file Adafruit_NeoTrellisM4.cpp
 *
 * @mainpage Adafruit NeoTrellis M4 Library
 *
 * @section intro_sec Introduction
 *
 * Example sketches for the NeoTrellis M4 Board
 *
 * @section author Author
 *
 * Written by Limor Fried/Ladyada for Adafruit Industries.
 *
 * @section license License
 *
 * BSD license, all text above must be included in any redistribution
 */

#include "Adafruit_NeoTrellisM4.h"

static const byte NEO_PIN = 10;

static const byte ROWS = 4; // four rows
static const byte COLS = 8; // eight columns

// define the symbols on the buttons of the keypads
static byte trellisKeys[ROWS][COLS] = {{0, 1, 2, 3, 4, 5, 6, 7},
                                       {8, 9, 10, 11, 12, 13, 14, 15},
                                       {16, 17, 18, 19, 20, 21, 22, 23},
                                       {24, 25, 26, 27, 28, 29, 30, 31}};
static byte rowPins[ROWS] = {14, 15, 16,
                             17}; // connect to the row pinouts of the keypad
static byte colPins[COLS] = {
    2, 3, 4, 5, 6, 7, 8, 9}; // connect to the column pinouts of the keypad

Adafruit_NeoTrellisM4::Adafruit_NeoTrellisM4(void)
    : Adafruit_Keypad(makeKeymap(trellisKeys), rowPins, colPins, ROWS, COLS),
      Adafruit_NeoPixel_ZeroDMA(ROWS * COLS, NEO_PIN, NEO_GRB) {
  _num_keys = ROWS * COLS;
  _rows = ROWS;
  _cols = COLS;
  _midi_usb = false;
  _midi_channel_usb = 0;
  _midi_uart = false;
  _midi_channel_uart = 0;
  _pending_midi = false;
  _auto_update = true;
}

/**************************************************************************/
/*!
    @brief  Initialize the NeoTrellis, start the keypad scanner and turn all
neopixels off.
x*/
/**************************************************************************/

void Adafruit_NeoTrellisM4::begin(void) {
  Adafruit_Keypad::begin();

  // Initialize all pixels to 'off'
  Adafruit_NeoPixel_ZeroDMA::begin();
  fill(0x0);
  Adafruit_NeoPixel_ZeroDMA::show();
  Adafruit_NeoPixel_ZeroDMA::setBrightness(255);
}

/**************************************************************************/
/*!
    @brief  Set one neopixel with a 24-bit color (RGB in 888 format)
    @param  pixel The pixel index from 0 (top left) to 31 (bottom right)
    @param  color The RGB888 color
x*/
/**************************************************************************/
void Adafruit_NeoTrellisM4::setPixelColor(uint32_t pixel, uint32_t color) {
  Adafruit_NeoPixel_ZeroDMA::setPixelColor(pixel, color);
  if (_auto_update) {
    Adafruit_NeoPixel_ZeroDMA::show();
  }
}

/**************************************************************************/
/*!
    @brief  Whether we should automatically update the neopixels whenever we
set/fill the colors. Auto-update is easier to use, but is slower.
    @param  flag true if we should auto update (show())
x*/
/**************************************************************************/
void Adafruit_NeoTrellisM4::autoUpdateNeoPixels(boolean flag) {
  _auto_update = flag;
}

/**************************************************************************/
/*!
    @brief  Fill the neopixels with a 24-bit color (RGB in 888 format)
    @param  color The RGB888 color
x*/
/**************************************************************************/
void Adafruit_NeoTrellisM4::fill(uint32_t color) {
  for (int i = 0; i < ROWS * COLS; i++) {
    Adafruit_NeoPixel_ZeroDMA::setPixelColor(i, color);
  }
  if (_auto_update) {
    Adafruit_NeoPixel_ZeroDMA::show();
  }
}

/**************************************************************************/
/*!
    @brief  Check the keypads to determine whather they were just
pressed/released.
x*/
/**************************************************************************/
void Adafruit_NeoTrellisM4::tick(void) {
  Adafruit_Keypad::tick();
  // look for an entire column being pressed at once and if it was, clear the
  // whole buffer
  uint8_t rcount[] = {0, 0, 0, 0, 0, 0, 0, 0};
  for (int i = 0; i < (COLS * ROWS) - 1; i++) {
    if (Adafruit_Keypad::justPressed(i + 1, false))
      rcount[i % COLS]++;
  }
  for (int i = 0; i < COLS; i++) {
    if (rcount[i] >= ROWS) {
      Adafruit_Keypad::clear();
      break;
    }
  }
}

/**************************************************************************/
/*!
    @brief  Whether to send MIDI messages over the USB port
    @param  flag true for enable, false to disable
x*/
/**************************************************************************/
void Adafruit_NeoTrellisM4::enableUSBMIDI(boolean flag) { _midi_usb = flag; }

/**************************************************************************/
/*!
    @brief  Whether to send MIDI messages over the UART port
    @param  flag true for enable, false to disable
x*/
/**************************************************************************/
void Adafruit_NeoTrellisM4::enableUARTMIDI(boolean flag) {
  _midi_uart = flag;
  if (_midi_uart) {
    Serial1.begin(31250);
  } else {
    Serial1.end();
  }
}

/**************************************************************************/
/*!
    @brief  Configure the MIDI channel to use when using USB
    @param  c Channel number, from 0-15
x*/
/**************************************************************************/
void Adafruit_NeoTrellisM4::setUSBMIDIchannel(uint8_t c) {
  _midi_channel_usb = min(15, c); // channel can only be between 0-15;
}

/**************************************************************************/
/*!
    @brief  Configure the MIDI channel to use when using UART
    @param  c Channel number, from 0-15
x*/
/**************************************************************************/
void Adafruit_NeoTrellisM4::setUARTMIDIchannel(uint8_t c) {
  _midi_channel_uart = min(15, c); // channel can only be between 0-15;
}

/**************************************************************************/
/*!
    @brief  Send MIDI note on
    @param  pitch 7-bit note pitch value
    @param  velocity 7-bit note velocity
*/
/**************************************************************************/
void Adafruit_NeoTrellisM4::noteOn(byte pitch, byte velocity) {
  pitch = min(pitch, 0x7F);
  velocity = min(velocity, 0x7F);

  if (_midi_usb) {
    midiEventPacket_t noteOn = {0x09, 0x90 | _midi_channel_usb, pitch,
                                velocity};
    MidiUSB.sendMIDI(noteOn);
    _pending_midi = true;
  }
  if (_midi_uart) {
    Serial1.write(0x90 | _midi_channel_uart);
    Serial1.write(pitch);
    Serial1.write(velocity);
  }
}

/**************************************************************************/
/*!
    @brief  Send MIDI note off
    @param  pitch 7-bit note pitch value
    @param  velocity 7-bit note velocity
*/
/**************************************************************************/
void Adafruit_NeoTrellisM4::noteOff(byte pitch, byte velocity) {
  pitch = min(pitch, 0x7F);
  velocity = min(velocity, 0x7F);

  if (_midi_usb) {
    midiEventPacket_t noteOff = {0x08, 0x80 | _midi_channel_usb, pitch,
                                 velocity};
    MidiUSB.sendMIDI(noteOff);
    _pending_midi = true;
  }
  if (_midi_uart) {
    Serial1.write(0x80 | _midi_channel_uart);
    Serial1.write(pitch);
    Serial1.write(velocity);
  }
}

/**************************************************************************/
/*!
    @brief  Send pitch bend MIDI message
    @param  value 14-bit pitchbend value, from -8191 to 8192
*/
/**************************************************************************/
void Adafruit_NeoTrellisM4::pitchBend(int value) {
  byte lowValue = min(value & 0x7F, 127);
  byte highValue = min(value >> 7, 127);

  if (_midi_usb) {
    midiEventPacket_t pitchBend = {0x0E, 0xE0 | _midi_channel_usb, lowValue,
                                   highValue};
    MidiUSB.sendMIDI(pitchBend);
    _pending_midi = true;
  }
  if (_midi_uart) {
    Serial1.write(0xE0 | _midi_channel_uart);
    Serial1.write(lowValue);
    Serial1.write(highValue);
  }
}

/**************************************************************************/
/*!
    @brief  Send control change MIDI message
    @param    control 7-bit control name
    @param    value 7-bit control value
*/
/**************************************************************************/
void Adafruit_NeoTrellisM4::controlChange(byte control, byte value) {
  control = min(0x7F, control);
  value = min(0x7F, value);

  if (_midi_usb) {
    midiEventPacket_t event = {0x0B, 0xB0 | _midi_channel_usb, control, value};
    MidiUSB.sendMIDI(event);
    _pending_midi = true;
  }
  if (_midi_uart) {
    Serial1.write(0xB0 | _midi_channel_uart);
    Serial1.write(control);
    Serial1.write(value);
  }
}

/**************************************************************************/
/*!
    @brief  Send program change MIDI message
    @param    channel Ranges from 0-15
    @param    program 7-bit program value
*/
/**************************************************************************/
void Adafruit_NeoTrellisM4::programChange(byte channel, byte program) {
  midiEventPacket_t pc = {0x0C, 0xC0 | channel, program, 0};
  MidiUSB.sendMIDI(pc);
}

/**************************************************************************/
/*!
    @brief  Flush any pending MIDI messages for sending
*/
/**************************************************************************/
void Adafruit_NeoTrellisM4::sendMIDI(void) {
  if (_midi_usb && _pending_midi) {
    MidiUSB.flush();
    _pending_midi = false;
  }
}
