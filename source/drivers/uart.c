/******************************************************************************
 * @file :   	uart.c
 * @brief : 	UART Functions 
 * @version : 	1.0
 * @author :	Emil Fresk
 ******************************************************************************/

#include "uart.h"

functiontype HandlerFunction = NULL;

void UART_Init(void)
{
	/** UART0 */
	LPC_SC->PCONP |= (1<<PCUART0);					// Power up the UART
	LPC_SC->PCLKSEL0 |= (3<<PCLK_UART0);			// UART1 clock source CLK/8 = 12.5MHz
	LPC_PINCON->PINSEL0 |= (1<<4)|(1<<6);			// Set pins for UART
	LPC_PINCON->PINMODE0 |= (2<<4)|(2<<6);			// No pull-up nor pull-down on RXD/TXD lines
	LPC_UART0->LCR = (3<<0)|(1<<7);					// 8 data bits, 1 stop bit, no parity, Enable access to divisor latches
	LPC_UART0->DLL = 4;
	LPC_UART0->FDR = (7<<DIVADDVAL)|(10<<MULVAL);	// Baudrate = PCLK/(16*DLL*(1+DIVADDVAL/MULVAL)) = 114890, 0.27% error from 115200 baud
	LPC_UART0->LCR &= ~(1<<7);						// Disable access to divisor latches
	LPC_UART0->FCR = (1<<0);						// Enable FIFO
	LPC_UART0->FCR |= (1<<1)|(1<<2);				// Reset TX and RX
	LPC_UART0->IER = (1<<0);					// Enable Recived & Sent interrupts
	
	/** UART1 
	
	SC->PCONP |= (1<<PCUART1);					// Power up the UART
	SC->PCLKSEL0 |= (3<<PCLK_UART1);			// UART1 clock source CLK/8 = 15MHz
	UART1->LCR = (3<<0)|(1<<7);					// 8 data bits, 1 stop bit, no parity, Enable access to divisor latches
	UART1->DLL = 6;
	UART1->FDR = (5<<DIVADDVAL)|(14<<MULVAL);	// Baudrate = PCLK/(16*DLL*(1+DIVADDVAL/MULVAL)) = 115131, 0.06% error from 115200 baud
	UART1->LCR &= ~(1<<7);						// Disable access to divisor latches
	PINCON->PINSEL0 |= (1<<30);					// Set pins for UART
	PINCON->PINSEL1 |= (1<<0);
	PINCON->PINMODE0 |= (2<<30);				// No pull-up nor pull-down
	PINCON->PINMODE1 |= (2<<0);
	UART1->FCR = (1<<0);						// Enable FIFO
	UART1->FCR |= (1<<1)|(1<<2);				// Reset TX and RX
	*/
	//NVIC_EnableIRQ(UART1_IRQn);				// Enable the interrupt

}

/**
 * @brief Send character function.
 */
void UART0_SendChar(char ch)
{
	while (!(LPC_UART0->LSR & (1<<5)));		// While transmitter is busy
	LPC_UART0->THR = ch;
}

/**
 * @brief 	Send string function.
 * 			IMPORTANT! '\0' is string terminator.
 */
void UART0_SendString(char* str)
{
	while (*str) 
		UART0_SendChar(*(str++));
}

/**
 * @brief 	Look if a char i waiting to be read.
 */
char UART0_CharReady(void)
{
	return (LPC_UART0->LSR & (1<<0));	// Reciver status
}

/**
 * @brief 	Get character
 */
char UART0_GetChar(void)
{
	return (char)LPC_UART0->RBR;		// Reciver status
}

void UART0_SetIRQHandler(functiontype func)
{
	HandlerFunction = func;
}

void UART0_IRQHandler(void)
{
	if (HandlerFunction != NULL)
		HandlerFunction();
}
