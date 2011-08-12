#include "telemetry.h"

volatile static uint8_t data_cnt;
volatile static uint8_t data_array[64];
volatile static uint8_t counter;

volatile static uint32_t data_rxerror = 0;
volatile static uint32_t data_rxsuccess = 0;
volatile static uint8_t CRC = 0;
volatile voidfunctype parser = NULL;

void startTelemetry(void)
{
    UART0_SetReceivedIRQHandler(rxWait);
    NVIC_EnableIRQ(UART0_IRQn);
}

void stopTelemetry(void)
{
    NVIC_DisableIRQ(UART0_IRQn);
    UART0_SetReceivedIRQHandler(NULL);
}

void rxWait(void)
{
	uint8_t cmd = UART0_GetChar();
	data_array[0] = cmd;
	
	switch (cmd & 0x7F)
	{
		case 0x01: // Ping
			UART0_SetReceivedIRQHandler(GetDataCount);
			parser = rxPing;
			break;
			
		case 0x02: // SaveToFlash
			UART0_SetReceivedIRQHandler(GetDataCount);
			parser = rxSaveToFlash;
			break;
		
		case 0x03: // GetRegulatorData
			break;
		
		case 0x04: // SetRegulatorData
			break;
		
		case 0x05: // GetChannelMix
			break;
		
		case 0x06: // SetChannelMix
			break;
		
		case 0x07: // SartRCCalibration
			break;
		
		case 0x08: // StopRCCalibration
			break;
		
		case 0x09: // CalibrateRCCenters
			break;
		
		case 0x0A: // GetRCCalibration
			break;
		
		case 0x0B: // SetRCCalibration
			break;
		
		case 0x0C: // GetRCValues
			break;
		
		default:
			break;
	}
}

void rxPing(void)
{
	UART0_SetReceivedIRQHandler(rxWait);
	uint8_t msg[] = {1, 0};
	UART0_SendChar(1);
	UART0_SendChar(0);
	UART0_SendChar(crc8(msg, 2));
}

void rxSaveToFlash(void)
{
	UART0_SetReceivedIRQHandler(rxWait);
	
	/* Add save to flash code */
	
}

void GetDataCount(void)
{
	data_cnt = UART0_GetChar();
	data_array[1] = data_cnt;
	counter = 2;
	UART0_SetReceivedIRQHandler(GetData);
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
		UART0_SetReceivedIRQHandler(rxWait);
	}
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
