all: 
	avr-gcc -Wall -Os -mmcu=attiny261 -c foamcutter.c -o foamcutter.o
	avr-gcc -Wall -Os -mmcu=attiny261 -o foamcutter.elf foamcutter.o
	
	avr-objcopy -j .text -j .data -O ihex foamcutter.elf foamcutter.hex

run: foamcutter.hex
	
