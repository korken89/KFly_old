#include "pid.h"

/** Private Globals **/
volatile static Bool bEnginesArmed = FALSE;
volatile static Bool bPIDArmed = FALSE;

/** Public Globals **/
volatile mix_data mixer;
volatile pid_data data_pitch;
volatile pid_data data_roll;
volatile pid_data data_yaw;

void vTaskControlLoop(void *pvParameters)
{
	portTickType xLastWakeTime = xTaskGetTickCount();

	static float acc_tmp[2];
	static float gyro_tmp[3];

	static kalman_data data_xz;
	static kalman_data data_yz;

	InitKalman(&data_xz);
	InitKalman(&data_yz);
	InitMixer();
	InitPID(PITCH_CHANNEL);
	InitPID(ROLL_CHANNEL);
	InitPID(YAW_CHANNEL);

	while (1)
	{
		ReadAccAngle(acc_tmp);
		ReadGyroRate(gyro_tmp);

		UpdKalman(&data_xz, acc_tmp[0], gyro_tmp[0]);
		UpdKalman(&data_yz, acc_tmp[1], gyro_tmp[1]);

		UpdOutputs(&data_xz, &data_yz, gyro_tmp[3]);

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

		if ((GetInputLevel(THROTTLE_CHANNEL) > (fix32)(0.1f*FP_MUL)) && (EnginesArmed() == TRUE))
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
		else if ((GetInputLevel(THROTTLE_CHANNEL) < (fix32)(0.05f*FP_MUL)) && (GetInputLevel(YAW_CHANNEL) > (fix32)(0.95f*FP_MUL)))
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
		else if ((GetInputLevel(THROTTLE_CHANNEL) < (fix32)(0.05f*FP_MUL)) && (GetInputLevel(YAW_CHANNEL) < (fix32)(-0.95f*FP_MUL)))
			disarm_counter++;
		else
			disarm_counter = 0;

		vTaskDelayUntil(&xLastWakeTime, 100 / portTICK_RATE_MS);
	}
}

void InitControlLoops(void)
{
	PWM_Init();
	I2C0_Init();

	/** Create FreeRTOS Tasks **/
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
}

void InitPID(uint8_t channel)
{
	if (channel == ROLL_CHANNEL)
    {
		data_roll.a_iState = 0;
		data_roll.r_iState = 0;

		if (EEMUL_DATA->ID == KFLY_ID)
		{
			data_roll.r_kp = (fix32)EEMUL_DATA->ROLL_R_KP;
			data_roll.r_ki = (fix32)EEMUL_DATA->ROLL_R_KI;
			
			data_roll.a_kp = (fix32)EEMUL_DATA->ROLL_A_KP;
			data_roll.a_ki = (fix32)EEMUL_DATA->ROLL_A_KI;
			
			data_roll.r_imax = (fix32)EEMUL_DATA->ROLL_R_IMAX;
			data_roll.a_imax = (fix32)EEMUL_DATA->ROLL_A_IMAX;
		}
		else
		{	
			data_roll.r_kp = (fix32)(2.0f*FP_MUL);
			data_roll.r_ki = (fix32)(0.0f*FP_MUL);
			
			data_roll.a_kp = (fix32)(0.6f*FP_MUL);
			data_roll.a_ki = (fix32)(0.0f*FP_MUL);
			
			data_roll.r_imax = (fix32)(30.0f*FP_MUL);
			data_roll.a_imax = (fix32)(30.0f*FP_MUL);
		}
	}
	
	else if (channel == PITCH_CHANNEL)
    {
		data_pitch.a_iState = 0;
		data_pitch.r_iState = 0;

		if (EEMUL_DATA->ID == KFLY_ID)
		{
			data_pitch.r_kp = (fix32)EEMUL_DATA->PITCH_R_KP;
			data_pitch.r_ki = (fix32)EEMUL_DATA->PITCH_R_KI;
			
			data_pitch.a_kp = (fix32)EEMUL_DATA->PITCH_A_KP;
			data_pitch.a_ki = (fix32)EEMUL_DATA->PITCH_A_KI;
			
			data_pitch.r_imax = (fix32)EEMUL_DATA->PITCH_R_IMAX;
			data_pitch.a_imax = (fix32)EEMUL_DATA->PITCH_A_IMAX;
		}
		else
		{	
			data_pitch.r_kp = (fix32)(2.0f*FP_MUL);
			data_pitch.r_ki = (fix32)(0.0f*FP_MUL);
			
			data_pitch.a_kp = (fix32)(0.6f*FP_MUL);
			data_pitch.a_ki = (fix32)(0.0f*FP_MUL);
			
			data_pitch.r_imax = (fix32)(30.0f*FP_MUL);
			data_pitch.a_imax = (fix32)(30.0f*FP_MUL);
		}
	}
	
	else if (channel == YAW_CHANNEL)
    {
		data_yaw.a_iState = 0;
		data_yaw.r_iState = 0;

		if (EEMUL_DATA->ID == KFLY_ID)
		{
			data_yaw.r_kp = (fix32)EEMUL_DATA->YAW_R_KP;
			data_yaw.r_ki = (fix32)EEMUL_DATA->YAW_R_KI;
			
			data_yaw.a_kp = (fix32)EEMUL_DATA->YAW_A_KP;
			data_yaw.a_ki = (fix32)EEMUL_DATA->YAW_A_KI;
			
			data_yaw.r_imax = (fix32)EEMUL_DATA->YAW_R_IMAX;
			data_yaw.a_imax = (fix32)EEMUL_DATA->YAW_A_IMAX;
		}
		else
		{	
			data_yaw.r_kp = (fix32)(2.0f*FP_MUL);
			data_yaw.r_ki = (fix32)(0.0f*FP_MUL);
			
			data_yaw.a_kp = (fix32)(0.6f*FP_MUL);
			data_yaw.a_ki = (fix32)(0.0f*FP_MUL);
			
			data_yaw.r_imax = (fix32)(30.0f*FP_MUL);
			data_yaw.a_imax = (fix32)(30.0f*FP_MUL);
		}
	}	
}

