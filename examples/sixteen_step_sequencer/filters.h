#include "Audio.h"

// Number of coefficients
#define NUM_COEFFS 100
extern short low_pass[];
extern short band_pass[];

struct fir_filter {
  short *coeffs;
  short num_coeffs;    // num_coeffs must be an even number, 4 or higher
};

extern struct fir_filter fir_list[];

extern AudioFilterFIR      myFilterL;
extern AudioFilterFIR      myFilterR;

extern int filterActive;

extern void calculateLPF(float fl);
extern void calculateHPF(float fl);
