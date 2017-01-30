
#include "WASHERS.h"

volatile uint8_t read_front = 0;

static WasherSettings_t * WASHERS_EEMEMGetAll (void);
/*
 * Set washers settings
 */
void WASHERS_Init (void)
{
	WasherSettings_t *washers_list;
	washers_list = WASHERS_EEMEMGetAll();

//	(washers_list + 0)->start_button_pin   = WASHER_SB1;
//	(washers_list + 0)->send_signal_relay  = RELAY_1;

//	(washers_list + 1)->start_button_pin   = WASHER_SB2;
//	(washers_list + 1)->send_signal_relay  = RELAY_2;

//	(washers_list + 2)->start_button_pin   = WASHER_SB3;
//	(washers_list + 2)->send_signal_relay  = RELAY_3;

//	(washers_list + 3)->start_button_pin   = WASHER_SB4;
//	(washers_list + 3)->send_signal_relay  = RELAY_4;

//	(washers_list + 4)->start_button_pin   = WASHER_SB5;
//	(washers_list + 4)->send_signal_relay  = RELAY_5;

//	(washers_list + 5)->start_button_pin   = WASHER_SB6;
//	(washers_list + 5)->send_signal_relay  = RELAY_6;

//	(washers_list + 6)->start_button_pin   = WASHER_SB7;
//	(washers_list + 6)->send_signal_relay  = RELAY_7;

//	(washers_list + 7)->start_button_pin   = WASHER_SB8;
//	(washers_list + 7)->send_signal_relay  = RELAY_8;

//    WASHERS_SBsInit();
}

WasherSettings_t * WASHERS_RAMGetAll (void)
{
	// RAM copy of EEPROM settings values (prices only, hidden from global scope)
	static WasherSettings_t washers_list[WASHERS_MAX_COUNT] = { {0} };
	return washers_list;
}

WasherSettings_t * WASHERS_EEMEMGetAll (void)
{
	uint8_t i;
	WasherSettings_t *washers_list;
	washers_list = WASHERS_RAMGetAll();
	//cli();
//	for (i = 0; i < WASHERS_MAX_COUNT; ++i)
//		washers_list[i].price = eeprom_read_word( (uint16_t *)(EEPROM_WashersPricesADDR + i * sizeof(uint16_t)) );
//	sei();
	return washers_list;
}

void WASHERS_EEMEMResetAll  (void)
{
	uint8_t i;
	//cli();
//	for (i = 0; i < WASHERS_MAX_COUNT; ++i)
//		eeprom_update_word( (uint16_t *)(EEPROM_WashersPricesADDR + i * sizeof(uint16_t)),  100 );
//	sei();
}

void WASHERS_EEMEMUpdateAll (void)
{
	uint8_t i;
	WasherSettings_t *washers_list;
	washers_list = WASHERS_RAMGetAll();
	//cli();
//	for (i = 0; i < WASHERS_MAX_COUNT; ++i)
//		eeprom_update_word( (uint16_t *)(EEPROM_WashersPricesADDR + i * sizeof(uint16_t)), washers_list[i].price );
//	sei();
}

/*
 * Returned value:
 *      corresponding price, from 0 to MAX_INT16_T
 */
uint16_t WASHER_RAMGetPrice (uint8_t washer_number)
{
	WasherSettings_t *washers_list;
	washers_list = WASHERS_RAMGetAll();
	return washers_list[washer_number - 1].price;
}
/*
 * Set the price of the selected washer
 */
void WASHER_RAMSetPrice (uint8_t washer_number, uint16_t new_price)
{
	WasherSettings_t *washers_list;
	washers_list = WASHERS_RAMGetAll();
	if (washer_number > WASHERS_MAX_COUNT)
		return;
	washers_list[washer_number - 1].price = new_price;
}

uint8_t WASHER_ReadStartButton(uint8_t washer_number)
{
	WasherSettings_t *washers_list;
	washers_list = WASHERS_RAMGetAll();
	return 0; //WASHER_SB_Read( washers_list[washer_number - 1].start_button_pin );
 //       return 0;
}
/*
 * Send Start Signal to washer
 */
void WASHER_SendStartSignal (uint8_t washer_number)
{
	WasherSettings_t *washers_list;
	washers_list = WASHERS_RAMGetAll();
	RELAYS_Turn( washers_list[washer_number - 1].send_signal_relay, ON );
	//_delay_ms(1000);
	RELAYS_Turn( washers_list[washer_number - 1].send_signal_relay, OFF);
}

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
	// return WASHER_ReadStartButton( washer_number );
	return read_front;
}

void WASHER_ClearPulseRegistered( uint8_t washer_number )
{
	read_front = 0;
}

//ISR( PCINT2_vect )
//{
//	read_front = 1;
//}
