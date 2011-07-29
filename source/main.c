#include "main.h"

void main( void )
{
	/* Configure the hardware for use. */
	prvSetupHardware();

	/* Load Tasks */
	xTaskCreate(vTask1,
				"Blink",
				80,
				NULL,
				1,
				NULL);
				
	xTaskCreate(vTaskControlLoop,
				"Control",
				200,
				NULL,
				2,
				NULL);
				
	xTaskCreate(vTaskArmDisarm,
				"Arm/Disarm",
				80,
				NULL,
				1,
				NULL);
	
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
	LED_Init();
	UART_Init();
	//GPS_UART_Init();
	PWM_Init();
	I2C0_Init();
	vInitInputs();
	Timer2_Init();
	SystemInit();
	__enable_irq();
	BMA180_Init(BW_300HZ, RANGE_2G);
	ITG3200_Init();
}

void vTask1(void *pvParameters)
{
	while (1)
	{

		vTaskDelay(100 / portTICK_RATE_MS);
	}
}

void vTaskControlLoop(void *pvParameters)
{
	portTickType xLastWakeTime = xTaskGetTickCount();
	
	static float acc_tmp[2];
	static float gyro_tmp[3];
	static float pid_tmp[3];
	static float thr_base, thr_raw;
	
	static kalman_data data_xz;
	static kalman_data data_yz;
	
	static pid_data data_roll;
	static pid_data data_pitch;
	static pid_data data_yaw;
	
	InitKalman(&data_xz);
	InitKalman(&data_yz);
	InitPID(&data_roll, ROLL_CHANNEL);
	InitPID(&data_pitch, PITCH_CHANNEL);
	InitPID(&data_yaw, YAW_CHANNEL);
	
	while (1)
	{
		ReadAccAngle(acc_tmp);
		ReadGyroRate(gyro_tmp);
		
		UpdKalman(&data_xz, acc_tmp[0], gyro_tmp[0]);
		UpdKalman(&data_yz, acc_tmp[1], gyro_tmp[1]);
		
		if (EnginesArmed() == TRUE)
		{
			if (PIDArmed() == TRUE)
			{
				pid_tmp[0] = PIDUpdateChannel(&data_pitch, &data_xz, PITCH_CHANNEL);
				pid_tmp[1] = PIDUpdateChannel(&data_roll, &data_yz, ROLL_CHANNEL);
				pid_tmp[2] = PIDUpdateYaw(&data_yaw, gyro_tmp[2]);
			}
			
			thr_raw = GetInputLevel(THROTTLE_CHANNEL);
			
			if (thr_raw > 0.05f)
			{
				thr_base = ((float)MAX_PWM*0.9f)*thr_raw;
			
				// Pitch base throttle and differential throttle
				PWM_setOutput((int)(thr_base + pid_tmp[0] + pid_tmp[2]), 0);
				PWM_setOutput((int)(thr_base - pid_tmp[0] + pid_tmp[2]), 1);
				
				// Roll base throttle and differential throttle
				PWM_setOutput((int)(thr_base + pid_tmp[1] - pid_tmp[2]), 2);
				PWM_setOutput((int)(thr_base - pid_tmp[1] - pid_tmp[2]), 3);
			}
			else
			{
				for (uint8_t i = 0; i < 6; i++)
					PWM_setOutput(MAX_PWM/15, i);
			}
		}
		else
		{
			for (uint8_t i = 0; i < 6; i++)
				PWM_setOutput(0, i);
		}

		vTaskDelayUntil(&xLastWakeTime, 5 / portTICK_RATE_MS);
	}
}

void vTaskArmDisarm(void *pvParameters)
{
	portTickType xLastWakeTime = xTaskGetTickCount();
	static uint16_t arm_counter = 0;
	static uint16_t disarm_counter = 0;
	
	while (1)
	{
		// Arming and disarming the regulator
		if (GetInputStatus() & CHANNELMASK)
		{	// No valid inputs. Disarm everything
			arm_counter = 0;
			disarm_counter = 0;
			PIDDisarm();
			DisarmEngines();
			clearLED(1);
			clearLED(2);
		}
		
		if ((GetInputLevel(THROTTLE_CHANNEL) > 0.1f) && (EnginesArmed() == TRUE))
		{
			PIDArm();
			setLED(2);
		}
		else
		{
			PIDDisarm();
			clearLED(2);
		}
			
		if (arm_counter > 9)
		{
			arm_counter = 0;
			disarm_counter = 0;
			ArmEngines();
			setLED(1);
		}
		else if ((GetInputLevel(THROTTLE_CHANNEL) < 0.05f) && (GetInputLevel(YAW_CHANNEL) < -0.95f))
			arm_counter++;
		else
			arm_counter = 0;
			
		if (disarm_counter > 9)
		{
			arm_counter = 0;
			disarm_counter = 0;
			DisarmEngines();
			clearLED(1);
		}
		else if ((GetInputLevel(THROTTLE_CHANNEL) < 0.05f) && (GetInputLevel(YAW_CHANNEL) > 0.95f))
			disarm_counter++;
		else
			disarm_counter = 0;
			
		vTaskDelayUntil(&xLastWakeTime, 100 / portTICK_RATE_MS);
	}
}

void vConfigureTimerForRunTimeStats( void )
{
	Timer1_Init();
}
