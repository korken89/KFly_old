#include "telemetry.h"

volatile static uint8_t data_cnt;
volatile static uint8_t data_array[256];
volatile static uint8_t counter;

volatile static uint32_t data_rxerror = 0;
volatile static uint32_t data_rxsuccess = 0;
volatile static uint8_t CRC = 0;
volatile functiontype parser = NULL;

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
	data_array[0] = cmd;
	
	switch (cmd & 0x7F)
	{
		case 0x01:
			UART0_SetIRQHandler(GetDataCount);
			parser = rxPing;
			break;
			
		case 0x02:
			UART0_SetIRQHandler(GetDataCount);
			parser = rxTest;
			break;
		
		default:
			UART0_SendChar(NACK);
	}
}

void GetDataCount(void)
{
	data_cnt = UART0_GetChar();
	data_array[1] = data_cnt;
	counter = 2;
	UART0_SetIRQHandler(GetData);
}

void GetData(void)
{
	data_array[counter++] = UART0_GetChar();
	
	if (counter > (data_cnt+2))
		CheckCRC();
}

void CheckCRC(void)
{	
	CRC = crc8((uint8_t *)data_array, (uint8_t)(data_cnt+2));
	
	if (CRC == data_array[data_cnt+2])
	{
		data_rxsuccess++;
		parser();
	}
	else
	{
		data_rxerror++;
		UART0_SetIRQHandler(rxWait);
	}
}

void rxPing(void)
{
	UART0_SetIRQHandler(rxWait);
	uint8_t msg[] = {1, 0};
	UART0_SendChar(1);
	UART0_SendChar(0);
	UART0_SendChar(crc8(msg, 2));
}

void rxTest(void)
{
	UART0_SetIRQHandler(rxWait);
	UART0_SendChar(UART0_GetChar());
}

uint8_t crc8(uint8_t *message, uint8_t nBytes)
{
    uint8_t remainder = 0;	

    for (uint8_t byte = 0; byte < nBytes; ++byte)
    {
        remainder ^= message[byte];

        for (uint8_t bit = 8; bit > 0; --bit)
        {
            if (remainder & 0x80)
                remainder = (remainder << 1) ^ 0xD8;
            else
                remainder = (remainder << 1);
        }
    }

    return remainder;
} 
