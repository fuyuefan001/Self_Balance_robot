/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


//我最新发的

#include "miniproject.h"

void nano_wait(int t) {
    asm("       mov r0,%0\n"
        "repeat:\n"
        "       sub r0,#83\n"
        "       bgt repeat\n"
        : : "r"(t) : "r0", "cc");
}

int16_t acc1[3]={0};
int16_t gyr1[3]={0};
int16_t dmp[3]={0};
float power;//-100 to 100
float p,i,d;
int16_t acczero[3]={0};
int16_t gyrzero[3]={0};
float ANG=0;
float GYO=0;
float angf;
float gyrf;
const float fRad2Deg = 57.295779513f; //»¡¶È»»Ëã½Ç¶È³ËµÄÏµÊý
const float dt = 0.01; //Ê±¼äÖÜÆÚ
//float angle[3] = {0,0,0};
float R = 0.98;
int speed;
float angle_last;
char buf1[32]={0};
int aaa=0;
void TIM2_IRQHandler(void){
	p=50;
	d=0;
	TIM_ClearFlag(TIM1,TIM_FLAG_Update);
	angle_last=ANG;
	GetAccGyro();
	gyr1[0]+=240;//零点矫正
	GYO=gyr1[0]*0.061035/4;//单位换算 量程-2000~+2000deg/s， -32768~+32768（int16_t）
    float temp=sqrt(acc1[1]*acc1[1]+acc1[2]*acc1[2]);//重力加速度
    ANG = R*(angle_last+GYO*dt) + (1-R)*fRad2Deg*atan(acc1[0]/temp);//角度融合
    //kalman_filter(fRad2Deg*atan(acc1[0]/temp),GYO,&angf,&gyrf);
    //atan 比 asin 运算速度快
	power=p*(ANG-8)+d*(GYO);//pd调控输出率
	//power=p*(ANG-6)+d*(GYO);
	duty_cyc1(powerscale((int)power));//更新pwm
//	aaa++;
//	if(aaa%50==0){
//    	sprintf(buf1,"ACC: [%5f %5f %5f] m/s",acc1[0]/835.92,acc1[1]/835.92,acc1[2]/835.92);
//    	println(buf1);
//    	sprintf(buf1,"GYO: [%5f %5f %5f] deg/s",gyr1[0]*1.0,gyr1[1]*1.0,gyr1[2]*1.0);
//    	println(buf1);
//    	sprintf(buf1,"ANG: [%5f %5f %5f] deg",angle[0],angle[1],angle[2]);
//    	println(buf1)	```;
//    	sprintf(buf1,"DMP: [%5f %5f %5f] ",dmp[0],dmp[1],dmp[2]);
//    	println(buf1);
    	sprintf(buf1,"%5f %5f",ANG,GYO);
    	println(buf1);
//       	sprintf(buf1,"power: %.2f",power);
//        println(buf1);
//	}
}

int main(void)
{
	tim1_init();
	duty_cyc1(4500);
	init_usart2();
	sendmsg("Finish init usart2",18);
	I2C_config();
	sendmsg("Finish init i2c",15);
	MPU6050_Init();
	sendmsg("Finish init gyroscope",21);
	nano_wait(1000000000);
	tim2_init();
    //while(1){}
    return 0;
}
