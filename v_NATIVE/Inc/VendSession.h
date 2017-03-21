#ifndef VENDSESSION_H
#define VENDSESSION_H

#include <stdint.h>
#include <string.h>
#include "simcom.h"
#include "WASHERS.h"

//#define EEPROM_VENDSESSION_OFFSET		   (0x00)
#define EEPROM_VendSessionStructADDR	   (0x00)
#define EEPROM_VendSessionCashboxADDR      (0x18)
#define EEPROM_VendSessionClientsCountADDR (0x1C) // offset includes the password
#define VendSession_EEMEMPasswordADDR	   (0x20)

#define WASHERS_MAX_COUNT	9


//#define EEPROM_WASHERS_OFFSET       (0x40)
#define EEPROM_WashersPricesADDR	(0x40)

#define VendSession_PwdSize	               6	   // up to 20 symbols max (because of EEPROM mapping)

#define TMP_DUR1 200
#define TMP_DUR2 800

// Values of a state machine
typedef enum {
	INVALID_STATE = -1,
	WAIT_FOR_START,

	SELECT_WASHER,
	INSERT_FUNDS,
	START_WASHING,
	START_WASHING_FAILED,
	WASHING_STARTED_SUCCESSFULLY,

	ENTER_PASSWORD,
	SERVICE_MENU,
	SET_NEW_PASSWORD,
	SERVICE_MENU2,
	SERVICE_INFO1,

	INITIALIZATION
} SessionState_t;

typedef enum {
	INVALID_SUBSTATE = -1,
	NO_SUBSTATE,
	TMP_SUBSTATE,
	TMP_SUBSTATE2,
	PROCESSING_SUBSTATE
} SessionSubState_t;

typedef struct {
	uint8_t selected_washer;
	SessionState_t current_state;
	SessionSubState_t current_substate;
	uint32_t tmp_substate_timeout;
	uint16_t inserted_funds;
	uint8_t washers_in_use[WASHERS_MAX_COUNT];
} VendSession_t;


#ifdef __cplusplus
extern "C" {
#endif

/*
 * The trick with volatile and non-volatile VendSession_t variables
 * is done to apply the same type VendSession_t to store non-volatile values in EEPROM
 * and update these in external process (timer), which requiers volatile VendSession_t
 * So the user works directly with volatile RAM copy,
 * and direct access to non-volatile EEPROM is hidden
 */
void VendSession_Init (void);
VendSession_t * VendSession_RAMGetSession      (void);

uint16_t * VendSession_RAMGetClientsCount (void);
uint32_t * VendSession_RAMGetCashbox(void);

void VendSession_RAMAddToCashbox(uint16_t delta);
void VendSession_RAMIncrementClientsCount(uint8_t Machine);

void VendSession_EEMEMResetSession (void);
void VendSession_EEMEMResetClientsCount (void);

char * VendSession_EEMEMGetPwd (void);
void   VendSession_EEMEMUpdPwd (const char *pwd_new_str);
char * VendSession_GetTypedPwd (void);
//void   VendSession_UpdTypedPwd (char *pwd_type_buf);

#ifdef __cplusplus
}
#endif

#endif /* VENDSESSION_H */
