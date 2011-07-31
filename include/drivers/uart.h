#ifndef __UART_H__
#define __UART_H__

#include "LPC17xx.h"
#include "lpc_types.h"
#include "bitvals.h"
#include "misc.h"

typedef void (*functiontype)();

void UART_Init(void);
void UART0_SendChar(char);
void UART0_SendString(char *);
char UART0_CharReady(void);
char UART0_GetChar(void);
void UART0_SetIRQHandler(functiontype);
void UART0_IRQHandler(void);

#endif
