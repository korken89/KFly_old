#ifndef __INPUT_H__
#define __INPUT_H__

#include "FreeRTOS.h"
#include "task.h"
#include "configKFly.h"
#include "extint.h"
#include "timer.h"
#include "fmath.h"
#include "eemul.h"

typedef struct
{
	uint8_t role;
	int16_t ch_center[8];
	int16_t ch_top[8];
	int16_t ch_bottom[8];
} input_calibration;

void InitInputs(void);
uint8_t GetInputStatus(void);
void ResetCalibration(void);
void CalibrateInputLevels(void);
void CalibrateCenterLevels(void);
void SaveCalibratedDataToFlashBuffer(void);		
fix32 GetInputLevel(uint8_t);
uint16_t GetRawInputLevel(uint8_t);

#endif
