#ifndef _GLOBAL_
#define _GLOBAL_
//include
#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include "stm32f0xx_i2c.h"
#include <string.h>
#include <stdlib.h> // for strtoul()

#define DEV_ADDR 0xD0 // 6050 ������ַ
#define	SMPLRT_DIV	0x19	//����������ʵķ�Ƶ������ֵ0x07,(1kHz)
#define	CONFIG		0x1A	//��ͨ�˲�Ƶ��,һ��0x01~0x05,��ֵԽ�����ԽС��ʱԽ��
#define	GYRO_CONFIG	0x1B
#define	ACCEL_CONFIG	0x1C
#define	ACCEL_XOUT_H	0x3B    //���ٶȼ�X�����ݸ�λ
#define	ACCEL_XOUT_L	0x3C    //���ٶȼ�X�����ݵ�λ
#define	ACCEL_YOUT_H	0x3D    //�Դ�����
#define	ACCEL_YOUT_L	0x3E
#define	ACCEL_ZOUT_H	0x3F
#define	ACCEL_ZOUT_L	0x40
#define	TEMP_OUT_H	0x41    //�¶ȴ���������
#define	TEMP_OUT_L	0x42
#define	GYRO_XOUT_H	0x43    //������X�����ݸ�λ
#define	GYRO_XOUT_L	0x44
#define	GYRO_YOUT_H	0x45
#define	GYRO_YOUT_L	0x46
#define	GYRO_ZOUT_H	0x47
#define	GYRO_ZOUT_L	0x48
#define	PWR_MGMT_1	0x6B	//��Դ��������ֵ��0x00(��������)
#define	WHO_AM_I	0x75	//IIC��ַ�Ĵ���(Ĭ����ֵ0x68��ֻ��)
#define	SlaveAddress	0x68 	//MPU6050ģ��AD0���Žӵ͵�ƽʱ�ĵ�ַ


 void nano_wait(int t);
void I2C_config(void);
void init_usart2(void);
void writechar(char );
char readchar(void);
void sendmsg(char*,int );
int print(const char *);
int println(const char *);
int readln(void *, int );
int i2c_checknack(void);
void RCCconfig(void);
void init_pwm(void);
//global variable
void tim1_init(void);
void tim2_init(void);
void TIM2_IRQHandler();
void duty_cyc1(int);
void duty_cyc2(int);
void duty_cyc3(int);
void kalman_filter(float, float , float *, float *);
#endif

