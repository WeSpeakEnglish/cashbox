#include "timer17.h"
#include "keyboard.h"
#include "stm32f3xx_hal.h"

void TIM17_Reset(void){
TIM17->CNT=0;
}  

void TIM17_Start(uint32_t Prescaler, uint32_t Count){ //prescaler and top number of count to interrupt (up mode)
TIM17->CNT=0;
TIM17->PSC = Prescaler-1;
TIM17->ARR = Count; 
TIM17->DIER |= TIM_DIER_UIE;
TIM17->CR1 |= TIM_CR1_CEN; //Counter enable
NVIC_EnableIRQ(TIM1_TRG_COM_TIM17_IRQn);
}
  
void TIM17_Stop(void){
TIM17->DIER &= ~TIM_DIER_UIE;
TIM17->CR1 &= ~TIM_CR1_CEN; //Counter enable
NVIC_DisableIRQ(TIM1_TRG_COM_TIM17_IRQn);
}  

void TIM1_TRG_COM_TIM17_IRQHandler(void){
static uint32_t InsideCounter = 0;  
   if (TIM17->SR & TIM_SR_UIF)
   {
      TIM17->SR &= ~TIM_SR_UIF;
      TIM17->CNT=0; // reset counter 
      
      if(!(InsideCounter%5))
                  ScanKeyboard();
      
      InsideCounter++;

   }



//EXTI->IMR|=EXTI_IMR_MR0; //разрешаю внешние прерывания

}

