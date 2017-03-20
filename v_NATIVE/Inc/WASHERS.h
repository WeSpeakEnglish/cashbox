#ifndef WASHERS_H
#define WASHERS_H

#include "VendSession.h"



#define DISABLE                 0
#define ENABLE                  1


typedef struct {
	uint16_t price;         // prices must be in eeprom
	uint8_t  enable;                // we can enable and disable the washer from the server
	uint8_t  send_signal_relay;     // to select the rgime of washing
        uint8_t  start;                 //this gone from server
} WasherSettings_t;

#define WASHERS_SBsInit()	CLR_REG(WASHER_SB_DDRReg);\
							SET_REG(WASHER_SB_PORTReg, 0xFF)

#define WASHER_SB_Read(b)	(CHK_BIT(WASHER_SB_PINReg, b) == 0)

#ifdef __cplusplus
extern "C" {
#endif

void WASHERS_Init (void);

WasherSettings_t * WASHERS_RAMGetAll   (void);

void WASHERS_EEMEMUpdateAll (void);

void     WASHER_RAMSetPrice (uint8_t washer_number, uint16_t new_price);

uint8_t WASHER_ReadFeedback (uint8_t washer_number);
void    WASHER_SendStartSignal (uint8_t washer_number, uint8_t SetCoil);

uint8_t WASHER_IsPulseRegistered( uint8_t washer_number );
void 	WASHER_ClearPulseRegistered( uint8_t washer_number );

#ifdef __cplusplus
}
#endif


#endif /* WASHER_H */
