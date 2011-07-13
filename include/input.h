#ifndef __INPUT_H__
#define __INPUT_H__

#include "extint.h"
#include "timer.h"

uint8_t GetInputStatus(void);
int16_t GetInputLevel(uint8_t);

#endif
