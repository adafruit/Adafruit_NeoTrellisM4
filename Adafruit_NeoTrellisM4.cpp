#include "Adafruit_NeoTrellisM4.h"

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
Adafruit_Keypad trellisKeypad = Adafruit_Keypad( makeKeymap(trellisKeys), rowPins, colPins, ROWS, COLS);

void tick_trellis()
{
	tick_trellis();

	// look for an entire column being pressed at once and if it was, clear the whole buffer
	uint8_t rcount[] = {0, 0, 0, 0, 0, 0, 0, 0};
	for(int i=0; i<31; i++){
		if(trellisKeypad.justPressed(i+1, false))
			rcount[i%8]++;
	}
	for(int i=0; i<8; i++){
		if(rcount[i] >= 4){
			trellisKeypad.clear();
			break;
		}
	}
}
