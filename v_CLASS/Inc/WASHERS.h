#ifndef WASHERS_H
#define WASHERS_H

#ifndef F_CPU
#define F_CPU 72000000
#endif

#include "relays.h"
#include "VendSession.h"
// addresses 0x00 -- 0x15 occupied by VendSession and Cashbox
#define EEPROM_WASHERS_OFFSET       (0x00 + EEPROM_VendSessionClientsCountADDR + 0x01)
#define EEPROM_WashersPricesADDR	(0x00 + EEPROM_WASHERS_OFFSET)

#define WASHERS_MAX_COUNT	8

#define DISABLE   0
#define ENABLE    1

// 4 bytes x WASHERS_MAX_COUNT
// price is only 2 bytes, so 2 x WASHERS_MAX_COUNT in EEPROM
// 16 bytes with 8 washers
typedef struct {
	volatile uint16_t price; // prices must be in eeprom
	uint8_t  start_button_pin;
	uint8_t  send_signal_relay;
} WasherSettings_t;

#define WASHER_SB_DDRReg	DDRK
#define WASHER_SB_PORTReg	PORTK
#define WASHER_SB_PINReg	PINK
#define WASHER_SB1			PK0
#define WASHER_SB2			PK1
#define WASHER_SB3			PK2
#define WASHER_SB4			PK3
#define WASHER_SB5			PK4
#define WASHER_SB6			PK5
#define WASHER_SB7			PK6
#define WASHER_SB8			PK7

/*
	Pins are input, pull-up resistor enabled
 	Pin Change Interrupts to monitor the washers states
 */
#define WASHERS_SBsInit()	CLR_REG(WASHER_SB_DDRReg);\
							SET_REG(WASHER_SB_PORTReg, 0xFF);\
                            SET_BIT(PCICR,  PCIE2);\
                            SET_REG(PCMSK2, PCINT16)

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

uint8_t WASHER_ReadStartButton (uint8_t washer_number);
void    WASHER_SendStartSignal (uint8_t washer_number);

uint8_t WASHER_IsPulseRegistered( uint8_t washer_number );
void 	WASHER_ClearPulseRegistered( uint8_t washer_number );

#ifdef __cplusplus
}
#endif


#endif /* WASHER_H */
