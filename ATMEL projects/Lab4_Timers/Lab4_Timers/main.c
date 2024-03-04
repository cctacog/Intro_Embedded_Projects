/*
 * Lab4_Timers.c
 *
 * Created: 2/26/2024 11:42:35 AM
 * Author : Sophia Garcia 
 */ 

#define F_CPU 16000000
#include <avr/io.h>

void init_Timer(void)
{
	TCCR0A |= (1 << WGM01); // Set the Timer Mode to CTC
	/*
		want 10ms
		1 -> 2^8 * 1/16Hz = 16u < 10m
		8 -> 16u * 8 = 128u < 10m
		16 -> 16u * 64 = 1024u < 10m
		256 -> 16u * 256 = 4068u = 4.068m < 10m
		1024 -> 16u * 1024 = 16384u = 16.384m > 10m 
		for 10m
			OCR0A = 256 * 10m / 16.384m = 156.25
			** rounding down ** -> 156
			156d = 0x9C
	*/
	OCR0A = 0x9C; // Set the value that you want to count to
	// set pre scaler to 1024 and start the timer to have the max be 10ms 
	TCCR0B |= (1 << CS02) | (1 << CS00);
	OCR0B = 0;
	//to set default ocr0b at 0 bc button never pressed 
}

void timer_loop()
{
	if(!(PINB & (1 << 7)) && OCR0B < OCR0A)//if the button is activated and not max (OCR0A) value not reached
		++OCR0B;//aggregate to the 0CR0B until it is equal to
	else if((PINB & (1 << 7)) && OCR0B > 0)
		--OCR0B;//decrease from OCR0B if the button deactivated and not value 0
	if(OCR0B != 0)//if not 100% off
		PORTB |= (1 << PORTB5);//turn LED ON//until point b reached do not continue	
	while(!(TIFR0 & (1 << OCF0B)));
	if(OCR0B != OCR0A)//if it is not at 100%
		PORTB &= ~(1 << PORTB5);//turn LED OFF
	TIFR0 |= (1 << OCF0B);//reset TIFR0 
	while(!(TIFR0 & (1 << OCF0A)));//until the top point reached do not continue
	TIFR0 |= (1 << OCF0A);//reset OCR0A
	/*
		1 second for an LED to go from 0 to 156
		for non decimal 
	*/
}


int main(void)
{
	init_Timer();//initialize the timer CTC and default OCR0A and OCR0B
	DDRB |= (1 << DDB5) | ~(1 << DDB7); // set the LED to OUTPUT and switch to INPUT
	PORTB &= ~(1 << PORTB5);//turn LED OFF
    while (1) 
    {
		timer_loop();//go through the 10ms timer
		
    }
}

