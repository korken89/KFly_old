#include "pid.h"

/** Public Globals **/
volatile limits_data FlightLimits;
volatile mix_data Mixer;
volatile pid_data DataPitch;
volatile pid_data DataRoll;
volatile pid_data DataYaw;

/** Private Globals **/
volatile static Bool bEnginesArmed = FALSE;
volatile static Bool bPIDArmed = FALSE;

volatile debug_data DebugStore;

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
	LED_Init();
	PWM_Init();
	I2C0_Init();

	InitFlightLimits();
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

void InitFlightLimits(void)
{
	if (EEMUL_DATA->ID == KFLY_ID)
	{
		FlightLimits.maxangle = (fix32)EEMUL_DATA->MAXANGLE;
		FlightLimits.maxrate = (fix32)EEMUL_DATA->MAXRATE;
		FlightLimits.maxyawrate = (fix32)EEMUL_DATA->MAXYAWRATE;
	}
	else
	{
		FlightLimits.maxangle = (fix32)MAX_ANGLE*FP_MUL;
		FlightLimits.maxrate = (fix32)MAX_RATE*FP_MUL;
		FlightLimits.maxyawrate = (fix32)MAX_YAW_RATE*FP_MUL;
	}
}

void InitPID(uint8_t channel)
{
	if (channel == ROLL_CHANNEL)
    {
		DataRoll.a_iState = 0;
		DataRoll.r_iState = 0;

		if (EEMUL_DATA->ID == KFLY_ID)
		{
			DataRoll.r_kp = (fix32)EEMUL_DATA->ROLL_R_KP;
			DataRoll.r_ki = (fix32)EEMUL_DATA->ROLL_R_KI;
			
			DataRoll.a_kp = (fix32)EEMUL_DATA->ROLL_A_KP;
			DataRoll.a_ki = (fix32)EEMUL_DATA->ROLL_A_KI;
			
			DataRoll.r_imax = (fix32)EEMUL_DATA->ROLL_R_IMAX;
			DataRoll.a_imax = (fix32)EEMUL_DATA->ROLL_A_IMAX;
		}
		else
		{	
			DataRoll.r_kp = (fix32)(2.0f*FP_MUL);
			DataRoll.r_ki = (fix32)(0.0f*FP_MUL);
			
			DataRoll.a_kp = (fix32)(0.6f*FP_MUL);
			DataRoll.a_ki = (fix32)(0.0f*FP_MUL);
			
			DataRoll.r_imax = (fix32)(30.0f*FP_MUL);
			DataRoll.a_imax = (fix32)(30.0f*FP_MUL);
		}
	}
	
	else if (channel == PITCH_CHANNEL)
    {
		DataPitch.a_iState = 0;
		DataPitch.r_iState = 0;

		if (EEMUL_DATA->ID == KFLY_ID)
		{
			DataPitch.r_kp = (fix32)EEMUL_DATA->PITCH_R_KP;
			DataPitch.r_ki = (fix32)EEMUL_DATA->PITCH_R_KI;
			
			DataPitch.a_kp = (fix32)EEMUL_DATA->PITCH_A_KP;
			DataPitch.a_ki = (fix32)EEMUL_DATA->PITCH_A_KI;
			
			DataPitch.r_imax = (fix32)EEMUL_DATA->PITCH_R_IMAX;
			DataPitch.a_imax = (fix32)EEMUL_DATA->PITCH_A_IMAX;
		}
		else
		{	
			DataPitch.r_kp = (fix32)(2.0f*FP_MUL);
			DataPitch.r_ki = (fix32)(0.0f*FP_MUL);
			
			DataPitch.a_kp = (fix32)(0.6f*FP_MUL);
			DataPitch.a_ki = (fix32)(0.0f*FP_MUL);
			
			DataPitch.r_imax = (fix32)(30.0f*FP_MUL);
			DataPitch.a_imax = (fix32)(30.0f*FP_MUL);
		}
	}
	
	else if (channel == YAW_CHANNEL)
    {
		DataYaw.a_iState = 0;
		DataYaw.r_iState = 0;

		if (EEMUL_DATA->ID == KFLY_ID)
		{
			DataYaw.r_kp = (fix32)EEMUL_DATA->YAW_R_KP;
			DataYaw.r_ki = (fix32)EEMUL_DATA->YAW_R_KI;
			
			DataYaw.a_kp = (fix32)EEMUL_DATA->YAW_A_KP;
			DataYaw.a_ki = (fix32)EEMUL_DATA->YAW_A_KI;
			
			DataYaw.r_imax = (fix32)EEMUL_DATA->YAW_R_IMAX;
			DataYaw.a_imax = (fix32)EEMUL_DATA->YAW_A_IMAX;
		}
		else
		{	
			DataYaw.r_kp = (fix32)(2.0f*FP_MUL);
			DataYaw.r_ki = (fix32)(0.0f*FP_MUL);
			
			DataYaw.a_kp = (fix32)(0.6f*FP_MUL);
			DataYaw.a_ki = (fix32)(0.0f*FP_MUL);
			
			DataYaw.r_imax = (fix32)(30.0f*FP_MUL);
			DataYaw.a_imax = (fix32)(30.0f*FP_MUL);
		}
	}	
}

