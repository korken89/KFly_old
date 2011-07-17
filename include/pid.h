#ifndef PID_H
#define PID_H

#include "kalman.h"
#include "input.h"

// Input channels
#define THROTTLE_CHANNEL	GetInputLevel(2)
#define PITCH_CHANNEL		GetInputLevel(3)
#define ROLL_CHANNEL		GetInputLevel(4)
#define YAW_CHANNEL			GetInputLevel(1)

// Maximum angle of the aircraft in stability mode (in deg)
#define MAX_ANGLE		30.0f	// 30 deg max

// Maximum angular rate of the aircraft in acrobatic mode (in deg/s)
#define MAX_RATE		180.0f	// 180 deg/s max

// Safeguards so the regulator won't overflow
#define PID_IMAX		1000000.0f
#define PID_IMIN		-1000000.0f

struct _pid_data {
    float iState;		// Integrator state
    float eState;		// Error state
    float ki;			// Integral gain ki = Ki*h
    float kp;			// Proportional gain 
    float kd;			// Derivative gain kp = Kp/h
    float km;			// Motor gain: km = Kv*[battery voltage]/PWM_MAX
};
typedef struct _pid_data pid_data;

void InitPID(pid_data *);
float PIDUpdatePitch(pid_data *, kalman_data *);
float PIDUpdateRoll(pid_data *, kalman_data *);
float PIDUpdateYaw(pid_data *, float);

#endif
