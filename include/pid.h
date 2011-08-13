#ifndef PID_H
#define PID_H

#include "configKFly.h"
#include "kalman.h"
#include "input.h"
#include "lpc_types.h"
#include "uart.h"
#include "pwm.h"
#include "fmath.h"
#include "eemul.h"


typedef struct {
	fix32 a_iState;
	fix32 r_iState;
    fix32 a_ki;
	fix32 a_kp;
	fix32 r_ki;
	fix32 r_kp;
	fix32 r_imax;
	fix32 a_imax;
} pid_data;

typedef struct {
	fix8 mix[8][4];
} mix_data;

void InitPID(pid_data *, uint8_t);
void InitMixer(void);
fix32 PIDUpdateChannel(pid_data *, kalman_data *, uint8_t);
fix32 PIDUpdateYaw(pid_data *, float);
void UpdOutputs(kalman_data *, kalman_data *, float, pid_data *, pid_data *, pid_data *);
void PIDArm(void);
void PIDDisarm(void);
Bool PIDArmed(void);
void ArmEngines(void);
void DisarmEngines(void);
Bool EnginesArmed(void);

#endif
