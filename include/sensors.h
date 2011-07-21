#ifndef SENSORS_H_
#define SENSORS_H_

#include "configKFly.h"
#include "itg3200.h"
#include "bma180.h"
#include "fmath.h"
#include "misc.h"

void ReadAcc(float *);
void ReadGyroRate(float *);
void ReadAccAngle(float *);

#endif

