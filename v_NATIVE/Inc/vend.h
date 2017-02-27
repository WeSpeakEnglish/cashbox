#ifndef _VEND_H
#define _VEND_H

#include <stdint.h>
#include "VendSession.h"


// Values of a state machine

extern VendSession_t Vend;

void enableCashInput(void);
void disableCashInput(void);
void VendInit(void);
//void MoneyParser(uint8_t * Buffer); // parse amount of the money
#endif