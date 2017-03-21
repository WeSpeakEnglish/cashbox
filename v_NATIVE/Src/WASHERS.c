
#include "WASHERS.h"
#include "vend.h"
#include "core.h"
#include "modbus.h"
#include "statemachine.h"

volatile uint8_t read_front = 0;

static WasherSettings_t * WASHERS_EEMEMGetAll (void);
/*
 * Set washers settings
 */
void WASHERS_Init (void)
{
	uint8_t i;
	WasherSettings_t *washers_list;

	washers_list = WASHERS_EEMEMGetAll();

    // check prices validity

    for (i = 0; i < WASHERS_MAX_COUNT; i++)
    {
    	if ( (washers_list + i)->price >= 1000 ) // not valid price (3-digit prices are valid only)
    	{
    		(washers_list + i)->price = 100; // default price //@@@
    		
    	}
    }
  
}

WasherSettings_t * WASHERS_RAMGetAll (void)
{

	return washers_list;
}

WasherSettings_t * WASHERS_EEMEMGetAll (void)
{
	return washers_list;
}

void WASHER_RAMSetPrice (uint8_t washer_number, uint16_t new_price)
{
	WasherSettings_t *washers_list;
	washers_list = WASHERS_RAMGetAll();
	if ( (washer_number < 1) || (washer_number > WASHERS_MAX_COUNT) )
		return;
	washers_list[washer_number - 1].price = new_price;
}

uint8_t WASHER_ReadFeedback(uint8_t w_number)
{
  if(CheckReadyWasher(w_number))return 0;
  else return 1;

}
/*
 * Send Start Signal to washer
 */
void WASHER_SendStartSignal (uint8_t washer_number, uint8_t SetCoil)
{
   Machine.SetCoil = SetCoil;    // we need to change
   Machine.SlaveAddr = washer_number;
}
