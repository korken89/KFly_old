#include "pid.h"

volatile Bool bEnginesArmed = FALSE;
volatile Bool bPIDArmed = FALSE;

void InitPID(pid_data *PID)
{
	PID->a_iState = 0.0f;
	PID->r_iState = 0.0f;
    
    //Fattigmanskalibrering!
    PID->a_ki = 0.0f;
	PID->a_kp = 20.0f;
	
    PID->r_ki = 0.001f;
	PID->r_kp = 0.25f;
}

float PIDUpdateChannel(pid_data *PID, kalman_data *data, uint8_t channel)
{	
	static int i = 0;
	float angle_error, rate_error, rate_i, rate_aim;
	
	/* Angle regulator starts here! */
	angle_error = MAX_ANGLE*GetInputLevel(channel) - data->x1;
	PID->a_iState = PID->a_iState + angle_error*PID->a_ki;
	
	if (PID->a_iState > PID_IMAX)
		PID->a_iState = PID_IMAX;
	else if (PID->a_iState < PID_IMIN)
		PID->a_iState = PID_IMIN;
	
	rate_aim = PID->a_iState + angle_error;
	
	if (i > 18)
	{	
		ftoa(MAX_ANGLE*GetInputLevel(channel));
		UART0_SendChar('\t');
		ftoa(data->x1);
		UART0_SendChar('\n');
		i = 0;
	}
	else
		i++;
	
	
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
	return 0.0f;
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
