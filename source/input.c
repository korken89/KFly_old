#include "input.h"

/** External Globals **/
extern input_data InputData;

/** Public Globals **/
volatile input_calibration InputCalibration;

/** Private Globals **/
volatile static Bool CalibrateCenters = TRUE;
volatile static xTaskHandle xCalibrationTaskHandle;

void InitInputs(void)
{
	if (EEMUL_DATA->ID == KFLY_ID)
	{
		uint32_t role = EEMUL_DATA->INPUT_ROLE;
		InputCalibration.role = 0;
		
		// Load role data
		/* Role 	(3 bits):
		 * Throttle	(0-2)
		 * Pitch 	(3-5)
		 * Roll		(6-8)
		 * Yaw		(9-11)
		 * Mode		(12-14)
		 * Aux 1	(15-17)
		 * Aux 2	(18-20)
		 * Aux 3	(21-23)
		 */
		for (uint8_t i = 0; i < 8; i++)
		{
			InputCalibration.role |= ((i << (3*(role & 0x07))) & 0x07);
			role >>= 3;
		}
		
		// Load calibration data
		for (int i = 0; i < 8; i++)
		{
			InputCalibration.ch_top[i] = EEMUL_DATA->INPUTCAL[i][0];
			InputCalibration.ch_center[i] = EEMUL_DATA->INPUTCAL[i][1];
			InputCalibration.ch_bottom[i] = EEMUL_DATA->INPUTCAL[i][2];
		}
	}
	else
	{	// No data was found... Use standard values
		
		/* Role 	(3 bits)	Channel
		 * Throttle	(0-2):		Ch 3 (2)
		 * Pitch 	(3-5):		Ch 2 (1)
		 * Roll		(6-8):		Ch 4 (3)
		 * Yaw		(9-11):		Ch 1 (0)
		 * Mode		(12-14):	Ch 5 (4)
		 * Aux 1	(15-17):	Ch 6 (5)
		 * Aux 2	(18-20):	Ch 7 (6)
		 * Aux 3	(21-23):	Ch 8 (7)
		 */
							//bit ..................... 9876543210
		InputCalibration.role = 0b00000000111110101100000011001010;
		
		for (int i = 0; i < 8; i++)
		{
			InputCalibration.ch_top[i] = 2000;
			InputCalibration.ch_center[i] = 1500;
			InputCalibration.ch_bottom[i] = 1000;
		}
		
		InputCalibration.ch_center[2] = 1000;	// Channel 3 is throttle
	}	
	
	// Initialize External Interrupts
	EINT_Init();
}

/**
 * Return the status of the inputs.
 *
 * Bit 0 = Channel 1
 * Bit 1 = Channel 2
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
	return (uint8_t)((InputCalibration.role >> role) & 0x07);
}

/**
 * Returns the current RC stick position with bias compensation in 24.8-bit signed fixed point.
 **/
fix32 GetInputLevel(uint8_t channel)
{	
	channel = GetRoleChannel(channel);
	int32_t data = GetRawInputLevel(channel);

	if (data == 0)
		return 0;
	
	fix32 level = (fix32)((data - InputCalibration.ch_center[channel])*FP_MUL);
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
int32_t GetRawInputLevel(uint8_t channel)
{
	return InputData.value[channel];
}
