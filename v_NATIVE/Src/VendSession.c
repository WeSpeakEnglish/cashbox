
#include "VendSession.h"


// EEPROM reading functions are local, used once in Init
static VendSession_t * VendSession_EEMEMGetSession      (void);
static volatile uint16_t      * VendSession_EEMEMGetCashbox      (void);
static volatile uint16_t      * VendSession_EEMEMGetClientsCount (void);

/*
 * Initiate VendSession
 * Loads last saved parameters from EEPROM to RAM
 */

void VendSession_RAMResetSession(void)
{
	// uint8_t selected_washer;
	// SessionState_t current_state;
	// SessionSubState_t current_substate;
	// uint32_t tmp_substate_timeout;
	// uint16_t inserted_funds;
	// uint8_t washers_in_use[8];

	VendSession_t *p_session;
	p_session = VendSession_RAMGetSession();
	p_session->selected_washer = 1;
	p_session->current_state = WAIT_FOR_START;
	p_session->current_substate = NO_SUBSTATE;
	p_session->tmp_substate_timeout = 3500;
	p_session->inserted_funds  = 0;
	// optional
	memset((void *)p_session->washers_in_use, 0, sizeof(p_session->washers_in_use));
}

void VendSession_Init(void)
{
/*
        VendSession_t *p_session;
	volatile uint16_t *p_cashbox;// = VendSession_RAMGetCashbox();
	volatile uint16_t *p_clients_count;// = VendSession_RAMGetClientsCount();
	p_session = VendSession_EEMEMGetSession();
	p_cashbox = VendSession_EEMEMGetCashbox();
	p_clients_count = VendSession_EEMEMGetClientsCount();
	VendSession_EEMEMGetPwd(); // will reset to default if not valid

	// check for valid values
	if (
		(p_session->selected_washer == 0xFF) || \
		(p_session->current_state == INVALID_STATE) || \
		(*p_cashbox == 0xFFFFFFFF) ||
		(*p_clients_count == 0xFFFF)
	)
	{
		// first time execution! - set default values
		VendSession_RAMResetSession();
	    VendSession_EEMEMResetCashbox();
	    VendSession_EEMEMResetClientsCount();
	    //@@@ HARDCODE
	    for (uint8_t i = 1; i <= 2; ++i)
	        WASHER_RAMSetPrice(i, 100);
	    for (uint8_t i = 3; i <= WASHERS_MAX_COUNT; ++i)
	        WASHER_RAMSetPrice(i, 0);
	    WASHERS_EEMEMUpdateAll();
		// save all to eeprom
		VendSession_EEMEMUpdateAll();
		// load again into RAM
		VendSession_EEMEMGetSession();
		VendSession_EEMEMGetCashbox();
		VendSession_EEMEMGetClientsCount();
	}
        */
}

/*
 * Reading and writing from/to EEPROM requiers disabled interrupts
 * so disable them (cli()) before any operation with EEPROM and
 *     enable them (sei()) after all operations are done
 */

VendSession_t * VendSession_RAMGetSession(void)
{
	static VendSession_t ram_session = {0};
	return &ram_session;
}

/*
 * Reads last saved VendSession values from EEPROM and writes them in RAM
 * returns pointer to this copy in RAM
 */
VendSession_t * VendSession_EEMEMGetSession(void)
{
	VendSession_t *p_session = VendSession_RAMGetSession();
	/*
	 * EEPROM function eeprom_read_block() expects first argument (_dst) as (void *) and discards volatile keyword
	 * This volatile struct can be suddenly updated only via interrupt service routines, and in other cases in the main state machine.
	 * As we explicitly disable global interrupts with cli(), the operation is technically safe.
	 * But to avoid warnings we can typecast the first argument -- (void *)&_dst 
	 */
//	cli();
//	eeprom_read_block((void *)p_session, (void *)EEPROM_VendSessionStructADDR, sizeof(VendSession_t));
//	sei();
	return p_session;
}

volatile uint16_t * VendSession_RAMGetCashbox(void)
{
	static volatile uint16_t cashbox = 0;
	return &cashbox;
}

void VendSession_RAMAddToCashbox(uint16_t delta)
{
	volatile uint16_t *p_cashbox = VendSession_RAMGetCashbox();
	*p_cashbox += delta;
}

void VendSession_RAMIncrementClientsCount(void)
{
	volatile uint16_t *p_clients_count = VendSession_RAMGetClientsCount();
	(*p_clients_count) += 1;
}

volatile uint16_t * VendSession_EEMEMGetCashbox(void)
{
	volatile uint16_t *p_cashbox;
	p_cashbox = VendSession_RAMGetCashbox();
	//*p_cashbox = eeprom_read_word((uint16_t *)EEPROM_VendSessionCashboxADDR);
	return p_cashbox;
}

volatile uint16_t * VendSession_RAMGetClientsCount(void)
{
	static volatile uint16_t clients_count = 0;
	return &clients_count;
}

