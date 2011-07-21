// Input channels
#define THROTTLE_CHANNEL	1
#define PITCH_CHANNEL		2
#define ROLL_CHANNEL		3
#define YAW_CHANNEL			0

// Maximum angle of the aircraft in stability mode (in deg)
#define MAX_ANGLE		30.0f	// 30 deg max

// Maximum angular rate of the aircraft in acrobatic mode (in deg/s)
#define MAX_RATE		180.0f	// 180 deg/s max

// Update rate for all regulators and filters in Hz
#define UPDATE_RATE 	100.0f

// Accelerometer and Gyro axis flipping
#define configFLIP_ACC_X	0
#define configFLIP_ACC_Y	0
#define configFLIP_ACC_Z	0

#define configFLIP_GYRO_X	0
#define configFLIP_GYRO_Y	1
#define configFLIP_GYRO_Z	0
