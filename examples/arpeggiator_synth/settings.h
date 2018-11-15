#define OCTAVE 4  //determines note pitch
#define BPM 320   //increase for faster tempo

//colors
uint32_t white = 0xFFFFFF;
uint32_t red = 0xFF0000;
uint32_t blue = 0x0000FF;
uint32_t green = 0x00FF00;
uint32_t teal = 0x00FFFF;
uint32_t magenta = 0xFF00FF;
uint32_t yellow = 0xFFFF00;
uint32_t offColor = blue;   //color of deactivated buttons
uint32_t onColor = magenta; //color of activated buttons

//waveform
int wave = 0;  // 0=sine, 1= square, 2= sawtooth, 3 = triangle

// Musical mode / scale intervals
uint8_t scale[] = { 0, 2, 3, 5, 7, 9, 10, 12 }; //dorian
//uint8_t scale[] = { 0, 2, 4, 5, 7, 9, 11, 12 }  //ionian
//uint8_t scale[] = { 0, 1, 2, 3, 5, 7,  8, 10,}  //phrygian
//uint8_t scale[] = { 0, 2, 4, 6, 7, 9, 10, 11 }  //lydian
//uint8_t scale[] = { 0, 2, 4, 5, 7, 9, 10, 12 }, //mixolydian
//uint8_t scale[] = { 0, 2, 3, 5, 7, 8, 10, 12 }, //aeolian
//uint8_t scale[] = { 0, 1, 3, 5, 6, 8, 10, 12 }; //locrian

//pattern
int8_t arp[6][2] = { //lil dipper
  {  0,  0 },
  {  1, 0 },
  {  2, 0 },
  {  2,  1 },
  {  1,  1 },
  {  1,  0 },
};

// Note patterns for arpeggiator
//extern int8_t arp[6][2] = { //circle
//  {  0,  0 },
//  {  0, -1 },
//  {  1,  0 },
//  {  0,  1 },
//  { -1,  0 },
//  {  0, -1 },
//};

//extern int8_t arp[6][2] = { //one note
//  {  0,  0 },
//  {  0,  0 },
//  {  0,  0 },
//  {  0,  0 },
//  {  0,  0 },
//  {  0,  0 },
//};

//extern int8_t arp[6][2] = { //square
//  {  0,  0 },
//  { -1, -1 },
//  {  1, -1 },
//  {  1,  1 },
//  { -1,  1 },
//  { -1, -1 },
//};

//extern int8_t arp[6][2] = { //S-shape
//  {  0,  0 },
//  {  0, -1 },
//  {  1, -1 },
//  {  0,  0 },
//  {  0,  1 },
//  { -1,  1 },
//};

//extern int8_t arp[6][2] = { //T-shape
//  {  0,  0 },
//  {  1, 0 },
//  {  2, 0 },
//  {  2,  -1 },
//  {  2,  0 },
//  {  2,  1 },
//};

