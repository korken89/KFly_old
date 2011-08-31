#include "telemetry_commands.h"

/** External Globals **/
extern uint8_t data_cnt;
extern uint8_t data_array[64];

extern input_data InputData;
extern input_calibration InputCalibration;
extern limits_data FlightLimits;
extern mix_data Mixer;
extern pid_data DataPitch;
extern pid_data DataRoll;
extern pid_data DataYaw;

void rxNothing(void)
{

}

void rxPing(void)
{
	uint8_t data[] = {1, 0};
	uint8_t msg[] = {1, 0, crc8(data, 2)};
	UART0_SendData(msg, 3);
}

void rxSaveToFlash(void)
{
	// Prepare data for flash save
	uint32_t flash_data[IAP_FLASH_PAGE_SIZE_WORDS*2];
	int32_t i = 0;

	memset((int *)flash_data, 0, IAP_FLASH_PAGE_SIZE_WORDS*2);

	// KFly ID and Version Info
	flash_data[i++] = KFLY_ID;
	i += 3;
	flash_data[i++] = FlightLimits.maxangle;
	flash_data[i++] = FlightLimits.maxrate;
	flash_data[i++] = FlightLimits.maxyawrate;

	// Pitch regulator Coefficients
	flash_data[i++] = DataPitch.r_kp;
	flash_data[i++] = DataPitch.r_ki;
	flash_data[i++] = DataPitch.a_kp;
	flash_data[i++] = DataPitch.a_ki;
	flash_data[i++] = DataPitch.r_imax;
	flash_data[i++] = DataPitch.a_imax;

	// Roll regulator Coefficients
	flash_data[i++] = DataRoll.r_kp;
	flash_data[i++] = DataRoll.r_ki;
	flash_data[i++] = DataRoll.a_kp;
	flash_data[i++] = DataRoll.a_ki;
	flash_data[i++] = DataRoll.r_imax;
	flash_data[i++] = DataRoll.a_imax;

	// Yaw regulator Coefficients
	flash_data[i++] = DataYaw.r_kp;
	flash_data[i++] = DataYaw.r_ki;
	flash_data[i++] = DataYaw.a_kp;
	flash_data[i++] = DataYaw.a_ki;
	flash_data[i++] = DataYaw.r_imax;
	flash_data[i++] = DataYaw.a_imax;

	// Mixer Constants
	for (int j = 0; j < 8; j++)
		for (int k = 0; k < 4; k++)
			flash_data[i++] = (uint32_t)Mixer.mix[j][k];

	// Role Data
	flash_data[i++] = InputCalibration.role;

	// Channel Calibration
	for (int j = 0; j < 8; j++)
	{
		flash_data[i++] = InputCalibration.ch_top[j];
		flash_data[i++] = InputCalibration.ch_center[j];
		flash_data[i++] = InputCalibration.ch_bottom[j];
	}

	taskENTER_CRITICAL();
	{
		u32IAP_PrepareSectors(29, 29);
		u32IAP_EraseSectors(29, 29);
		u32IAP_PrepareSectors(29, 29);
		u32IAP_CopyRAMToFlash(EEMUL_BASE, (uint32_t)flash_data, IAP_FLASH_PAGE_SIZE_BYTES*2);
	}
	taskEXIT_CRITICAL();
}

