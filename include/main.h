#ifndef __MAIN_H__
#define __MAIN_H__

/* Standard includes */
#include <stdint.h>
#include <stdio.h>

/* System includes */
#include "LPC17xx.h"
#include "system_LPC17xx.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

/* KFly includes */
#include "configKFly.h"

/* Init includes */
#include "telemetry.h"
#include "input.h"
#include "misc.h"
#include "sensors.h"

void prvSetupHardware(void);
void main(void);
void vTask1(void *);
void vConfigureTimerForRunTimeStats(void);

#endif
