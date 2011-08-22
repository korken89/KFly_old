#include "telemetry.h"

/** External Globals **/
// ---

/** Public Globals **/
volatile uint8_t data_cnt;
volatile uint8_t data_array[64];

/** Private Globals **/
volatile static uint8_t counter;
volatile static uint32_t data_rxerror = 0;
volatile static uint32_t data_rxsuccess = 0;
volatile static uint8_t CRC = 0;
volatile static PFV parser = NULL;
volatile static data_length data_len;

void InitTelemetry(void)
{
	InitUART0();
	startTelemetry();
}

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

void SendACK(void)
{
	const uint8_t ack_data[3] = {ACK, 0, crc8((uint8_t *)ack_data, 2)};
	UART0_SendData((uint8_t *)ack_data, 3);
}

void rxWait(void)
{
	uint8_t cmd = UART0_GetChar();
	data_array[0] = cmd;
	UART0_SetReceivedIRQHandler(GetDataCount);
	
	switch (cmd & 0x7F)
	{
		case 0x01: // Ping

			parser = rxPing;
			data_len = PingLength;
			break;
			
		case 0x02: // SaveToFlash
			parser = rxSaveToFlash;
			data_len = SaveToFlashLength;
			break;
		
		case 0x03: // GetRegulatorData
			parser = rxGetRegulatorData;
			data_len = GetRegulatorDataLength;
			break;
		
		case 0x04: // SetRegulatorData
			parser = rxSetRegulatorData;
			data_len = SetRegulatorDataLength;
			break;
		
		case 0x05: // GetChannelMix
			parser = rxGetChannelMix;
			data_len = GetChannelMixLength;
			break;
		
		case 0x06: // SetChannelMix
			parser = rxSetChannelMix;
			data_len = SetChannelMixLength;
			break;
		
		case 0x07: // StartRCCalibration
			parser = rxSartRCCalibration;
			data_len = StartRCCalibrationLength;
			break;
		
		case 0x08: // StopRCCalibration
			parser = rxStopRCCalibration;
			data_len = StopRCCalibrationLength;
			break;
		
		case 0x09: // CalibrateRCCenters
			data_len = CalibrateRCCentersLength;
			parser = rxCalibrateRCCenters;
			break;
		
		case 0x0A: // GetRCCalibration
			parser = rxGetRCCalibration;
			data_len = GetRCCalibrationLength;
			break;
		
		case 0x0B: // SetRCCalibration
			parser = rxSetRCCalibration;
			data_len = SetRCCalibrationLength;
			break;
		
		case 0x0C: // GetRCValues
			parser = rxGetRCValues;
			data_len = GetRCValuesLength;
			break;
		
		default:
			UART0_SetReceivedIRQHandler(rxWait);
			break;
	}
}

void GetDataCount(void)
{
	data_cnt = UART0_GetChar();
	if ((data_cnt > 61) || ((int)data_len != data_cnt))
	{
		UART0_SetReceivedIRQHandler(rxWait);
		data_rxerror++;
		return;
	}
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
	UART0_SetReceivedIRQHandler(rxWait);
	CRC = crc8((uint8_t *)data_array, (uint8_t)(data_cnt+2));
	
	if (CRC == data_array[data_cnt+2])
	{
		data_rxsuccess++;
		parser();
		if (data_array[0] & 0x80)
			SendACK();
	}
	else
	{
		data_rxerror++;
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
