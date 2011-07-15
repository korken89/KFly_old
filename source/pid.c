#include "pid.h"

void PIDInit(pid_data *PID)
{
	PID->iState = 0.0f;
    PID->eState = 0.0f;
    
    
    //Fattigmanskalibrering!
    PID->ki = 1.0f;
	PID->kp = 1.0f;
    PID->kd = 1.0f;
    PID->km = 1.0f;
}

void PIDUpdatePitch(pid_data *PID, kalman_data *data)
{	
	float error2, der;
	PID->iState = PID->iState + (MAX_ANGLE*PITCH_CHANNEL - data->x1)*PID->ki;
	der = data->x1 - PID->eState;
	PID->eState = data->x1;
	error2 = (PID->iState - der*PID->kd - data->x1*kp)*km;
}

void PIDUpdateRoll(pid_data *PID, kalman_data *data)
{
	
}

void PIDUpdateYaw(pid_data *PID, float yawrate)
{
	
}
