#include "main.h"

void main( void )
{
	/* Configure the hardware for use. */
	prvSetupHardware();

	/* Load Tasks */
	xTaskCreate(vTask1,
				"Blink",
				20,
				NULL,
				2,
				NULL);
				
	xTaskCreate(vTask2,
				"Data",
				80,
				NULL,
				2,
				NULL);
				
	/*xTaskCreate(vTask3,
				"Check",
				240,
				NULL,
				2,
				NULL);*/
	
	/* Start the scheduler. */
	vTaskStartScheduler();

	/* 	Will only get here if there was insufficient memory to create the idle
		task.  The idle task is created within vTaskStartScheduler(). */
	while(1);
}

void prvSetupHardware( void )
{
	__disable_irq();
	LED_Init();
	UART_Init();
	//GPS_UART_Init();
	//Timer1_Init();
	//Timer2_Init();
	PWM_Init();
	I2C0_Init();
	SystemInit();
	__enable_irq();
	BMA180_Init(BW_150HZ, RANGE_2G);
	ITG3200_Init();
}
/*-----------------------------------------------------------*/

void vTask1(void *pvParameters)
{
	while (1)
	{
		setLED(1);
		clearLED(2);
		vTaskDelay(200 / portTICK_RATE_MS);
		
		setLED(2);
		clearLED(1);
		vTaskDelay(200 / portTICK_RATE_MS);
	}
}

void vTask2(void *pvParameters)
{
	portTickType xLastWakeTime = xTaskGetTickCount();
	
	while (1)
	{

		for (volatile int i = 0; i < 100000; i++);


		vTaskDelayUntil(&xLastWakeTime, 100 / portTICK_RATE_MS);
	}
}

void vTask3(void *pvParameters)
{
	static signed char buffer[100] = {0};
	while (1)
	{
		vTaskDelay(5000 / portTICK_RATE_MS);
		//vTaskGetRunTimeStats(buffer);
		//UART0_SendString(buffer);
		//UART0_SendString("Pitch:");
	}
}

void vConfigureTimerForRunTimeStats( void )
{
	Timer1_Init();
}

/*-----------------------------------------------------------*/