void InitMixer(void)
{
	/*if (EEMUL_DATA->ID == KFLY_ID)
	{
		for (int i = 0; i < 8; i++)
			for (int j = 0; j < 4; j++)
				mixer.mix[i][j] = (fix8)EEMUL_DATA->MIX[i][j];
	}
	else
	{	*/
		for (int i = 0; i < 8; i++)
			for (int j = 0; j < 4; j++)
				mixer.mix[i][j] = 0;
	//}
}

fix32 PIDUpdatePitch(kalman_data *data)
{
	static int i = 0;
	fix32 angle_error, rate_error, rate_aim, input;

	input = GetInputLevel(PITCH_CHANNEL);

	/* Angle regulator starts here! */
	angle_error = fix32Mul((fix32)(MAX_ANGLE*FP_MUL), input) - (fix32)(data->x1*FP_MUL);
	data_pitch.a_iState += fix32Mul(angle_error, data_pitch.a_ki);

	if (data_pitch.a_iState > data_pitch.a_imax)
		data_pitch.a_iState = data_pitch.a_imax;
	else if (data_pitch.a_iState < -data_pitch.a_imax)
		data_pitch.a_iState = -data_pitch.a_imax;

	rate_aim = data_pitch.a_iState + fix32Mul(angle_error, data_pitch.a_kp);

	if (rate_aim > (fix32)(MAX_RATE*FP_MUL))
		rate_aim = (fix32)(MAX_RATE*FP_MUL);
	else if (rate_aim < -(fix32)(MAX_RATE*FP_MUL))
		rate_aim = -(fix32)(MAX_RATE*FP_MUL);

	/* Rate regulator starts here! */
	rate_error = rate_aim - (fix32)((data->x2 - data->x3)*FP_MUL);
	data_pitch.r_iState = data_pitch.r_iState + fix32Mul(rate_error, data_pitch.r_ki);

	if (data_pitch.r_iState > data_pitch.r_imax)
		data_pitch.r_iState = data_pitch.r_imax;
	else if (data_pitch.r_iState < -data_pitch.r_imax)
		data_pitch.r_iState = -data_pitch.r_imax;

	return data_pitch.r_iState + fix32Mul(rate_error, data_pitch.r_kp);
}

