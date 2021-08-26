 /******************************************************************************
 *
 * File Name: timer0.c
 *
 * Description: Source file for the Timer0 AVR driver
 *
 * Author: islam
 *
 * Created on: Nov 15, 2020
 *
 *******************************************************************************/

#include "timer0.h"
/*************************************************************************
 *                           Global Variables                            *
 *************************************************************************/
/* Global variables to hold the address of the call back function in the application */
uint8 g_initialCounter =0;
uint8 g_CTC_OCR0 =0;

static volatile void (*g_callBackPtr)(void) = NULL_PTR;

/*************************************************************************
 *                       Interrupt Service Routines                      *
 *************************************************************************/
ISR(TIMER0_OVF_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		(*g_callBackPtr)();
	}
}

ISR(TIMER0_COMP_vect)
{
	TCNT0 = g_initialCounter;
	if(g_callBackPtr != NULL_PTR)
	{
		(*g_callBackPtr)();
	}
}

/*************************************************************************
 *                           Function Definition                         *
 *************************************************************************/

void Timer0_init(const Timer0_ConfigType * Config_Ptr)
{
	TCNT0 = Config_Ptr->ctc_inital_value;
	/*OCR0  = Config_Ptr->ctc_value;
	TIMSK = (TIMSK & 0xFC) | (Config_Ptr->enable_interrupt);
	SET_BIT(TCCR0,FOC0);
	TCCR0 = (TCCR0 & 0xF8) | (Config_Ptr->clock);  //TCCR0 = (TCCR0 & 0xF8) | ((Config_Ptr->clock) & 0x07);
	TCCR0 = (TCCR0 & 0xCF) | ((Config_Ptr->ctc_mode) << COM00);
	TCCR0 =(((Config_Ptr->mode) & 0x01) << WGM00) | (((Config_Ptr->mode) & 0x02) << WGM01);
	*/
	TCCR0 |= (1<<FOC0) | (Config_Ptr->clock) | ((Config_Ptr->ctc_mode) << COM00) | ((Config_Ptr->mode) << WGM01);
	if(Config_Ptr->enable_interrupt == OVERFLOW)
	{
		TIMSK |= (1<<TOIE0);
	}
	else if(Config_Ptr->enable_interrupt == CTC)
	{
		TIMSK |= (1<<OCIE0);
		OCR0 = Config_Ptr->ctc_final_value;
	}


}

void Timer0_setCallBack(void(*a_ptr)(void))
{
	g_callBackPtr = a_ptr;
}
