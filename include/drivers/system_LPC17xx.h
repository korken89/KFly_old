/**************************************************************************//**
 * @file     system_LPC17xx.h
 * @brief    CMSIS Cortex-M3 Device Peripheral Access Layer Header File
 *           for the NXP LPC17xx Device Series
 * @version  V1.02
 * @date     08. September 2009
 *
 * @note
 * Copyright (C) 2009 ARM Limited. All rights reserved.
 *
 * @par
 * ARM Limited (ARM) is supplying this software for use with Cortex-M 
 * processor based microcontrollers.  This file can be freely distributed 
 * within development tools that are supporting such ARM based processors. 
 *
 * @par
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ******************************************************************************/


#ifndef __SYSTEM_LPC17xx_H
#define __SYSTEM_LPC17xx_H

#include <stdint.h>

#define OSCRANGE 		4
#define OSCEN			5
#define OSCSTAT			6
#define SCS_Val         (1<<OSCRANGE)|(1<<OSCEN)

#define CLKSRC			0
#define CLKSRCSEL_Val   (1<<CLKSRC);

#define MSEL0			0
#define NSEL0			16
#define PLL0CFG_Val     (74<<MSEL0)|(5<<NSEL0)

#define CCLKSEL			0
#define CCLKCFG_Val     (2<<CCLKSEL)

#define FLASHTIM		12
#define FLASHCFG_Val	(0x0000003A)|(4<<FLASHTIM)	// Flash access 5 clocks

#define PLLE0 			0
#define PLLC0 			1
#define PLOCK0			26

#define XTAL        	12000000UL			// Oscillator frequency
#define OSC_CLK     	XTAL        		// Main oscillator frequency
#define RTC_CLK     	32000UL     		// RTC oscillator frequency
#define IRC_OSC     	4000000UL   		// Internal RC oscillator frequency

void SystemInit(void);

#endif /* __SYSTEM_LPC17xx_H */
