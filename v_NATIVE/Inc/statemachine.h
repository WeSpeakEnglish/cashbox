#ifndef _STATEMACHINE_H
#define _STATEMACHINE_H
#include <stdint.h>
#include "VendSession.h"


int8_t switch_state(SessionState_t to);
void enter_substate(SessionSubState_t to);
int8_t on_enter_state(SessionState_t to);
int8_t on_exit_state(void);
void waitForSession(void);
void waitForSelection(void);
void waitForFunds(void);
void checkIfWashingWasStarted(void);
void doAlmostNothing(SessionState_t after);
void waitForPassword(void);
void setNewPassword(void);
void serviceMenu(void);
void serviceMenu2(void);
char readKey(void);
void loop(void);

#define IS_WASHER_VALID(washer_num) \
  ( ( washer_num >= 1 ) && \
    ( washer_num <= WASHERS_MAX_COUNT ) && \
    ( WASHER_RAMGetPrice( washer_num ) > 0 ) )

#endif