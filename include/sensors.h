#ifndef SENSORS_H_
#define SENSORS_H_

#include "itg3200.h"
#include "bma180.h"
#include "fmath.h"

#define configFLIP_ACC_X	0
#define configFLIP_ACC_Y	0
#define configFLIP_ACC_Z	0

#define configFLIP_GYRO_X	0
#define configFLIP_GYRO_Y	1
#define configFLIP_GYRO_Z	0

void vReadAcc(float *);
void vReadGyroRate(float *);
void vReadAccAngle(float *);

#endif

