
#include "VendSession.h"
#include "vend.h"
#include "sd_files.h"




uint16_t      * VendSession_GetClientsCount (void);

/*
 * Initiate VendSession
 * Loads last saved parameters from EEPROM to RAM
 */

void VendSession_RAMResetSession(void)
{
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

}


VendSession_t * VendSession_RAMGetSession(void)
{

	return p_session;
}

void VendSession_RAMAddToCashbox(uint16_t delta)
{
	CashBOX += delta;
}

void VendSession_RAMIncrementClientsCount(uint8_t Machine)
{
	UserCounter[Machine-1] += 1;
}


uint16_t * VendSession_RAMGetClientsCount(void)
{
	uint8_t i; // calculate all the clients
        TotalClientsCounter = 0;
        for(i = 0; i < WASHERS_MAX_COUNT; i++)
          TotalClientsCounter += UserCounter[i];
        
	return &TotalClientsCounter;
}

uint16_t * VendSession_GetClientsCount(void)
{
	uint8_t i; // calculate all the clients
        TotalClientsCounter = 0;
        for(i = 0; i < WASHERS_MAX_COUNT; i++)
          TotalClientsCounter += UserCounter[i];
        
	return &TotalClientsCounter;
}

void VendSession_ResetClientsCount(void)
{
uint8_t i;
  for(i = 0; i < WASHERS_MAX_COUNT; i++)
    UserCounter[i] = 0;
  
  SD_SetSession();

}


// Password input functions
// ~~~~~~~~~~~~~~~~~~~~~~~~
char * VendSession_GetPwd(void)
{

	static char password[VendSession_PwdSize + 1]; // including null terminator

	password[VendSession_PwdSize] = '\0';

	strncpy(password, (char const *)Password, VendSession_PwdSize);


	return password;
}
void VendSession_UpdPwd(const char *pwd_new_str)
{
  uint8_t i;
  for(i=0; i < VendSession_PwdSize; i++)
                  Password[i] = pwd_new_str[i];
  
  SD_SetSession();
  
}
char * VendSession_GetTypedPwd(void)
{
	static char pwd_buffer[VendSession_PwdSize + 1] = "      "; // +1 is for \0
	return pwd_buffer;
}


