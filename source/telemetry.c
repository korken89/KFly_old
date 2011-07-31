#include "telemetry.h"

void startTelemetry(void)
{
    UART0_SetIRQHandler(rxWait);
    NVIC_EnableIRQ(UART0_IRQn);
}

void stopTelemetry(void)
{
    NVIC_DisableIRQ(UART0_IRQn);
    UART0_SetIRQHandler(NULL);
}

void rxWait(void)
{
	uint8_t cmd = UART0_GetChar();
	
	switch (cmd)
	{
		case 0x01:
			rxPing();
			break;
			
		case 0x02:
			UART0_SetIRQHandler(rxTest);
			UART0_SendChar(ACK);
			break;
		
		default:
			UART0_SendChar(NACK);
	}
}

void rxPing(void)
{
	UART0_SendChar(ACK);
}

void rxTest(void)
{
	UART0_SetIRQHandler(rxWait);
	UART0_SendChar(UART0_GetChar());
}
