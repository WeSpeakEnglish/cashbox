#include "timer16.h"
#include "stm32f3xx_hal.h"
//#include "sdcard.hpp"
#include <stdint.h>
#include <iwdg.h>


void TIM16_Start(uint32_t Prescaler, uint32_t Count){

TIM16->PSC = Prescaler-1;
TIM16->ARR = Count; 
TIM16->DIER |= TIM_DIER_UIE;
TIM16->CR1 |= TIM_CR1_CEN; //Counter enable
TIM16->CNT=0;  
NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);
}
  
void TIM16_Stop(void){
TIM16->DIER &= ~TIM_DIER_UIE;
TIM16->CR1 &= ~TIM_CR1_CEN; //Counter enable
NVIC_DisableIRQ(TIM1_UP_TIM16_IRQn);
}  

void TIM16_Reset(void){
TIM16->CNT=0;
}  

void TIM1_UP_TIM16_IRQHandler(void){
static uint32_t InsideCounter = 0;  

TIM16->SR&=~TIM_SR_UIF; // сбрасываю флаг прерывани€
TIM16->CNT=0; // обнул€ю счетчик... 

//EXTI->IMR|=EXTI_IMR_MR0; //разрешаю внешние прерывани€
// HAL_IWDG_Refresh(&hiwdg);
 InsideCounter++;
}


  