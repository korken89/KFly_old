/******************************************************************************
 * @file :   	fmath.c
 * @brief : 	Floating point math functions
 * @version : 	V1.0
 * @author :	Emil Fresk
 ******************************************************************************/
 
#include "fmath.h"
#include "uart.h"

float fInvSqrt(float x)
{
	float xhalf = 0.5f*x;
	int i = *(int*)&x; // get bits for floating value
	i = 0x5f375a86- (i>>1); // gives initial guess y0
	x = *(float*)&i; // convert bits back to float
	x = x*(1.5f-xhalf*x*x); // Newton step, repeating increases accuracy
	
	return x;
}

void ftoa(float a)
{
	if (a < 0)
	{
		UART0_SendChar('-');
		a = -a;
	}
	
	int heltal = (int)a;
	int decimal = (int)((a-(float)heltal)*1000);
	UART0_SendString(itoa(heltal,10));
	UART0_SendChar('.');
	UART0_SendString(itoa(decimal,10));
}

float fabs(float num)
{
	if (num <  0)
		return -num;
	else
		return num;
}

float fArctan2(float y, float x)
{
	float angle;
	const float coeff_1 = M_PI/4;
	const float coeff_2 = 3*coeff_1;
	float abs_y = fabs(y)+1e-10;      // kludge to prevent 0/0 condition
	
	if (x >= 0)
	{
		float r = (x - abs_y) / (x + abs_y);
		angle = coeff_1 - coeff_1 * r;
	}
	else
	{
		float r = (x + abs_y) / (abs_y - x);
		angle = coeff_2 - coeff_1 * r;
	}


	if (y < 0)
		return -angle;     // negate if in quad III or IV
	else
		return angle;
}