void rxGetRegulatorData(void)
{
	int i = 0;
	uint8_t data[45];
	data[i++] = 0x03;
	data[i++] = 42;

	// From FP to INT
	data[i++] = (uint8_t)(FlightLimits.maxangle/FP_MUL);
	data[i++] = (uint8_t)(FlightLimits.maxangle/FP_MUL>>8);
	data[i++] = (uint8_t)(FlightLimits.maxrate/FP_MUL);
	data[i++] = (uint8_t)(FlightLimits.maxrate/FP_MUL>>8);
	data[i++] = (uint8_t)(FlightLimits.maxyawrate/FP_MUL);
	data[i++] = (uint8_t)(FlightLimits.maxyawrate/FP_MUL>>8);

	data[i++] = (uint8_t)DataPitch.r_kp;
	data[i++] = (uint8_t)(DataPitch.r_kp>>8);
	data[i++] = (uint8_t)DataPitch.r_ki;
	data[i++] = (uint8_t)(DataPitch.r_ki>>8);
	data[i++] = (uint8_t)DataPitch.r_imax;
	data[i++] = (uint8_t)(DataPitch.r_imax>>8);

	data[i++] = (uint8_t)DataRoll.r_kp;
	data[i++] = (uint8_t)(DataRoll.r_kp>>8);
	data[i++] = (uint8_t)DataRoll.r_ki;
	data[i++] = (uint8_t)(DataRoll.r_ki>>8);
	data[i++] = (uint8_t)DataRoll.r_imax;
	data[i++] = (uint8_t)(DataRoll.r_imax>>8);

	data[i++] = (uint8_t)DataYaw.r_kp;
	data[i++] = (uint8_t)(DataYaw.r_kp>>8);
	data[i++] = (uint8_t)DataYaw.r_ki;
	data[i++] = (uint8_t)(DataYaw.r_ki>>8);
	data[i++] = (uint8_t)DataYaw.r_imax;
	data[i++] = (uint8_t)(DataYaw.r_imax>>8);

	data[i++] = (uint8_t)DataPitch.a_kp;
	data[i++] = (uint8_t)(DataPitch.a_kp>>8);
	data[i++] = (uint8_t)DataPitch.a_ki;
	data[i++] = (uint8_t)(DataPitch.a_ki>>8);
	data[i++] = (uint8_t)DataPitch.a_imax;
	data[i++] = (uint8_t)(DataPitch.a_imax>>8);

	data[i++] = (uint8_t)DataRoll.a_kp;
	data[i++] = (uint8_t)(DataRoll.a_kp>>8);
	data[i++] = (uint8_t)DataRoll.a_ki;
	data[i++] = (uint8_t)(DataRoll.a_ki>>8);
	data[i++] = (uint8_t)DataRoll.a_imax;
	data[i++] = (uint8_t)(DataRoll.a_imax>>8);

	data[i++] = (uint8_t)DataYaw.a_kp;
	data[i++] = (uint8_t)(DataYaw.a_kp>>8);
	data[i++] = (uint8_t)DataYaw.a_ki;
	data[i++] = (uint8_t)(DataYaw.a_ki>>8);
	data[i++] = (uint8_t)DataYaw.a_imax;
	data[i++] = (uint8_t)(DataYaw.a_imax>>8);

	data[i++] = crc8(data, 44);

	UART0_SendData(data, 45);
}

void rxSetRegulatorData(void)
{
	int i = 2;

	//From INT to FP
	FlightLimits.maxangle = (fix32)((uint16_t)data_array[i] | (uint16_t)data_array[i+1]<<8)*FP_MUL;
	i += 2;
	FlightLimits.maxrate = (fix32)((uint16_t)data_array[i] | (uint16_t)data_array[i+1]<<8)*FP_MUL;
	i += 2;
	FlightLimits.maxyawrate = (fix32)((uint16_t)data_array[i] | (uint16_t)data_array[i+1]<<8)*FP_MUL;
	i += 2;

	DataPitch.r_kp = (fix32)((uint16_t)data_array[i] | (uint16_t)data_array[i+1]<<8);
	i += 2;
	DataPitch.r_ki = (fix32)((uint16_t)data_array[i] | (uint16_t)data_array[i+1]<<8);
	i += 2;
	DataPitch.r_imax = (fix32)((uint16_t)data_array[i] | (uint16_t)data_array[i+1]<<8);
	i += 2;

	DataRoll.r_kp = (fix32)((uint16_t)data_array[i] | (uint16_t)data_array[i+1]<<8);
	i += 2;
	DataRoll.r_ki = (fix32)((uint16_t)data_array[i] | (uint16_t)data_array[i+1]<<8);
	i += 2;
	DataRoll.r_imax = (fix32)((uint16_t)data_array[i] | (uint16_t)data_array[i+1]<<8);
	i += 2;

	DataYaw.r_kp = (fix32)((uint16_t)data_array[i] | (uint16_t)data_array[i+1]<<8);
	i += 2;
	DataYaw.r_ki = (fix32)((uint16_t)data_array[i] | (uint16_t)data_array[i+1]<<8);
	i += 2;
	DataYaw.r_imax = (fix32)((uint16_t)data_array[i] | (uint16_t)data_array[i+1]<<8);
	i += 2;

	DataPitch.a_kp = (fix32)((uint16_t)data_array[i] | (uint16_t)data_array[i+1]<<8);
	i += 2;
	DataPitch.a_ki = (fix32)((uint16_t)data_array[i] | (uint16_t)data_array[i+1]<<8);
	i += 2;
	DataPitch.a_imax = (fix32)((uint16_t)data_array[i] | (uint16_t)data_array[i+1]<<8);
	i += 2;

	DataRoll.a_kp = (fix32)((uint16_t)data_array[i] | (uint16_t)data_array[i+1]<<8);
	i += 2;
	DataRoll.a_ki = (fix32)((uint16_t)data_array[i] | (uint16_t)data_array[i+1]<<8);
	i += 2;
	DataRoll.a_imax = (fix32)((uint16_t)data_array[i] | (uint16_t)data_array[i+1]<<8);
	i += 2;

	DataYaw.a_kp = (fix32)((uint16_t)data_array[i] | (uint16_t)data_array[i+1]<<8);
	i += 2;
	DataYaw.a_ki = (fix32)((uint16_t)data_array[i] | (uint16_t)data_array[i+1]<<8);
	i += 2;
	DataYaw.a_imax = (fix32)((uint16_t)data_array[i] | (uint16_t)data_array[i+1]<<8);
}

