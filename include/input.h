#ifndef __INPUT_H__
#define __INPUT_H__

#include "FreeRTOS.h"
#include "task.h"
#include "extint.h"
#include "timer.h"
#include "fmath.h"

typedef struct
{
	int16_t ch_center[6];
	int16_t ch_top[6];
	int16_t ch_bottom[6];
} input_calibration;

void vInitInputs(void);
uint8_t GetInputStatus(void);
void ResetCalibration(void);
void CalibrateInputLevels(void);
void CalibrateCenterLevels(void);
void SaveCalibratedDataToFlashBuffer(void);		
fix32 GetInputLevel(uint8_t);
uint16_t GetRawInputLevel(uint8_t);

#endif
