/* MIDI synth example specifically for pixelsynth.com
 *  First three rows are in 'pairs' for pulsewidth thru threshold CC messages
 *  The left button decreases CC value, right button increases
 *  Last row has RGB background and foreground adjust. Press red, green blue buttons
 *  to increase that color element, will wrap around to 0 once it gets to max.
 *  Second to last button doesnt do anything. Last button is random
 */

#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL343.h>
#include <Adafruit_NeoTrellisM4.h>

#define MIDI_CHANNEL     0  // default channel # is 0

// Accel doesnt do anything ~yet~
Adafruit_ADXL343 accel = Adafruit_ADXL343(123, &Wire1);

#define CC_NUM 20
#define KEYS_CC_START 7
uint16_t cc_curr_value[CC_NUM] = {0};
uint32_t button_colors[32] = {0};

Adafruit_NeoTrellisM4 trellis = Adafruit_NeoTrellisM4();

void setup(){
  Serial.begin(115200);
  //while (!Serial);

  trellis.begin();
  trellis.setBrightness(80);
  trellis.enableUSBMIDI(true);
  trellis.setUSBMIDIchannel(MIDI_CHANNEL);

  Serial.println("CC MIDI keypad");

  if(!accel.begin()) {
    Serial.println("No accelerometer found");
    while(1);
  }

  // set to center
  for (int i=0; i<CC_NUM; i++) {
    cc_curr_value[i] = 64;
    trellis.controlChange(i, cc_curr_value[i]);
    trellis.sendMIDI(); // and send all MIDI messages
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
  trellis.tick();

  for (int key=0; key<32; key++) {
    //Serial.println(key);
    // Key is not pressed, revert color and continue
    if (!trellis.isPressed(key)) {
      //Serial.print(key); Serial.println(" is NOT pressed");
      // Revert to the default color if not pressed
      trellis.setPixelColor(key, button_colors[key]);
      continue;
    }
    Serial.print(key); Serial.println(" is pressed");

    trellis.setPixelColor(key, 0xFFFFFF);
    if (key < 24) {
      // first 3 rows are basic CC values
      int cc_idx = (key) / 2;
      bool inc = (key) % 2;
      uint16_t val = cc_curr_value[cc_idx];
      if (inc) {
        val = min(127, val+1);  // no larger than 127!
      } else {
        val = min(127, val-1);  // no smaller than 0!
      }
      cc_curr_value[cc_idx] = val;
      trellis.controlChange(cc_idx + KEYS_CC_START, cc_curr_value[cc_idx]);
    }
    // Cycle foreground colors
    if (key >= 24 && key <=29 ) {
      int cc_idx = 12 + (key-24);
      uint16_t val = cc_curr_value[cc_idx];
      val++;
      if (val > 127) val = 0;  // wrap around
      cc_curr_value[cc_idx] = val;
      // RGB messages start with CC #1 thru #6
      trellis.controlChange(key-23, cc_curr_value[cc_idx]);
    }
    if (key == 31) {
      trellis.controlChange(19, 1); // randomize on channel 19
    }
  }

  // Check for accelerometer
  //sensors_event_t event;
  //accel.getEvent(&event);
  /* Display the results (acceleration is measured in m/s^2) */
  //Serial.print("X: "); Serial.print(event.acceleration.x); Serial.print("  ");
  //Serial.print("Y: "); Serial.print(event.acceleration.y); Serial.print("  ");
  //Serial.print("Z: "); Serial.print(event.acceleration.z); Serial.print("  ");Serial.println("m/s^2 ");

  trellis.sendMIDI(); // and send all MIDI messages

  delay(10);
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
    return trellis.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return trellis.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return trellis.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
