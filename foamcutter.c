/*
 * FoamCutter
 */ 

#define F_CPU 8000000UL // 8 MHz

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include <avr/sleep.h>
#include <stdio.h>

//Fuses:
//Extended   b11111111
//High       b11011111
//Low        b11100010

//Ports:
//A:
//0 ADC0 - Input potentiometer measurement

//B:
//0 ISP(MOSI)
//1 ISP(MISO) - OC1A (high side)
//2 ISP(SCK) - Serial debugging output
//3 OC1B (low side)
//4 Crystal (Optional)
//5 Crystal (Optional)
//6 
//7 Reset (pulluped)

//Init I/O pins
void boardInit(){
	_delay_ms(100);
	DDRA = 0b00000000; 
	DDRB = 0b00001110; 
	
	//Everything starts up turned off
	PORTB = 0x00;
	PORTA = 0x00;
}

//Init timers and PLL for fast PWM
void pwmInit(){
	//Enable PLL
	PLLCSR=0x02;
	_delay_us(100);
	//Wait for it to be stable
	while ((PLLCSR & 1)==0);
	//Enable 64 MHz clock for timer1
	//When using 8 bit, it yields a PWM frequency of 250 KHz
	PLLCSR|=0x04;
	
	TCCR1C=0; 
	TCCR1D=0;
	TCCR1E=0;
	TCCR1A=0b10100011; //Online output, non inverting, PWM enabled for channels A, B
	TCCR1B=0b00000011; //Prescaler: CLK/4
	
	TC1H=0x00;
	TCNT1=0x00;
	DT1=0x00;		//No dead times
	
	TC1H = 0x03; //2 lowest bits in "1"
	OCR1C=0xFF; //Top value of PWM
	
	TC1H = 0x00;
	OCR1A=0x00;	//PWM value
	
	TC1H = 0x00;
	OCR1B=0x00;
}

void adcInit(){ 
	ADMUX = 0b00000000; //Right aligned, channel 0
	ADCSRA = 0b10000111;
	ADCSRB = 0b00000000;
	DIDR0 = 0b01010111; //Disable digital drivers for analog inputs
}

uint8_t adcLow, adcHigh;

void adcRead(uint8_t canal){
	ADMUX=0x00 | canal;
	_delay_us(10);
	
	ADCSRA|=0x40;
	while ((ADCSRA & 0x10)==0);
	ADCSRA|=0x10;
	
	adcLow = ADCL; //Low first, then high
	adcHigh = ADCH;
}

int main(){
	boardInit();
	pwmInit();
	adcInit();
	
	while(1){
		adcRead(0);
		
		TC1H = adcHigh;
		OCR1A = adcLow;

		_delay_ms(1);
	}
	
}
		
	


