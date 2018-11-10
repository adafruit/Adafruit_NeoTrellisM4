/* MIDI synth example specifically for pixelsynth.com
 *  First three rows are in 'pairs' for pulsewidth thru threshold CC messages
 *  The left button decreases CC value, right button increases
 *  Last row has RGB background and foreground adjust. Press red, green blue buttons
 *  to increase that color element, will wrap around to 0 once it gets to max.
 *  Second to last button doesnt do anything. Last button is random
 */
#include <Adafruit_Keypad.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL343.h>

#include "MIDIUSB.h"
#define MIDI_CHANNEL     0  // default channel # is 0

#define NEO_PIN 10
#define NUM_KEYS 32
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_KEYS, NEO_PIN, NEO_GRB + NEO_KHZ800);

Adafruit_ADXL343 accel = Adafruit_ADXL343(123, &Wire1);

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

#define CC_NUM 16
#define KEYS_CC_START 7 
uint8_t cc_curr_value[CC_NUM] = {0};
uint32_t button_colors[32] = {0};


void setup(){
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  strip.setBrightness(80);
  customKeypad.begin();

  Serial.begin(9600);
  //while (!Serial);
  Serial.println("CC MIDI keypad");
  
  if(!accel.begin()) {
    Serial.println("No accelerometer found");
    while(1);
  }

  // set to center
  for (int i=0; i<CC_NUM; i++) {
    cc_curr_value[i] = 8192;
    send_cc(MIDI_CHANNEL, i, cc_curr_value[i]);
    MidiUSB.flush(); // and send all MIDI messages
  }
  // first three rows are control messages
  for (int i=0; i<24; i++) {
    button_colors[i] = Wheel((i/2)*256 / 12);
  }
  // first three of last row are RGB foreground
  button_colors[24] = 0xFF0000;
  button_colors[25] = 0x00FF00;
  button_colors[26] = 0x0000FF;
  // next three of last row are RGB background
  button_colors[27] = 0xFF0000;
  button_colors[28] = 0x00FF00;
  button_colors[29] = 0x0000FF;
  // next two are off and random
  button_colors[30] = 0x000000;
  button_colors[31] = 0xFFFFFF;
}
  
void loop() {
  // put your main code here, to run repeatedly:
  customKeypad.tick();
  
  for (int key_name=1; key_name<=32; key_name++) {
   // Serial.println(key_name);
    int key = key_name-1;  // go from starting at 1 to 0

    // Key is not pressed, revert color and continue
    if (!customKeypad.isPressed(key_name)) {
      //Serial.print(key_name); Serial.println(" is NOT pressed");
      // Revert to the default color if not pressed
      strip.setPixelColor(key, button_colors[key]);
      continue;
    }
    Serial.print(key); Serial.println(" is pressed");
    
    strip.setPixelColor(key, 0xFFFFFF);
    if (key < 24) {
      // first 3 rows are basic CC values
      int cc_idx = (key) / 2;
      bool inc = (key) % 2;
      int val = cc_curr_value[cc_idx];
      if (inc) {
        val = min(127, val+1);  // no larger than 127!
      } else {
        val = min(127, val-1);  // no smaller than 0!
      }
      cc_curr_value[cc_idx] = val;
      send_cc(MIDI_CHANNEL, cc_idx + KEYS_CC_START, cc_curr_value[cc_idx]);
    }
    // Cycle foreground colors
    if (key >= 24 && key <=29 ) {
      int cc_idx = 12 + (key-24);
      int val = cc_curr_value[cc_idx];
      val++;
      if (val > 127) val = 0;  // wrap around
      cc_curr_value[cc_idx] = val;
      // RGB messages start with CC #1 thru #6
      send_cc(MIDI_CHANNEL, key-23, cc_curr_value[cc_idx]);
    }
    if (key == 31) {
      send_cc(MIDI_CHANNEL, 19, 1); // randomize on channel 19
    }
  }

  // Check for accelerometer
  sensors_event_t event; 
  accel.getEvent(&event);
  /* Display the results (acceleration is measured in m/s^2) */
  //Serial.print("X: "); Serial.print(event.acceleration.x); Serial.print("  ");
  //Serial.print("Y: "); Serial.print(event.acceleration.y); Serial.print("  ");
  //Serial.print("Z: "); Serial.print(event.acceleration.z); Serial.print("  ");Serial.println("m/s^2 ");
  
  strip.show();  // update LEDs
  MidiUSB.flush(); // and send all MIDI messages

  delay(10);
}

void send_cc(byte channel, uint8_t cc, uint8_t value) {
  Serial.print("Sending CC #"); Serial.print(cc);
  Serial.print(" = "); Serial.println(value);
  midiEventPacket_t cc_packet = {0x0B, 0xB0 | channel, cc & 0x7F, value & 0x7F};
  MidiUSB.sendMIDI(cc_packet);
}

void pitchBend(byte channel, int value) {
  byte lowValue = value & 0x7F;
  byte highValue = value >> 7;
  midiEventPacket_t pitchBend = {0x0E, 0xE0 | channel, lowValue, highValue};
  MidiUSB.sendMIDI(pitchBend);
}

// floating point map
float ofMap(float value, float inputMin, float inputMax, float outputMin, float outputMax, bool clamp) {
    float outVal = ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);
  
    if (clamp) {
      if (outputMax < outputMin) {
        if (outVal < outputMax)  outVal = outputMax;
        else if (outVal > outputMin)  outVal = outputMin;
      } else {
        if (outVal > outputMax) outVal = outputMax;
        else if (outVal < outputMin)  outVal = outputMin;
      }
    }
    return outVal;

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
