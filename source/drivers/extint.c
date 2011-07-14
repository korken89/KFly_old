#include "extint.h"

volatile static input_data *sData;
volatile static uint8_t check = 0;

void EINT_Init(input_data *sDataLocation)	
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
	
	sData = sDataLocation;
	
	NVIC_EnableIRQ(EINT3_IRQn);				//Enable Interrupts for GPIO
}

void EINT_NoConnectionCheck(void)
{
	if (check == 0)
		check++;
	else if (check > 0)
		sData->status |= 0b01111110;		// No connection on all inputs
}

void EINT3_IRQHandler (void)	
{	
	static uint32_t temp[6] = {0, 0, 0, 0, 0, 0};			// Temporary measurement data
	static uint32_t ch_timing[6] = {0, 0, 0, 0, 0, 0};		// No connection counter
	
	uint32_t statF = LPC_GPIOINT->IO0IntStatF & INTMASK;	// Get status on all falling edge interrupts
	uint32_t statR = LPC_GPIOINT->IO0IntStatR & INTMASK;	// Get status on all rising edge interrupts
	LPC_GPIOINT->IO0IntClr = INTMASK;						// Clear all interrupts
	
	uint32_t Ticks = GetTickCount();
	check = 0;			// Reset check counter
	
	/**
	 * 	Channel 1 - 6 check and measurement.
	 *  If there are no inputs the checks can't be made so another function
	 *  in a timer interrupt will check the status of the whole thing.
	 * 	But if one or more inputs are avalible then this function will check them.
	 **/
	
	for (uint8_t i = 0; i < 6; i++)
	{
		if (statR & (1<<(INT_CH1 +i))) 				/* Code for Rising edge of Channel 1-6 */
			temp[i] = Ticks;
		
		else if (statF & (1<<(INT_CH1 +i)))			/* Code for Falling edge of Channel 1-6 */
		{
			sData->ch[i] = Ticks - temp[i];			// Calculate
			ch_timing[i] = Ticks;					// Reset counter for no connection bit
			sData->status &= (1<<(i+1));			// Clear the no connection bit
			
		}
		else if ((Ticks - ch_timing[i]) > 200000) 	// No signal for 10 periods (200ms) set no connection bit
			sData->status |= (1<<(i+1));			// No connection, set the no connection bit
	}
}
