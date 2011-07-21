/**
 *		Hardware Abstraction Layer for sensors. 
 **/

#include "sensors.h"

void ReadAcc(float *data)
{
	int16_t temp[3];
	//BMA180_BurstRead(temp);
	
	// X-axis
	#if configFLIP_ACC_X == 0
		*data = (float)*temp;
	#else
		*data = -(float)*temp;
	#endif
	
	// Y-axis
	#if configFLIP_ACC_Y == 0
		*(data+1) = (float)*(temp+1);
	#else
		*(data+1) = -(float)*(temp+1);
	#endif
	
	// Z-axis
	#if configFLIP_ACC_Z == 0
		*(data+2) = (float)*(temp+2);
	#else
		*(data+2) = -(float)*(temp+2);
	#endif
}


/**
 * Returns an arrays with X, Y and Z in deg/s.
 **/
void ReadGyroRate(float *data)
{
	int16_t temp[3];
	
	// X-axis
	#if configFLIP_GYRO_X == 0
		*data = (float)*(temp+1)*RATE2DEGPS;
	#else
		*data = -(float)*(temp+1)*RATE2DEGPS;
	#endif
	
	// Y-axis
	#if configFLIP_GYRO_Y == 0
		*(data+1) = (float)(*temp)*RATE2DEGPS;
	#else
		*(data+1) = -(float)(*temp)*RATE2DEGPS;
	#endif
	
	// Z-axis
	#if configFLIP_GYRO_Z == 0
		*(data+2) = (float)*(temp+2)*RATE2DEGPS;
	#else
		*(data+2) = -(float)*(temp+2)*RATE2DEGPS;
	#endif
}

/**
 * data[0] = xz angle
 * data[1] = yz angle
 * 
 * Returns angle in degrees NOT radians.
 * 
 * Works for -90 to 90 degrees.
 **/
void ReadAccAngle(float *data)
{
	float temp[3];
	ReadAcc(temp);
		
	float zsq = temp[2]*temp[2];
	float xz = fInvSqrt(temp[0]*temp[0]+zsq);
	float yz = fInvSqrt(temp[1]*temp[1]+zsq);
		
	*data = fArctan2(temp[0]*xz, temp[2]*xz)*RAD2DEG;
	*(data+1) = fArctan2(temp[1]*yz, temp[2]*yz)*RAD2DEG;
}
