#include "simcom.h"
#include "parse_sim800.h"
#include <stdlib.h>
#include <string.h>
#include "core.h"

///SMS commands
const char sms_DeleteAll[] = "AT+CMGDA=\"DEL ALL\"";
const char sms_TextModeSet[] = "AT+CMGF=1";
const char sms_BalanceNmb[] ="AT+CMGS=\"000100\"";
const char sms_PUSH[2]={26,0};
const char sms_GET_str[]="AT+CMGR=";

const char GSM_ATcmd_Enable_Echo2[]="ATE1\r";   



void SIM800_get_Balance(void){
  char Str[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
  char SMS_N[2] = {0,0};
  char * pointerN;
  SIM800_AddCMD((char *)sms_TextModeSet,sizeof(sms_TextModeSet),1);
  SIM800_waitAnswer(1);
  SIM800_AddCMD((char *)sms_DeleteAll,sizeof(sms_DeleteAll),1);
  SIM800_waitAnswer(1);  
  SIM800_AddCMD((char *)sms_BalanceNmb,sizeof(sms_BalanceNmb),1);
  SIM800_waitAnswer(1); 
  SIM800_AddCMD((char *)sms_PUSH,sizeof(sms_PUSH),1);
  Sim800.flush_SMS = 0;
  Sim800.SMS_received = 0;
  SIM800_waitAnswer(2); 
 // Delay_ms_OnFastQ(10000); // wait while sms
  pointerN = strstr((char const *)Sim800.pReadyBuffer,"\"SM\",");
  
  if(pointerN != NULL){
    Sim800.flush_SMS = 1;
    Sim800.SMS_received = 0;
    SMS_N[0] = pointerN[5];
    strcpy(Str,sms_GET_str);
    strcat(Str,SMS_N);
    SIM800_AddCMD((char *)Str,sizeof(Str),0);
     SIM800_waitAnswer(1); 
     Delay_ms_OnFastQ(1000);
     SIM800_parse_Balance();
  }
     
Delay_ms_OnFastQ(1000);
  
return;
}

