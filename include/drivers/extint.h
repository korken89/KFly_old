#ifndef __EXTINT_H  
#define __EXTINT_H 

#include "LPC17xx.h"
#include "bitvals.h"
#include "timer.h"
#include "leds.h"

#define INT_CH1		4
#define INT_CH2		5
#define INT_CH3		6
#define INT_CH4		7
#define INT_CH5		8
#define INT_CH6		9

#define INTMASK 	((1<<INT_CH1)|(1<<INT_CH2)|(1<<INT_CH3)|(1<<INT_CH4)|(1<<INT_CH5)|(1<<INT_CH6))

struct _input_data
{
	uint8_t status;		// bit cleared = functioning normaly, bit set = connection lost
						// bit 1 = ch1, bit 2 = ch2, ... bit 6 = ch6	
	int16_t ch[6];		// Channel values
};
typedef struct _input_data input_data;

void EINT_Init(input_data *sDataLocation); 
void EINT_NoConnectionCheck(void);
void EINT3_IRQHandler(void);

#endif
