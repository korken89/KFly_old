/**
 *		Hardware Abstraction Layer for sensors. 
 **/

#include "sensors.h"

void InitSensors(void)
{
	BMA180_Init(BW_300HZ, RANGE_2G);
	ITG3200_Init();
}

void ReadAcc(float *data)
{
	uint8_t temp[6];
	BMA180_BurstRead(temp, NULL);
	
	int32_t rate[3];

	// Cast to int16_t to get correct types
	rate[0] = (int16_t)((temp[1]<<8)|temp[0]);
	rate[1] = (int16_t)((temp[3]<<8)|temp[2]);
	rate[2] = (int16_t)((temp[5]<<8)|temp[4]);

	// Shift away the new_data bit and 0-bit (Page 22 of the BMA180 data sheet)
	rate[0] >>= 2;
	rate[1] >>= 2;
	rate[2] >>= 2;

	// X-axis
	#if configFLIP_ACC_X == 0
		*data = (float)rate[0];
	#else
		*data = -(float)rate[0];
	#endif
	
	// Y-axis
	#if configFLIP_ACC_Y == 0
		*(data+1) = (float)rate[1];
	#else
		*(data+1) = -(float)rate[1];
	#endif
	
	// Z-axis
	#if configFLIP_ACC_Z == 0
		*(data+2) = (float)rate[2];
	#else
		*(data+2) = -(float)rate[2];
	#endif
}


/**
 * Returns an arrays with X, Y and Z in deg/s.
 **/
void ReadGyroRate(float *data)
{
	uint8_t temp[6];
	ITG3200_BurstRead(temp, NULL);
	
	int32_t rate[3];

	// Cast to int16_t to get correct types
	rate[0] = (int16_t)((temp[0]<<8)|temp[1]);
	rate[1] = (int16_t)((temp[2]<<8)|temp[3]);
	rate[2] = (int16_t)((temp[4]<<8)|temp[5]);
	
	// X-axis
	#if configFLIP_GYRO_X == 0
		*data = (float)(rate[1])*RATE2DEGPS;
	#else
		*data = -(float)(rate[1])*RATE2DEGPS;
	#endif
	
	// Y-axis
	#if configFLIP_GYRO_Y == 0
		*(data+1) = (float)(rate[0])*RATE2DEGPS;
	#else
		*(data+1) = -(float)(rate[0])*RATE2DEGPS;
	#endif
	
	// Z-axis
	#if configFLIP_GYRO_Z == 0
		*(data+2) = (float)(rate[2])*RATE2DEGPS;
	#else
		*(data+2) = -(float)(rate[2])*RATE2DEGPS;
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
