#include "vend.h"
#include "cctalk.h"

#include "VendSession.h"
#include "core.h"
#include "modbus.h"
//#include "ccl"

VendSession_t Vend;
WasherSettings_t WL[MAX_WASHINGS]; 


uint16_t terminal_UID = 0; //initialValue, actual value will be get fromSD


VendSession_t Session;
uint16_t UserCounter[WASHERS_MAX_COUNT]={0,0,0,0,0,0,0,0};
uint32_t CashBOX = 0;
uint8_t Password[VendSession_PwdSize]={'4','7','9','1','5','7'};
uint16_t TotalClientsCounter = 0; // the total number of clients

VendSession_t* p_session = &Session;

uint16_t SD_Prices_WM[WASHERS_MAX_COUNT]={{0}};  //we store here our SD data prices as weel for triggering on the changes

uint8_t CheckPriceChanges(void){
 uint8_t i;
 for(i = 0; i < WASHERS_MAX_COUNT; i++){
  if (WL[i].price != SD_Prices_WM[i]) return 1; // Yeah! We need to write updated data 
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

//zero is ready 
uint8_t CheckReadyWasher(uint8_t Washer){ // is it ready or not (by modbus register) // from 0ne to eight
  union{
    struct{
      uint8_t b0 : 1;
      uint8_t b1 : 1;
      uint8_t b2 : 1;
      uint8_t b3 : 1;
      uint8_t b4 : 1;
      uint8_t b5 : 1;
      uint8_t b6 : 1;
      uint8_t b7 : 1;      
    }bits;
    uint8_t byte;
  }Washers;
  
  Washers.byte = (uint8_t)(regs[1] & 0x000000FF);
  switch(Washer){
        case 1:
          return (uint8_t)Washers.bits.b0;
        case 2:
          return (uint8_t)Washers.bits.b1;
        case 3:
          return (uint8_t)Washers.bits.b2;
        case 4:
          return (uint8_t)Washers.bits.b3;
        case 5:
          return (uint8_t)Washers.bits.b4;
        case 6:
          return (uint8_t)Washers.bits.b5;
        case 7:
          return (uint8_t)Washers.bits.b6;
        case 8:
          return (uint8_t)Washers.bits.b7;          
          
  }
  return 0;
}