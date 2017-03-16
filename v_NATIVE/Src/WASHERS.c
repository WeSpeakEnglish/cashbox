
#include "WASHERS.h"
#include "vend.h"
#include "core.h"
#include "modbus.h"

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
    // update eeprom if some default prices loaded
  
}

WasherSettings_t * WASHERS_RAMGetAll (void)
{
	// RAM copy of EEPROM settings values (prices only, hidden from global scope)
	static WasherSettings_t washers_list[WASHERS_MAX_COUNT] = { {0} };
	return washers_list;
}

WasherSettings_t * WASHERS_EEMEMGetAll (void)
{

	WasherSettings_t *washers_list;
	washers_list = WASHERS_RAMGetAll();

	return washers_list;
}



/*
 * Returned value:
 *      corresponding price, from 0 to MAX_INT16_T
 */
uint16_t WASHER_RAMGetPrice (uint8_t washer_number)
{
	return WL[washer_number-1].price;
}
/*
 * Set the price of the selected washer
 */
void WASHER_RAMSetPrice (uint8_t washer_number, uint16_t new_price)
{
	WasherSettings_t *washers_list;
	washers_list = WASHERS_RAMGetAll();
	if ( (washer_number < 1) || (washer_number > WASHERS_MAX_COUNT) )
		return;
	washers_list[washer_number - 1].price = new_price;
}

uint8_t WASHER_ReadFeedback(uint8_t washer_number)
{
  
//	WasherSettings_t *washers_list;
	//washers_list = WASHERS_RAMGetAll();
	//uint8_t fb;

	//fb = WASHER_SB_Read( washers_list[washer_number - 1].start_button_pin );

	// @@@
    // #ifdef DEBUG_ENABLED
    //     DEBUG.print( "Feedback read from washer " );
    //     DEBUG.print( washer_number );
    //     DEBUG.print( ": " );
    //     DEBUG.println( fb );
    // #endif

//	return fb;

  if(CheckReadyWasher(washer_number))return 0;
  else return 1;

}
/*
 * Send Start Signal to washer
 */
void WASHER_SendStartSignal (uint8_t washer_number)
{
//	WasherSettings_t *washers_list;
	//washers_list = WASHERS_RAMGetAll();
	//RELAYS_Turn( washers_list[washer_number - 1].send_signal_relay, ON );
	//_delay_ms(1000);
	//RELAYS_Turn( washers_list[washer_number - 1].send_signal_relay, OFF);
   SetCoil = washer_number;
  
    #ifdef DEBUG_ENABLED
        DEBUG.print( "Signal sent to washer " );
        DEBUG.println( washer_number );
    #endif

}

/* @@@ unused
void WASHERS_UpdatePrices (uint16_t *price_arr)
{
	return;
}

uint8_t WASHERS_CheckOccupancy( uint8_t washer_number )
{
	WASHERS_RAMGetAll();
	return 1;
}

uint8_t WASHER_IsPulseRegistered( uint8_t washer_number )
{
	// return WASHER_ReadFeedback( washer_number );
	return read_front;
}

void WASHER_ClearPulseRegistered( uint8_t washer_number )
{
	read_front = 0;
}

ISR( PCINT2_vect )
{
	read_front = 1;
}
*/
