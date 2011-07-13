/******************************************************************************
 * @file :   	gps.c
 * @brief : 	GPS Functions 
 * @version : 	V1.0
 * @author :	Emil Fresk
 ******************************************************************************/

#include "gps.h"

/**
 * @brief UART2 Init...
 */
void GPS_UART_Init(void)
{
	/** UART2 */
	LPC_SC->PCONP |= (1<<PCUART2);					// Power up the UART
	LPC_SC->PCLKSEL1 |= (3<<PCLK_UART2);			// UART1 clock source CLK/8 = 12.5MHz
	LPC_PINCON->PINSEL4 |= (2<<16)|(2<<18);			// Set pins for UART
	LPC_PINCON->PINMODE4 |= (2<<16)|(2<<18);		// No pull-up nor pull-down on RXD/TXD lines
	LPC_UART2->LCR = (3<<0)|(1<<7);					// 8 data bits, 1 stop bit, no parity, Enable access to divisor latches
	LPC_UART2->DLL = 48;
	LPC_UART2->FDR = (7<<DIVADDVAL)|(10<<MULVAL);	// Baudrate = PCLK/(16*DLL*(1+DIVADDVAL/MULVAL)) = 114890, 0.27% error from 115200 baud
	LPC_UART2->LCR &= ~(1<<7);						// Disable access to divisor latches
	LPC_UART2->FCR = (1<<0);						// Enable FIFO
	LPC_UART2->FCR |= (1<<1)|(1<<2);				// Reset TX and RX
}

/**
 * @brief 	Look if a char i waiting to be read.
 */
char GPS_CharReady(void)
{
	return (LPC_UART2->LSR & (1<<0));		// Reciver status
}

/**
 * @brief 	Get character
 */
char GPS_GetChar(void)
{
	return (char)LPC_UART2->RBR;		// Reciver status
}


void GPS_SendChar(char ch)
{
	while (!(LPC_UART2->LSR & (1<<5)));		// While transmitter is busy
	LPC_UART2->THR = ch;
}
