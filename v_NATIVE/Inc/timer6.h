#ifndef _TIMER6_H
#define _TIMER6_H
#include <stdint.h>

void TIM6_DAC_IRQHandler(void);
void TIM6_Start(uint32_t Prescaler, uint32_t Count);
void TIM6_Stop(void);
void TIM6_Reset(void);
extern volatile uint32_t usDelayTime;
  
#endif