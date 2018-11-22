#include "miniproject.h"

    //TIM2_Encoder_Init,Tim2_CH1(PA0);Tim2_CH2(PA1)
    //arr：自动重装值 0XFFFF
    //psc：时钟预分频数        ，不分频
    //见STM32中文手册 14.3.12编码器接口模式
void TIM2_Encoder_Init(uint16_t arr,uint16_t psc)    {
        RCC->APB1ENR|=1<<0;       //TIM2时钟使能
        RCC->APB2ENR|=1<<2;       //使能PORTA时钟

        GPIOA->MODER&=!(0b1111);
        GPIOA->PUPDR&=!(0b1111);   //PA0、PA1 浮空输入

        TIM2->ARR=arr;            //设定计数器自动重装值
        TIM2->PSC=psc;            //预分频器

        TIM2->CCMR1 |= 1<<0;          //输入模式，IC1FP1映射到TI1上
        TIM2->CCMR1 |= 1<<8;          //输入模式，IC2FP2映射到TI2上
        TIM2->CCER |= 0<<1;                  //IC1不反向
        TIM2->CCER |= 0<<5;           //IC2不反向
        TIM2->SMCR |= 3<<0;                  //所用输入均在上升沿或下降沿有效
        TIM2->CR1 |= 1<<0;                  //使能计数器
}
    //计数寄存器赋值
void TIM2_Encoder_Write(int data){
	TIM2->CNT = data;
}
    //读计数个数
int TIM2_Encoder_Read(void){
	TIM2_Encoder_Write(0);        //计数器清0
    nano_wait(10000000); //检测时间，可调节
//    delay_ms(10);
    return (int)(short)(TIM2->CNT);           //数据类型转换                             //记录边沿变化次数（一个栅格被记录4次）
}
