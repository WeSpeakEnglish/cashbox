#ifndef _TIMER16_H
#define _TIMER16_H
#include <stdint.h>

void TIM1_UP_TIM16_IRQHandler(void);
void TIM16_Start(uint32_t Prescaler, uint32_t Count);

void TIM16_Stop(void);
void TIM16_Reset(void);
uint32_t millis(void);

void simpleTimeoutInit(volatile uint32_t *simpleTimeout_instance);
uint8_t simpleTimeoutHasRunOut(volatile uint32_t *simpleTimeout_instance, uint32_t milliseconds);

#endif