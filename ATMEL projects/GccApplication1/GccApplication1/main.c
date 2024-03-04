/*
 * GccApplication1.c
 *
 * Created: 1/26/2024 12:19:36 PM
 * Author : gasop
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>


int main(void)
{
	DDRB |= (1<<DDB5);
    while (1) 
    {
		PORTB |= (1<<PORTB5);
		_delay_ms(100);
		PORTB &= ~(1<<PORTB5);
		_delay_ms(100);
    }
}

