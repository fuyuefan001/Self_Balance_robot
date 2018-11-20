


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
    I2C_InitStruct.I2C_Timing=0x20400A8D;
    // From table 83. p642 of FRM.  Set for 400 kHz with 8MHz clock.
            // SCLL
    I2C_Init(I2C1,&I2C_InitStruct);
    I2C_Cmd(I2C1, ENABLE);
    I2C_AcknowledgeConfig(I2C1, ENABLE);

}
void I2C_Send7bitAddress(I2C_TypeDef* I2Cx, uint8_t Address, uint8_t I2C_Direction)
{
  /* Check the parameters */
  assert_param(IS_I2C_ALL_PERIPH(I2Cx));
  assert_param(IS_I2C_DIRECTION(I2C_Direction));
  /* Test on the direction to set/reset the read/write bit */
  if (I2C_Direction != I2C_Direction_Transmitter)
  {
	/* Shift the address one bit to the left */
	Address = Address << 1;
	/* Set the address bit0 for read */
	Address |= 0b00000001;
	I2Cx->TXDR = Address;
  }
  else
  {
	/* Shift the address one bit to the left */
	Address = Address << 1;
	/* Reset the address bit0 for write */
	Address &= 0b11111110;
	I2Cx->TXDR = Address;
  }
  /* Send the address */

}

// Send multiple bytes to a target slave.
// - Wait for idle.
// - Send data, byte by byte.
// - After all bytes sent, hardware sets the TC flag.
//   Software waits until TC is set.
//   Hardware automatically clears TC flag when START bit or STOP bit in
//   CR2 is set.

void I2C_WrReg(uint8_t Reg, uint8_t Val){

	//Wait until I2C isn't busy
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) == SET);

	//"Handle" a transfer - The STM32F0 series has a shocking
	//I2C interface... Regardless! Send the address of the HMC
	//sensor down the I2C Bus and generate a start saying we're
	//going to write one byte. I'll be completely honest,
	//the I2C peripheral doesn't make too much sense to me
	//and a lot of the code is from the Std peripheral library
	I2C_TransferHandling(I2C1, DEV_ADDR, 1, I2C_Reload_Mode, I2C_Generate_Start_Write);

	//Ensure the transmit interrupted flag is set
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_TXIS) == RESET);

	//Send the address of the register we wish to write to
	I2C_SendData(I2C1, Reg);

	//Ensure that the transfer complete reload flag is
	//set, essentially a standard TC flag
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_TCR) == RESET);

	//Now that the HMC5883L knows which register
	//we want to write to, send the address again
	//and ensure the I2C peripheral doesn't add
	//any start or stop conditions
	I2C_TransferHandling(I2C1, DEV_ADDR, 1, I2C_AutoEnd_Mode, I2C_No_StartStop);

	//Again, wait until the transmit interrupted flag is set
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_TXIS) == RESET);

	//Send the value you wish you write to the register
	I2C_SendData(I2C1, Val);

	//Wait for the stop flag to be set indicating
	//a stop condition has been sent
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF) == RESET);

	//Clear the stop flag for the next potential transfer
	I2C_ClearFlag(I2C1, I2C_FLAG_STOPF);
}

uint8_t I2C_RdReg(int8_t Reg, int8_t *Data, uint8_t DCnt){
	int8_t Cnt, SingleData = 0;

	//As per, ensure the I2C peripheral isn't busy!
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) == SET);

	//Again, start another tranfer using the "transfer handling"
	//function, the end bit being set in software this time
	//round, generate a start condition and indicate you will
	//be writing data to the HMC device.
	I2C_TransferHandling(I2C1, DEV_ADDR, 1, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);

	//Wait until the transmit interrupt status is set
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_TXIS) == RESET);

	//Send the address of the register you wish to read
	I2C_SendData(I2C1, (uint8_t)Reg);

	//Wait until transfer is complete!
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_TC) == RESET);

	//As per, start another transfer, we want to read DCnt
	//amount of bytes. Generate a start condition and
	//indicate that we want to read.
	I2C_TransferHandling(I2C1, DEV_ADDR, DCnt, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);

	//Read in DCnt pieces of data
	for(Cnt = 0; Cnt<DCnt; Cnt++){
        //Wait until the RX register is full of luscious data!
        while(I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET);
        //If we're only reading one byte, place that data direct into the
        //SingleData variable. If we're reading more than 1 piece of data
        //store in the array "Data" (a pointer from main)
        if(DCnt > 1) Data[Cnt] = I2C_ReceiveData(I2C1);
	else SingleData = I2C_ReceiveData(I2C1);
     }

     //Wait for the stop condition to be sent
     while(I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF) == RESET);

     //Clear the stop flag for next transfers
     I2C_ClearFlag(I2C1, I2C_FLAG_STOPF);

     //Return a single piece of data if DCnt was
     //less than 1, otherwise 0 will be returned.
	return SingleData;

}
//===========================================================================

void MPU6050_Init(void)
{
//	int8_t tmp[1];
//	int x=-1;
//	tmp[0]=0x00;
	uint8_t reg;
	uint8_t val;
	reg=PWR_MGMT_1;val=0x00;
	I2C_WrReg(reg,val);
	reg=SMPLRT_DIV;val=0x07;
	I2C_WrReg(reg,val);
	reg=CONFIG;val=0x06;
	I2C_WrReg(reg,val);
	reg=GYRO_CONFIG;val=0x18;
	I2C_WrReg(reg,val);
	reg=ACCEL_CONFIG;val=0x08;
	I2C_WrReg(reg,val);

}
int16_t GetData(uint8_t regAddr)
{
	int a;
	int sum=0;
	int8_t data[2]={0,0};
	for(a=0;a<8;a++){

		I2C_RdReg(regAddr, data, 2);
		sum+=((data[0]<<8)+data[1]);
	}
	return (int16_t) (sum/8);
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






