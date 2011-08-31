#include "main.h"

void main( void )
{
	/* Configure the hardware for use. */
	prvSetupHardware();

	/* Load Tasks */
	/*xTaskCreate(vTask1,
				"Blink",
				200,
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
	{
		InitTelemetry();
		InitInputs();
		InitControlLoops();
		SystemInit();
	}
	__enable_irq();
	InitSensors();
}

void vTask1(void *pvParameters)
{
	while (1)
	{
		vTaskDelay(1000 / portTICK_RATE_MS);
		rxSaveToFlash();
		vTaskSuspend(NULL);
	}
}

void vConfigureTimerForRunTimeStats( void )
{
	Timer1_Init();
}
