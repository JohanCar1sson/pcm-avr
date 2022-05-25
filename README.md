# pcm-tn85
Library for audio playback using pulse code modulation (PCM) on ATtiny85 (and supposedly also on 45 and 25).

pcm-tn85 has support for 8-bit audio samples with 8 kHz sampling rate, as well as bit-crushed audio with 4, 2 or 1 bits per sample. The PWM carrier frequency is high, 250 kHz or higher, which makes it easy to remove from the output signal with a low-pass filter.

Development is in a very early and active stage. Currently the PCM functionality only exists in some snippets of test code in the repo. As I work through some use cases I will develop a sensible API and a proper library.

The output pin for the PCM signal is PB1, physical pin 6. A simple RC low-pass filter with cutoff frequency of about 4 kHz is sufficient to suppress the ultrasonic carrier frequency from the output signal.
