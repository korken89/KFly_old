#include "telemetry.h"

/** External Globals **/
extern input_calibration TempCalibration;
extern mix_data mixer;
extern pid_data data_pitch;
extern pid_data data_roll;
extern pid_data data_yaw;

/** Public Globals **/
// ---

/** Private Globals **/
volatile static uint8_t data_cnt;
volatile static uint8_t data_array[64];
volatile static uint8_t counter;
volatile static uint32_t data_rxerror = 0;
volatile static uint32_t data_rxsuccess = 0;
volatile static uint8_t CRC = 0;
volatile static PFV parser = NULL;

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
	UART0_SetReceivedIRQHandler(GetDataCount);
	
	switch (cmd & 0x7F)
	{
		case 0x01: // Ping
			parser = rxPing;
			break;
			
		case 0x02: // SaveToFlash
			parser = rxSaveToFlash;
			break;
		
		case 0x03: // GetRegulatorData
			parser = rxGetRegulatorData;
			break;
		
		case 0x04: // SetRegulatorData
			parser = rxSetRegulatorData;
			break;
		
		case 0x05: // GetChannelMix
			parser = rxGetChannelMix;
			break;
		
		case 0x06: // SetChannelMix
			parser = rxSetChannelMix;
			break;
		
		case 0x07: // SartRCCalibration
			parser = rxNothing;
			break;
		
		case 0x08: // StopRCCalibration
			parser = rxNothing;
			break;
		
		case 0x09: // CalibrateRCCenters
			parser = rxNothing;
			break;
		
		case 0x0A: // GetRCCalibration
			parser = rxNothing;
			break;
		
		case 0x0B: // SetRCCalibration
			parser = rxNothing;
			break;
		
		case 0x0C: // GetRCValues
			parser = rxNothing;
			break;
		
		default:
			UART0_SetReceivedIRQHandler(rxWait);
			break;
	}
}

void rxNothing(void)
{
	UART0_SetReceivedIRQHandler(rxWait);
}

void rxPing(void)
{
	UART0_SetReceivedIRQHandler(rxWait);
	uint8_t data[] = {1, 0};
	uint8_t msg[] = {1, 0, crc8(data, 2)};
	UART0_SendData((uint8_t *)msg, 3);
}

void rxSaveToFlash(void)
{
	UART0_SetReceivedIRQHandler(rxWait);

	/* Add save to flash code */

}

void rxGetRegulatorData(void)
{
	UART0_SetReceivedIRQHandler(rxWait);

	uint8_t i = 0;
	uint8_t data[39];
	data[i++] = 0x03;
	data[i++] = 36;
	data[i++] = (uint8_t)data_pitch.r_kp;
	data[i++] = (uint8_t)(data_pitch.r_kp>>8);
	data[i++] = (uint8_t)data_pitch.r_ki;
	data[i++] = (uint8_t)(data_pitch.r_ki>>8);
	data[i++] = (uint8_t)data_pitch.r_imax;
	data[i++] = (uint8_t)(data_pitch.r_imax>>8);

	data[i++] = (uint8_t)data_roll.r_kp;
	data[i++] = (uint8_t)(data_roll.r_kp>>8);
	data[i++] = (uint8_t)data_roll.r_ki;
	data[i++] = (uint8_t)(data_roll.r_ki>>8);
	data[i++] = (uint8_t)data_roll.r_imax;
	data[i++] = (uint8_t)(data_roll.r_imax>>8);

	data[i++] = (uint8_t)data_yaw.r_kp;
	data[i++] = (uint8_t)(data_yaw.r_kp>>8);
	data[i++] = (uint8_t)data_yaw.r_ki;
	data[i++] = (uint8_t)(data_yaw.r_ki>>8);
	data[i++] = (uint8_t)data_yaw.r_imax;
	data[i++] = (uint8_t)(data_yaw.r_imax>>8);

	data[i++] = (uint8_t)data_pitch.a_kp;
	data[i++] = (uint8_t)(data_pitch.a_kp>>8);
	data[i++] = (uint8_t)data_pitch.a_ki;
	data[i++] = (uint8_t)(data_pitch.a_ki>>8);
	data[i++] = (uint8_t)data_pitch.a_imax;
	data[i++] = (uint8_t)(data_pitch.a_imax>>8);

	data[i++] = (uint8_t)data_roll.a_kp;
	data[i++] = (uint8_t)(data_roll.a_kp>>8);
	data[i++] = (uint8_t)data_roll.a_ki;
	data[i++] = (uint8_t)(data_roll.a_ki>>8);
	data[i++] = (uint8_t)data_roll.a_imax;
	data[i++] = (uint8_t)(data_roll.a_imax>>8);

	data[i++] = (uint8_t)data_yaw.a_kp;
	data[i++] = (uint8_t)(data_yaw.a_kp>>8);
	data[i++] = (uint8_t)data_yaw.a_ki;
	data[i++] = (uint8_t)(data_yaw.a_ki>>8);
	data[i++] = (uint8_t)data_yaw.a_imax;
	data[i++] = (uint8_t)(data_yaw.a_imax>>8);

	data[i++] = crc8(data, 38);

	UART0_SendData(data, 39);
}

