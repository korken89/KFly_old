#include "telemetry_commands.h"

/** External Globals **/
extern uint8_t data_cnt;
extern uint8_t data_array[64];

extern input_data InputData;
extern input_calibration TempCalibration;
extern mix_data mixer;
extern pid_data data_pitch;
extern pid_data data_roll;
extern pid_data data_yaw;

void rxNothing(void)
{

}

void rxPing(void)
{
	UART0_SetReceivedIRQHandler(rxWait);
	uint8_t data[] = {1, 0};
	uint8_t msg[] = {1, 0, crc8(data, 2)};
	UART0_SendData(msg, 3);
}

void rxSaveToFlash(void)
{
	/* Add save to flash code */
}

void rxGetRegulatorData(void)
{
	int i = 0;
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
	int i = 2;

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
	int i = 0;
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
	int i = 2;

	for (int j = 0; j < 8; j++)
		for (int k = 0; k < 4; k++)
			mixer.mix[j][k] = (fix32)((int8_t)data_array[i++]);
}

void rxSartRCCalibration(void)
{
	RunCalibrationState(CalibrateInputs);
}

void rxStopRCCalibration(void)
{
	RunCalibrationState(NoCommand);
}

void rxCalibrateRCCenters(void)
{
	RunCalibrationState(CalibrateCenter);
}

void rxGetRCCalibration(void)
{
	int i = 0;
	uint8_t data[54];

	data[i++] = 0x0A;
	data[i++] = 51;

	data[i++] = (uint8_t)TempCalibration.role;
	data[i++] = (uint8_t)(TempCalibration.role >> 8);
	data[i++] = (uint8_t)(TempCalibration.role >> 16);

	for (int j = 0; j < 8; j++)
	{
		data[i++] = (uint8_t)TempCalibration.ch_top[j];
		data[i++] = (uint8_t)(TempCalibration.ch_top[j] >> 8);
		data[i++] = (uint8_t)TempCalibration.ch_center[j];
		data[i++] = (uint8_t)(TempCalibration.ch_center[j] >> 8);
		data[i++] = (uint8_t)TempCalibration.ch_bottom[j];
		data[i++] = (uint8_t)(TempCalibration.ch_bottom[j] >> 8);
	}

	data[i++] = crc8(data, 53);
	UART0_SendData(data, 54);
}

void rxSetRCCalibration(void)
{
	int i = 2;

	TempCalibration.role = (uint32_t)data_array[i++];
	TempCalibration.role |= ((uint32_t)data_array[i++] << 8);
	TempCalibration.role |= ((uint32_t)data_array[i++] << 16);

	for (int j = 0; j < 8; j++)
	{
		TempCalibration.ch_top[j] = (uint32_t)data[i++];
		TempCalibration.ch_top[j] |= ((uint32_t)data[i++] << 8);
		TempCalibration.ch_center[j] = (uint32_t)data[i++];
		TempCalibration.ch_center[j] |= ((uint32_t)data[i++] << 8);
		TempCalibration.ch_bottom[j] = (uint32_t)data[i++];
		TempCalibration.ch_bottom[j] |= ((uint32_t)data[i++] << 8);
	}
}

void rxGetRCValues(void)
{
	int i = 0;
	uint8_t data[19];

	data[i++] = 0x0C;
	data[i++] = 16;

	for (int j = 0; j < 8; j++)
	{
		data[i++] = (uint8_t)InputData.value[j];
		data[i++] = (uint8_t)(InputData.value[j] >> 8);
	}

	data[i++] = crc8(data, 53);
	UART0_SendData(data, 54);
}
