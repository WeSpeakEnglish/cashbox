#include "timer16.h"
#include "stm32f3xx_hal.h"
#include "sdcard.hpp"
#include <stdint.h>
#include <iwdg.h>

void TIM16_Init(void){
  TIM16->PSC = 7;
  TIM16->ARR = 576; //one second
  TIM16->DIER |= TIM_DIER_UIE; //interrupts
  TIM16->CR1 |= TIM_CR1_CEN; // 
  NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn); //enable interrupt
 // HAL_NVIC_SetPriority(TIM1_UP_TIM16_IRQn, 0, 0);
return;

}

void TIM16_Start(void){
TIM16->CNT=0;
TIM16->DIER |= TIM_DIER_UIE;
TIM16->CR1 |= TIM_CR1_CEN; //Counter enable
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

SD_Card_On_Off((uint8_t)InsideCounter%2);

//EXTI->IMR|=EXTI_IMR_MR0; //разрешаю внешние прерывани€
 InsideCounter++;
}


  