fix32 PIDUpdateRoll(kalman_data *data)
{	
	static int i = 0;
	fix32 angle_error, rate_error, rate_aim, input;
	
	input = GetInputLevel(ROLL_CHANNEL);
		
	/* Angle regulator starts here! */
	angle_error = fix32Mul((fix32)(MAX_ANGLE*FP_MUL), input) - (fix32)(data->x1*FP_MUL);
	data_roll.a_iState += fix32Mul(angle_error, data_roll.a_ki);
	
	if (data_roll.a_iState > data_roll.a_imax)
		data_roll.a_iState = data_roll.a_imax;
	else if (data_roll.a_iState < -data_roll.a_imax)
		data_roll.a_iState = -data_roll.a_imax;
	
	rate_aim = data_roll.a_iState + fix32Mul(angle_error, data_roll.a_kp);
	
	if (rate_aim > (fix32)(MAX_RATE*FP_MUL))
		rate_aim = (fix32)(MAX_RATE*FP_MUL);
	else if (rate_aim < -(fix32)(MAX_RATE*FP_MUL))
		rate_aim = -(fix32)(MAX_RATE*FP_MUL);
	
	/* Rate regulator starts here! */
	rate_error = rate_aim - (fix32)((data->x2 - data->x3)*FP_MUL);
	data_roll.r_iState = data_roll.r_iState + fix32Mul(rate_error, data_roll.r_ki);
	
	if (data_roll.r_iState > data_roll.r_imax)
		data_roll.r_iState = data_roll.r_imax;
	else if (data_roll.r_iState < -data_roll.r_imax)
		data_roll.r_iState = -data_roll.r_imax;
		
	return data_roll.r_iState + fix32Mul(rate_error, data_roll.r_kp);
}
 
fix32 PIDUpdateYaw(float yawrate)
{
	fix32 rate_error, rate_aim, input;
	
	input = GetInputLevel(YAW_CHANNEL);
	rate_aim = fix32Mul(input, (fix32)(MAX_YAW_RATE*FP_MUL));
	
	/* Rate regulator starts here! */
	rate_error = rate_aim - (fix32)(yawrate*FP_MUL);
	data_yaw.r_iState = data_yaw.r_iState + fix32Mul(rate_error, data_yaw.r_ki);
	
	if (data_yaw.r_iState > data_yaw.r_imax)
		data_yaw.r_iState = data_yaw.r_imax;
	else if (data_yaw.r_iState < -data_yaw.r_imax)
		data_yaw.r_iState = -data_yaw.r_imax;
		
	return data_yaw.r_iState + fix32Mul(rate_error, data_yaw.r_kp);
}

void UpdOutputs(kalman_data *data_xz, kalman_data *data_yz, float gyro_tmp)
{
	fix32 pid_pitch, pid_roll, pid_yaw, thr_base;
	
	if (EnginesArmed() == TRUE)
	{
		if (PIDArmed() == TRUE)
		{
			pid_pitch = PIDUpdatePitch(data_xz);
			pid_roll = PIDUpdateRoll(data_yz);
			pid_yaw = PIDUpdateYaw(gyro_tmp);
			
			thr_base = fix32Mul(MAX_PWM, GetInputLevel(THROTTLE_CHANNEL));
			
			for (int i = 0; i < 8; i++)		// Manual fixed point multiplication and FP to INT
			{
				PWM_setOutput((int)((thr_base*mixer.mix[i][0] + pid_pitch*mixer.mix[i][1] + \
								pid_roll*mixer.mix[i][2] + pid_yaw*mixer.mix[i][3]) >> 15), i);
			}
		}
		
		else
		{
			for (uint8_t i = 0; i < 8; i++)
				PWM_setOutput(MAX_PWM/18, i);
		}
	}
	else
	{
		for (uint8_t i = 0; i < 8; i++)
			PWM_setOutput(0, i);
	}
}

void PIDArm(void)
{
	bPIDArmed = TRUE;
}

void PIDDisarm(void)
{
	bPIDArmed = FALSE;
}

Bool PIDArmed(void)
{
	return (Bool)bPIDArmed;
}

void ArmEngines(void)
{
	bEnginesArmed = TRUE;
}

void DisarmEngines(void)
{
	bEnginesArmed = FALSE;
}

Bool EnginesArmed(void)
{
	return (Bool)bEnginesArmed;
}
