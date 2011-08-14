#ifndef __UART_H__
#define __UART_H__

#include "LPC17xx.h"
#include "lpc_types.h"
#include "bitvals.h"
#include "misc.h"

#define FIFOBUFSIZE		128

void UART_Init(void);
uint8_t UART0_GetChar(void);
void UART0_SetReceivedIRQHandler(PFV);
void UART0_SendData(uint8_t *, uint8_t);
void UART0_IRQHandler(void);

#endif
