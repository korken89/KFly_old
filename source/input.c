#include "input.h"

volatile static input_data sInputData;

void vInitInputs(void)
{
	sInputData.status = 0b01111110;		// Start with no connection.
	for (int i = 0; i < 6; i++)
		sInputData.ch[i] = 0;			// All inputs to zero
		
	EINT_Init(&sInputData);
}

uint8_t GetInputStatus(void)
{
	return sInputData.status;
}

/* Add calibration data! */
int16_t GetInputLevel(uint8_t channel)
{
	return sInputData.ch[channel -1];
}
