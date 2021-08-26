 /******************************************************************************
 *
 * File Name: uart.c
 *
 * Description: Source file for the UART AVR driver
 *
 * Author: islam
 *
 * Created on: Nov 6, 2020
 *
 *******************************************************************************/

#include "uart.h"

#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 8UL))) - 1)

/***********************************************************************
 *                      Function Definitions                           *
 ***********************************************************************/
void UART_init(const Uart_ConfigType * Config_Ptr)
{
	UCSRA = (1<<U2X);
	UCSRB = (1<<RXEN) | (1<<TXEN);


	SET_BIT(UCSRC,URSEL);
	//CLEAR_BIT(UCSRC,UCPOL);
	UCSRC |= ((Config_Ptr->char_size)<<UCSZ0) | ((Config_Ptr->stop_bit)<<USBS) | ((Config_Ptr->partiy_mode)<<UPM0) | ((Config_Ptr->mode)<<UMSEL);
/*
    UCSRC = (UCSRC & 0xF8) | ((Config_Ptr->char_size)<<UCSZ0);
	UCSRC = (UCSRC & 0xF4) | ((Config_Ptr->stop_bit)<<USBS);
	UCSRC = (UCSRC & 0xCF) | ((Config_Ptr->partiy_mode)<<UPM0);
	UCSRC = (UCSRC & 0xBF) | ((Config_Ptr->mode)<<UMSEL);

 */
	UBRRH = BAUD_PRESCALE>>8;
	UBRRL = BAUD_PRESCALE;

}

void UART_sendByte(const uint8 data)
{
	while(BIT_IS_CLEAR(UCSRA,UDRE));
	UDR = data;
}

uint8 UART_recieveByte(void)
{
	while(BIT_IS_CLEAR(UCSRA,RXC));
	return UDR;
}

void UART_sendString(const uint8 *Str)
{
	uint8 i = 0;
	while(Str[i] != '\0')
	{
		UART_sendByte(Str[i]);
		i++;
	}
}

void UART_receiveString(uint8 *Str)
{
	uint8 i = 0;
	Str[i] = UART_recieveByte();
	while(Str[i] != '#')
	{
		i++;
		Str[i] = UART_recieveByte();
	}
	Str[i] = '\0';
}
