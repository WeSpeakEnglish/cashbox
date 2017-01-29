
#ifndef VENDSESSION_H
#define VENDSESSION_H
#include <stdint.h>
#include <string.h>

#include "bitOperations.h"
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
#define EEPROM_VENDSESSION_OFFSET		   (0x00)
#define EEPROM_VendSessionStructADDR	   (0x00 + EEPROM_VENDSESSION_OFFSET)
#define EEPROM_VendSessionCashboxADDR      (0x0D + EEPROM_VENDSESSION_OFFSET)
#define VendSession_PwdSize	               6	   // up to 20 symbols max (because of EEPROM mapping)
#define VendSession_EEMEMPasswordADDR	   (0x0F + EEPROM_VENDSESSION_OFFSET)
#define EEPROM_VendSessionClientsCountADDR (0x15 + EEPROM_VENDSESSION_OFFSET + VendSession_PwdSize) // offset includes the password


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
void VendSession_RAMResetSession (void);
//
void VendSession_EEMEMUpdateAll (void);
void VendSession_EEMEMResetSession (void);
void VendSession_EEMEMResetCashbox (void);
void VendSession_EEMEMResetClientsCount (void);

char * VendSession_EEMEMGetPwd (void);
void   VendSession_EEMEMUpdPwd (char *pwd_new_str);
char * VendSession_GetTypedPwd (void);
void   VendSession_UpdTypedPwd (char *pwd_type_buf);

#ifdef __cplusplus
}
#endif

#endif /* VENDSESSION_H */
