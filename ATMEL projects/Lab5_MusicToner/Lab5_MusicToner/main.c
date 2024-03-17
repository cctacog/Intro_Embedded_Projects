/*
 * Lab5_MusicToner.c
 *
 * Created: 3/7/2024 6:09:24 PM
 * Author : Sophia Garcia
 */ 

#define F_CPU 16000000UL
#define PRESCALE 1024
#include <avr/io.h>
#include <avr/delay.h>
#include <avr/interrupt.h>

const uint8_t vals[4][4] = {
	{71,67,63,59},
	{56,53,50,47},
	{44,42,39,37},
	{35,33,31,29},
};

void init_IO()
{
	//from lab 3:
	DDRD |= (1 << 0)|(1 << 1)|(1 << 2)|(1 << 3)|(1<<5);//SET PORT D TO OUTPUT
	DDRB &= ~(1 << 0)|~(1 << 1)|~(1 << 2)|~(1 << 3);//SET PORT B TO INPUT [PIN]
	PORTB |= (1 << 0)|(1 << 1)|(1 << 2)|(1 << 3);//SET PIN B TO PULL UP RESISTOR
	
	//PRESET THE OUTPUTS ALL AS HIGH
	PIND |= (1 << 0)|(1 << 1)|(1 << 2)|(1 << 3); 
}

void init_Timer()
{
	TCCR0A |= (1 << WGM01) | (1 << WGM00) | (1 << COM0B1); // Set the Timer Mode to fpwm
	OCR0A = 0xfd;
	OCR0B = OCR0A;
	/*
		want our max time to achieve at ~4.54ms
		1 -> 2^8 * 1/16Hz = 16u < ~4.54m
		8 -> 16u * 8 = 128u < ~4.54m
		16 -> 16u * 64 = 1024u < ~4.54m
		256 -> 16u * 256 = 4068u = 4.068m < ~4.54m
		1024 -> 16u * 1024 = 16384u = 16.384m > ~4.54m 
	** general pre scale for program to run smooth at 1024 system clock **
	*/
	// set pre scaler to 1024 and start the timer to have the max be 10ms 
	TCCR0B |= (1 << WGM02) | (1 << CS02) | (1 << CS00);
}

void keyPad_checker()
{
	for(uint8_t i = 0; i < 4; ++i)//loop through each row as PORTD0-3 (four total)
	{
		//turning off the output to the keypad at row i
		PORTD &= ~(1 << i);
		for(uint8_t j = 0; j < 4; ++j)//loop through each column PORTB0-3 (four total)
		{
			if(!(PINB & (1 << j)))//if there is a short of a row then the input will be 0
			{
				OCR0A = vals[3 - i][3 - j] - 1;
				//default truncation will suffice
				OCR0B = (OCR0A / 2);//50% duty cycle
				while(!(PINB & (1 << j)));
			}
			OCR0A = 0xFD;
			OCR0B = OCR0A;
		}
		PORTD |= (1 << i);//return the output of 1 to current row
	}
}

int main(void)
{    
	init_IO();
	init_Timer();
    while (1) 
    {
		keyPad_checker();
    }
}

