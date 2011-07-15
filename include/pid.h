#ifndef PID_H
#define PID_H

#include "kalman.h"
#include "input.h"

// Input channels
#define THROTTLE_CHANNEL	GetInputLevel(2)
#define PITCH_CHANNEL		GetInputLevel(3)
#define ROLL_CHANNEL		GetInputLevel(4)
#define YAW_CHANNEL			GetInputLevel(1)


// Maximum angle of the aircraft in stability mode
#define MAX_ANGLE		30.0f

// Safeguards so the regulator won't overflow
#define PID_IMAX		100000.0f
#define PID_IMIN		-100000.0f

struct _pid_data {
    float iState;		// Integrator state
    float eState;		// Error state
    float ki;			// Integral gain ki = K/Ti*h
    float kp;			// Proportional gain kp = K*Td/h
    float kd;			// Derivative gain
    float km;			// Motor gain
};
typedef struct _pid_data pid_data;

void PIDInit(pid_data *);
void PIDUpdatePitch(pid_data *, kalman_data *);
void PIDUpdateRoll(pid_data *, kalman_data *);
void PIDUpdateYaw(pid_data *, float);

#endif
