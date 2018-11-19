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
int main(void)
{
	char buf1[15]={0};
	char buf2[15]={0};
	char buf3[15]={0};
	init_usart2();
	sendmsg("Finish init usart2",18);
	I2C_config();
	sendmsg("Finish init i2c",15);
	MPU6050_Init();
	sendmsg("Finish init gyroscope",21);
    while(1){
    	GetAccGyro();
    	sprintf(buf1,"%d",acc1[0]);
    	sprintf(buf2,"%d",acc1[1]);
    	sprintf(buf3,"%d",acc1[2]);
    	sendmsg(buf1,15);
    	sendmsg(buf2,15);
    	sendmsg(buf3,15);
    	sprintf(buf1,"%d",gyr1[0]);
    	sprintf(buf2,"%d",gyr1[1]);
    	sprintf(buf3,"%d",gyr1[2]);
    	sendmsg(buf1,15);
    	sendmsg(buf2,15);
    	sendmsg(buf3,15);
    	nano_wait(1000000000);
    	writechar('\n');

    }

    return 0;
}
