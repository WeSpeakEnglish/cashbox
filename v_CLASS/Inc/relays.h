/*
 * RELAYS HAL
 */

#ifndef RELAYS_H
#define RELAYS_H

// #include <Arduino.h>
#include <stdint.h>

#include "bitOperations.h"

#define ON    1
#define OFF   0
// relay port settings
#define RELAYS_DDR   DDRF
#define RELAYS_PORT  PORTF
// uint8_t arguments, 8 relays max for 8-bit AVR
#define RELAY_1 PF0
#define RELAY_2 PF1
#define RELAY_3 PF2
#define RELAY_4 PF3
#define RELAY_5 PF4
#define RELAY_6 PF5
#define RELAY_7 PF6
#define RELAY_8 PF7

#define RELAY_On(r_n)			CLR_BIT(RELAYS_PORT, r_n)
#define RELAY_Off(r_n)			SET_BIT(RELAYS_PORT, r_n)

#define RELAYS_InitPorts()		SET_BIT(RELAYS_DDR, RELAY_1);\
 								SET_BIT(RELAYS_DDR, RELAY_2);\
 								SET_BIT(RELAYS_DDR, RELAY_3);\
 								SET_BIT(RELAYS_DDR, RELAY_4);\
 								SET_BIT(RELAYS_DDR, RELAY_5);\
 								SET_BIT(RELAYS_DDR, RELAY_6);\
 								SET_BIT(RELAYS_DDR, RELAY_7);\
 								SET_BIT(RELAYS_DDR, RELAY_8);\
 								RELAY_Off(RELAY_1);\
 								RELAY_Off(RELAY_2);\
 								RELAY_Off(RELAY_3);\
 								RELAY_Off(RELAY_4);\
 								RELAY_Off(RELAY_5);\
 								RELAY_Off(RELAY_6);\
 								RELAY_Off(RELAY_7);\
 								RELAY_Off(RELAY_8)

#ifdef __cplusplus
extern "C" {
#endif

void RELAYS_Init(void);
void RELAYS_Turn(uint8_t relay_num, uint8_t state);

#ifdef __cplusplus
}
#endif

#endif /* RELAYS_H */