
 /******************************************************************************
 *
 * File Name: MC2.c
 *
 * Description: the main functio of the MC2
 *
 * Author: islam
 *
 *  Created on: Dec 15, 2020
 *
 *******************************************************************************/

#include <string.h>
#include "external_eeprom.h"
#include "uart.h"
#include "timer0.h"

#define USED_FIRST 0x05
#define FIRST_USED_VAL_LOCATION 0x0D
#define PASSWORD_LOCATION 0x320
#define M1_READY 0x01


uint8 volatile g_tick = 0, tick;

void ReceivePassword(void);
void SendOldPassword(void);
void PrepareToSend(uint8 array1[], uint8 array2[]);
void open_CloseDoor(void);
void Prepare_Motor_Timer(void);
void TurnOnBuzzer(void);
void TurnOffBuzzer(void);
void SetFirstUsed(void);
void UsingCheck(void);
void UART_Message(uint8 message);





void main()
{
	uint8 msg;

	SET_BIT(DDRC,0); //Make Buzzer as Output Pin


	Uart_ConfigType Uart_Config = {ASYNCHRONOUS,DISABLED,ONE_BIT,_8_BIT}; //Configuration UART
	UART_init(&Uart_Config); //Initialize UART
	EEPROM_init();
	/*Timer0_ConfigType Timer0_Config = {CTC,NORMAL_OC0_DISCONNECTED,F_CPU_64,CTC_INTERRUPT};
	SREG |=(1<<7);
	Timer0_init(&Timer0_Config);
	Timer0_setCallBack();*/

	while(1)
	{
		msg = UART_recieveByte();
		UART_Message(msg);
	}

}

/***********************************************************
 *                        Functions                        *
 ***********************************************************/
/* Function that receive the password and write it in EEPROM  */
void ReceivePassword(void)
{
	uint8 firstPassword[6] ;

	UART_receiveString(firstPassword);
	EEPROM_writeString(PASSWORD_LOCATION, firstPassword);

}

/* Function that send the password that saved in EEPROM to MC1 by UART */
void SendOldPassword(void)
{
	uint8 okay=0;
	uint8  oldPassword1 [6] , oldPassword2[6];
	EEPROM_readString(PASSWORD_LOCATION, oldPassword1, 6);
	PrepareToSend(oldPassword1, oldPassword2);
	okay=UART_recieveByte();
	while(okay != M1_READY){}; // wait until MC2 is ready

	UART_sendString(oldPassword2);

}

/* Function to add # to the new password in the buffer that is save in to prepare send it by UART to MC1 */
void PrepareToSend(uint8 array1[], uint8 array2[])
{
	int i=0;
	for(i=0; i<6; i++)
	{
		array2[i] = array1[i];
		if(i == 5)
		{
			array2[i] = '#';
		}
	}

}

/* It open the door for 15s by rotate motor clock wise then close the door after 15s by rotate motor anti clock wise then stop */
void open_CloseDoor(void)
{
	g_tick++;
	if(g_tick == 120)
	{
		//Unti_clockwise
		SET_BIT(PORTC,PC6);
		CLEAR_BIT(PORTC,PC7);
	}
	else if(g_tick == 240)
	{
		//Stop Motor
		CLEAR_BIT(PORTC,PC6);
		CLEAR_BIT(PORTC,PC7);
		//g_tick=0;
	}
}

void Prepare_Motor_Timer(void)
{
	//Make Motor as Output Pins
	//DDRC|=(1<<PC6)|(1<<PC7);
	SET_BIT(DDRC,PC6);
	SET_BIT(DDRC,PC7);

	SET_BIT(PORTC,PC7);
	CLEAR_BIT(PORTC,PC6);

	Timer0_ConfigType Timer0_Config = {CTC,NORMAL_OC0_DISCONNECTED,F_CPU_1024,CTC_INTERRUPT,0,250};
	SREG |=(1<<7);
	Timer0_init(&Timer0_Config);
	Timer0_setCallBack(open_CloseDoor);
}

void TurnOnBuzzer(void)
{
	SET_BIT(PORTB,0);
	Timer0_ConfigType Timer0_Config = {CTC,NORMAL_OC0_DISCONNECTED,F_CPU_1024,CTC_INTERRUPT,0,250};
	SREG |=(1<<7);
	Timer0_init(&Timer0_Config);
	Timer0_setCallBack(TurnOffBuzzer);
}

void TurnOffBuzzer(void)
{
	tick ++;
		if (tick == 120)
		{
			CLEAR_BIT(PORTB,0);
			UART_sendByte(BUZZER_OFF);
			//tick =0;
		}
}

void SetFirstUsed(void)
{
	EEPROM_writeByte(FIRST_USED_VAL_LOCATION, USED_FIRST);
}

void UsingCheck(void)
{
	uint8 val;
	EEPROM_readByte(FIRST_USED_VAL_LOCATION,&val);
	UART_sendByte(val);
}

/* Identify the messages that MC1 transmit it to MC2  by UART to make MC2 take actions according to it */
void UART_Message(uint8 message)
{
	switch(message)
	{
	case SAVE_PASSWORD :
		ReceivePassword();
		break;
	case OPEN_DOOR_REQ :
		SendOldPassword();
		break;
	case OPEN_DOOR :
		Prepare_Motor_Timer();
		break;
	case CHANGE_PASSWORD :
		SendOldPassword();
		break;
	case WRONG_PASSWORD :
		TurnOnBuzzer();
		break;
	case FIRST_USE_CHEK :
		UsingCheck();
		break;
	case SET_FIRST_USE :
		SetFirstUsed();
		break;
	}
}

