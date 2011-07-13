/******************************************************************************
 * @file :   	main.c
 * @brief : 	Main program
 * @version : 	V1.0
 * @author :	Emil Fresk
 ******************************************************************************/

#include <math.h>
#include "../include/main.h"

float fSqrt(float num)
{
	float a = num;
	uint8_t i = 15;
	while (i)
	{
		a = (a + num/a)/2;
		i--;
	}
	
	return a;
}

float InvSqrt(float x)
{
	float xhalf = 0.5f*x;
	int i = *(int*)&x; // get bits for floating value
	i = 0x5f375a86- (i>>1); // gives initial guess y0
	x = *(float*)&i; // convert bits back to float
	x = x*(1.5f-xhalf*x*x); // Newton step, repeating increases accuracy
	
	return x;
}

void main(void)
{
	__disable_irq();
	LED_Init();
	UART_Init();
	//GPS_UART_Init();
	Timer0_Init();
	PWM_Init();
	//ADC_Init();
	//EINT_Init();
	I2C0_Init();
	SystemInit();
	__enable_irq();
	clearLED(1);
	clearLED(2);
	
	volatile float fa = 20.0;
	volatile float fb = 43232.3;
	volatile float asd = 0;
	UART0_SendString("Float Multiplication: \0");
	Timer2_StartCount();
	asd = fb*fa;
	Timer2_StopCount();
	UART0_SendChar('\n');

	UART0_SendString("Float Division: \0");
	Timer2_StartCount();
	asd = fb/fa;
	Timer2_StopCount();
	UART0_SendChar('\n');

	UART0_SendString("Fast Inverse Square Root: \0");
	Timer2_StartCount();
	asd = InvSqrt(16000000.0);
	Timer2_StopCount();
	UART0_SendChar('\n');
	
	volatile int16_t temp[3];
	ITG3200_Init();
	BMA180_Init(BW_150HZ, RANGE_2G);
	
	UART0_SendString("Accelerometer Burst Read: \0");
	Timer2_StartCount();
	BMA180_BurstRead((int16_t *)temp);
	Timer2_StopCount();
	UART0_SendChar('\n');
	
	UART0_SendString("Gyro Burst Read: \0");
	Timer2_StartCount();
	ITG3200_BurstRead((int16_t *)temp);
	Timer2_StopCount();
	UART0_SendChar('\n');
	/*UART_SendString("AtoI: \0");
	Timer2_StartCount();
	volatile int i = atoi("123582452\0");
	Timer2_StopCount();
	UART_SendChar('\n');
	
	UART_SendString("ItoA: \0");
	Timer2_StartCount();
	char* ch = itoa(2192857321, 10);
	Timer2_StopCount();
	UART_SendChar('\n');
	
	volatile fixed a = (1<<18);
	volatile fixed b = 12345632;
	UART_SendString("Fixed Point division: \0");
	Timer2_StartCount();
	volatile fixed c = fDiv(a, b);
	Timer2_StopCount();
	UART_SendChar('\n');
	
	UART_SendString("Fixed Point multiplication: \0");
	Timer2_StartCount();
	c = fMul(a, b);
	Timer2_StopCount();
	UART_SendChar('\n');
	
	UART_SendString("Floating Point multiplication: \0");
	Timer2_StartCount();
	fc = fa*fb;
	Timer2_StopCount();
	UART_SendChar('\n');
	
	UART_SendString("Floating Point division: \0");
	Timer2_StartCount();
	fb = fa/fb;
	Timer2_StopCount();
	UART_SendChar('\n');
	UART_SendChar('\n');*/
	//UART0_SendChar(ReadWho());
	//ITG3200_Init();
	//int16_t temp[3];
	//uint8_t temp = BMA180_Read(ID);
	//UART0_SendChar(temp);
	//BMA180_Init(BW_150HZ, RANGE_2G);
	while(1)
	{
		//BMA180_test();
		//BMA180_BurstRead(temp);
		//temp[0] += 106;		// Bias...
		//temp[1] -= 86;
		//temp[2] -= 13;
		
		/*if (temp[0] < 0)
		{
			temp[0] = -temp[0];
			UART0_SendChar('-');
		}
		UART0_SendString(itoa((int)temp[0], 10));
		UART0_SendChar('\t');
		if (temp[1] < 0)
		{
			temp[1] = -temp[1];
			UART0_SendChar('-');
		}
		UART0_SendString(itoa((int)temp[1], 10));
		UART0_SendChar('\t');
		if (temp[2] < 0)
		{
			temp[2] = -temp[2];
			UART0_SendChar('-');
		}
		UART0_SendString(itoa((int)temp[2], 10));
		
		
		UART0_SendChar('\n');*/
		/*if (GPS_CharReady())
		{
			UART0_SendChar(GPS_GetChar());
		}
		
		if (UART0_CharReady())
		{
			GPS_SendChar(UART0_GetChar());
		}*/
		
		//UART0_SendChar(ITG3200_Read(0x00));
		//for (volatile uint32_t i = 0; i < 1000000; i++);
	}
}
