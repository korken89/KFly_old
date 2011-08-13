#include "pid.h"

volatile Bool bEnginesArmed = FALSE;
volatile Bool bPIDArmed = FALSE;
volatile mix_data mixer;

void InitPID(pid_data *PID, uint8_t channel)
{
	PID->a_iState = 0;
	PID->r_iState = 0;
    
    //Fattigmanskalibrering!
	if ((channel == ROLL_CHANNEL) || (channel == PITCH_CHANNEL))
    {
		PID->r_kp = (fix32)(2.0f*FP_MUL);
		PID->r_ki = (fix32)(2.0f*FP_MUL);
		
		PID->a_kp = (fix32)(0.6f*FP_MUL);
		PID->a_ki = (fix32)(0.0f*FP_MUL);
	}
	
	if (channel == YAW_CHANNEL)
    {
		PID->r_kp = (fix32)(2.0f*FP_MUL);
		PID->r_ki = (fix32)(0.0f*FP_MUL);
		
		PID->a_kp = (fix32)(0.0f*FP_MUL);
		PID->a_ki = (fix32)(0.0f*FP_MUL);
	}	
}

fix32 PIDUpdateChannel(pid_data *PID, kalman_data *data, uint8_t channel)
{	
	static int i = 0;
	fix32 angle_error, rate_error, rate_aim, input;
	
	input = GetInputLevel(channel);
		
	/* Angle regulator starts here! */
	angle_error = fix32Mul((fix32)(MAX_ANGLE*FP_MUL), input) - (fix32)(data->x1*FP_MUL);
	PID->a_iState += fix32Mul(angle_error, PID->a_ki);
	
	if (PID->a_iState > PID->a_imax)
		PID->a_iState = PID->a_imax;
	else if (PID->a_iState < -PID->a_imax)
		PID->a_iState = -PID->a_imax;
	
	rate_aim = PID->a_iState + fix32Mul(angle_error, PID->a_kp);
	
	if (rate_aim > (fix32)(MAX_RATE*FP_MUL))
		rate_aim = (fix32)(MAX_RATE*FP_MUL);
	else if (rate_aim < -(fix32)(MAX_RATE*FP_MUL))
		rate_aim = -(fix32)(MAX_RATE*FP_MUL);
	
	/* Rate regulator starts here! */
	rate_error = rate_aim - (fix32)((data->x2 - data->x3)*FP_MUL);
	PID->r_iState = PID->r_iState + fix32Mul(rate_error, PID->r_ki);
	
	if (PID->r_iState > PID->r_imax)
		PID->r_iState = PID->r_imax;
	else if (PID->r_iState < -PID->r_imax)
		PID->r_iState = -PID->r_imax;
		
	return PID->r_iState + fix32Mul(rate_error, PID->r_kp);
}
 
fix32 PIDUpdateYaw(pid_data *PID, float yawrate)
{
	fix32 rate_error, rate_aim, input;
	
	input = GetInputLevel(YAW_CHANNEL);
	rate_aim = fix32Mul(input, (fix32)(MAX_YAW_RATE*FP_MUL));
	
	/* Rate regulator starts here! */
	rate_error = rate_aim - (fix32)(yawrate*FP_MUL);
	PID->r_iState = PID->r_iState + fix32Mul(rate_error, PID->r_ki);
	
	if (PID->r_iState > PID->r_imax)
		PID->r_iState = PID->r_imax;
	else if (PID->r_iState < -PID->r_imax)
		PID->r_iState = -PID->r_imax;
		
	return PID->r_iState + fix32Mul(rate_error, PID->r_kp);
}

void UpdOutputs(kalman_data *data_xz, kalman_data *data_yz, float gyro_tmp, \
				pid_data *data_pitch, pid_data *data_roll, pid_data *data_yaw)
{
	fix32 pid_pitch, pid_roll, pid_yaw, thr_base;
	
	if (EnginesArmed() == TRUE)
	{
		if (PIDArmed() == TRUE)
		{
			pid_pitch = PIDUpdateChannel(data_pitch, data_xz, PITCH_CHANNEL);
			pid_roll = PIDUpdateChannel(data_roll, data_yz, ROLL_CHANNEL);
			pid_yaw = PIDUpdateYaw(data_yaw, gyro_tmp);
			
			thr_base = fix32Mul(MAX_PWM, GetInputLevel(THROTTLE_CHANNEL));
			
			for (int i = 0; i < 8; i++)		// Manual fixed point multiplication
			{
				PWM_setOutput((int)(thr_base*mixer.mix[i][0] + pid_pitch*mixer.mix[i][1] + \
								pid_roll*mixer.mix[i][2] + pid_yaw*mixer.mix[i][3])/(256*128), i);
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
