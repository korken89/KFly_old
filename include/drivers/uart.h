#ifndef __UART_H__
#define __UART_H__

#include "LPC17xx.h"
#include "lpc_types.h"
#include "bitvals.h"
#include "misc.h"

#define FIFOBUFSIZE		128

void UART_Init(void);
void UART0_SendChar(uint8_t);
void UART0_SendString(uint8_t *);
uint8_t UART0_CharReady(void);
uint8_t UART0_GetChar(void);
void UART0_SetReceivedIRQHandler(voidfunctype);
void UART0_SendData(uint8_t *, uint8_t);
void UART0_SendFIFO(void);
void UART0_IRQHandler(void);

#endif
