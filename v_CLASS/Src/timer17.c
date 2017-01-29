#include "timer17.h"
#include "stm32f3xx_hal.h"
#include "sdcard.hpp"
#include <stdint.h>

void TIM1_TRG_COM_TIM17_IRQHandler(void){
static uint32_t InsideCounter = 0;  

TIM17->SR&=~TIM_SR_UIF; // сбрасываю флаг прерывани€
TIM17->CNT=0; // обнул€ю счетчик... 
//EXTI->IMR|=EXTI_IMR_MR0; //разрешаю внешние прерывани€
InsideCounter++;
}

void TIM17_Init(void){ // Set counter frequency, 72000000 / 4608 == 15625 Hz
  TIM17->PSC = 7;
  TIM17->ARR = 576; //one second
  TIM17->DIER |= TIM_DIER_UIE; //interrupts
  TIM17->CR1 |= TIM_CR1_CEN; // 
  NVIC_EnableIRQ(TIM1_TRG_COM_TIM17_IRQn); //enable interrupt
 // HAL_NVIC_SetPriority(TIM1_UP_TIM16_IRQn, 0, 0);
return;

}

void TIM17_Reset(void){
TIM17->CNT=0;
}  

void TIM17_Start(void){
TIM17->CNT=0;
TIM17->DIER |= TIM_DIER_UIE;
TIM17->CR1 |= TIM_CR1_CEN; //Counter enable
NVIC_EnableIRQ(TIM1_TRG_COM_TIM17_IRQn);
}
  
void TIM17_Stop(void){
TIM17->DIER &= ~TIM_DIER_UIE;
TIM17->CR1 &= ~TIM_CR1_CEN; //Counter enable
NVIC_DisableIRQ(TIM1_TRG_COM_TIM17_IRQn);
}  

