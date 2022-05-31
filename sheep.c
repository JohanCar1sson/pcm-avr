/* ATtiny85 source code for Noa's sheep
 * checks 18650 charge, bleats (using PCM), blinks some LEDs, and shuts down until button pressed again
 *
 * Copyright 2022 Johan Carlsson
 *
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#define F_CPU 1000000
#include <util/delay.h>

#include "bleat.h"

/* declare nbyte as volatile, so that user writes to it are atomic when 'rupts are enabled */
volatile unsigned int nbyte;
/* declare nsample as volatile too, just to be safe */
volatile unsigned char nsample;

/* gets called at the sample rate of 8 kHz to load the next raudio sample */
/* sets the duty cycle (OCR1A + 1) / (OCR1C + 1) for T1 comparator A */
ISR(TIM0_COMPA_vect/*, ISR_NOBLOCK*/) /* Enable nested interrupts so that this ISR won't disrupt T1's PWM (the OCF1A interrupt) */
{
	static unsigned char databyte;

	if (nbyte < raudio_length)
	{
		if (!nsample) databyte = pgm_read_byte(raudio_data + nbyte++);

		switch (raudio_bitdepth)
		{
			case 1:
				OCR1A = (databyte >> nsample) & 1;
				nsample = ++nsample % 8;
				break;
			case 2:
				OCR1A = (databyte >> (nsample << 1)) & 3;
				nsample = ++nsample % 4;
				break;
			case 4:
				OCR1A = (databyte >> (nsample << 2)) & 15;
				nsample = ++nsample % 2;
				break;
			case 8:
				OCR1A = databyte;
		}
		if (raudio_length == nbyte) nbyte = 0;
	}
}

int main()
{
	/* don't bump system clock CK from 1 MHz 8 MHz */
	/* CLKPR = (1 << CLKPCE); */ /* set CLKPCE to 1 and the other bits to zero to enable prescaler change */
	/* CLKPR = 0; */ /* set CLKPS0-3 to zero (disable the clock prescaler) and CLKPCE to zero to effect the change */
	/* CK is the clock source for T0 */

	/* _NOP(); */

	/* change the T1 clock source from CK to PCK (it runs faster) */
	/* the three statements below MUST BE done one by one and in that order, see section 12.3.9 in datasheet */
	PLLCSR |= (1 << PLOCK); /* lock PLL to reference clock */
	do _delay_us(100.); while (PLLCSR & PLOCK); /* wait 100 us and check that PLOCK has been set */
	PLLCSR |= (1 << PLLE); /* start the PLL */
	PLLCSR |= (1 << PCKE); /* use PLL as T1 clock source */
	/* PCK is now the clock source for T1, and it's running at 64 MHz */

	/*** SET UP TIMER1 TO DO PWM ON PB1 ***/

	/* configure T1 prescaler (CS1[0:3] = 0001 => 1, CS1[0:3] = 0010 => 1/2, etc.) */
	TCCR1 |= (1 << CS10);
	TCCR1 &= ~((1 << CS11) | (1 << CS12) | (1 << CS13));
	GTCCR |= (1 << PSR1); /* effect the new PS setting */

	/* set TOP value for the T1 counter */
	switch (raudio_bitdepth)
	{
		case 1:
			OCR1C = 1;
			OCR1A = 0;
			break;
		case 2:
			OCR1C = 3;
			OCR1A = 2;
			break;
		case 4:
			OCR1C = 15;
			OCR1A = 8;
			break;
		case 8:
			OCR1C = 255;
			OCR1A = 128;
	}
	/* put PWM output signal on PB1, nothing on PB0 (see Table 12-1 in datasheet) */
	TCCR1 &= ~(1 << COM1A0);
	TCCR1 |= (1 << COM1A1);

	/* set PWM mode for comparator OCR1A */
	TCCR1 |= (1 << PWM1A);

	/*** SET UP TIMER0 TO DO THE MODULATION ***/

	/* prescale T0 to run at 1 MHz */
	/* TCCR0B = (TCCR0B & ~((1 << CS02) | (1 << CS00))) | (1 << CS01); */ /* divide by 8 for 8 MHz CK */
	TCCR0B = (TCCR0B & ~((1 << CS02) | (1 << CS01))) | (1 << CS00); /* don't prescale for 1 MHz CK */

	/* set CTC mode for T0 (see Table 11-5 in datasheet) */
	TCCR0A &= ~(1 << WGM00);
	TCCR0A |= (1 << WGM01);
	TCCR0B &= ~(1 << WGM02);

	/* set the compare register */
	OCR0A = 124; /* 1,000,000 Hz / (124 + 1) = 8,000 Hz */

	nbyte = 0; /* initialize 2-byte raudio byte counter (used in ISR) before turning on 'rupts */
	nsample = 0; /* initialize single-byte raudio sample-within-byte counter (used in ISR) */

	/* enable interrupts */
	sei();

	/* fire interrupt when TCNT0 == OCR0A */
	TIMSK |= (1 << OCIE0A);

	/* non-timer initialization */
	DDRB |= (1 << DDB1); /* make PB1 (a.k.a. OC1A) an output pin */

	/* inits are done, nothing else do do in main, just idle while the timers and ISR do their work... */
	while (1);
}
