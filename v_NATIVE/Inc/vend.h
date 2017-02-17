#ifndef _VEND_H
#define _VEND_H

#include <stdint.h>

// Values of a state machine
typedef enum {
	WAIT_FOR_START,
	SELECT_WASHER,
	INSERT_FUNDS,
	PAYMENT_DONE,
	PROCESSING,
	ENTER_PASSWORD,
	SERVICE_MENU
} SessionState_t;

typedef volatile struct {
	uint8_t selected_washer;
	SessionState_t current_state;
	uint16_t inserted_funds;
	uint8_t washers_in_use[8];
} VendSession_t;


extern VendSession_t Vend;
void MoneyParser(uint8_t * Buffer); // parse amount of the money
#endif