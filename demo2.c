/* driver that demonstrates more advanced use of the pcm-tn85 library
 * (synchronous playback, and halting the timers to release them for other use)
 *
 * outputs: PCM on physical pin 6 (connect a loudspeaker)
 * and an LED (w/ ballast resistor of 1 kOhm or so) on physical pin 5
 *
 * Copyright 2022 Johan Carlsson
 *
 */

/* this header file has all the PCM functionality, no need to link in a library */
#include "pcm-tn85.h"

/* avoid name clashes when multiple raudio headers are included by prefixing */
#define RAUDIO_PREFIX upchirp_
#include "upchirp.h"
#define RAUDIO_PREFIX downchirp_
#include "downchirp.h"

int main()
{
	unsigned char i;

	pcm_init(); /* set up the carrier and modulation timers for PCM */

	/* do some synchronous playback */
	for (i = 0; i < 3; i++)
	{
		pcm_play(upchirp_raudio_data, upchirp_raudio_length, upchirp_raudio_bitdepth);
		pcm_play(downchirp_raudio_data, downchirp_raudio_length, downchirp_raudio_bitdepth);
	}
	pcm_exit(); /* halt the timers and turn PCM off */

	/* the chirping phase of your life is now over, use Timer 1 to slowly blink an LED instead */

	TCCR1 |= (1 << CS10) | (1 << CS11) | (1 << CS12) | (1 << CS13); /* reduce T1 frequency 16,384x */
	GTCCR |= (1 << PSR1); /* effect the new PS setting */
	OCR1C = 255; /* set TOP value for the T1 counter */
	OCR1A = 127; /* set duty cycle (OCR1A + 1) / (OCR1C + 1) for T1 comparator A */
	/* put PWM output signal on PB1, inverted signal on PB0 (see Table 12-1 in datasheet) */
	TCCR1 |= (1 << COM1A0); TCCR1 &= ~(1 << COM1A1);
	TCCR1 |= (1 << PWM1A); /* set PWM mode for comparator OCR1A */
	DDRB |= (1 << DDB0); /* make PB0 (a.k.a. OC1A-bar) an output pin */

	while (1);
}
