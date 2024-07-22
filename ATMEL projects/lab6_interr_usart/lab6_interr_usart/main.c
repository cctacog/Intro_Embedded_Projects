/*
 * Lab6_Interrupts_.c
 *
 * Created: 3/21/2024 2:02:03 PM
 * Author : Sophia Garcia
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#define BAUD 9600
#define BAUDRATE (F_CPU/(16UL * BAUD) - 1)

char look_up[3][3] = {{'1', '2', '3'},{ '4', '5', '6'}, {'7', '8', '9'}};
uint8_t row = 0;
uint8_t col = 0;
	

//REDID: 826433407
/*
	X = 4
	Y = 0
	Z = 7
*/

void init_USART()
{
	//Set baud rate 
	UBRR0H = 0;
	UBRR0L = BAUDRATE;
	//Enable receiver and transmitter 
	UCSR0B = (1 << RXCIE0) | (1 << TXCIE0) | (1<<RXEN0) | (1<<TXEN0);
	// Set frame format: 8 data, default 1 stop bit
	UCSR0C |= (1<<UCSZ00)|(1<<UCSZ01); 
	/*
	UCSR0C is data size for USART comm
	the bits for this are UCSZ[2:0]
	
	leave USBn as 0 bc we only want 1 stop bit
	
	Parity bit would be by UPM[1] = 1 (bit) from UPM[1:0]
	*/
}


void init_IO(void)
{
	DDRD |= (1 << 3)|(1 << 4)|(1 << 5);//SET PORT D0:2 TO OUTPUT
	DDRB &= ~(1 << 0)|~(1 << 1)|~(1 << 2);//SET PORT B0:2 TO INPUT [PIN]
	DDRB |= (1 << 5);//set the port B5 to output for the LED 
	PORTB |= (1 << 0)|(1 << 1)|(1 << 2);//SET PIN B TO PULL UP RESISTOR
	//only considering the pins wit numbers so no pin B3 or port D3 here!
	//PRESET THE OUTPUTS ALL AS HIGH
	PIND |= (1 << 3)|(1 << 4)|(1 << 5);
}
void init_timer0(void)
{
	/*
		FOR THE KEYPADCHECKER
	*/
	//set the first timer to have a frequency of X + 1 ms = 5ms of just OCR0A
	TCCR0A |= (1 << WGM01) | (1 << WGM00) | (1 << COM0A1) | (1 << COM0A1); // Set the Timer Mode to fpwm
	// When OC0A flag hit then set flag when at BOTTOM clear bit
	/*
		want our max time to achieve at 5ms
		1 -> 2^8 * 1/16Hz = 16u < 5m
		8 -> 16u * 8 = 128u < 5m
		16 -> 16u * 64 = 1024u < 5m
		256 -> 16u * 256 = 4068u = 4.068m < 5m
		1024 -> 16u * 1024 = 16384u = 16.384m > 5m 
	** general pre scale for program to run smooth at 1024 system clock **
	*/
	// set pre scaler to 1024 and start the timer to have the max be 5ms 
	OCR0A = 77;// one cycle less than ~5ms
	TCCR0B |= (1 << WGM02) | (1 << CS02) | (1 << CS00);
	TIMSK0 |= (1 << 1); //activate interrupts when OC0A flag enabled
}

void init_timer2(void)
{
	/*
		FOR THE LED
	*/
	//set the second timer to have a pwm of (Z + 1) * 100 Hz = 800Hz
	TCCR2A |= (1 << WGM21) | (1 << WGM20) | (1 << COM2B1) | (1 << COM2B0) | (1 << COM2A1); // Set the Timer Mode to fpwm
	// When OC0A flag hit then set flag when at BOTTOM clear bit
	/*
		want our max time to achieve at 1.25ms
		1 -> 2^8 * 1/16Hz = 16u < 1.25m
		8 -> 16u * 8 = 128u < 1.25m
		16 -> 16u * 64 = 1024u < 1.25m
		256 -> 16u * 256 = 4068u = 4.068m > 1.25m
	** general pre scale for program to run smooth at 256 system clock **
	*/
	// set pre scaler to 256 and start the timer to have the max be 5ms 
	OCR2A = 77;// one cycle less than ~5ms
	OCR2B = 38;//default 50% duty cycle
	TCCR2B |= (1 << WGM22) | (1 << CS22) | (1 << CS21);
	TIMSK2 |= (1 << OCIE2A) | (1 << OCIE2B); //activate interrupts when OC2A and OC2B flag enabled
}

ISR(TIMER0_COMPB_vect)
{
	cli();// turn off interrupts
	uint8_t rw = 0;
	uint8_t cl = 0;
	uint8_t cnt = 1;
	for(uint8_t i = 3; i < 6; ++i)//loop through each row as PORTD0:2 (four total)
	{
		//turning off the output to the keypad at row i
		PORTD &= ~(1 << i);
		for(uint8_t j = 0; j < 3; ++j)//loop through each column PORTB0:2 (four total)
		{
			if(!(PINB & (1 << j)))//if there is a short of a row then the input will be 0
			{
				OCR2B = (((OCR2A + 1)*(cnt) / 10) - 1);
				row = rw;
				col = cl;
				//PORTB |= (1 << 5);
				/*
					77 + 1 = 78
					78 * (ex: 5) = 390
					390 / 10 = 39 
					39 - 1 = 38
					gives 50% duty cycle without losing bits to decimal places
					lose of decimal values would be if equations was ((OCR2A + 1)*(cnt / 10) - 1 
					77 + 1 = 78
					(ex: 5) / 10 = 0
					77 * 0 = 0 
					0 + 1 = 1 INVALID VALUE
				*/
				//default truncation will suffice
				//PORTB &= ~(1 << 5);
			}
			++cl;		
			++cnt;
		}
		PORTD |= (1 << i);//return the output of 1 to current row
		++rw;
	}
	sei();//turn on interrupts
}
ISR(TIMER2_COMPB_vect)
{
	cli();
	PORTB &= ~(1 << 5);//turn off after % time 
	sei();
}
ISR(TIMER2_COMPA_vect)
{
	cli();
	PORTB |= (1 << 5);//turn on when timer begins at time BOTTOM
	sei();
}

ISR(USART0_UDRE_vect)
{
	UDR0 = look_up[row][col];
}

int main(void)
{
	sei();//turn on interrupts
	init_IO();//set io pins of keypad
	init_timer0();//set timer0 settings
	init_timer2();//set timer2 settings
	init_USART();
	//PORTB |= (1 << 5);
    while (1) 
    {
		//nothing should be in here		
    }
}

