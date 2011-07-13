#ifndef __UART_H__
#define __UART_H__

#include "LPC17xx.h"
#include "bitvals.h"
#include "misc.h"

#define BUFFER_SIZE	30

void UART_Init(void);
void UART0_SendChar(char);
void UART0_SendString(char *);
char UART0_CharReady(void);
char UART0_GetChar(void);
//void UART0_Process(void);

#endif
