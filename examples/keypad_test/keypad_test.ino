#include "Adafruit_NeoTrellisM4.h"

// The NeoTrellisM4 object is a keypad and neopixel strip subclass
// that does things like auto-update the NeoPixels and stuff!
Adafruit_NeoTrellisM4 trellis = Adafruit_NeoTrellisM4();

void setup() {
  Serial.begin(115200);
  trellis.begin();
  Serial.println("basic keypad test!");
}

void loop() {
  // put your main code here, to run repeatedly:
  trellis.tick();

  while(trellis.available()){
    keypadEvent e = trellis.read();
    Serial.print((int)e.bit.KEY);
    if (e.bit.EVENT == KEY_JUST_PRESSED) {
      Serial.println(" pressed");
      trellis.setPixelColor(e.bit.KEY, 0xFFFFFF);
    }
    else if (e.bit.EVENT == KEY_JUST_RELEASED) {
      Serial.println(" released");
      trellis.setPixelColor(e.bit.KEY, 0x0);
    }
  }
  
  delay(10);
}
