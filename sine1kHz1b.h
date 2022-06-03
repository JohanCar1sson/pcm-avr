/* Single-period 1 kHz sine wave (w/ 1 bit per sample), made by hand */

#ifndef RAUDIO_PREFIX
#define RAUDIO_PREFIX
#endif

#define KONK(a, b) KONK_(a, b)
#define KONK_(a, b) a ## b

const unsigned char KONK(RAUDIO_PREFIX, raudio_bitdepth) = 1;
const unsigned int KONK(RAUDIO_PREFIX, raudio_length) = 1;

const unsigned char KONK(RAUDIO_PREFIX, raudio_data)[] PROGMEM = {15}; /* Yup, that's a sine wave */
