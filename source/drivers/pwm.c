#include "pwm.h"

void PWM_Init(void)
{
	LPC_SC->PCONP |= (1<<PCPWM1);								// Power up PWM1
	LPC_SC->PCLKSEL0 |= (2<<PCLK_PWM1);							// PWM1 clock source CLK/2 = 50MHz
    LPC_PINCON->PINSEL4 |= (1<<0)|(1<<2)|(1<<4)|(1<<6);			//set bits 7:6 5:4 3:2 1:0 to 0b01. PWM port 0, 1, 2 & 3.
    LPC_GPIO2->FIODIR |= (1<<0)|(1<<1)|(1<<2)|(1<<3);
    
    LPC_PWM1->PR = 49;
    LPC_PWM1->MR0 = 2499;					// Frequency = 50Mhz/50/2500 = 400Hz
    LPC_PWM1->MCR |= (1<<PWMMR0R);			// MR0 resets the PWM
    LPC_PWM1->TCR |= (1<<0)|(1<<3);			// Start PWM
    LPC_PWM1->PCR &= ~((1<<PWMSEL2)|(1<<PWMSEL3)|(1<<PWMSEL4)|(1<<PWMSEL5)|(1<<PWMSEL6)); 	// Single edge PWM
    LPC_PWM1->PCR |= (1<<PWMENA1)|(1<<PWMENA2)|(1<<PWMENA3)|(1<<PWMENA4);					//Enable channel 1, 2, 3 and 4.
        
	for (uint8_t i = 0; i < 6; i++)
		PWM_setOutput(0, i);
}

void PWM_setOutput(int16_t value, uint8_t channel)
{
	if (value > MAX_PWM)
		value = MAX_PWM;
	
	else if (value < 0)
		value = 0;
		
    switch(channel)
    {
		case 0:
			LPC_PWM1->MR1 = PWM_LOW-value;
			break;
            
		case 1:
			LPC_PWM1->MR2 = PWM_LOW-value;
			break;
            
		case 2:
			LPC_PWM1->MR3 = PWM_LOW-value;
            break;
            
        case 3:
			LPC_PWM1->MR4 = PWM_LOW-value;
            break;
            
		case 4:
			LPC_PWM1->MR5 = PWM_LOW-value;
            break;
            
        case 5:
			LPC_PWM1->MR6 = PWM_LOW-value;
            break;
            
        default:
            return;
    }
    
    LPC_PWM1->LER |= (1<<channel);
}
