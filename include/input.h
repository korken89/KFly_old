#ifndef __INPUT_H__
#define __INPUT_H__

#include "FreeRTOS.h"
#include "task.h"
#include "configKFly.h"
#include "extint.h"
#include "fmath.h"
#include "eemul.h"

typedef struct
{
	uint32_t role;
	int32_t ch_center[8];
	int32_t ch_top[8];
	int32_t ch_bottom[8];
} input_calibration;

void InitInputs(void);
uint8_t GetInputStatus(void);
fix32 GetInputLevel(uint8_t);
int32_t GetRawInputLevel(uint8_t);

#endif
