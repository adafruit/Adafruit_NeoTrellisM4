#include <Adafruit_Keypad.h>
#include <Adafruit_NeoPixel.h>

#include "MIDIUSB.h"
#define MIDI_CHANNEL  0  // default channel # is 0
#define FIRST_MIDI_NOTE 24

#define NEO_PIN 10
#define NUM_KEYS 32
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_KEYS, NEO_PIN, NEO_GRB + NEO_KHZ800);

const byte ROWS = 4; // four rows
const byte COLS = 8; // eight columns
//define the symbols on the buttons of the keypads
byte trellisKeys[ROWS][COLS] = {
  {1,  2,  3,  4,  5,  6,  7,  8},
  {9,  10, 11, 12, 13, 14, 15, 16},
  {17, 18, 19, 20, 21, 22, 23, 24},
  {25, 26, 27, 28, 29, 30, 31, 32}
};
byte rowPins[ROWS] = {14, 15, 16, 17}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {2, 3, 4, 5, 6, 7, 8, 9}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Adafruit_Keypad customKeypad = Adafruit_Keypad( makeKeymap(trellisKeys), rowPins, colPins, ROWS, COLS); 

void setup(){
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  strip.setBrightness(80);
  customKeypad.begin();
  
  Serial.begin(9600);
  Serial.println("MIDI test!");

}
  
void loop() {
  // put your main code here, to run repeatedly:
  customKeypad.tick();

  boolean changed = false; // did any keys get pressed?
  while (customKeypad.available()){
    keypadEvent e = customKeypad.read();
    int key_name = (int)e.bit.KEY;
    Serial.print(key_name);
    
    if (e.bit.EVENT == KEY_JUST_PRESSED) {
      Serial.println(" pressed");
      strip.setPixelColor(key_name-1, 0xFFFFFF);
      noteOn(MIDI_CHANNEL, FIRST_MIDI_NOTE+key_name-1, 64);
      changed = true;
    }
    else if (e.bit.EVENT == KEY_JUST_RELEASED) {
      Serial.println(" released");
      strip.setPixelColor(key_name-1, 0x0);
      noteOff(MIDI_CHANNEL, FIRST_MIDI_NOTE+key_name-1, 64);
      changed = true;
    }
  }

  if (changed) {
    strip.show();  // update LEDs
    MidiUSB.flush(); // and send all MIDI messages
  }

  delay(10);
}

// First parameter is the event type (0x09 = note on, 0x08 = note off).
// Second parameter is note-on/note-off, combined with the channel.
// Channel can be anything between 0-15. Typically reported to the user as 1-16.
// Third parameter is the note number (48 = middle C).
// Fourth parameter is the velocity (64 = normal, 127 = fastest).

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
