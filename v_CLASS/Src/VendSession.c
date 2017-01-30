
#include "VendSession.h"
#include "WASHERS.h"

// EEPROM reading functions are local, used once in Init
static VendSession_t * VendSession_EEMEMGetSession      (void);
static volatile uint16_t      * VendSession_EEMEMGetCashbox      (void);
static volatile uint16_t      * VendSession_EEMEMGetClientsCount (void);

/*
 * Initiate VendSession
 * Loads last saved parameters from EEPROM to RAM
 */
void VendSession_Init(void)
{
	VendSession_RAMResetSession();
	// VendSession_EEMEMGetSession();
	// VendSession_EEMEMGetCashbox();
	// VendSession_EEMEMGetClientsCount();
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
	VendSession_t *p_ram_session;
	p_ram_session = VendSession_RAMGetSession();
	/*
	 * EEPROM function eeprom_read_block() expects first argument (_dst) as (void *) and discards volatile keyword
	 * This volatile struct can be suddenly updated only via interrupt service routines, and in other cases in the main state machine.
	 * As we explicitly disable global interrupts with cli(), the operation is technically safe.
	 * But to avoid warnings we can typecast the first argument -- (void *)&_dst 
	 */
//	cli();
//	eeprom_read_block((void *)p_ram_session, (void *)EEPROM_VendSessionStructADDR, sizeof(VendSession_t));
//	sei();
	return p_ram_session;
}

volatile uint16_t * VendSession_RAMGetCashbox(void)
{
	static volatile uint16_t cashbox = 0;
	return &cashbox;
}

volatile uint16_t * VendSession_EEMEMGetCashbox(void)
{
	volatile uint16_t *p_cashbox;
	p_cashbox = VendSession_RAMGetCashbox();
//	*p_cashbox = eeprom_read_word((uint16_t *)EEPROM_VendSessionCashboxADDR);
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

/*
 * Takes VendSession values from RAM and updates them in EEPROM
 */
void VendSession_EEMEMUpdateAll(void)
{
	VendSession_t *p_ram_session;
	volatile uint16_t *p_cashbox;
	volatile uint16_t *p_clients_count;
	p_ram_session = VendSession_RAMGetSession();
	p_cashbox = VendSession_RAMGetCashbox();
	p_clients_count = VendSession_RAMGetClientsCount();
	// eeprom functions expect _src argument as (const void * (non-volatile)
	// so one way is to explicitly typecast (void *)&_src
	//cli();
	//eeprom_update_block((void *)p_ram_session, (void *)EEPROM_VendSessionStructADDR, sizeof( VendSession_t ));
	//eeprom_update_word((uint16_t *)EEPROM_VendSessionCashboxADDR, *p_cashbox);
	//eeprom_update_byte((uint8_t *)EEPROM_VendSessionClientsCountADDR, *p_clients_count);
	//sei();
}

void VendSession_RAMResetSession(void)
{
	VendSession_t *p_ram_session;
	p_ram_session = VendSession_RAMGetSession();
	p_ram_session->selected_washer = 1;
	p_ram_session->current_state   = WAIT_FOR_START;
	p_ram_session->inserted_funds  = 0;
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
//	cli();
//	eeprom_update_byte((uint8_t *)EEPROM_VendSessionClientsCountADDR, *p_clients_count);
//	sei();
}

/*
 * Resets values in RAM and in EEPROM to default
 */
void VendSession_EEMEMResetSession(void)
{
	VendSession_t *p_ram_session;
	p_ram_session = VendSession_RAMGetSession();
	// eeprom functions expect _src argument as const void * (non-volatile)
	// so one way is to explicitly typecast (void *)&_src
	VendSession_RAMResetSession();
//	cli();
	//eeprom_update_block((void *)p_ram_session, (void *)EEPROM_VendSessionStructADDR, sizeof(VendSession_t));
	//sei();
}
/*
 * Password input functions
 */
char * VendSession_EEMEMGetPwd(void)
{
	static char password[VendSession_PwdSize + 1]; // including null terminator

	password[VendSession_PwdSize] = '\0';
	//cli();
	//eeprom_read_block((void *)password, (void *)VendSession_EEMEMPasswordADDR, VendSession_PwdSize);
	//sei();
	return password;
}

void VendSession_EEMEMUpdPwd(char *pwd_new_str)
{
	//cli();
	////eeprom_update_block((void *)pwd_new_str, (void *)VendSession_EEMEMPasswordADDR, VendSession_PwdSize);
	//sei();
}

char * VendSession_GetTypedPwd(void)
{
	static char pwd_buffer[VendSession_PwdSize + 1] = "      "; // +1 is for \0
	return pwd_buffer;
}

void VendSession_UpdTypedPwd(char *pwd_type_buf)
{
	char *pwd_buffer;
	pwd_buffer = VendSession_GetTypedPwd();
	memcpy(pwd_buffer, pwd_type_buf, VendSession_PwdSize);
}

