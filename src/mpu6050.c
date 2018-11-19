

#define DEV_ADDR 0xD0 // 6050 器件地址
#define	SMPLRT_DIV	0x19	//陀螺仪输出率的分频，典型值0x07,(1kHz)
#define	CONFIG		0x1A	//低通滤波频率,一般0x01~0x05,数值越大带宽越小延时越长
#define	GYRO_CONFIG	0x1B
#define	ACCEL_CONFIG	0x1C
#define	ACCEL_XOUT_H	0x3B    //加速度计X轴数据高位
#define	ACCEL_XOUT_L	0x3C    //加速度计X轴数据低位
#define	ACCEL_YOUT_H	0x3D    //以此类推
#define	ACCEL_YOUT_L	0x3E
#define	ACCEL_ZOUT_H	0x3F
#define	ACCEL_ZOUT_L	0x40
#define	TEMP_OUT_H	0x41    //温度传感器数据
#define	TEMP_OUT_L	0x42
#define	GYRO_XOUT_H	0x43    //陀螺仪X轴数据高位
#define	GYRO_XOUT_L	0x44
#define	GYRO_YOUT_H	0x45
#define	GYRO_YOUT_L	0x46
#define	GYRO_ZOUT_H	0x47
#define	GYRO_ZOUT_L	0x48
#define	PWR_MGMT_1	0x6B	//电源管理，典型值：0x00(正常启用)
#define	WHO_AM_I	0x75	//IIC地址寄存器(默认数值0x68，只读)
#define	SlaveAddress	0x68 	//MPU6050模块AD0引脚接低电平时的地址

#include "miniproject.h"

extern int16_t acc1[];
extern int16_t gyr1[];
void I2C_config(void)
{
    //定义结构体
    GPIO_InitTypeDef GPIO_InitStructure;
    I2C_InitTypeDef I2C_InitStruct;
    //开启时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
    //初始化GPIO
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType=GPIO_OType_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    /* Connect PXx to I2C_SCL*/
    GPIO_PinAFConfig( GPIOB , GPIO_PinSource6, GPIO_AF_1);
    /* Connect PXx to I2C_SDA*/
    GPIO_PinAFConfig( GPIOB ,GPIO_PinSource7, GPIO_AF_1);


    //初始化I2C，开启I2C1
    I2C_InitStruct.I2C_Ack=I2C_Ack_Enable;
    I2C_InitStruct.I2C_AcknowledgedAddress=I2C_AcknowledgedAddress_7bit;
    I2C_InitStruct.I2C_AnalogFilter=I2C_AnalogFilter_Enable;
    I2C_InitStruct.I2C_DigitalFilter=0x00;
    I2C_InitStruct.I2C_Mode=I2C_Mode_I2C;
    I2C_InitStruct.I2C_OwnAddress1=0x00;
    // From table 83. p642 of FRM.  Set for 400 kHz with 8MHz clock.
    I2C1->TIMINGR = 0;
    I2C1->TIMINGR &= ~I2C_TIMINGR_PRESC;// Clear prescaler
    I2C1->TIMINGR |= 0 << 28;           // Set prescaler to 0
    I2C1->TIMINGR |= 3 << 20;           // SCLDEL
    I2C1->TIMINGR |= 1 << 16;           // SDADEL
    I2C1->TIMINGR |= 3 << 8;            // SCLH
    I2C1->TIMINGR |= 9 << 0;            // SCLL
    I2C_Init(I2C1,&I2C_InitStruct);
    I2C_Cmd(I2C1, ENABLE);
    I2C_AcknowledgeConfig(I2C1, ENABLE);

}


int i2c_checknack(void) {
    if (I2C1->ISR & I2C_ISR_NACKF)
        return 1;
    return 0;
}

//===========================================================================
// Clear NACK condition.
void i2c_clearnack(void) {
    if (I2C1->ISR & I2C_ISR_NACKF)
        I2C1->ICR |= I2C_ICR_NACKCF;
}

//===========================================================================
// Clear STOP condition.
int i2c_checkstop(void) {
    if (I2C1->ISR & I2C_ISR_STOPF)
        return 1;
    return 0;
}

//===========================================================================
// Clear STOP condition.
void i2c_clearstop(void) {
    if (I2C1->ISR & I2C_ISR_STOPF)
        I2C1->ICR |= I2C_ICR_STOPCF;
}

void i2c_start(uint32_t devaddr, uint8_t size, uint8_t dir) {
    // dir: 0 = master requests a write transfer
    // dir: 1 = master requests a read transfer
    uint32_t tmpreg = I2C1->CR2;
    tmpreg &= ~(I2C_CR2_SADD | I2C_CR2_NBYTES |
                I2C_CR2_RELOAD | I2C_CR2_AUTOEND |
                I2C_CR2_RD_WRN | I2C_CR2_START | I2C_CR2_STOP);
    if (dir == 1)
        tmpreg |= I2C_CR2_RD_WRN;  // Read from slave
    else
        tmpreg &= I2C_CR2_RD_WRN;  // Write to slave
    tmpreg |= ((devaddr<<1) & I2C_CR2_SADD) | ((size << 16) & I2C_CR2_NBYTES);
    tmpreg |= I2C_CR2_START;
    I2C1->CR2 = tmpreg;
}

//===========================================================================
// Generate a stop bit.
void i2c_stop(void) {
    if (I2C1->ISR & I2C_ISR_STOPF)
        return;
    // Master: Generate STOP bit after current byte has been transferred.
    I2C1->CR2 |= I2C_CR2_STOP;
    // Wait until STOPF flag is reset
    while( (I2C1->ISR & I2C_ISR_STOPF) == 0);
    I2C1->ICR |= I2C_ICR_STOPCF; // Write  to clear STOPF flag
}

