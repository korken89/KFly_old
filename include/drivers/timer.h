#ifndef __TIMER_H__
#define __TIMER_H__

#include "LPC17xx.h"
#include "bitvals.h"
#include "extint.h"
#include "uart.h"
#include "fmath.h"

#define GetTickCount() 	LPC_TIM1->TC

void Timer0_Init(void);
void Timer1_Init(void);
void Timer2_Init(void);
void Timer2_StartCount(void);
void Timer2_StopCount(void);
void Timer0_IRQHandler(void);

#endif
