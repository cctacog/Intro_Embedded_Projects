/*
 * Lab7_ADC.c
 *
 * Created: 3/29/2024 12:42:27 PM
 * Author : Sophia Garcia
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL
uint16_t readADC_conversion = 0;

//REDID: 826433407
/*
	X = 4
	Y = 0
	Z = 7
*/

/*
	want our max time to achieve at 5ms
	1 -> 2^8 * 1/16Hz = 16u < 5m
	8 -> 16u * 8 = 128u < 5m
	16 -> 16u * 64 = 1024u < 5m
	256 -> 16u * 256 = 4068u = 4.068m < 5m
	1024 -> 16u * 1024 = 16384u = 16.384m > 5m 
** general pre scale for program to run smooth at 1024 system clock **
*/

/*
	want our max time to achieve at 1.25ms
	1 -> 2^8 * 1/16Hz = 16u < 1.25m
	8 -> 16u * 8 = 128u < 1.25m
	16 -> 16u * 64 = 1024u < 1.25m
	256 -> 16u * 256 = 4068u = 4.068m > 1.25m
** general pre scale for program to run smooth at 256 system clock **
*/

void init_IO()
{
	DDRD |= (1 << 3);//oc2b value as outpu from timer 2
	DDRB |= (1 << 5); //OUTPUT LED
	DDRC &= ~(1 << 0);//INPUT FROM POTENTIOMETER
	PORTB |= (1 << 5); //DEFUALT On
}

void init_AD()
{
	ADMUX |= (1 << REFS0);//channel 0 & Vcc ref 5V
	ADMUX &= ~(1 << MUX0 | 1 << MUX1 | 1 << MUX2 | 1 << MUX3);//DOUBLE CHECK
	// Y % 7 = 0 % 7 = 0 THEREFORE CHANNEL 0 THEREFORE NO CHANGE TO MUX[3:0]
	/*
		ENABLE CONVERISONS
		ENABLE AUTO TRIGGER
		ENABLE INTERRUPTS
		PRESCALE 128
	*/
	ADCSRA |= (1 << ADEN) | (1 << ADATE) | (1 << ADIE) | (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);
	ADCSRB |= (1 << ADTS1) | (1 << ADTS0); //set when compare match of timer0A to activate analog comparator
}

void init_Timer0()
{
	/*
		FOR THE AD CONVERSION
	*/
	//set the first timer to have a frequency of X + 1 ms = 5ms of just OCR0A
	TCCR0A |= (1 << WGM01); // Set the Timer Mode to ctc

	// set pre-scaler to 1024 and start the timer to have the max be 5ms
	OCR0A = 77;// one cycle less than ~5ms
	TCCR0B |= (1 << CS02) | (1 << CS00);
	TIMSK0 |= (1 << 1); //activate interrupts when OC0A flag enabled
}

void init_Timer2(void)
{
	/*	FOR THE LED	*/
	//set the second timer to have a pwm of (Z + 1) * 100 Hz = 800Hz
	//1/800Hz = 0.00125 s => 1.25 ms
	TCCR2A |= (1 << WGM21) | (1 << WGM20) | (1 << COM2B1); // Set the Timer Mode to fpwm
	// When OC0B flag hit then CLR flag when at BOTTOM SET bit

	// set pre-scaler to 256 and start the timer to have the max be ~5ms
	OCR2A = 77;// one cycle less than ~1.25ms
	OCR2B = 77;
	TCCR2B |= (1 << WGM22) | (1 << CS22) | (1 << CS21);
}

ISR(TIMER0_COMPA_vect)
{	
}

ISR(ADC_vect)
{
	readADC_conversion = ADC;
	OCR2B = 0;
	OCR2B = readADC_conversion/13.28;//to set in range of (0-1023] 
	if (OCR2B > 77){
		OCR2B = 77;
	}
	if(OCR2B < 0)//in case off invalid value 
	{
		DDRD &= (1<<3);
	}
	else
	{
		DDRD |= (1<<3);
	}
}

int main(void)
{
	sei();
	init_Timer0();
	init_Timer2();
	init_AD();
	init_IO();
	
    while (1){
		//OCR2B = 1;// test range 
	}
	
}