#include "vend.h"
#include "cctalk.h"

#include "VendSession.h"
#include "core.h"
//#include "ccl"

VendSession_t Vend;
WasherSettings_t WL[MAX_WASHINGS]; 


uint16_t terminal_UID = 0; //initialValue, actual value will be get fromSD


VendSession_t Session;
uint16_t UserCounter[WASHERS_MAX_COUNT]={10,11,12,13,14,15,16,17};
uint32_t CashBOX;
uint8_t Password[PASSWORD_LENGTH]={'1','2','3','4','5','6'};

VendSession_t* p_session = &Session;


void enableCashInput(void){
    ccTalkSendCMD(CC_OPEN);
    Delay_ms_OnFastQ(300);
    ccTalkSendCMD(CC_MASTER_READY);
    Delay_ms_OnFastQ(300);
    ccTalkSendCMD(CC_DISABLEESCROW);
    Delay_ms_OnFastQ(300);
    ccTalk.gotMoney = 0;
}


void disableCashInput(void){
    ccTalkSendCMD(CC_CLOSE);
    Delay_ms_OnFastQ(300);
}
//
	uint8_t selected_washer;
	SessionState_t current_state;
	SessionSubState_t current_substate;
	uint32_t tmp_substate_timeout;
	uint16_t inserted_funds;
	uint8_t washers_in_use[MAX_WASHINGS];
        
void VendInit(void){ // simple initialization
 uint16_t i;
  Vend.selected_washer = 1;
  Vend.current_state = WAIT_FOR_START;
  Vend.current_substate = NO_SUBSTATE;
  Vend.inserted_funds = 0;
  Vend.selected_washer = 1;
  Vend.tmp_substate_timeout = 0;
  for(i = 0; i < MAX_WASHINGS; i++)
                Vend.washers_in_use[i] = 0;
  
}