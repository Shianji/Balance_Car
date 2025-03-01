#ifndef __COM_PID_H_
#define __COM_PID_H_


int Com_PID_Balance(float kp, float kd, float kalman_angle, float balance_angle, short gy);
int Com_PID_Velocity(float kp, float ki, int encoder_a, int encoder_b, int remove_move);
int Com_PID_Trun(float kp, short gz);

#endif


