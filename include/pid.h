#ifndef PID_H
#define PID_H

#include "configKFly.h"
#include "kalman.h"
#include "input.h"
#include "lpc_types.h"

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
void PIDArm(void);
void PIDDisarm(void);
Bool PIDArmed(void);
void ArmEngines(void);
void DisarmEngines(void);
Bool EnginesArmed(void);

#endif
