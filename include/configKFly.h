// Input channels
#define THROTTLE_CHANNEL	1
#define PITCH_CHANNEL		2
#define ROLL_CHANNEL		3
#define YAW_CHANNEL			0
#define CHANNELMASK			((1<<THROTTLE_CHANNEL)|(1<<PITCH_CHANNEL)|(1<<ROLL_CHANNEL)|(1<<YAW_CHANNEL))

// Safeguards so the regulator won't overflow
#define PID_IMAX		20.0f
#define PID_IMIN		-PID_IMAX

// Maximum angle of the aircraft in stability mode (in deg)
#define MAX_ANGLE		35.0f	// 35 deg max

// Maximum angular rate of the aircraft in acrobatic mode (in deg/s)
#define MAX_RATE		180.0f	// 180 deg/s max
#define MIN_RATE		-MAX_RATE

// Maximum angular rate of the aircraft in Yaw
#define MAX_YAW_RATE	90.0f

// Update rate for all regulators and filters in Hz
#define UPDATE_RATE 	200.0f

// Accelerometer and Gyro axis flipping
#define configFLIP_ACC_X	0
#define configFLIP_ACC_Y	0
#define configFLIP_ACC_Z	0

#define configFLIP_GYRO_X	0
#define configFLIP_GYRO_Y	1
#define configFLIP_GYRO_Z	1
