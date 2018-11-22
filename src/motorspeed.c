#include "miniproject.h"

    //TIM2_Encoder_Init,Tim2_CH1(PA0);Tim2_CH2(PA1)
    //arr���Զ���װֵ 0XFFFF
    //psc��ʱ��Ԥ��Ƶ��        ������Ƶ
    //��STM32�����ֲ� 14.3.12�������ӿ�ģʽ
void TIM2_Encoder_Init(uint16_t arr,uint16_t psc)    {
        RCC->APB1ENR|=1<<0;       //TIM2ʱ��ʹ��
        RCC->APB2ENR|=1<<2;       //ʹ��PORTAʱ��

        GPIOA->MODER&=!(0b1111);
        GPIOA->PUPDR&=!(0b1111);   //PA0��PA1 ��������

        TIM2->ARR=arr;            //�趨�������Զ���װֵ
        TIM2->PSC=psc;            //Ԥ��Ƶ��

        TIM2->CCMR1 |= 1<<0;          //����ģʽ��IC1FP1ӳ�䵽TI1��
        TIM2->CCMR1 |= 1<<8;          //����ģʽ��IC2FP2ӳ�䵽TI2��
        TIM2->CCER |= 0<<1;                  //IC1������
        TIM2->CCER |= 0<<5;           //IC2������
        TIM2->SMCR |= 3<<0;                  //����������������ػ��½�����Ч
        TIM2->CR1 |= 1<<0;                  //ʹ�ܼ�����
}
    //�����Ĵ�����ֵ
void TIM2_Encoder_Write(int data){
	TIM2->CNT = data;
}
    //����������
int TIM2_Encoder_Read(void){
	TIM2_Encoder_Write(0);        //��������0
    nano_wait(10000000); //���ʱ�䣬�ɵ���
//    delay_ms(10);
    return (int)(short)(TIM2->CNT);           //��������ת��                             //��¼���ر仯������һ��դ�񱻼�¼4�Σ�
}
