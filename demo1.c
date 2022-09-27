/* driver that demonstrates the most basic use of the pcm-tn85 library
 *
 * output: PCM audio on physical pin 6
 *
 * Copyright 2022 Johan Carlsson
 *
 */

/* this header file has all the PCM functionality, no need to link in a library */
#include "pcm-avr.h"

/* provide your raudio data here */
/* #include "sqw1kHz4b.h" */ /* try this one on m328p */
#include "sine1kHz4b.h"

int main()
{
	nloop = 1;
	pcm_init();
	pcm_play_async(raudio_data, raudio_length, raudio_bitdepth);
	/* the timers and the interrupts are now handling the raudio playback, nothing further to do in main, so just loop idly */
	while (1);
}
