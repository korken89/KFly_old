#include "extint.h"

volatile input_data sData;
volatile uint8_t check = 0;

void EINT_Init(void)	
{ 
	/* Pin settings */
	LPC_SC->PCONP |= (1<<PCGPIO); 								// Turn on power for GPIO  
	LPC_GPIO0->FIODIR0 &= ~((1<<4)|(1<<5)|(1<<6)|(1<<7));		// Set P0[4-9] as inputs	
	LPC_GPIO0->FIODIR1 &= ~((1<<0)|(1<<1));						//
	LPC_PINCON->PINMODE0 |=  ((3<<8)|(3<<10)|(3<<12));			// Enable pull-downs on the inputs
	LPC_PINCON->PINMODE0 |=  ((3<<14)|(3<<16)|(3<<18));			//
	
	/* Interrupt specific settings */
	LPC_GPIOINT->IO0IntEnR = INTMASK;		// Enable interrupts on P0[4-9] at falling and rising edge
	LPC_GPIOINT->IO0IntEnF = INTMASK;		//
	
	for (int i = 0; i < 6; i++)
		sData.ch[i] = 0;			// All inputs to zero
	
	//sDataLocation = &sData;
	Timer0_Init();
	Timer1_Init();
	
	NVIC_EnableIRQ(EINT3_IRQn);				//Enable Interrupts for GPIO
}

input_data *EINT_GetPointerToValues(void)
{
	return (input_data *)&sData;
}

void EINT_NoConnectionCheck(void)
{
	if (check == 0)
		check++;
	else if (check > 0)
		for (uint8_t i = 0; i < 6; i++)
				sData.ch[i] = 0;	
}

void EINT3_IRQHandler (void)	
{	
	static uint32_t temp[6] = {0, 0, 0, 0, 0, 0};			// Temporary measurement data
	static uint32_t nocon[6] = {0, 0, 0, 0, 0, 0};			// Time since last input
	
	uint32_t statF = LPC_GPIOINT->IO0IntStatF & INTMASK;	// Get status on all falling edge interrupts
	uint32_t statR = LPC_GPIOINT->IO0IntStatR & INTMASK;	// Get status on all rising edge interrupts
	LPC_GPIOINT->IO0IntClr = INTMASK;						// Clear all interrupts
	
	uint32_t Ticks = GetTickCount();
	check = 0;
	
	/**
	 * 	Channel 1 - 6 measurement
	 **/
	
	for (uint8_t i = 0; i < 6; i++)
	{
		if (statR & (1<<(INT_CH1 +i))) 				/* Code for Rising edge of Channel 1-6 */
			temp[i] = Ticks;
		
		else if (statF & (1<<(INT_CH1 +i)))			/* Code for Falling edge of Channel 1-6 */
		{
			sData.ch[i] = Ticks - temp[i];			// Calculate
			nocon[i] = Ticks;
		}
		else if ((Ticks - nocon[i]) > 200000)
			sData.ch[i] = 0;
			
	}
}
