#ifndef __FMATH_H__
#define __FMATH_H__

#include <stdint.h>

#define M_PI 		3.14159265f
#define RAD2DEG		180.0f/M_PI

/* Fixed Point definitions 
 * Obs! All calculations are made in xx.8-bit SIGNED fixed point format unless otherwise stated
 * 		fix8 are one sign-bit and 7 fraction-bits (s.7-bit) thus max and min is 127 to -127.
 */
#define FP_MUL		256

typedef int8_t 		fix8;
typedef uint8_t 	fixu8;
typedef int16_t 	fix16;
typedef uint16_t 	fixu16;
typedef int32_t 	fix32;
typedef uint32_t 	fixu32;

float fInvSqrt(float);
void ftoa(float);
float fabs(float);
float fArctan2(float, float);
fix32 fix32Mul(fix32, fix32);
uint8_t ksizeof(char *);

#endif
