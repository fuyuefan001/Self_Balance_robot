#include "miniproject.h"
extern float fRad2Deg ; //弧度换算角度乘的系数
extern float dt; //时间周期
extern float angle[] ;
extern float R;
extern int16_t acc1[];
extern int16_t gyr1[];
void ImuCalculate_Complementary(void)//计算角度
{
    uint8_t i;
    static float angle_last[3]={0};
    float temp[3] = {0};
    temp[0] = sqrt(acc1[1]*acc1[1]+acc1[2]*acc1[2]);
    temp[1] = sqrt(acc1[0]*acc1[0]+acc1[2]*acc1[2]);

    for(i = 0; i < 2; i++)//pitch and roll
    {
        angle[i] = R*(angle_last[i]+gyr1[i]*dt)
                    + (1-R)*fRad2Deg*atan(acc1[i]/temp[i]);
        angle_last[i] = angle[i];
    }

    angle[2] = angle_last[2]+gyr1[2]*dt;//yaw
    angle_last[2] = angle[2];
}
/**
 * float angle_m 加速度计计算的角度，
 * float gyro_m陀螺仪角速度，
 * float *angle_f融合后的角度，
 * float *angle_dot_f融合后的角速度
 */

void kalman_filter(float angle_m, float gyro_m, float *angle_f, float *angle_dot_f)
{
    //------------------------------
    static float angle, angle_dot;
    const float Q_angle = 0.01, Q_gyro = 0.05, R_angle = 0.5, dt = 0.01;
    static float P[2][2]={
                       { 1, 0 },
                       { 0, 1 }
                     };
    static float Pdot[4] = {0, 0, 0, 0};
    const uint8_t C_0 = 1;
    static float q_bias, angle_err, PCt_0, PCt_1, E, K_0, K_1, t_0, t_1;
    //------------------------------
    angle += (gyro_m - q_bias) * dt;

    Pdot[0]  =Q_angle - P[0][1] - P[1][0];
    Pdot[1] = -P[1][1];
    Pdot[2] = -P[1][1];
    Pdot[3] = Q_gyro;

    P[0][0] += Pdot[0] * dt;
    P[0][1] += Pdot[1] * dt;
    P[1][0] += Pdot[2] * dt;
    P[1][1] += Pdot[3] * dt;

    angle_err = angle_m - angle;

    PCt_0=C_0 * P[0][0];
    PCt_1=C_0 * P[1][0];

    E = R_angle + C_0 * PCt_0;

    K_0 = PCt_0 / E;
    K_1 = PCt_1 / E;

    t_0 = PCt_0;
    t_1 = C_0 * P[0][1];

    P[0][0] -= K_0 * t_0;
    P[0][1] -= K_0 * t_1;
    P[1][0] -= K_1 * t_0;
    P[1][1] -= K_1 * t_1;

    angle	+= K_0 * angle_err;
    q_bias += K_1 * angle_err;
    angle_dot = gyro_m - q_bias;

    *angle_f = angle;
    *angle_dot_f = angle_dot;
}

int powerscale(int power){
	int ccr=4500;
	if(power>0){
		ccr=4500+100+power;
	}else if(power<0){
		ccr=4500-100+power;
	}
	if(ccr>5900){ccr=5900;}
	if(ccr<3100){ccr=3100;}
	return ccr;
}

