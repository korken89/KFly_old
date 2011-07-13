#ifndef PID_H
#define PID_H

struct _pid_data {
    float iState;		// Integrator state
    float eState;		// Error state
    float ki;			// Integral gain ki = K/Ti*h
    float kp;			// Proportional gain kp = K*Td/h
    float kd;			// Derivative gain
    float ke;			// Motor gain
};
typedef struct _pid_data pid_data;

void PID_Init(pid_data *data);
void PID_Update(pid_data *data);

#endif
