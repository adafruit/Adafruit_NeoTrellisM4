
// misc//////////////////////////
#define OCTAVE 3                 // determines note pitch
#define BPM 300                  // increase for faster tempo
#define MIDI_OUT false           // enables MIDI output & disables synth
#define MIDI_CHANNEL 0           // default channel # is 0
#define SYNTH_SCALE lydian_scale // see below, dorian, ionian, etc!
// choose a CC number to control with x axis tilting of the board. 1 is mod
// wheel, for example.
#define MIDI_XCC 1
#define ARPEGGIATOR_PATTERN circle_arp // see below, many shapes to choose from!
#define HOLD_ENABLED                                                           \
  0 // Buttons are toggled on & off instead of only on while pressed, O = off, 1
    // = on

// colors//////////////////////////
uint32_t white = 0xFFFFFF;
uint32_t red = 0xFF0000;
uint32_t blue = 0x0000FF;
uint32_t green = 0x00FF00;
uint32_t teal = 0x00FFFF;
uint32_t magenta = 0xFF00FF;
uint32_t yellow = 0xFFFF00;
uint32_t off = 0x000000;
uint32_t offColor = blue;   // color of deactivated buttons
uint32_t onColor = magenta; // color of activated buttons
uint32_t holdColor = white; // color for toggled buttons in Hold mode

// waveform//////////////////////////
int wave = 0; // 0=sine, 1= square, 2= sawtooth, 3 = triangle

// Musical modes / scales
uint8_t dorian_scale[] = {0, 2, 3, 5, 7, 9, 10, 12}; // dorian
uint8_t ionian_scale[] = {0, 2, 4, 5, 7, 9, 11, 12}; // ionian
uint8_t phrygian_scale[] = {
    0, 1, 2, 3, 5, 7, 8, 10,
};                                                       // phrygian
uint8_t lydian_scale[] = {0, 2, 4, 6, 7, 9, 10, 11};     // lydian
uint8_t mixolydian_scale[] = {0, 2, 4, 5, 7, 9, 10, 12}; // mixolydian
uint8_t aeolian_scale[] = {0, 2, 3, 5, 7, 8, 10, 12};    // aeolian
uint8_t locrian_scale[] = {0, 1, 3, 5, 6, 8, 10, 12};    // locrian

// pattern//////////////////////////

int8_t dipper_arp[6][2] = {
    // lil dipper
    {0, 0}, {1, 0}, {2, 0}, {2, 1}, {1, 1}, {1, 0},
};

int8_t circle_arp[6][2] = {
    // circle
    {0, 0}, {0, -1}, {1, 0}, {0, 1}, {-1, 0}, {0, -1},
};

int8_t onenote_arp[6][2] = {
    // one note
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
};

int8_t square_arp[6][2] = {
    // square
    {0, 0}, {-1, -1}, {1, -1}, {1, 1}, {-1, 1}, {-1, -1},
};

int8_t sshape_arp[6][2] = {
    // S-shape
    {0, 0}, {0, -1}, {1, -1}, {0, 0}, {0, 1}, {-1, 1},
};

int8_t tshape_arp[6][2] = {
    // T-shape
    {0, 0}, {1, 0}, {2, 0}, {2, -1}, {2, 0}, {2, 1},
};
