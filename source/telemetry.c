#include "telemetry.h"

functiontype funcList[] = { NULL,
							&rxPing,
							&rxPing,
							&rxPing,
							&rxPing,
							&rxPing,
							&rxPing,
							&rxPing };

void startTelemetry(void)
{
    UART0_SetIRQHandler(&rxWait);
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
	UART0_SetIRQHandler(funcList[cmd]);
}

void rxPing(void)
{
	UART0_SetIRQHandler(&rxWait);
	UART0_SendChar(ACK);
}