//===========================================================================
// Check wait for the bus to be idle.
void i2c_waitidle(void) {
    while ( (I2C1->ISR ^ I2C_ISR_BUSY) == I2C_ISR_BUSY);  // while busy, wait.
}

//===========================================================================
// Send multiple bytes to a target slave.
// - Wait for idle.
// - Send data, byte by byte.
// - After all bytes sent, hardware sets the TC flag.
//   Software waits until TC is set.
//   Hardware automatically clears TC flag when START bit or STOP bit in
//   CR2 is set.
int8_t i2c_senddata(uint8_t devaddr, void *pdata, uint8_t size) {
    int i;
    if (size <= 0 || pdata == 0) return -1;
    int8_t *udata = (int8_t*)pdata;
    I2C_SlaveAddressConfig( I2C1, SlaveAddress);
    i2c_waitidle();
    // Last argument is dir: 0 = sending data to the slave.
    //i2c_start(devaddr, size, 0);
    I2C_GenerateSTART(I2C1, ENABLE);
//    i2c_clearnack() ;
//    I2C_SendData(I2C1,devaddr);
    for(i=0; i<size; i++) {
        // TXIS bit is set by hardware when the TXDR register is empty and the
        // data to be transmitted must be written in the TXDR register.  It is
        // cleared when the next data to be sent is written in the TXDR reg.
        // The TXIS flag is not set when a NACK is received.
        int count = 0;
        while( (I2C1->ISR & I2C_ISR_TXIS) == 0) {
            count += 1;
            if (count > 1000000)
                return -1;
            if (i2c_checknack()) {
                i2c_clearnack();
                i2c_stop();
                return -2;
            }
        }

        // TXIS is cleared by writing to the TXDR register.
        I2C1->TXDR = udata[i] & I2C_TXDR_TXDATA;
    }

    // Wait until TC flag is set or the NACK flag is set.
    while((I2C1->ISR & I2C_ISR_TC) == 0 && (I2C1->ISR & I2C_ISR_NACKF) == 0);

    if ( (I2C1->ISR & I2C_ISR_NACKF) != 0)
        return -3;
    i2c_stop();
    return 0;
}

//===========================================================================
// Receive multiple bytes from a target slave.
int8_t i2c_recvdata(uint8_t devaddr, void *pdata, uint8_t size) {
    int i;
    if (size <= 0 || pdata == 0) return -1;
    int8_t *udata = (int8_t*)pdata;
    I2C_SlaveAddressConfig( I2C1,SlaveAddress);
    //i2c_start(devaddr, size, 1); // 1 = receiving from the slave
    I2C_GenerateSTART(I2C1, ENABLE);

    for(i=0; i<size; i++) {
        // Wait until RXNE flag is set
        int count = 0;
        while ( (I2C1->ISR & I2C_ISR_RXNE) == 0) {
            count += 1;
            if (count > 1000000)
                return -1;
            if (i2c_checknack()) {
                i2c_clearnack();
                i2c_stop();
                return -2;
            }
        }
        udata[i] = I2C1->RXDR & I2C_RXDR_RXDATA;
    }

    int x=0;
    while((I2C1->ISR & I2C_ISR_TC) == 0)
        x++;  // Wait until TCR flag is set

    i2c_stop();
    return 0;
}

void MPU6050_Init(void)
{
	int8_t tmp[1];
	int x=-1;
	tmp[0]=0x00;

	x=i2c_senddata(PWR_MGMT_1,tmp,1 );    //解除休眠状态
	if(x==-1){sendmsg("timeout1",8);}else if(x==-2){sendmsg("nack1",5);}
	tmp[0]=0x07;
	x=i2c_senddata(SMPLRT_DIV, tmp,1);    //陀螺仪采样率，典型值：0x07(125Hz)
	if(x==-1){sendmsg("timeout2",8);}else if(x==-2){sendmsg("nack2",5);}
	tmp[0]=0x06;
	x=i2c_senddata(CONFIG, tmp,1);        //低通滤波频率，典型值：0x06(5Hz)
	if(x==-1){sendmsg("timeout3",8);}else if(x==-2){sendmsg("nack3",5);}
	tmp[0]=0x18;
	x=i2c_senddata(GYRO_CONFIG,tmp,1);   //陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
	if(x==-1){sendmsg("timeout4",8);}else if(x==-2){sendmsg("nack4",5);}
	tmp[0]=0x01;
	x=i2c_senddata(ACCEL_CONFIG, tmp,1);  //加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz)
	if(x==-1){sendmsg("timeout5",8);}else if(x==-2){sendmsg("nack5",5);}
}
int16_t GetData(uint8_t regAddr)
{
    uint8_t Data_H, Data_L;
    uint16_t data;
    Data_H=i2c_recvdata(regAddr,&Data_H,1);
    Data_L=i2c_recvdata(regAddr + 1,&Data_L,1);
    data = (Data_H << 8) | Data_L;  // 合成数据
    return data;
}

void GetAccGyro(void)//读取6轴数据
{
    acc1[0] = GetData(ACCEL_XOUT_H);
    acc1[1] = GetData(ACCEL_YOUT_H);
    acc1[2] = GetData(ACCEL_ZOUT_H);
    gyr1[0] = GetData(GYRO_XOUT_H);
    gyr1[1] = GetData(GYRO_YOUT_H);
    gyr1[2] = GetData(GYRO_ZOUT_H);
}









