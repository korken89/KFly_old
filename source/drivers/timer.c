/******************************************************************************
 * @file :   	timer.c
 * @brief : 	Timer functions 
 * @version : 	V1.0
 * @author :	Emil Fresk
 ******************************************************************************/

#include "timer.h"

void Timer0_Init(void)
{
	LPC_SC->PCONP |= (1<<PCTIM0);			// Power up the timer
	LPC_SC->PCLKSEL0 |= (3<<PCLK_TIMER0);	// Timer clock source CLK/8 = 12.5MHz
	LPC_TIM0->PR = 1249;					// Timer prescaler
	LPC_TIM0->MR0 = 5000;					// Match value,	Frequency of interrupts = 12.5MHz/((PR+1)*MR0)
	LPC_TIM0->MCR = (1<<MR0I)|(1<<MR0R);	// Interupt on Match & Reset Counter
	LPC_TIM0->TCR = (1<<TIM_START);			// Start Timer
	LPC_TIM0->IR |= (1<<0);					// Clear interupt
	NVIC_EnableIRQ(TIMER0_IRQn);			// Enable the interrupt
}

/**
 * @brief 	Timer1 is for the timing of inputs and other miscellaneous stuff.
 * 			Counting at 1 MHz, no interrupt, just reset on match.
 */
void Timer1_Init(void)
{
	LPC_SC->PCONP |= (1<<PCTIM1);			// Power up the timer
	LPC_SC->PCLKSEL0 |= (1<<PCLK_TIMER1);	// Timer clock source CLK = 100MHz
	LPC_TIM1->PR = 99;						// Timer prescaler
	LPC_TIM1->MR0 = 0xFFFFFFFF;				// Match value,	Frequency of reset = 100MHz/((PR+1)*MR0)
	LPC_TIM1->MCR = (1<<MR0R);				// Reset Counter on Match
	LPC_TIM1->TCR = (1<<TIM_START);			// Start Timer
}

void Timer2_Init(void)
{
	LPC_SC->PCONP |= (1<<PCTIM2);			// Power up the timer
	LPC_SC->PCLKSEL1 |= (1<<PCLK_TIMER2);	// Timer clock source CLK = 100MHz
	LPC_TIM2->PR = 0;						// Timer prescaler
	LPC_TIM2->MR0 = 0xFFFFFFFF;				// Match value,	Frequency of reset = 100MHz/((PR+1)*MR0)
	LPC_TIM2->MCR = (1<<MR0R);				// Reset Counter on Match
	LPC_TIM2->TCR = (1<<TIM_START);			// Start Timer
}

void Timer2_StartCount(void)
{
	LPC_TIM2->TCR |= (1<<TIM_RESET);		// Reset and halt timer
	LPC_TIM2->TCR &= ~(1<<TIM_RESET);		// Start counting clock cycles!
}

void Timer2_StopCount(void)
{
	int tmp = LPC_TIM2->TC;
	UART0_SendString(itoa(tmp-15,10));		// 15 is the number of cycles it takes for the timing functions to execute.
	UART0_SendString(" clocks\0");
}

void Timer0_IRQHandler(void)
{
	LPC_TIM0->IR |= (1<<0);					// Clear interupt
	EINT_NoConnectionCheck();
}
