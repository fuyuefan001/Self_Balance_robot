
#include "stm32f0xx_adc.h"
#include "stm32f0xx_dac.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include "stm32f0xx_tim.h"
#include <stdio.h>
#define SCL 1
#define SDI 2
#define SS 4
#define SPI_DELAY 400
//#include "nano_wait.S"
void nano_wait(int);

void RCCconfig(void){
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA|RCC_AHBPeriph_GPIOB|RCC_AHBPeriph_GPIOC|RCC_AHBPeriph_GPIOD,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2|RCC_APB1Periph_TIM3,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1|RCC_APB2Periph_USART1,ENABLE);

}
void tim2_init(void){
	RCC->APB1ENR|=RCC_APB1ENR_TIM2EN;
	TIM_TimeBaseInitTypeDef Tinit;
	Tinit.TIM_ClockDivision=0;
	Tinit.TIM_CounterMode=TIM_CounterMode_Up;
	Tinit.TIM_Period=10-1;
	Tinit.TIM_Prescaler=48000-1;
	Tinit.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM2,&Tinit);
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM2, ENABLE);
    NVIC->ISER[0]|=1<<TIM2_IRQn ;
}


void tim1_init(void) {

	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	GPIOA -> MODER &= ~(3<<16);
	GPIOA -> MODER |= 2<<16;
	GPIOA -> MODER &= ~(3<<18);
	GPIOA -> MODER |= 2<<18;
	GPIOA -> MODER &= ~(3<<20);
	GPIOA -> MODER |= 2<<20;
	GPIOA->AFR[1] &= ~0xfff;
	GPIOA->AFR[1] |= 0x222;
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	TIM1->CR1 &= ~TIM_CR1_DIR;
	TIM1->PSC = 16-1;
	TIM1->ARR=60000-1;
//	TIM1->CCR1 =5000;
//	TIM1->CCR2 =5000;
//	TIM1->CCR3 =5000;
	TIM1->BDTR |= TIM_BDTR_MOE;
	uint16_t tmpccmr1 = 0;
	tmpccmr1 = TIM1->CCMR1;
	/* Reset the OC1PE Bit */
	tmpccmr1 &= (uint16_t)~((uint16_t)TIM_CCMR1_OC1PE);
	/* Enable or Disable the Output Compare Preload feature */
	tmpccmr1 |= TIM_OCPreload_Enable;
	  /* Write to TIMx CCMR1 register */

	/* Reset the OC2PE Bit */
	tmpccmr1 &= (uint16_t)~((uint16_t)TIM_CCMR1_OC2PE);
	/* Enable or Disable the Output Compare Preload feature */
	tmpccmr1 |= (uint16_t)(TIM_OCPreload_Enable << 8);
	/* Write to TIMx CCMR1 register */
	TIM1->CCMR1 = tmpccmr1;

	uint16_t tmpccmr2 = 0;
	tmpccmr2 = TIM1->CCMR2;
	/* Reset the OC2PE Bit */
	tmpccmr2 &= (uint16_t)~((uint16_t)TIM_CCMR2_OC3PE);
	/* Enable or Disable the Output Compare Preload feature */
	tmpccmr2 |= (uint16_t)(TIM_OCPreload_Enable);
	/* Write to TIMx CCMR1 register */
	TIM1->CCMR2 = tmpccmr2;

	TIM1->CCMR1 |= TIM_CCMR1_OC1M;
	TIM1->CCMR1 |= TIM_CCMR1_OC2M;
	TIM1->CCMR2 |= TIM_CCMR2_OC3M;
	TIM1->CCER |= TIM_CCER_CC1E;
	TIM1->CCER |= TIM_CCER_CC2E;
	TIM1->CCER |= TIM_CCER_CC3E;
	TIM1->CR1 |= TIM_CR1_CEN;
}
/**
 * from 0 t0 1000
 */

void duty_cyc1(int num){
	TIM1->CCR1 =TIM1->ARR-num;

}
void duty_cyc2(int num){

	TIM1->CCR2 =TIM1->ARR-num;

}
void duty_cyc3(int num){

	TIM1->CCR3 =TIM1->ARR-num;
}
