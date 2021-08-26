 /******************************************************************************
 *
 * File Name: timer0.h
 *
 * Description: Header file for the Timer0 AVR driver
 *
 * Author: islam
 *
 * Created on: Nov 15, 2020
 *
 *******************************************************************************/

#ifndef TIMER0_H_
#define TIMER0_H_

#include "std_types.h"
#include "common_macros.h"
#include "micro_config.h"

/********************************************************
 *                    Types Declaration                 *
 ********************************************************/
typedef enum
{
	OVERFLOW,CTC=2
}Timer0_Mode;

typedef enum
{
	NORMAL_OC0_DISCONNECTED,TOGGEL_OC0,CLEAR_OC0,SET_OC0
}Timer0_CTC_Mode;

typedef enum
{
	NO_CLOCK,F_CPU_CLOCK,F_CPU_8,F_CPU_64,F_CPU_256,F_CPU_1024
}Timer0_Clock;

typedef enum
{
	NO_INTERRUPT,OVERFLOW_INTERRUPT,CTC_INTERRUPT
}Timer0_Interrupt_Mode;

typedef struct
{
	Timer0_Mode mode;
	Timer0_CTC_Mode ctc_mode;
	Timer0_Clock clock;
	Timer0_Interrupt_Mode enable_interrupt;
	uint8 ctc_inital_value;
	uint8 ctc_final_value;
}Timer0_ConfigType;

/********************************************************
 *                    Function Prototypes               *
 ********************************************************/
void Timer0_init(const Timer0_ConfigType * Config_Ptr);
void Timer0_setCallBack(void(*a_ptr)(void));

#endif /* TIMER0_H_ */
