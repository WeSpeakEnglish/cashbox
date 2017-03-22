#include "vend.h"
#include "cctalk.h"

#include "VendSession.h"
#include "core.h"
#include "modbus.h"
//#include "ccl"

//VendSession_t Vend;
WasherSettings_t WL[WASHERS_MAX_COUNT]; 
WasherSettings_t * washers_list = &WL[0]; // that is an array of structs

uint16_t terminal_UID = 0; //initialValue, actual value will be get fromSD


VendSession_t Session;
uint16_t UserCounter[WASHERS_MAX_COUNT]={0,0,0,0,0,0,0,0};
uint32_t CashBOX = 0;
uint8_t Password[VendSession_PwdSize]={'4','7','9','1','5','7'};
uint16_t TotalClientsCounter = 0; // the total number of clients

VendSession_t* p_session = &Session;

uint16_t SD_Prices_WM[WASHERS_MAX_COUNT]={{0}};  //we store here our SD data prices as weel for triggering on the changes
uint16_t SD_Enables_WM[WASHERS_MAX_COUNT]={{0}};

uint8_t selected_washer;
SessionState_t current_state;
SessionSubState_t current_substate;
uint32_t tmp_substate_timeout;
uint16_t inserted_funds;
uint8_t washers_in_use[WASHERS_MAX_COUNT];

uint8_t CheckPriceChanges(void){
 uint8_t i;
 for(i = 0; i < WASHERS_MAX_COUNT; i++){
  if (WL[i].price != SD_Prices_WM[i]) return 1; // Yeah! We need to write updated data 
  if (WL[i].enable != SD_Enables_WM[i]) return 1;
 }
return 0;
}

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

uint8_t CheckReadyWasher(uint8_t Washer){ // is it ready or not (by modbus register) // from 0ne to eight
  
  if(regs[Washer] & 0x00000001)  
    return 1;        
  return 0;
}