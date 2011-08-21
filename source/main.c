#include "main.h"

void main( void )
{
	/* Configure the hardware for use. */
	prvSetupHardware();

	/* Load Tasks */
	/*xTaskCreate(vTask1,
				"Blink",
				80,
				NULL,
				1,
				NULL);*/
	
	/* Start the scheduler. */
	vTaskStartScheduler();

	/* 	Will only get here if there was insufficient memory to create the idle
		task.  The idle task is created within vTaskStartScheduler(). */
	
	while(1)
	{
		
	}
}

void prvSetupHardware( void )
{
	__disable_irq();
	InitTelemetry();
	InitInputs();
	InitControlLoops();
	SystemInit();
	__enable_irq();
	InitSensors();
}

void vTask1(void *pvParameters)
{
	char *str;
	int temp;
	while (1)
	{
		temp = GetInputLevel(THROTTLE_CHANNEL);
		if (temp < 0)
		{
			temp = -temp;
			UART0_SendData("-", 1);
		}
		str = itoa(temp, 10);
		UART0_SendData(str, ksizeof(str));
		UART0_SendData("\t", 1);
		temp = GetInputLevel(PITCH_CHANNEL);
		if (temp < 0)
		{
			temp = -temp;
			UART0_SendData("-", 1);
		}
		str = itoa(temp, 10);
		UART0_SendData(str, ksizeof(str));
		UART0_SendData("\t", 1);
		temp = GetInputLevel(ROLL_CHANNEL);
		if (temp < 0)
		{
			temp = -temp;
			UART0_SendData("-", 1);
		}
		str = itoa(temp, 10);
		UART0_SendData(str, ksizeof(str));
		UART0_SendData("\t", 1);
		temp = GetInputLevel(YAW_CHANNEL);
		if (temp < 0)
		{
			temp = -temp;
			UART0_SendData("-", 1);
		}
		str = itoa(temp, 10);
		UART0_SendData(str, ksizeof(str));
		UART0_SendData("\n", 1);
		vTaskDelay(100 / portTICK_RATE_MS);
	}
}

void vConfigureTimerForRunTimeStats( void )
{
	Timer1_Init();
}
