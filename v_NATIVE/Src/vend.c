#include "vend.h"
#include "cctalk.h"
#include "cmsis_os.h"
#include "VendSession.h"
//#include "ccl"

VendSession_t Vend;

void enableCashInput(void){
    ccTalkSendCMD(CC_OPEN);
    vTaskDelay(300);
    ccTalkSendCMD(CC_MASTER_READY);
    vTaskDelay(300);
    ccTalkSendCMD(CC_DISABLEESCROW);
    vTaskDelay(300);
    ccTalk.gotMoney = 0;
}

void disableCashInput(void){
    ccTalkSendCMD(CC_CLOSE);
    vTaskDelay(300);
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