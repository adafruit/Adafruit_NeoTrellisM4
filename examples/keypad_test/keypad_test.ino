#include <Adafruit_Keypad.h>
#include <Adafruit_NeoPixel.h>

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

void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  strip.setBrightness(255);
  
  Serial.begin(115200);
  customKeypad.begin();
  Serial.println("keypad test!");
}

void loop() {
  // put your main code here, to run repeatedly:
  customKeypad.tick();

  while(customKeypad.available()){
    keypadEvent e = customKeypad.read();
    Serial.print((int)e.bit.KEY);
    if (e.bit.EVENT == KEY_JUST_PRESSED) {
      Serial.println(" pressed");
      strip.setPixelColor(e.bit.KEY-1, 0xFFFFFF);
    }
    else if (e.bit.EVENT == KEY_JUST_RELEASED) {
      Serial.println(" released");
      strip.setPixelColor(e.bit.KEY-1, 0x0);
    }
    strip.show();
  }
  
  delay(10);
}
