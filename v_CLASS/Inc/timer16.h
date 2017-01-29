#ifndef _TIMER16_H
#define _TIMER16_H

void TIM1_UP_TIM16_IRQHandler(void);
void TIM16_Init(void);
void TIM16_Start(void);
void TIM16_Stop(void);
void TIM16_Reset(void);
#endif