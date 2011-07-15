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

/* Application includes */
#include "leds.h"
#include "uart.h"
#include "input.h"
#include "pwm.h"
//#include "gps.h"
#include "sensors.h"
#include "misc.h"
#include "kalman.h"

void prvSetupHardware( void );
void main(void);
void vTask1(void *);
void vTaskControlLoop(void *);
void vConfigureTimerForRunTimeStats(void);

#endif
