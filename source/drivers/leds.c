#include "leds.h"

void LED_Init(void)
{
	LPC_PINCON->PINSEL2 &= ~((3<<0)|(3<<2)|(3<<8));        //set bits 9:8 3:2 1:0 to 00 00 00. GPIO port 0,1 4.
	LPC_GPIO1->FIODIR |= (1<<0)|(1<<1)|(1<<4);            //set bit 0 1 4 to one. Set pin 0,1,4 to output.
}

void setLED(int ledNum)
{
    switch(ledNum)
    {
        case 1:
            LPC_GPIO1->FIOSET = (1<<0);		//sets bit 0 to 1.
            break;
        case 2:
            LPC_GPIO1->FIOSET = (1<<1);		//sets bit 1 to 1.
            break;
        default:
            break;
    }	
}

void clearLED(int ledNum)
{
    switch(ledNum)
    {
        case 1:
            LPC_GPIO1->FIOCLR = (1<<0);   //sets bit 0 to 1.
            break;
        case 2:
            LPC_GPIO1->FIOCLR = (1<<1);   //sets bit 1 to 1.
            break;
        default:
            break;
    }
}
