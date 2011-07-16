#include "pid.h"

void PIDInit(pid_data *PID)
{
	PID->iState = 0.0f;
    PID->eState = 0.0f;
    
    
    //Fattigmanskalibrering!
    PID->ki = 1.0f;
	PID->kp = 1.0f;
    PID->kd = 1.0f;
    PID->km = 9.0f;		// Motor konstant: km = Kv*[battery voltage]/PWM_MAX
}

float PIDUpdatePitch(pid_data *PID, kalman_data *data)
{	
	float der;
	PID->iState = PID->iState + (MAX_ANGLE*PITCH_CHANNEL - data->x1)*PID->ki;
	der = data->x1 - PID->eState;
	PID->eState = data->x1;
	return (PID->iState - der*PID->kd - data->x1*PID->kp)*PID->km;
}

float PIDUpdateRoll(pid_data *PID, kalman_data *data)
{
	float der;
	PID->iState = PID->iState + (MAX_ANGLE*ROLL_CHANNEL - data->x1)*PID->ki;
	der = data->x1 - PID->eState;
	PID->eState = data->x1;
	return (PID->iState - der*PID->kd - data->x1*PID->kp)*PID->km;
}

float PIDUpdateYaw(pid_data *PID, float yawrate)
{
	return 0.0f;
}
