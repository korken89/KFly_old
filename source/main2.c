#include "../include/fmath.h"
#include "../include/kalman.h"

// Structs for containing filter data
kalman_data pitch_data;
kalman_data roll_data;

// Main function
int main()
{
	fixed acc_x, acc_y, acc_z, acc_z2;
	fixed gyr_x, gyr_y, gyr_z;
	fixed acc_pitch, acc_roll;
	fixed pitch, roll;
	fixed pitch_norm, roll_norm;
	Init_Kalman(&pitch_data);
	Init_Kalman(&roll_data);
	
	while(1)
	{
		// Read sensor
		read_accelerometer(&acc_x, &acc_y, &acc_z); // Måste göras om till fixed point innan de används bara.
		read_gyro(&gyr_x, &gyr_y, &gyr_z);
		
		// Normalize acceleration vector and calculate pitch and roll
		// based only on acceleration.
		pitch_norm = fNorm(acc_x, acc_z);
		roll_norm = fNorm(acc_y, acc_z);
		acc_pitch = atan2(fDiv(acc_x, pitch_norm), fDiv(-acc_z, pitch_norm));
		acc_roll = -atan2(fDiv(acc_y, roll_norm), fDiv(-acc_z, roll_norm));
		
		// Perform filtering
		Upd_Kalman(&pitch_data, acc_pitch, gyr_x);
		Upd_Kalman(&roll_data, acc_roll, gyr_y);
		
		// The angle is stored in state 1
		pitch = pitch_data.x1;
		roll = roll_data.x1;
		delay(/* some time */);
	}
	
	return 0;
}
