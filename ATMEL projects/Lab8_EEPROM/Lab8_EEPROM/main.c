/*
 * Lab8_EEPROM.c
 *
 * Created: 4/18/2024 9:54:18 AM
 * Author : Sophia Garcia
 */ 

#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>

const uint8_t LED_precentages[11] = {0, 7, 15, 23, 30, 38, 46, 53, 61, 69, 76};//0%-100%

void init_Timer0()
{
	TCCR0A |= (1 << WGM01) | (1 << WGM00) | (1 << COM0B1); // Set the Timer Mode to fast pwm
	// set pre-scaler to 1024 and start the timer to have the max be 5ms
	OCR0A = 77;//~5ms
	OCR0B = 0;
	TCCR0B |= (1 << CS02) | (1 << CS00)| (1 << WGM02);
	TIMSK0 |= (1 << 1)|(1 << 2); //activate interrupts when OC0B flag enabled
}

void init_LED()
{
	DDRD |= (1 << 3);//oc0b output to led
	DDRB |= (1 << 5);//set LED at portb5 to output
	//PORTB |= (1 << 5); //default on LED
}

void init_LEDPWM()
{
	uint8_t LED_percent = eeprom_read_byte((uint8_t*) 46);
	if(LED_percent > 10)
    {	  
	   LED_percent = 0;
    }
	OCR0B = LED_precentages[LED_percent];
	LED_percent += 1;
	eeprom_write_byte((uint8_t*) 46, LED_percent);
}

ISR(TIMER0_COMPA_vect)
{
	if(OCR0B)
		PORTB |= (1 << 5); //turn on
}

ISR(TIMER0_COMPB_vect)
{
	PORTB &= ~(1 << 5); //turn off
}


int main(void)
{
	sei();
	init_LED();
	init_Timer0();
	init_LEDPWM();
    while (1);
}