volatile uint16_t * VendSession_EEMEMGetClientsCount(void)
{
	volatile uint16_t *p_clients_count;
	p_clients_count = VendSession_RAMGetClientsCount();
//	*p_clients_count = eeprom_read_byte((uint8_t *)EEPROM_VendSessionClientsCountADDR);
	return p_clients_count;
}

// Takes VendSession values from RAM and updates them in EEPROM
void VendSession_EEMEMUpdateAll(void)
{
	VendSession_t *p_session;
	volatile uint16_t *p_cashbox;
	volatile uint16_t *p_clients_count;
	p_session = VendSession_RAMGetSession();
	p_cashbox = VendSession_RAMGetCashbox();
	p_clients_count = VendSession_RAMGetClientsCount();
	// eeprom functions expect _src argument as (const void * (non-volatile)
	// so one way is to explicitly typecast (void *)&_src
	//cli();
	//eeprom_update_block((void *)p_session, (void *)EEPROM_VendSessionStructADDR, sizeof( VendSession_t ));
	//eeprom_update_word((uint16_t *)EEPROM_VendSessionCashboxADDR, *p_cashbox);
	//eeprom_update_byte((uint8_t *)EEPROM_VendSessionClientsCountADDR, *p_clients_count);
	//sei();
}


void VendSession_EEMEMUpdateCashbox(void)
{
	volatile uint16_t *p_cashbox;
	p_cashbox = VendSession_RAMGetCashbox();
	//cli();
	//eeprom_update_word((uint16_t *)EEPROM_VendSessionCashboxADDR, *p_cashbox);
	//sei();
}

void VendSession_EEMEMUpdateClientsCount(void)
{
	volatile uint16_t *p_clients_count;
	p_clients_count = VendSession_RAMGetClientsCount();
	//cli();
	//eeprom_update_byte((uint8_t *)EEPROM_VendSessionClientsCountADDR, *p_clients_count);
	//sei();
}

void VendSession_EEMEMResetCashbox(void)
{
	volatile uint16_t *p_cashbox;
	p_cashbox = VendSession_RAMGetCashbox();
	*p_cashbox = 0;
	//cli();
	//eeprom_update_word((uint16_t *)EEPROM_VendSessionCashboxADDR, *p_cashbox);
	//sei();
}

void VendSession_EEMEMResetClientsCount(void)
{
	volatile uint16_t *p_clients_count;
	p_clients_count = VendSession_RAMGetClientsCount();
	*p_clients_count = 0;
	//cli();
	//eeprom_update_byte((uint8_t *)EEPROM_VendSessionClientsCountADDR, *p_clients_count);
	//sei();
}


// Password input functions
// ~~~~~~~~~~~~~~~~~~~~~~~~
char * VendSession_EEMEMGetPwd(void)
{
	const char *default_pwd = "479157"; // default password
	static char password[VendSession_PwdSize + 1]; // including null terminator
	uint8_t i;

	password[VendSession_PwdSize] = '\0';

	//cli();
	//eeprom_read_block((void *)password, (void *)VendSession_EEMEMPasswordADDR, VendSession_PwdSize);
	//sei();

	// check for validity
	for (i = 0; i < VendSession_PwdSize; i++)
	{
		if ( (password[i] < '0') || (password[i] > '9') )
		{
			strncpy(password, default_pwd, VendSession_PwdSize);
			break;
		}
	}

	return password;
}
void VendSession_EEMEMUpdPwd(const char *pwd_new_str)
{
	char *curr_eemem_pwd = VendSession_EEMEMGetPwd();
    // update password if modified only
    if (memcmp(curr_eemem_pwd, pwd_new_str, VendSession_PwdSize) != 0)
    {
		//cli();
		//eeprom_update_block((void *)pwd_new_str, (void *)VendSession_EEMEMPasswordADDR, VendSession_PwdSize);
		//sei();
	}
}
char * VendSession_GetTypedPwd(void)
{
	static char pwd_buffer[VendSession_PwdSize + 1] = "      "; // +1 is for \0
	return pwd_buffer;
}
/*
void VendSession_UpdTypedPwd(char *pwd_type_buf)
{
	char *pwd_buffer;
	pwd_buffer = VendSession_GetTypedPwd();
	memcpy(pwd_buffer, pwd_type_buf, VendSession_PwdSize);
}
*/

// @@@
// BYTE volatile EEPROM_read(unsigned int uiAddress)
// {
// 	// Wait for completion of previous write
// 	while(EECR & (1<<EEPE));
// 	// Set up address register
// 	EEAR = uiAddress;
// 	// Start eeprom read by writing EERE
// 	EECR |= (1<<EERE);
// 	asm("nop");
// 	asm("nop");
// 	// Return data from Data Register
// 	return EEDR;
// }
// void EEPROM_write(unsigned int uiAddress, BYTE ucData)
// {
// 	// Wait for completion of previous write
// 	while(EECR & (1<<EEPE));
// 	// Set up address and Data Registers
// 	EEAR = uiAddress;
// 	EEDR = ucData;
// 	// Write logical one to EEMPE
// 	EECR |= (1<<EEMPE);
// 	// Start eeprom write by setting EEPE
// 	EECR |= (1<<EEPE);
// }