void rxGetChannelMix(void)
{
	int i = 0;
	uint8_t data[35];

	data[i++] = 0x05;
	data[i++] = 32;

	for (int j = 0; j < 8; j++)
		for (int k = 0; k < 4; k++)
			data[i++] = (uint8_t)Mixer.mix[j][k];

	data[i++] = crc8(data, 34);

	UART0_SendData(data, 35);
}

void rxSetChannelMix(void)
{
	int i = 2;

	for (int j = 0; j < 8; j++)
		for (int k = 0; k < 4; k++)
			Mixer.mix[j][k] = (fix32)((int8_t)data_array[i++]);
}


void rxGetRCCalibration(void)
{
	int i = 0;
	uint8_t data[54];

	data[i++] = 0x07;
	data[i++] = 51;

	data[i++] = (uint8_t)InputCalibration.role;
	data[i++] = (uint8_t)(InputCalibration.role >> 8);
	data[i++] = (uint8_t)(InputCalibration.role >> 16);

	for (int j = 0; j < 8; j++)
	{
		data[i++] = (uint8_t)InputCalibration.ch_top[j];
		data[i++] = (uint8_t)(InputCalibration.ch_top[j] >> 8);
		data[i++] = (uint8_t)InputCalibration.ch_center[j];
		data[i++] = (uint8_t)(InputCalibration.ch_center[j] >> 8);
		data[i++] = (uint8_t)InputCalibration.ch_bottom[j];
		data[i++] = (uint8_t)(InputCalibration.ch_bottom[j] >> 8);
	}

	data[i++] = crc8(data, 53);
	UART0_SendData(data, 54);
}

void rxSetRCCalibration(void)
{
	int i = 2;

	InputCalibration.role = (uint32_t)data_array[i++];
	InputCalibration.role |= ((uint32_t)data_array[i++] << 8);
	InputCalibration.role |= ((uint32_t)data_array[i++] << 16);

	for (int j = 0; j < 8; j++)
	{
		InputCalibration.ch_top[j] = (uint32_t)data_array[i++];
		InputCalibration.ch_top[j] |= ((uint32_t)data_array[i++] << 8);
		InputCalibration.ch_center[j] = (uint32_t)data_array[i++];
		InputCalibration.ch_center[j] |= ((uint32_t)data_array[i++] << 8);
		InputCalibration.ch_bottom[j] = (uint32_t)data_array[i++];
		InputCalibration.ch_bottom[j] |= ((uint32_t)data_array[i++] << 8);
	}
}

void rxGetRCValues(void)
{
	int i = 0;
	uint8_t data[19];

	data[i++] = 0x09;
	data[i++] = 16;

	for (int j = 0; j < 8; j++)
	{
		data[i++] = (uint8_t)InputData.value[j];
		data[i++] = (uint8_t)(InputData.value[j] >> 8);
	}

	data[i++] = crc8(data, 18);
	UART0_SendData(data, 19);
}
