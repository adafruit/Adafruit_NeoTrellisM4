#include "Adafruit_NeoTrellisM4.h"
#include <Adafruit_NeoPixel.h>

#define NEO_PIN 10
#define NUM_KEYS 32
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_KEYS, NEO_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  strip.setBrightness(255);
  
  Serial.begin(115200);
  trellisKeypad.begin();
  Serial.println("keypad test!");
}

void loop() {
  // put your main code here, to run repeatedly:
  trellisKeypad.tick();

  while(trellisKeypad.available()){
    keypadEvent e = trellisKeypad.read();
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
