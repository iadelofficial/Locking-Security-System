 /******************************************************************************
 *
 * File Name: uart.h
 *
 * Description: Header file for the UART AVR driver
 *
 * Author: islam
 *
 * Created on: Nov 6, 2020
 *
 *******************************************************************************/
#ifndef UART_H_
#define UART_H_

#include "micro_config.h"
#include "std_types.h"
#include "common_macros.h"
#include "uart_messeage.h"
/********************************************************
 *                    Types Declaration                 *
 ********************************************************/
typedef enum
{
	ASYNCHRONOUS,SYNCHRONOUS
}Uart_Mode;

typedef enum
{
	DISABLED,RESERVED,EVEN,ODD
}Uart_Partiy_Mode;

typedef enum
{
	ONE_BIT,TWO_BIT
}Uart_StopType;

typedef enum
{
	_5_BIT,_6_BIT,_7_BIT,_8_BIT
}Uart_CharSize;

typedef struct
{
	Uart_Mode mode;
	Uart_Partiy_Mode partiy_mode;
	Uart_StopType stop_bit;
	Uart_CharSize char_size;
}Uart_ConfigType;

/**********************************************************************
 *                           Preprocessor Macros                      *
 **********************************************************************/

// UART Driver Baud Rate
#define USART_BAUDRATE 9600

/***********************************************************************
 *                          Function Prototypes                        *
 ***********************************************************************/
void UART_init(const Uart_ConfigType * Config_Ptr);

void UART_sendByte(const uint8 data);

uint8 UART_recieveByte(void);

void UART_sendString(const uint8 *Str);

void UART_receiveString(uint8 *Str);

#endif /* UART_H_ */
