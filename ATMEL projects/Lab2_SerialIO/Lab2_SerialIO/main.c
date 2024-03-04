/*
 * Lab2_SerialIO.c
 *
 * Created: 2/2/2024 8:58:07 AM
 * Author : Sophia Garcia
 */ 
#define F_CPU 16000000UL
//set baud equal to that of comm device(s)
#define BAUD 9600
#define BAUDRATE (F_CPU/(16UL * BAUD) - 1)
//equation to define baud rate based on baud (bits per second)
/*
if baud rate faster than receiver: will skips bits based upon difference
if baud rate slower than receiver: will duplicate bits based upon difference
*/
#include <avr/io.h>
//library for AVR key terms
#include <util/delay.h>
//delay

void USART_initz()
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
	
	leave USBn as 0 bc we only want 1 stop bit
	
	Parity bit would be by UPM[1] = 1 (bit) from UPM[1:0]
	*/
	 
}

void USART_transmit(const char data)
{
	while (!( UCSR0A & (1<<UDRE0)));
	//until bit 5 of UCSR0A[7:0] is 1 such as UDRE0 means the data register is empty 
	//gives ability to send info without mixing with previous info sent/sending
	UDR0 = data;
}

uint8_t USART_reciever(void)
{
	while(!((UCSR0A) & (1<<RXC0)));
	//when ready flag has been complete from our side then 
	//THIS IS NOT NEEDED BECAUSE NOT RECIEVEING INFO FROM FT232R :)
	return UDR0;
}

int main(void)
{
    USART_initz();
	//initialize and set ATmega to USART mode 
	char redID[] = "826433407\r\n";
	//string per character to be transmitted
    while (1) 
    {
		static uint8_t cnt = 0;
		//counter only to be defined and initialized once
		USART_transmit(redID[cnt++]);
		//send in current char at string then increment to next char by ctr increment by 1
		if(redID[cnt] == '\0')
		{
			cnt = 0;
			//reset counter when reach end of string 
			_delay_ms(600);
			//delay by 500 ms in between each string complete transfer
		}
		
    }
}