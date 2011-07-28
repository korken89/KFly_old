#include "pid.h"

volatile Bool bEnginesArmed = FALSE;
volatile Bool bPIDArmed = FALSE;

void InitPID(pid_data *PID, uint8_t channel)
{
	PID->a_iState = 0.0f;
	PID->r_iState = 0.0f;
    
    //Fattigmanskalibrering!
    if (channel == PITCH_CHANNEL)
    {
		PID->r_kp = 1.8f;
		PID->r_ki = 0.001f;
		
		PID->a_kp = 0.5f;
		PID->a_ki = 0.0f;
	}
	
	if (channel == ROLL_CHANNEL)
    {
		PID->r_kp = 1.8f;
		PID->r_ki = 0.001f;
		
		PID->a_kp = 0.5f;
		PID->a_ki = 0.0f;
	}
	
	if (channel == YAW_CHANNEL)
    {
		PID->r_kp = 2.0f;
		PID->r_ki = 0.0f;
		
		PID->a_kp = 0.0f;
		PID->a_ki = 0.0f;
	}	
}

float PIDUpdateChannel(pid_data *PID, kalman_data *data, uint8_t channel)
{	
	static int i = 0;
	float angle_error, rate_error, rate_i, rate_aim, input;
	
	input = GetInputLevel(channel);
		
	/* Angle regulator starts here! */
	angle_error = MAX_ANGLE*input - data->x1;
	PID->a_iState = PID->a_iState + angle_error*PID->a_ki;
	
	if (PID->a_iState > PID_IMAX)
		PID->a_iState = PID_IMAX;
	else if (PID->a_iState < PID_IMIN)
		PID->a_iState = PID_IMIN;
	
	rate_aim = PID->a_iState + angle_error*PID->a_kp;
	
	if (rate_aim > MAX_RATE)
		rate_aim = MAX_RATE;
	else if (rate_aim < MIN_RATE)
		rate_aim = MIN_RATE;
	
	/* Rate regulator starts here! */
	rate_error = rate_aim - (data->x2 - data->x3);
	PID->r_iState = PID->r_iState + rate_error*PID->r_ki;
	
	if (PID->r_iState > PID_IMAX)
		PID->r_iState = PID_IMAX;
	else if (PID->r_iState < PID_IMIN)
		PID->r_iState = PID_IMIN;
		
	return PID->r_iState + rate_error*PID->r_kp;
}
 
float PIDUpdateYaw(pid_data *PID, float yawrate)
{
	return (MAX_YAW_RATE*GetInputLevel(YAW_CHANNEL) - yawrate)*PID->r_kp;
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
