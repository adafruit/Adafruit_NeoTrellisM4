#include "Adafruit_NeoTrellisM4.h"
#include <Adafruit_NeoPixel.h>

#define NEO_PIN 10
#define NUM_KEYS 32

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_KEYS, NEO_PIN, NEO_GRB + NEO_KHZ800);

boolean lit[NUM_KEYS];

void setup(){
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  strip.setBrightness(80);
  trellisKeypad.begin();
  
  Serial.begin(9600);
  Serial.println("keypad1 test!");

  for (int i=0; i<NUM_KEYS; i++) {
    lit[i] = false;
  }
}
  
void loop() {
  // put your main code here, to run repeatedly:
  trellisKeypad.tick();

  while (trellisKeypad.available()){
    keypadEvent e = trellisKeypad.read();
    
    if (e.bit.EVENT == KEY_JUST_PRESSED) {
      Serial.print((int)e.bit.KEY);
      Serial.println(" pressed");
      int led = e.bit.KEY-1;
      lit[led] = !lit[led];
      if (lit[led]) {
        strip.setPixelColor(led, Wheel(random(255)));
      } else {
        strip.setPixelColor(led, 0);
      }      
    }
    strip.show();
  }
  
  delay(10);
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
