# pcm-avr
Library for audio playback using pulse code modulation (PCM) on some AVR microcontrollers, including ATtiny85 (a.k.a. tn85) and ATmega328P (a.k.a. m328p). It's intended for use with C code built with avr-gcc and might or might not work with other C compilers, the Arduino IDE, etc. It was developed from scratch in 2022 and is available under an MIT license.

pcm-avr has support for 8-bit audio samples with 8 kHz sampling rate, as well as bit-crushed audio with 4, 2 or 1 bits per sample. The PWM carrier frequency is as high as possible (e.g. 4 MHz for 4-bit audio on tn85) to make it easy to remove from the output signal with a low-pass filter.

The PCM functionality is implemented in the single header file **pcm-avr.h**. The API has the four functions **pcm_init**, **pcm_play_async**, **pcm_play** and **pcm_exit**, and the global variable **nloop**. The example driver **demo1.c** shows the most basic usage.

On tn85 the output pin for the PCM signal is PB1, physical pin 6. On m328p the output pin for the PCM signal is PD5, pin 5 on an Arduino Uno.
