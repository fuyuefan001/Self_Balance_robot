#include "stm32f0xx_gpio.h"
#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include <stdio.h>
#define SCL 1
#define SDI 2
#define SS 4
#define SPI_DELAY 400


void RCCconfig(void){
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA|RCC_AHBPeriph_GPIOB|RCC_AHBPeriph_GPIOC|RCC_AHBPeriph_GPIOD,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2|RCC_APB1Periph_TIM3,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1|RCC_APB2Periph_USART1,ENABLE);

}
static void GPIOconfig(void){

	GPIO_InitTypeDef GPIOCinit;
	GPIOCinit.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2;
	GPIOCinit.GPIO_Mode=GPIO_Mode_OUT;
	GPIOCinit.GPIO_PuPd=GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC,&GPIOCinit);
}
static void sendbit(int b) {
	GPIOC->BRR = SCL;
	GPIOC->BSRR = b ? SDI : (SDI << 16);
	nano_wait(SPI_DELAY);
	GPIOC->BSRR = SCL;
	nano_wait(SPI_DELAY);
}
static void sendbyte(char b){
	int x;
	for(x=8; x>0; x--) {
		sendbit(b & 0x80);
		b <<= 1;

	}
}

static void cmd(char b) {
	GPIOC->BRR = SS;
	nano_wait(SPI_DELAY);
	sendbit(0); // RS = 0 for command
	sendbit(0); //j R/W = 0 for write
	sendbyte(b);
	nano_wait(SPI_DELAY);
	GPIOC->BSRR = SS;
	nano_wait(SPI_DELAY);
}

static void data(char b) {
	GPIOC->BRR = SS;
	nano_wait(SPI_DELAY);
	sendbit(1); // RS = 1 for data
	sendbit(0); // R/W = 0 for write
	sendbyte(b);
	nano_wait(SPI_DELAY);
	GPIOC->BSRR = SS;
	nano_wait(SPI_DELAY);
}

void init_lcd(void){
	RCCconfig();
	GPIOconfig();
	//GPIOC->ODR|=7;
	nano_wait(100000000);
	cmd(0x38);
	cmd(0x0c);
	cmd(0x01);
	nano_wait(6200000);
	cmd(0x02);
	cmd(0x06);
}
void display1(const char* str){
	cmd(0x02);
	int len;
	for(len=0;len<16;len++){
		if(str[len]=='\0'){
			break;
		}else{
			data(str[len]);
		}
	}
}

void display2(const char* str){
	cmd(0xc0);
	int len;
	for(len=0;len<16;len++){
		if(str[len]=='\0'){
			break;
		}else{
			data(str[len]);
		}
	}
}

void init_pwm(void) {
	GPIO_InitTypeDef GPIOAinit;
	GPIOAinit.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10;
	GPIOAinit.GPIO_Mode=GPIO_Mode_AF;
	GPIOAinit.GPIO_OType=GPIO_OType_PP;
	GPIOAinit.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&GPIOAinit);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_2);
	TIM_TimeBaseInitTypeDef Tinit;
	Tinit.TIM_ClockDivision=0;
	Tinit.TIM_CounterMode=TIM_CounterMode_Up;
	Tinit.TIM_Period=100;
	Tinit.TIM_Prescaler=48000;
	Tinit.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM1,&Tinit);
	TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_Low;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);
    TIM_OC2Init(TIM1, &TIM_OCInitStructure);
    TIM_OC3Init(TIM1, &TIM_OCInitStructure);
    TIM_Cmd(TIM1, ENABLE);
    TIM_CtrlPWMOutputs(TIM1, ENABLE);

}

void update_freq(int freq) {
    // Your code goes here.
	int a;
	a = (48000000/(freq*100))-1;
	TIM1->PSC = a;
}

void update_rgb(int r, int g, int b) {
    // Your code goes here.
	TIM1->CCR1 = 101-r;
	TIM1->CCR2 = 101-g;
	TIM1->CCR3 = 101-b;
}