void rxSetRegulatorData(void)
{
	UART0_SetReceivedIRQHandler(rxWait);
	uint8_t i = 2;

	data_pitch.r_kp = (fix32)((uint16_t)data_array[i] | (uint16_t)data_array[i+1]<<8);
	i += 2;
	data_pitch.r_ki = (fix32)((uint16_t)data_array[i] | (uint16_t)data_array[i+1]<<8);
	i += 2;
	data_pitch.r_imax = (fix32)((uint16_t)data_array[i] | (uint16_t)data_array[i+1]<<8);
	i += 2;

	data_roll.r_kp = (fix32)((uint16_t)data_array[i] | (uint16_t)data_array[i+1]<<8);
	i += 2;
	data_roll.r_ki = (fix32)((uint16_t)data_array[i] | (uint16_t)data_array[i+1]<<8);
	i += 2;
	data_roll.r_imax = (fix32)((uint16_t)data_array[i] | (uint16_t)data_array[i+1]<<8);
	i += 2;

	data_yaw.r_kp = (fix32)((uint16_t)data_array[i] | (uint16_t)data_array[i+1]<<8);
	i += 2;
	data_yaw.r_ki = (fix32)((uint16_t)data_array[i] | (uint16_t)data_array[i+1]<<8);
	i += 2;
	data_yaw.r_imax = (fix32)((uint16_t)data_array[i] | (uint16_t)data_array[i+1]<<8);
	i += 2;

	data_pitch.a_kp = (fix32)((uint16_t)data_array[i] | (uint16_t)data_array[i+1]<<8);
	i += 2;
	data_pitch.a_ki = (fix32)((uint16_t)data_array[i] | (uint16_t)data_array[i+1]<<8);
	i += 2;
	data_pitch.a_imax = (fix32)((uint16_t)data_array[i] | (uint16_t)data_array[i+1]<<8);
	i += 2;

	data_roll.a_kp = (fix32)((uint16_t)data_array[i] | (uint16_t)data_array[i+1]<<8);
	i += 2;
	data_roll.a_ki = (fix32)((uint16_t)data_array[i] | (uint16_t)data_array[i+1]<<8);
	i += 2;
	data_roll.a_imax = (fix32)((uint16_t)data_array[i] | (uint16_t)data_array[i+1]<<8);
	i += 2;

	data_yaw.a_kp = (fix32)((uint16_t)data_array[i] | (uint16_t)data_array[i+1]<<8);
	i += 2;
	data_yaw.a_ki = (fix32)((uint16_t)data_array[i] | (uint16_t)data_array[i+1]<<8);
	i += 2;
	data_yaw.a_imax = (fix32)((uint16_t)data_array[i] | (uint16_t)data_array[i+1]<<8);
}

void rxGetChannelMix(void)
{
	UART0_SetReceivedIRQHandler(rxWait);

	uint8_t i = 0;
	uint8_t data[35];

	data[i++] = 0x05;
	data[i++] = 32;

	for (int j = 0; j < 8; j++)
		for (int k = 0; k < 4; k++)
			data[i++] = (uint8_t)mixer.mix[j][k];

	data[i++] = crc8(data, 34);

	UART0_SendData(data, 35);
}

void rxSetChannelMix(void)
{
	UART0_SetReceivedIRQHandler(rxWait);

	uint8_t i = 2;

	for (int j = 0; j < 8; j++)
		for (int k = 0; k < 4; k++)
			mixer.mix[j][k] = (fix32)((int8_t)data_array[i++]);
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
