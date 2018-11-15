/* Arpeggiator Synth for Adafruit Neotrellis M4
 *  by Collin Cunningham for Adafruit Industries, inspired by Stretta's Polygome
 *  https://www.adafruit.com/product/3938
 * 
 *  Change color, scale, pattern, bpm, and waveform variables in settings.h file!
 * 
 */

#include <Adafruit_Keypad.h>
#include <Adafruit_NeoPixel.h>
#include "settings.h"

#define CHANNEL   1  // MIDI channel number
#define EXT_CLOCK 0  // 0 for internal clock, 1 for external
#define WIDTH      8
#define HEIGHT     4

#define N_BUTTONS  WIDTH*HEIGHT
#define ARP_NOTE_COUNT  6
#define NULL_INDEX 255
#define NEO_PIN 10
#define NUM_KEYS 32

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_KEYS, NEO_PIN, NEO_GRB + NEO_KHZ800);

const byte ROWS = HEIGHT; // four rows
const byte COLS = WIDTH; // eight columns
byte trellisKeys[ROWS][COLS] = {  //define the symbols on the buttons of the keypads
  {1,  2,  3,  4,  5,  6,  7,  8},
  {9,  10, 11, 12, 13, 14, 15, 16},
  {17, 18, 19, 20, 21, 22, 23, 24},
  {25, 26, 27, 28, 29, 30, 31, 32}
};
byte rowPins[ROWS] = {14, 15, 16, 17}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {2, 3, 4, 5, 6, 7, 8, 9}; //connect to the column pinouts of the keypad
Adafruit_Keypad customKeypad = Adafruit_Keypad( makeKeymap(trellisKeys), rowPins, colPins, ROWS, COLS); //initialize keypad

unsigned long prevReadTime = 0L; // Keypad polling timer
uint8_t       quantDiv = 8;      // Quantization division, 2 = half note
uint8_t       clockPulse = 0;

//#define QUANT_PULSE (96/quantDiv)// Number of pulses per quantization division

boolean pressed[N_BUTTONS] = {false};        // Pressed state for each button

uint8_t pitchMap[N_BUTTONS];

uint8_t arpSeqIndex[N_BUTTONS] = {NULL_INDEX};   // Current place in button arpeggio sequence
uint8_t arpButtonIndex[N_BUTTONS] = {NULL_INDEX};   // Button index being played for each actual pressed button

unsigned long beatInterval = 60000L / BPM; // ms/beat - should be merged w bpm in a function!
unsigned long prevArpTime  = 0L;

void setup(){
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  strip.setBrightness(255);
  customKeypad.begin();
  
//  Serial.begin(9600);
//  while (!Serial){}
//  Serial.println("Arp Synth ...");

  //Set up the notes for grid
  writePitchMap();

  audioSetup(); //comment out this line for serial debugging

  for (uint8_t i=0; i<N_BUTTONS; i++) {
    strip.setPixelColor(i, offColor);
  }
  strip.show();
}

  
void loop() {

  customKeypad.tick();

  unsigned long t = millis();
  unsigned long tDiff = t - prevReadTime;

  while (customKeypad.available()){
    keypadEvent e = customKeypad.read();
    uint8_t i = e.bit.KEY - 1;
    if (e.bit.EVENT == KEY_JUST_PRESSED) {
      pressed[i] = true;     
    }
    else if (e.bit.EVENT == KEY_JUST_RELEASED) {
        pressed[i] = false;
        stopArp(i);
      }
  }

  // INTERNAL CLOCK
  if ((t - prevArpTime) >= beatInterval) {
    respondToPresses();
    prevArpTime = t;
  }

  strip.show();

  prevReadTime = t;

}


void writePitchMap() {

  for (int i = 0; i < N_BUTTONS; i++) {
    int octMod = i/8 + OCTAVE;
    pitchMap[i] = scale[i%8] + (octMod*12);
  }
  
}

void respondToPresses() {

  for (uint8_t i = 0; i < N_BUTTONS; i++) {
    if (pressed[i]) {
      playArp(i);
    }
  }
}


void playArp(uint8_t buttonIndex) {

  uint8_t seqIndex, seqButtonIndex, seqNote, x, y;

  //if not starting arp, then increment index
  if (arpSeqIndex[buttonIndex] == NULL_INDEX) seqIndex = 0;
  else seqIndex = arpSeqIndex[buttonIndex] + 1;

  // Loop sequence
  if (seqIndex >= ARP_NOTE_COUNT) {
    seqIndex = 0;
  }

  // Find current button coordinates
  y = buttonIndex / WIDTH;
  x = buttonIndex - (y * WIDTH);

  // Add note offsets
  x = x + arp[seqIndex][0];
  y = y + arp[seqIndex][1];

  // Wrap notes to grid
  if (x >= WIDTH)  x %= WIDTH;
  if (y >= HEIGHT) y %= HEIGHT;

  // Find new note and index
  seqNote = findNoteFromXY(x, y);
  seqButtonIndex = indexFromXY(x, y);

  // Stop prev note in sequence
  stopNoteForButton(arpButtonIndex[buttonIndex]);

  // Store new note
  arpSeqIndex[buttonIndex] = seqIndex;
  arpButtonIndex[buttonIndex] = seqButtonIndex;

  // Play new note
  playNoteForButton(seqButtonIndex);

}


void stopArp(uint8_t button) {

  //stop playing the note
  stopNoteForButton(arpButtonIndex[button]);

  //store an invalid button index in its place
  arpSeqIndex[button] = NULL_INDEX;  //check for invalid

}

uint8_t indexFromXY(uint8_t x, uint8_t y) {

  return (y * WIDTH + x);

}


uint8_t findNoteFromXY(uint8_t x, uint8_t y) {

  return pitchMap[y * WIDTH + x];

}

uint8_t findNoteFromIndex(uint8_t buttonIndex) {

  uint8_t x, y;
  y = buttonIndex / WIDTH;
  x = buttonIndex - (y * WIDTH);

  return findNoteFromXY(x, y);

}


void playNoteForButton(uint8_t buttonIndex) {

//MIDI stub
//  midiEventPacket_t noteOn = {0x09, 0x90 | CHANNEL, findNoteFromIndex(buttonIndex), 100};
//  MidiUSB.sendMIDI(noteOn);

  noteOn(findNoteFromIndex(buttonIndex), buttonIndex);

  strip.setPixelColor(buttonIndex, onColor);

}


void stopNoteForButton(uint8_t buttonIndex) {

//MIDI stub
//  midiEventPacket_t noteOff = {0x08, 0x80 | CHANNEL, findNoteFromIndex(buttonIndex), 0};
//  MidiUSB.sendMIDI(noteOff);
  
  noteOff(findNoteFromIndex(buttonIndex), buttonIndex);
  
  strip.setPixelColor(buttonIndex, offColor);

}

void debugLed(bool light){

  if (light) strip.setPixelColor(0, blue);
  else strip.setPixelColor(0, 0);
  strip.show();
}

