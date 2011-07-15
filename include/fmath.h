#ifndef __FMATH_H__
#define __FMATH_H__

#include <stdint.h>

#define M_PI 		3.14159265f
#define RAD2DEG		180.0f/M_PI

float fInvSqrt(float);
void ftoa(float);
float fabs(float);
float fArctan2(float, float);
uint8_t ksizeof(char *);

#endif
