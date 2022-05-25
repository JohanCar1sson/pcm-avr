/* Single-period 1 kHz sine wave (w/ 4 bits per sample), made by hand */

#ifndef RAUDIO_PREFIX
#define RAUDIO_PREFIX
#endif

#define KONK(a, b) KONK_(a, b)
#define KONK_(a, b) a ## b

const unsigned char KONK(RAUDIO_PREFIX, raudio_bitdepth) = 4;
const unsigned long KONK(RAUDIO_PREFIX, raudio_length) = 8;

const unsigned char KONK(RAUDIO_PREFIX, raudio_data)[] PROGMEM = {216, 223, 56, 49};
/* {255, 255, 17, 17}; */ /* square wave (for debugging) */