void InitMixer(void)
{
	if (EEMUL_DATA->ID == KFLY_ID)
	{
		for (int i = 0; i < 8; i++)
			for (int j = 0; j < 4; j++)
				Mixer.mix[i][j] = (fix8)EEMUL_DATA->MIX[i][j];
	}
	else
	{
		for (int i = 0; i < 8; i++)
			for (int j = 0; j < 4; j++)
				Mixer.mix[i][j] = 0;

		for (int j = 0; j < 4; j++)
				Mixer.mix[j][0] = 127;

	}
}

fix32 PIDUpdatePitch(kalman_data *data)
{
	fix32 angle_error, rate_error, rate_aim, input;

	input = GetInputLevel(PITCH_CHANNEL);

	/* Angle regulator starts here! */
	angle_error = fix32Mul(FlightLimits.maxangle, input) - (fix32)(data->x1*FP_MUL);
	DataPitch.a_iState += fix32Mul(angle_error, DataPitch.a_ki);

	// If integral is to big, limit it
	if (DataPitch.a_iState > DataPitch.a_imax)
		DataPitch.a_iState = DataPitch.a_imax;
	else if (DataPitch.a_iState < -DataPitch.a_imax)
		DataPitch.a_iState = -DataPitch.a_imax;

	// Calculate the desired rate and limit if it's to big
	rate_aim = DataPitch.a_iState + fix32Mul(angle_error, DataPitch.a_kp);

	if (rate_aim > FlightLimits.maxrate)
		rate_aim = FlightLimits.maxrate;
	else if (rate_aim < -FlightLimits.maxrate)
		rate_aim = -FlightLimits.maxrate;

	/* Rate regulator starts here! */
	rate_error = rate_aim - (fix32)((data->x2 - data->x3)*FP_MUL);
	DataPitch.r_iState = DataPitch.r_iState + fix32Mul(rate_error, DataPitch.r_ki);

	// If integral is to big, limit it
	if (DataPitch.r_iState > DataPitch.r_imax)
		DataPitch.r_iState = DataPitch.r_imax;
	else if (DataPitch.r_iState < -DataPitch.r_imax)
		DataPitch.r_iState = -DataPitch.r_imax;

	return DataPitch.r_iState + fix32Mul(rate_error, DataPitch.r_kp);
}

fix32 PIDUpdateRoll(kalman_data *data)
{	
	fix32 angle_error, rate_error, rate_aim, input;
	
	input = GetInputLevel(ROLL_CHANNEL);

	/* Angle regulator starts here! */
	angle_error = fix32Mul(FlightLimits.maxangle, input) - (fix32)(data->x1*FP_MUL);
	DataRoll.a_iState += fix32Mul(angle_error, DataRoll.a_ki);

	// If integral is to big, limit it
	if (DataRoll.a_iState > DataRoll.a_imax)
		DataRoll.a_iState = DataRoll.a_imax;
	else if (DataRoll.a_iState < -DataRoll.a_imax)
		DataRoll.a_iState = -DataRoll.a_imax;
	
	// Calculate the desired rate and limit if it's to big
	rate_aim = DataRoll.a_iState + fix32Mul(angle_error, DataRoll.a_kp);

	if (rate_aim > FlightLimits.maxrate)
		rate_aim = FlightLimits.maxrate;
	else if (rate_aim < -FlightLimits.maxrate)
		rate_aim = -FlightLimits.maxrate;
	
	/* Rate regulator starts here! */
	rate_error = rate_aim - (fix32)((data->x2 - data->x3)*FP_MUL);
	DataRoll.r_iState = DataRoll.r_iState + fix32Mul(rate_error, DataRoll.r_ki);

	// If integral is to big, limit it
	if (DataRoll.r_iState > DataRoll.r_imax)
		DataRoll.r_iState = DataRoll.r_imax;
	else if (DataRoll.r_iState < -DataRoll.r_imax)
		DataRoll.r_iState = -DataRoll.r_imax;
		
	return DataRoll.r_iState + fix32Mul(rate_error, DataRoll.r_kp);
}
 
fix32 PIDUpdateYaw(float yawrate)
{
	fix32 rate_error, rate_aim, input;
	
	input = GetInputLevel(YAW_CHANNEL);
	rate_aim = fix32Mul(input, FlightLimits.maxyawrate);
	
	/* Rate regulator starts here! */
	rate_error = rate_aim - (fix32)(yawrate*FP_MUL);
	DataYaw.r_iState = DataYaw.r_iState + fix32Mul(rate_error, DataYaw.r_ki);
	
	if (DataYaw.r_iState > DataYaw.r_imax)
		DataYaw.r_iState = DataYaw.r_imax;
	else if (DataYaw.r_iState < -DataYaw.r_imax)
		DataYaw.r_iState = -DataYaw.r_imax;
		
	return DataYaw.r_iState + fix32Mul(rate_error, DataYaw.r_kp);
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
			
			thr_base = fix32Mul(MAX_PWM*FP_MUL, GetInputLevel(THROTTLE_CHANNEL));
			
			for (int i = 0; i < 8; i++)		// Manual fixed point multiplication and FP to INT
			{
				PWM_setOutput((int)((thr_base*Mixer.mix[i][0] + pid_pitch*Mixer.mix[i][1] + \
									 pid_roll*Mixer.mix[i][2] + pid_yaw*Mixer.mix[i][3]) >> 15), i);
			}
		}
		
		else
		{
			for (int i = 0; i < 8; i++)
				PWM_setOutput(MAX_PWM/18, i);
		}
	}
	else
	{
		for (int i = 0; i < 8; i++)
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
