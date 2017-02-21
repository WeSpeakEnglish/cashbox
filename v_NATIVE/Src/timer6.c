#include "timer6.h"
#include "keyboard.h"
#include "stm32f3xx_hal.h"

volatile uint32_t usDelayTime = 0;

void TIM6_Reset(void) {
    TIM6->CNT = 0;
}

void TIM6_Start(uint32_t Prescaler, uint32_t Count) { //prescaler and top number of count to interrupt (up mode)
    TIM6->CNT = 0;
    TIM6->PSC = Prescaler - 1;
    TIM6->ARR = Count;
    TIM6->DIER |= TIM_DIER_UIE;
    TIM6->CR1 |= TIM_CR1_CEN; //Counter enable
    NVIC_EnableIRQ(TIM6_DAC_IRQn);
}

void TIM6_Stop(void) {
    TIM6->DIER &= ~TIM_DIER_UIE;
    TIM6->CR1 &= ~TIM_CR1_CEN; //Counter enable
    NVIC_DisableIRQ(TIM6_DAC_IRQn);
}

void TIM6_DAC_IRQHandler(void){
 // static uint32_t InsideCounter = 0;
    if (TIM6->SR & TIM_SR_UIF) {
        TIM6->SR &= ~TIM_SR_UIF;
        TIM6->CNT = 0; // reset counter 


            usDelayTime+=50;
    }

}

