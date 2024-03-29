/**
 * Original by Linus Helgesson (LHelge at www.elektronikforumet.com/forum)
 * Edited & optimized by Emil Fresk (Korken at www.elektronikforumet.com/forum)
 **/

#include "kalman.h"

void InitKalman(kalman_data *data)
{
	data->x1 = 0.0f;
	data->x2 = 0.0f;
	data->x3 = 0.0f;
	data->p11 = 1000.0f;
	data->p12 = 0.0f;
	data->p13 = 0.0f;
	data->p21 = 0.0f;
	data->p22 = 1000.0f;
	data->p23 = 0.0f;
	data->p31 = 0.0f;
	data->p32 = 0.0f;
	data->p33 = 1000.0f;
}

void UpdKalman(kalman_data *data, float z1, float z2)
{
	float y1, y2;
	float a, b, c;
	float sDet;
	float s11, s12, s21, s22;
	float k11, k12, k21, k22, k31, k32;

	// Step 1
	// x(k) = Fx(k-1) + Bu + w:
	data->x1 = data->x1 + (data->x2 - data->x3)*DT;
	//x2 = x2;
	//x3 = x3;

	// Step 2
	// P = FPF'+Q
	a = data->p11 + (data->p21 - data->p31)*DT;
	b = data->p12 + (data->p22 - data->p32)*DT;
	c = data->p13 + (data->p23 - data->p33)*DT;
	data->p11 = a + (b - c)*DT + Q1;
	data->p12 = b;
	data->p13 = c;
	data->p21 = data->p21 + (data->p22 - data->p23)*DT;
	data->p22 = data->p22 + Q2;
	//p23 = p23;
	data->p31 = data->p31 + (data->p32 - data->p33)*DT;
	//p32 = p32;
	data->p33 = data->p33 + Q3;


	// Step 3
	// y = z(k) - Hx(k)
	y1 = z1-data->x1;
	y2 = z2-data->x2;

	// Step 4
	// S = HPT' + R
	s11 = data->p11 + R1;
	s12 = data->p12;
	s21 = data->p21;
	s22 = data->p22 + R2;

	// Step 5
	// K = PH*inv(S)
	sDet = 1.0f/(s11*s22 - s12*s21);
	k11 = (data->p11*s22 - data->p12*s21)*sDet;
	k12 = (data->p12*s11 - data->p11*s12)*sDet;
	k21 = (data->p21*s22 - data->p22*s21)*sDet;
	k22 = (data->p22*s11 - data->p21*s12)*sDet;
	k31 = (data->p31*s22 - data->p32*s21)*sDet;
	k32 = (data->p32*s11 - data->p31*s12)*sDet;

	// Step 6
	// x = x + Ky
	data->x1 = data->x1 + k11*y1 + k12*y2;
	data->x2 = data->x2 + k21*y1 + k22*y2;
	data->x3 = data->x3 + k31*y1 + k32*y2;

	// Step 7
	// P = (I-KH)P
	data->p11 = data->p11*(1.0f - k11) - data->p21*k12;
	data->p12 = data->p12*(1.0f - k11) - data->p22*k12;
	data->p13 = data->p13*(1.0f - k11) - data->p23*k12;
	data->p21 = data->p21*(1.0f - k22) - data->p11*k21;
	data->p22 = data->p22*(1.0f - k22) - data->p12*k21;
	data->p23 = data->p23*(1.0f - k22) - data->p13*k21;
	data->p31 = data->p31 - data->p21*k32 - data->p11*k31;
	data->p32 = data->p32 - data->p22*k32 - data->p12*k31;
	data->p33 = data->p33 - data->p22*k32 - data->p13*k31;
}
