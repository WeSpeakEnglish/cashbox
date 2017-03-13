#ifndef VENDSESSION_H
#define VENDSESSION_H

#include <stdint.h>
#include <string.h>
#include "simcom.h"
#include "WASHERS.h"

/* Address of saved data in EEPROM
 * Required size is: VendSession_t:
 					 1 byte  -- uint8_t  (selected washer)   start at 0x00
					 2 bytes -- sizeof   (SessionState_t)    start at 0x01
					 2 bytes -- uint16_t (inserted funds)    start at 0x03
					 8 bytes -- uint8_t  (occupancy array)   start at 0x05
					 13 bytes total                          last byte at 0x0C
					 Cashbox:
					 2 bytes -- uint16_t ()					 start at 0x0D, end at 0x0E
					 6 bytes -- password ()				     start at 0x0F, end at 0x14
					 Clients count:
					 1 byte  -- uint8_t  ()				     start at 0x15
 * Should be enough for 1-2 years, then just move the address offset 
 */
//#define EEPROM_VENDSESSION_OFFSET		   (0x00)
#define EEPROM_VendSessionStructADDR	   (0x00)
#define EEPROM_VendSessionCashboxADDR      (0x18)
#define EEPROM_VendSessionClientsCountADDR (0x1C) // offset includes the password
#define VendSession_EEMEMPasswordADDR	   (0x20)

#define WASHERS_MAX_COUNT	8


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
/*
 * To read values from EEPROM user must first read them into RAM
 * it is simply done by calling VendSession_EEMEMGetSession(), which returns
 * a pointer to a RAM copy:
 *     VendSession_t *p_session;
 *     p_session = VendSession_EEMEMGetSession();
 *     // now you can access values in RAM by p_session->...
 
 * To update values in EEPROM user must first update these
 * values in RAM copy via pointer, like so:
 *
 *     VendSession_t *p_session;
 *
 *     p_session = VendSession_RAMGetSession();
 *	   p_session->selected_washer = val1;
 *     p_session->current_state   = state1;
 *     p_session->inserted_funds  = funds1;
 *
 *     VendSession_EEMEMUpdateAll();
 */

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
volatile uint16_t      * VendSession_RAMGetCashbox      (void);
volatile uint16_t      * VendSession_RAMGetClientsCount (void);

void VendSession_RAMAddToCashbox(uint16_t delta);
void VendSession_RAMIncrementClientsCount(void);

//@@@void VendSession_RAMResetSession (void); // called from init function

void VendSession_EEMEMUpdateAll (void);
void VendSession_EEMEMUpdateSession (void);
void VendSession_EEMEMUpdateCashbox (void);
void VendSession_EEMEMUpdateClientsCount (void);
void VendSession_EEMEMResetSession (void);
void VendSession_EEMEMResetCashbox (void);
void VendSession_EEMEMResetClientsCount (void);

char * VendSession_EEMEMGetPwd (void);
void   VendSession_EEMEMUpdPwd (const char *pwd_new_str);
char * VendSession_GetTypedPwd (void);
//void   VendSession_UpdTypedPwd (char *pwd_type_buf);

#ifdef __cplusplus
}
#endif

#endif /* VENDSESSION_H */
