/******************************************************************************
 * @file :   	uart.c
 * @brief : 	UART Functions 
 * @version : 	V1.0
 * @author :	Emil Fresk
 ******************************************************************************/

#include "uart.h"
/**
 * @brief UART1 Init...
 */
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
	return (LPC_UART0->LSR & (1<<0));		// Reciver status
}

/**
 * @brief 	Get character
 */
char UART0_GetChar(void)
{
	return (char)LPC_UART0->RBR;		// Reciver status
}

/**
 * @brief 	UART process function.
 * 			Checks for characters to beread and store them in the buffer.
 *
void UART_Process(void)
{
	static int cnt = 0;
	char tmp;
	if (UART_CharReady())
	{
		if (UART_buffer->rIndex >= BUFFER_SIZE)
		{
			UART_SendString("Buffer overflow!\nResetting buffer...\n\0");
			UART_buffer->rIndex = 0;
			for (char i = 0; i < BUFFER_SIZE; i++)
				UART_buffer->rBuffer[i] = '\0';
		}
		else
		{
			tmp = (char)UART0->RBR;
			int tmp2;
			UART_buffer->rBuffer[UART_buffer->rIndex++] = tmp;
			
			if (tmp == '\0' || tmp == '\r' || tmp == '\n')
			{
				tmp2 = atoi(UART_buffer->rBuffer);
				if (tmp2 > 0)
				{
					tmp_buffer[cnt] = tmp2;
					tmp_buffer[BUF_SIZE-1] = cnt++;
				}
				else if (cmpString("flush\0", UART_buffer->rBuffer))
				{
					for (int f_i = 0; f_i <= tmp_buffer[BUF_SIZE-1]; f_i++)
					{
						UART_SendString(itoa(tmp_buffer[f_i], 10));
						UART_SendChar('\n');
					}
					
					cnt = 0;
				}
				UART_buffer->rIndex = 0;
			}
		}				
	}
}
*/
