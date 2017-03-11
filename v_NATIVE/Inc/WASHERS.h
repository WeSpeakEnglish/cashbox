#ifndef WASHERS_H
#define WASHERS_H

#include "VendSession.h"

#define WASHERS_MAX_COUNT	8

#define DISABLE                 0
#define ENABLE                  1

// 4 bytes x WASHERS_MAX_COUNT
// price is only 2 bytes, so 2 x WASHERS_MAX_COUNT in EEPROM
// 16 bytes with 8 washers
typedef struct {
	uint16_t price; // prices must be in eeprom
	uint8_t  start_button_pin;
	uint8_t  send_signal_relay;
        uint8_t  start;            //this gone from server
} WasherSettings_t;


/*
	Pins are input, pull-up resistor enabled
 	Pin Change Interrupts to monitor the washers states
 */

//@@@ unused
// #define WASHERS_SBsInit()	CLR_REG(WASHER_SB_DDRReg);\
// 							SET_REG(WASHER_SB_PORTReg, 0xFF);\
//                             SET_BIT(PCICR,  PCIE2);\
//                             SET_REG(PCMSK2, PCINT16)

#define WASHERS_SBsInit()	CLR_REG(WASHER_SB_DDRReg);\
							SET_REG(WASHER_SB_PORTReg, 0xFF)

	                        // SET_BIT(PCIFR, PCIF2);\
// check button state -- return 1 if signal is LOW
#define WASHER_SB_Read(b)	(CHK_BIT(WASHER_SB_PINReg, b) == 0)

#ifdef __cplusplus
extern "C" {
#endif

void WASHERS_Init (void);

WasherSettings_t * WASHERS_RAMGetAll   (void);
void WASHERS_EEMEMResetAll  (void);
void WASHERS_EEMEMUpdateAll (void);

uint16_t WASHER_RAMGetPrice (uint8_t washer_number);
void     WASHER_RAMSetPrice (uint8_t washer_number, uint16_t new_price);

uint8_t WASHER_ReadFeedback (uint8_t washer_number);
void    WASHER_SendStartSignal (uint8_t washer_number);

uint8_t WASHER_IsPulseRegistered( uint8_t washer_number );
void 	WASHER_ClearPulseRegistered( uint8_t washer_number );

#ifdef __cplusplus
}
#endif


#endif /* WASHER_H */
