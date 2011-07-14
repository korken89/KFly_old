#include "input.h"

volatile static input_data *sInputData;

void vInitInputs(void)
{
	EINT_Init();
}

uint8_t GetInputStatus(void)
{
	
}

/* Add calibration data! */
int16_t GetInputLevel(uint8_t channel)
{
	return sInputData->ch[channel -1];
}
