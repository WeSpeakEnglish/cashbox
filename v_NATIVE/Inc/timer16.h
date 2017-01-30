#ifndef _TIMER16_H
#define _TIMER16_H
#include <stdint.h>

void TIM1_UP_TIM16_IRQHandler(void);
void TIM16_Start(uint32_t Prescaler, uint32_t Count);
void TIM16_Stop(void);
void TIM16_Reset(void);
#endif