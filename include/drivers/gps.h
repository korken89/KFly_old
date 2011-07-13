#ifndef __GPS_H__
#define __GPS_H__

#include "LPC17xx.h"
#include "bitvals.h"
#include "misc.h"

void GPS_UART_Init(void);
char GPS_CharReady(void);
char GPS_GetChar(void);
void GPS_SendChar(char);

#endif

