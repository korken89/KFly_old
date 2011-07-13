/******************************************************************************
 * @file :   	system_LPC17xx.c
 * @brief : 	Clock and Flash Accelerator setup for the MCU. Kept names for
 * 				simplicity in LPC17xx.h
 * @version : 	V1.0
 * @author :	Emil Fresk
 ******************************************************************************/

#include <stdint.h>
#include "LPC17xx.h"
#include "system_LPC17xx.h"

volatile uint32_t SystemFrequency = OSC_CLK;

void SystemInit(void)
{
	LPC_SC->SCS = SCS_Val;
	while (!(LPC_SC->SCS & (1<<OSCSTAT)));		// Wait for Oscillator to be ready

	LPC_SC->CLKSRCSEL = CLKSRCSEL_Val;			// Select Clock Source for PLL0
	LPC_SC->PLL0CFG   = PLL0CFG_Val;
	LPC_SC->PLL0CON   = (1<<PLLE0);				// PLL0 Enable
	LPC_SC->PLL0FEED  = 0xAA;					// Feed sequence
	LPC_SC->PLL0FEED  = 0x55;					// Feed sequence
	while (!(LPC_SC->PLL0STAT & (1 << PLOCK0)));// Wait for PLOCK0
	LPC_SC->CCLKCFG   = CCLKCFG_Val;			// Setup Clock Divider

	LPC_SC->PLL0CON   = (1<<PLLE0)|(1<<PLLC0);	// PLL0 Enable & Connect
	LPC_SC->PLL0FEED  = 0xAA;					// Feed sequence
	LPC_SC->PLL0FEED  = 0x55;					// Feed sequence
	
	LPC_SC->FLASHCFG  = FLASHCFG_Val;
	
	/** Lite osäker vart jag hittade den här koden, med det var på NXPs hemsida någon stanns */
	SystemFrequency = 	(OSC_CLK *
                        (((2 * ((LPC_SC->PLL0STAT & 0x7FFF) + 1))) /
                        (((LPC_SC->PLL0STAT >> 16) & 0xFF) + 1))   /
                        ((LPC_SC->CCLKCFG & 0xFF)+ 1));
                        
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");
}
