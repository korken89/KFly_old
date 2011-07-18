#include "pid.h"

void InitPID(pid_data *PID)
{
	PID->iState = 0.0f;
    PID->eState = 0.0f;
    
    
    //Fattigmanskalibrering!
    PID->ki = 1.0f;
	PID->kp = 1.0f;
    PID->kd = 1.0f;
    PID->km = 9.0f;		// Motor konstant: km = Kv*[battery voltage]/PWM_MAX
}

/**
 * These regulators use a special type of design, not a "normal" PID.
 * The P and D comes from the process not the error and the error is only integrated.

 */
 
float PIDUpdatePitch(pid_data *PID, kalman_data *data)
{	
	float der;
	PID->iState = PID->iState + (MAX_ANGLE*GetInputLevel(PITCH_CHANNEL) - data->x1)*PID->ki;
	
	if (PID->iState > PID_IMAX)
		PID->iState = PID_IMAX;
	else if (PID->iState < PID_IMIN)
		PID->iState = PID_IMIN;
		
	der = (data->x1 - PID->eState)*PID->kd;
	PID->eState = data->x1;
	return (PID->iState - der - data->x1*PID->kp)*PID->km;
}

float PIDUpdateRoll(pid_data *PID, kalman_data *data)
{
	float der;
	PID->iState = PID->iState + (MAX_ANGLE*GetInputLevel(ROLL_CHANNEL) - data->x1)*PID->ki;
	
	if (PID->iState > PID_IMAX)
		PID->iState = PID_IMAX;
	else if (PID->iState < PID_IMIN)
		PID->iState = PID_IMIN;
		
	der = (data->x1 - PID->eState)*PID->kd;
	PID->eState = data->x1;
	return (PID->iState - der - data->x1*PID->kp)*PID->km;
}

float PIDUpdateYaw(pid_data *PID, float yawrate)
{
	return 0.0f;
}
