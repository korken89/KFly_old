// KFly ID
#define KFLY_ID			0x4B466C79 // KFly string as hex

// Input channels (DO NOT CHANGE!)
#define THROTTLE_CHANNEL	0
#define PITCH_CHANNEL		3
#define ROLL_CHANNEL		6
#define YAW_CHANNEL			9
#define MODE_CHANNEL		12
#define AUX1_CHANNEL		15
#define AUX2_CHANNEL		18
#define AUX3_CHANNEL		21
#define CHANNELMASK			(0x0F)

// Default values
// Maximum angle of the aircraft in stability mode (in deg)
#define MAX_ANGLE		35.0f
// Maximum angular rate of the aircraft in acrobatic mode (in deg/s)
#define MAX_RATE		180.0f
// Maximum angular rate of the aircraft in Yaw (in deg/s)
#define MAX_YAW_RATE	90.0f

// Update rate for all regulators and filters in Hz
#define UPDATE_RATE 	200.0f

// Accelerometer axis flipping
#define configFLIP_ACC_X	0
#define configFLIP_ACC_Y	0
#define configFLIP_ACC_Z	0

// Gyro axis flipping
#define configFLIP_GYRO_X	0
#define configFLIP_GYRO_Y	1
#define configFLIP_GYRO_Z	1
