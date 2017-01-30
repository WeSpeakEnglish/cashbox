
#ifndef TIMEOUT_H
#define TIMEOUT_H

#include "timer16.h"
#include <stdint.h>

#define MINUTES_TIMEOUT_DEFAULT 1

#define MINUTES_UNTIL_UPDATE    10
#define MINUTES_TO_INSERT_FUNDS	2
#define MINUTES_TO_PRESS_START	5

typedef volatile struct {
	uint8_t seconds;
	uint16_t minutes;
	// uint8_t hours; // no need for hours now
} Time_t;

#ifdef __cplusplus
extern "C" {
#endif

void TIMEOUT_Init (void);
void TIMEOUT_Stop (void);
void TIMEOUT_Start (uint16_t minutes, uint8_t seconds);
uint8_t TIMEOUT_HasRanOut (void);
Time_t * TIMEOUT_GetTime(void);

void TIMEOUT_Update(void);

#ifdef __cplusplus
}
#endif

#endif /* TIMEOUT_H */