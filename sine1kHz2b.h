/* Single-period 1 kHz sine wave (w/ 2 bits per sample), made by hand */

#ifndef RAUDIO_PREFIX
#define RAUDIO_PREFIX
#endif

#define KONK(a, b) KONK_(a, b)
#define KONK_(a, b) a ## b

const unsigned char KONK(RAUDIO_PREFIX, raudio_bitdepth) = 2;
const unsigned long KONK(RAUDIO_PREFIX, raudio_length) = 8;

const unsigned char KONK(RAUDIO_PREFIX, raudio_data)[] PROGMEM = {254, 86};
/* {255, 85}; */ /* square wave (for debugging) */
