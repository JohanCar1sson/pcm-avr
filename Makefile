.PHONY : all
all : test.hex sheep.hex

test.hex : test.elf
	avr-objcopy -O ihex -R .eeprom test.elf test.hex
# avr-objcopy -j .text -j .data -O ihex test.elf test.hex

test.elf : test.o
	avr-gcc -mmcu=attiny85 test.o -o test.elf

test.o : test.c
	avr-gcc -c test.c -mmcu=attiny85 -Os -o test.o

sheep.hex : sheep.elf
	avr-objcopy -O ihex -R .eeprom sheep.elf sheep.hex

sheep.elf : sheep.o
	avr-gcc -mmcu=attiny85 sheep.o -o sheep.elf

sheep.o : sheep.c
	avr-gcc -c sheep.c -mmcu=attiny85 -Os -o sheep.o

.PHONY : flash
flash : test.hex
	avrdude -v -B 10 -p attiny85 -c stk500v1 -P /dev/ttyUSB0 -b 19200 -U flash:w:test.hex:i
#avrdude -v -B 10 -p attiny85 -c usbasp -P usb -U flash:w:test.hex:i

.PHONY : sizecheck
sizecheck :
	avr-size -A test.elf

.PHONY : clean
clean :
	rm -f test.hex test.elf test.o sheep.hex sheep.elf sheep.o
