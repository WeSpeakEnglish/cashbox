#ifndef _VEND_H
#define _VEND_H

#include <stdint.h>
#include "VendSession.h"
#include "WASHERS.H"


// Values of a state machine


extern WasherSettings_t WL[MAX_WASHINGS]; // that is an array of structs
extern VendSession_t Vend;
extern VendSession_t Session;
extern VendSession_t* p_session;

void enableCashInput(void);
void disableCashInput(void);
void VendInit(void);
//void MoneyParser(uint8_t * Buffer); // parse amount of the money
#endif