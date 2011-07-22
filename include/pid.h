#ifndef PID_H
#define PID_H

#include "configKFly.h"
#include "kalman.h"
#include "input.h"
#include "lpc_types.h"
#include "uart.h"
#include "fmath.h"


typedef struct {
	float a_iState;
	float r_iState;
	
    float a_ki;
	float a_kp;

	float r_ki;
	float r_kp;
} pid_data;

void InitPID(pid_data *);
float PIDUpdateChannel(pid_data *, kalman_data *, uint8_t);
float PIDUpdateYaw(pid_data *, float);
void PIDArm(void);
void PIDDisarm(void);
Bool PIDArmed(void);
void ArmEngines(void);
void DisarmEngines(void);
Bool EnginesArmed(void);

#endif
