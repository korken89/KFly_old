#ifndef PID_H
#define PID_H


/* Standard includes */
#include <stdint.h>
#include <stdio.h>

/* System includes */
#include "LPC17xx.h"
#include "system_LPC17xx.h"
#include "lpc_types.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

/* KFly includes */
#include "configKFly.h"
#include "fmath.h"
#include "eemul.h"

/* Includes */
#include "kalman.h"
#include "input.h"
#include "uart.h"
#include "pwm.h"
#include "sensors.h"
#include "leds.h"

typedef struct {
	fix32 maxangle;
	fix32 maxrate;
	fix32 maxyawrate;
} limits_data;

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
	fix32 mix[8][4];
} mix_data;

void vTaskControlLoop(void *);
void vTaskArmDisarm(void *);
void InitControlLoops(void);
void InitFlightLimits(void);
void InitPID(uint8_t);
void InitMixer(void);
fix32 PIDUpdatePitch(kalman_data *);
fix32 PIDUpdateRoll(kalman_data *);
fix32 PIDUpdateYaw(float);
void UpdOutputs(kalman_data *, kalman_data *, float);
void PIDArm(void);
void PIDDisarm(void);
Bool PIDArmed(void);
void ArmEngines(void);
void DisarmEngines(void);
Bool EnginesArmed(void);

#endif
