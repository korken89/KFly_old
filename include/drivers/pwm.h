#ifndef PWM_H_INCLUDED
#define PWM_H_INCLUDED
#include "LPC17xx.h"
#include "bitvals.h"

#define MAX_PWM		1000

void PWM_Init(void);
void PWM_setOutput(int16_t, uint8_t);

#endif
