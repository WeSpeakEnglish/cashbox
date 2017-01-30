#ifndef _TIMER17_H
#define _TIMER17_H
#include <stdint.h>

void TIM1_TRG_COM_TIM17_IRQHandler(void);
void TIM17_Start(uint32_t Prescaler, uint32_t Count);
void TIM17_Stop(void);
void TIM17_Reset(void);
  
#endif