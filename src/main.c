/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/




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
int power1,power2;//-100 to 100
int16_t acczero[3]={0};
int16_t gyrzero[3]={-264,-263,3};

const float fRad2Deg = 57.295779513f; //弧度换算角度乘的系数
const float dt = 0.05; //时间周期
float angle[3] = {0,0,0};
float R = 0.98f;
int main(void)
{
//	init_pwm();
	tim1_init();
	duty_cyc1(100);
	duty_cyc2(100);
//	while(1) asm("wfi");
	char buf1[32]={0};
	char buf2[20]={0};
	char buf3[20]={0};
	init_usart2();
	sendmsg("Finish init usart2",18);
	I2C_config();
	sendmsg("Finish init i2c",15);
	MPU6050_Init();
	sendmsg("Finish init gyroscope",21);

    while(1){
    	GetAccGyro();
    	int i;
    	for(i=0;i<3;i++){
    		gyr1[i]-=gyrzero[i];
    	}
    	//ImuCalculate_Complementary();
    	//TODO:pid control
    	//calculate power for two motor
    	power1=50;
    	power2=-50;
    	//scale to 50 ~100
    	duty_cyc1(power1/4+75);
    	duty_cyc2(power2/4+75);
    	duty_cyc3(999);
    	sprintf(buf1,"ACC: [%5f %5f %5f] m/s",acc1[0]/835.92,acc1[1]/835.92,acc1[2]/835.92);

    	println(buf1);

    	sprintf(buf1,"GYO: [%5f %5f %5f] deg/s",gyr1[0]/16.4,gyr1[1]/16.4,gyr1[2]/16.4);

    	println(buf1);

    	sprintf(buf1,"GYO: [%5f %5f %5f] deg",angle[0],angle[1],angle[2]);
    	println(buf1);
    	//nano_wait(50000000);
    	writechar('\n');

    }

    return 0;
}
