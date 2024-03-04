/*	
 * Lab1_MorseCode.c
 *
 * Created: 1/26/2024 12:44:32 PM
 * Author : Sophia Garcia
 */ 
#define F_CPU 16000000UL // 16MHz clock from the debug processor
#include <avr/io.h>
#include <util/delay.h>

#include "Morse_codes.h"

void delay_ms_var(uint8_t delay_t)
{
/*
	function initialized in order for there to be a variable 
	parameterized delay; current _delay_ms(int time) 
	included library function unable to take in a variable 
	time to output
*/
	while(0 < delay_t)
	{
		_delay_ms(10);	//delay in batches of 10 seconds 
		delay_t -= 10;	//per round 10 seconds taken from input
	}
}

void display(const uint16_t time)
{	
	PORTB |= (1<<PORTB5); //Set port bit B5 to 1 to turn on the LED
	delay_ms_var(time); //delay based on input time; 200 ms if dot; 600 ms if dash
	PORTB &= ~(1<<PORTB5); //Clear port bit B5 to 0 to turn off the LED
	_delay_ms(200); //delay : 200ms
}

int letter_val(const uint16_t val)
{
/*
	match the character with their Morse code value either:
		octal value equivalent ['A'-'Z', '0'-'9']
*/
	uint16_t ret_val = val;
	volatile uint16_t alphabet[27] = {A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z};
	volatile uint16_t numbers[10] = {zero, one, two, three, four, five, six, seven, eight, nine};
	if(val >= 65)
	{
		ret_val = alphabet[val - 65];
	}
	else if(val <= 57 && val != 32)
	{
		ret_val = numbers[val - 48];
	}
	return ret_val;
}

void space_dot_dash(uint16_t val, const uint16_t next)
{
	// 'fourBit_sect' that is taken from val in the form of an octal value [0O] equal to [0bBBB]
	if(val == 32) //if next char a space then print space of 1400 time
	{
		_delay_ms(1200);
		return;
	}
	volatile uint8_t threeBit_sect;	
	while(val != 0)	//if val == 00 end loop
	{
		threeBit_sect = val & 07; //isolate LMO of val				
		display(200 * threeBit_sect);		
		// 03 = dash 
		// 01 = dot		
		val = val >> 3;
		//go to next hex value until val = 00
	}		
	if(next != 32)	//if the next character in string not space print space of 400 ms
		_delay_ms(400);	
}

int main(void)
{
    DDRB |= (1<<DDB5);
	char sgn[] = "826433407";	//string to be printed
    while (1)	
    {
		static uint8_t cnt = 0;	//forever loop counter
		uint16_t val = sgn[cnt];	//one char from sgn string of pos [cnt]
		uint8_t nxt = sgn[++cnt];	//the following pos char 
		if(val >= 'a')
			val -= ('a' - 'A');	//if lower case val then change to upper
		val = letter_val(val);//change to header file value
		space_dot_dash(val, nxt);//print the Morse code form of character val
		if(nxt == '\0')
			cnt = 0;		//if string ended restart
    }
}