#include "../include/pid.h"

void PID_Init(pid_data *data)
{
	data->iState = 0f;
    data->eState = 0f;
    data->ki = 1f;
	data->kp = 1f;
    data->kd = 1f;
    data->ke = 1f;
}

void PID_Update(pid_data *data)
{
	float up, ud, ui, error, cur;
	cur = GetCurrent();
	
	error = GetReferance() - cur;				// Calcualte error term: referance angle - actual angle
	
	if (data->iState > PID_IMAX)
		data->iState = PID_IMAX;				// Calculate the integral state with appropriate limiting
	else if (PID->iState_x < PID_IMIN)
		data->iState = PID_IMIN;
	
	ui = data->iState_x + data->ki*error;		// Calculate the integral term
	up = data->kp*cur;							// Calculate the proportional term
	ud = data->kd*(cur - data->eState);			// Calculate the differential term
	
	data->eState = cur;							// Update old state for error
	data->iState = ui;							// Update old state for ui

	float out = (ui - up - ud)*data->ke;		// Output
}
