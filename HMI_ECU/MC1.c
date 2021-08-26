 /******************************************************************************
 *
 * File Name: MC1.c
 *
 * Description: the main functio of the HMI_ECU
 *
 * Author: islam
 *
 *  Created on: Dec 10, 2020
 *
 *******************************************************************************/
#include <string.h>
#include "keypad.h"
#include "lcd.h"
#include "uart.h"
#include "timer0.h"

void SignUp(void);
void SystemSetting(void);
void EnterPassword(uint8 *passwordArr);
void PrepareToSend(uint8 array1[], uint8 array2[]);
void Door_Closing(void);
void PrepareTimer(void);


#define USED_FIRST 0x05   /* The value that will be save in the Internal EEPROM in the first time of the program */
#define EXTERNAL_EEPROM_ADDRESS
#define M1_READY 0x01


uint8 flag=0; /* This flag for switch between system setting and sign up functions */
uint8 passwordMessage[6]; /* Contains the string to be sent by UART_sendString */
uint8 g_tick=0;
uint8 openingMsgFlag=0;


int main (void)
{
	uint8 val;

	LCD_init();  //Initialize LCD
	Uart_ConfigType Uart_Config = {ASYNCHRONOUS,DISABLED,ONE_BIT,_8_BIT}; //Configuration UART
	UART_init(&Uart_Config); //Initialize UART
	//Timer0_ConfigType Timer0_Config = {CTC,NORMAL_OC0_DISCONNECTED,F_CPU_64,CTC_INTERRUPT};
	//SREG |=(1<<7);
	//Timer0_init(&Timer0_Config);
	//Timer0_setCallBack(Door_Closing);

	UART_sendByte(FIRST_USE_CHEK);
	val = UART_recieveByte();



	if(val != USED_FIRST)
	{
		flag = 0;
	}
	else if(val == USED_FIRST)
	{
		flag = 1;
	}

	while(1)
	{
		if(flag == 0)
		{
			SignUp();
		}

		else if(flag == 1)
		{
			if(openingMsgFlag == 0)
			{
				SystemSetting();
			}
		}
	}
}


/****************************************************************
 *                   Functions                                  *
 ****************************************************************/
void SignUp(void)
{
	uint8 firstPassword[6] = {0}, secondPassword[6] = {0};

	LCD_clearScreen();
	LCD_displayString("Please enter new");
	LCD_goToRowColumn(1,0);
	LCD_displayString("password ");

	EnterPassword(firstPassword);

	while(1)
	{
		LCD_clearScreen();
		LCD_displayString("rewrite new Pass");
		LCD_goToRowColumn(1,0);

		EnterPassword(secondPassword);



		if(strcmp(firstPassword,secondPassword) == 0)
		{
			UART_sendByte(SET_FIRST_USE);
			flag = 1;
			LCD_clearScreen();
			LCD_displayString("Done");
			_delay_ms(200);
			PrepareToSend(firstPassword,passwordMessage);
			UART_sendByte(SAVE_PASSWORD);
			UART_sendString(passwordMessage);

			break;
		}
		else
		{
			//flag=0;

			LCD_clearScreen();
			LCD_displayString("Invalid Match");

			_delay_ms(300);
		}
	}
}
/***********************************************************************************************/

void SystemSetting(void)
{
	uint8 MyPassword[6];
	uint8 OldPassword[6];
	uint8 count = 0;

	LCD_clearScreen();
	LCD_displayString("+:ChangePassword");
	LCD_goToRowColumn(1,0);
	LCD_displayString("-: Open The Door");
	_delay_ms(500);

	// Change Password
	if(KeyPad_getPressedKey() == '+')
	{
		uint8 CurrentPass[6];
		UART_sendByte(CHANGE_PASSWORD);

		UART_sendByte(M1_READY); // MC1 is ready

		UART_receiveString(OldPassword);

		while(1)
		{
			LCD_clearScreen();
			LCD_displayString("Enter Old Pass");
			LCD_goToRowColumn(1,0);

			EnterPassword(CurrentPass);

			if(count == 2)
			{
				LCD_clearScreen();
				LCD_displayString("tried 3 times");
				UART_sendByte(WRONG_PASSWORD);

				uint8 buzzerOff = UART_recieveByte();
				if(buzzerOff == BUZZER_OFF)
					break;
			}


			if((strcmp(OldPassword,CurrentPass)) == 0)
			{
				count = 0;
				LCD_clearScreen();
				LCD_displayString("SUCCESS");
				_delay_ms(200);

				SignUp();

				break;
			}
			count++;
			LCD_clearScreen();
			LCD_displayString("Invalid Match");
			_delay_ms(200);
		}
	}

	// Open Door
	else if(KeyPad_getPressedKey() == '-')
	{
		UART_sendByte(OPEN_DOOR_REQ);
		UART_receiveString(OldPassword);

		while(1)
		{
			LCD_clearScreen();
			LCD_displayString("Enter PASS !");
			LCD_goToRowColumn(1,0);

			EnterPassword(MyPassword);

			if(count == 2)
			{
				LCD_clearScreen();
				LCD_displayString("Tried 3 Times");
				UART_sendByte(WRONG_PASSWORD);

				uint8 buzzerOff = UART_recieveByte();
				if(buzzerOff == BUZZER_OFF)
					break;
			}


			if((strcmp(OldPassword,MyPassword)) == 0)
			{
				count = 0;
				UART_sendByte(OPEN_DOOR);

				LCD_clearScreen();
				LCD_displayString("Door Is Opening");
				openingMsgFlag = 1;

				//prepare timer
				PrepareTimer();

				break;
			}
			count++;
			LCD_clearScreen();
			LCD_displayString("Invalid Match");
			_delay_ms(300);

		}
	}
}

void EnterPassword(uint8 *passwordArr)
{
	for(int i=0; i<5; i++)
	{
		passwordArr[i] = KeyPad_getPressedKey();

		LCD_displayCharacter('*');
		_delay_ms(50);
	}
	passwordArr[5]='\0';
}

/* Function to add # to the new password in the buffer that is save in to prepare send it by UART to MC1 */
void PrepareToSend(uint8 array1[], uint8 array2[])
{
	int i = 0;
	for( i=0; i<6; i++)
	{
		array2[i] = array1[i];
		if(i == 5)
		{
			array2[i] = '#';
		}
	}
}

void Door_Closing(void)
{
	g_tick++;
	if(g_tick == 120)
	{
		LCD_clearScreen();
		LCD_displayString("Door Is Closing");
	}
	else if(g_tick == 240 )
	{
		LCD_clearScreen();
		openingMsgFlag = 0;
		//g_tick=0;
	}
}
void PrepareTimer(void)
{
	Timer0_ConfigType Timer0_Config = {CTC,NORMAL_OC0_DISCONNECTED,F_CPU_1024,CTC_INTERRUPT,0,250};
	SREG |=(1<<7);
	Timer0_init(&Timer0_Config);
	Timer0_setCallBack(Door_Closing);
}


