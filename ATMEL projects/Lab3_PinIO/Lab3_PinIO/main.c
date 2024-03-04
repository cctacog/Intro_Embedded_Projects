/*
 * Lab3_PinIO.c
 *
 * Created: 2/9/2024 12:16:36 PM
 * Author : Sophia Garcia
 */ 
#define F_CPU 16000000UL
#define BAUD 9600
#define BAUDRATE (F_CPU/(16UL * BAUD) - 1)

#include <avr/io.h>
#include <avr/delay.h>

void init_IO(void)
{
	DDRD |= (1 << DDD4)|(1 << DDD5)|(1 << DDD6)|(1 << DDD7);//SET PORT D TO OUTPUT
	DDRB &= ~(1 << DDB0)|~(1 << DDB1)|~(1 << DDB2)|~(1 << DDB3);//SET PORT B TO INPUT [PIN]
	PORTB |= (1 << PORTB0)|(1 << PORTB1)|(1 << PORTB2)|(1 << PORTB3);//SET PIN B TO PULL UP RESISTOR	
	
	//PRESET THE OUTPUTS ALL AS HIGH
	PIND |= (1 << 4)|(1 << 5)|(1 << 6)|(1 << 7);
}

void init_USART(void)
{
	//Set baud rate 
	UBRR0H = 0;
	UBRR0L = BAUDRATE;
	//Enable receiver and transmitter 
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	// Set frame format: 8 data, default 1 stop bit
	UCSR0C |= (1<<UCSZ00)|(1<<UCSZ01); 
	/*
	UCSR0C is data size for USART comm
	the bits for this are UCSZ[2:0]
	
	leave USBn as 0 because we only want 1 stop bit
	
	Parity bit would be by UPM[1] = 1 (bit) from UPM[1:0]
	*/
}

void transmit(uint8_t data)
{
	while (!( UCSR0A & (1<<UDRE0)));
	//until bit 5 of UCSR0A[7:0] is 1 such as UDRE0 means the data register is empty
	//gives ability to send info without mixing with previous info sent/sending
	UDR0 = data;

}

void keyPadScan(void)
{
	uint8_t keyPad[4][4] = {{'1', '2', '3', 'A'}, {'4', '5', '6', 'B'}, {'7', '8', '9', 'C'}, {'*', '0', '#', 'D'}};
	/*
		2d array representing the keypad configuration
	*/
	for(uint8_t i = 4; i < 8; ++i)//loop through each row as PORTD4-7 (four total)
	{
		//turning off the output to the keypad at row i
		PORTD &= ~(1 << i);
		for(uint8_t j = 0; j < 4; ++j)//loop through each column PORTB0-3 (four total)
		{									
			if(!(PINB & (1 << j)))//if there is a short of a row then the input will be 0
			{
				transmit(keyPad[i-4][j]);//transmit the character from the array through TX
				while(!(PINB & (1 << j)));//while the input is being shorted to 0 then stay in if-statement
				_delay_ms(50);//de-bounce delay of max 50 milliseconds
			}
		}
		PORTD |= (1 << i);//return the output of 1 to current row 
	}
}

int main(void)
{
	init_IO();//initialization method
	init_USART();//uart initialization
    while (1) 
    {
		keyPadScan();//loop through the keypad 2d array 
    }
}