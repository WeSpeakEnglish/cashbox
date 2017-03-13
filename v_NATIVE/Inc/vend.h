#ifndef _VEND_H
#define _VEND_H

#include <stdint.h>
#include "VendSession.h"
#include "WASHERS.H"


// Values of a state machine

#define PASSWORD_LENGTH 6

extern WasherSettings_t WL[MAX_WASHINGS]; // that is an array of structs
extern VendSession_t Vend;

extern VendSession_t* p_session;
extern uint16_t terminal_UID;

extern VendSession_t Session;
extern uint16_t UserCounter[WASHERS_MAX_COUNT];
extern uint32_t CashBOX;
extern uint8_t Password[PASSWORD_LENGTH];

void enableCashInput(void);
void disableCashInput(void);
void VendInit(void);
//void MoneyParser(uint8_t * Buffer); // parse amount of the money
#endif