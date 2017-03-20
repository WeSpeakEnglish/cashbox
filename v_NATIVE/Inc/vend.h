#ifndef _VEND_H
#define _VEND_H

#include <stdint.h>
#include "VendSession.h"
#include "WASHERS.H"

extern WasherSettings_t WL[MAX_WASHINGS]; // that is an array of structs
extern WasherSettings_t * washers_list;
//extern VendSession_t Vend;

extern VendSession_t* p_session;
extern uint16_t terminal_UID;

extern VendSession_t Session;
extern uint16_t UserCounter[WASHERS_MAX_COUNT];
extern uint32_t CashBOX;
extern uint8_t Password[VendSession_PwdSize];
extern uint16_t SD_Prices_WM[WASHERS_MAX_COUNT];
extern uint16_t TotalClientsCounter;

uint8_t CheckReadyWasher(uint8_t Washer);
uint8_t CheckPriceChanges(void);
void enableCashInput(void);
void disableCashInput(void);
//void VendInit(void);
//void MoneyParser(uint8_t * Buffer); // parse amount of the money
#endif