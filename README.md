# pcm-tn85
Library for audio playback using pulse code modulation (PCM) on ATtiny85 (and supposedly also on 45 and 25).

pcm-tn85 has support for 8-bit audio samples with 8 kHz sampling rate, as well as bit-crushed audio with 4, 2 or 1 bits per sample. The PWM carrier frequency is high, 250 kHz or higher, which makes it easy to remove from the output signal with a low-pass filter.

The PCM functionality is implemented in the single header file **pcm-tn85.h**. The API has the four functions **pcm_init**, **pcm_play_async**, **pcm_play** and **pcm_exit**, and the global variable **nloop**. The example driver **demo1.c** shows the most basic usage.

The output pin for the PCM signal is PB1, physical pin 6. A simple RC low-pass filter with cutoff frequency of about 4 kHz is sufficient to suppress the ultrasonic carrier frequency from the output signal.
