#include "input.h"

volatile static input_data *InputData;
volatile static input_calibration InputCalibration, TempCalibration;

void InitInputs(void)
{
	if (EEMUL_DATA->ID == KFLY_ID)
	{
		uint8_t role = (uint8_t)EEMUL_DATA->INPUT_ROLE;
		InputCalibration.role = 0;
		
		// Load role data
		/* Role (bits):
		 * Throttle (1-0)
		 * Pitch (3-2)
		 * Roll (5-4)
		 * Yaw (7-6)
		 */
		for (uint8_t i = 0; i < 4; i++)
		{
			InputCalibration.role |= ((i << (2*(role & 0x03))) & 0x03);
			role >>= 2;
		}
		
		// Load calibration data
		for (int i = 0; i < 8; i++)
		{
			InputCalibration.ch_bottom[i] = EEMUL_DATA->INPUTCAL[i][2];
			InputCalibration.ch_center[i] = EEMUL_DATA->INPUTCAL[i][1];
			InputCalibration.ch_top[i] = EEMUL_DATA->INPUTCAL[i][0];
		}
	}
	else
	{	// No data was found... Use standard values
		
		/* Role (bits):
		 * Throttle (1-0): 	Ch 3
		 * Pitch (3-2): 	Ch 2
		 * Roll (5-4): 		Ch 4
		 * Yaw (7-6): 		Ch 1
		 */
		 
		InputCalibration.role = 0b00110110;
		
		for (int i = 0; i < 8; i++)
		{
			InputCalibration.ch_bottom[i] = 1000;
			InputCalibration.ch_center[i] = 1500;
			InputCalibration.ch_top[i] = 2000;
		}
		
		InputCalibration.ch_center[2] = 1000;	// Channel 3 is throttle
	}	
	
	// Copy data to TempCalibration
	for (int i = 0; i < 8; i++)
	{
		TempCalibration.ch_bottom[i] = InputCalibration.ch_bottom[i];
		TempCalibration.ch_center[i] = InputCalibration.ch_center[i];
		TempCalibration.ch_top[i] = InputCalibration.ch_top[i];
	}
	
	EINT_Init();
	InputData = EINT_GetPointerToValues();
}

/**
 * Return the status of the inputs.
 * 
 * Bit 0 = Channel 1
 * Bit 1 = Cahnnel 2
 * 		...
 * Bit 7 = Channel 8
 * 
 * Bit Cleared = Normal Operation
 * Bit Set = No Connection
 **/
uint8_t GetInputStatus(void)
{
	uint8_t status;
	for (uint8_t i = 0; i < 8; i++)
	{
		if (GetRawInputLevel(i) == 0)
			status |= (1<<i);
	}
	
	return status;
}

uint8_t GetRoleChannel(uint8_t role)
{
	return ((InputCalibration.role >> role) & 0x03);
}

/**
 * Resets old calibrated values in order for new
 * values to be written.
 **/
void ResetCalibration(void)
{
	for (uint8_t i = 0; i < 8; i++)
	{
		InputCalibration.ch_bottom[i] = 1500;
		InputCalibration.ch_center[i] = 1500;
		InputCalibration.ch_top[i] = 1500;
	}
}

/**
 * Calibrates the top and bottom level of the inputs.
 * Loop it while calibrating
 **/
void CalibrateInputLevels(void)
{
	for (int i = 0; i < 8; i++)
	{
		if (GetRawInputLevel(i) > InputCalibration.ch_top[i])
			InputCalibration.ch_top[i] = GetRawInputLevel(i);
			
		if (GetRawInputLevel(i) < InputCalibration.ch_bottom[i])
			InputCalibration.ch_top[i] = GetRawInputLevel(i);
	}
}

/**
 * Calibrates the center level of the inputs.
 * 
 * If the vaules are withing 12.5% of top or bottom then the
 * center value will equal top or bottom when calibrated in order 
 * to get -100 to 100%, 0 to 100% and -100 to 0%.
 **/
void CalibrateCenterLevels(void)
{
	for (int i = 0; i < 8; i++)
	{
		if (GetRawInputLevel(i) > (InputCalibration.ch_top[i] - InputCalibration.ch_top[i]>>3))
			InputCalibration.ch_center[i] = InputCalibration.ch_top[i];
		
		else if (GetRawInputLevel(i) < (InputCalibration.ch_bottom[i] + InputCalibration.ch_bottom[i]>>3))
			InputCalibration.ch_center[i] = InputCalibration.ch_bottom[i];
		
		else
			InputCalibration.ch_center[i] = GetRawInputLevel(i);	
	}
}

void SaveCalibratedDataToFlashBuffer(void)
{
	/* Save all calibrated data to the flash buffer function goes here... */
}

/**
 * Returns the current RC stick position with bias compensation in 24.8-bit signed fixed point.
 **/
fix32 GetInputLevel(uint8_t channel)
{
	channel = GetRoleChannel(channel);
	
	if (GetRawInputLevel(channel) == 0)
		return 0;
	
	fix32 level = (fix32)(((int32_t)GetRawInputLevel(channel) - (int32_t)InputCalibration.ch_center[channel])*FP_MUL);
	fix32 temp;
	
	if (level > 0)
	{
		if (InputCalibration.ch_center[channel] == InputCalibration.ch_top[channel])
			return 0;
		
		temp = level/(fix32)(InputCalibration.ch_top[channel] - InputCalibration.ch_center[channel]);
		
		if (temp > 1*FP_MUL)	// Just in case something has happened
			return 1*FP_MUL;
		else 
			return temp;
	}
		
	else if (level < 0)
	{
		if (InputCalibration.ch_center[channel] == InputCalibration.ch_bottom[channel])
			return 0;
		
		temp = level/(fix32)(InputCalibration.ch_center[channel] - InputCalibration.ch_bottom[channel]);
		
		if (temp < -1*FP_MUL)	// Just in case something has happened
			return -1*FP_MUL;
		else 
			return temp;
	}
	else
		return 0;	
}

/**
 * Returns the current RC stick position without bias compensation.
 **/
uint16_t GetRawInputLevel(uint8_t channel)
{
	channel = GetRoleChannel(channel);
	return InputData->value[channel];
}
