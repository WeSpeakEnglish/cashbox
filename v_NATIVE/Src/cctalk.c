#include "stm32f303xc.h"
#include "stm32f3xx_hal.h"
#include "cctalk.h"
#include <stdint.h>
#include "usart.h"
#include "cmsis_os.h"
#include "lcd.h"
#include <stdio.h>
#include <string.h>
#include <vend.h>
#include "calculations.h"

// several commands what we need for
const uint8_t ccOpen[]                          = {0,2,1,231,255,255,24};
const uint8_t ccMasterReady[]                   = {0,1,1,228,255,27};
const uint8_t DisableEscrowMode[]               = {0,1,1,153,0,101};//
const uint8_t ccRequest[]                       = {0,0,1,29,226};
const uint8_t ccReadBufferedBill[]              = {0,0,1,159,96};
const uint8_t ccRequestAccepterCounter[]        = {0,0,1,225,30};
//const uint8_t ccRequestIndAccepterCounter[]     = {0,1,1,150,1,104};
//const uint8_t ccRequestIndAccepterCounter[]     = {0,0,1,236,19};
const uint8_t SimplePool[]                      = {2,0,1,254,255};





const uint8_t ccClose[]        = {0,1,1,231,0,0};
CC_struct ccTalk;

/*

For example, the ccTalk command to enable all coin inhibits is…
[ 2 ] [ 2 ] [ 1 ] [ 231 ] [ 255 ] [ 255 ] [ checksum ]
2 + 2 + 1 + 231 + 255 + 255 = 746 = 234 modulo 256 ( = 256 x 2 + 234 ).
Checksum = 256 - 234 = 22
Therefore the complete message is
[ 2 ] [ 2 ] [ 1 ] [ 231 ] [ 255 ] [ 255 ] [ 22 ] 


*/
uint8_t ccTalkChecksum(uint8_t * pBuff, uint8_t size){
 uint16_t Sum = 0;
 uint8_t i;
 
 for(i = 0; i < size; i++){
   Sum += *pBuff;
   pBuff++;
 }
 Sum %= 256;
 Sum = 256 - Sum;
 return (uint8_t) Sum;
}

uint8_t ccTalkParseOK(void){ //if command receive
  while(!ccTalk.readyBuff) {
  vTaskDelay(10);
  taskYIELD();
  }
  if(ccTalk.buffer[3] == 0x00)
    return 1; // one is OK
  else 
    return 0;


}




uint8_t byteDistance(uint8_t First, uint8_t Second){ // overflow byte calculate distance
 uint8_t retVal;
 //char Str[80];
 //char StrShot[5];
 
  if(Second < First){
     retVal = 0xFF - First;
     retVal += Second + 1;  
  }
  else 
    retVal = Second - First;
  
 // lcd_clear();
  //Utoa(retVal, Str);

  
   return retVal;
}

void ccTalkParseAccCount(void){ // we need to register change of this value
 static int16_t OldValue = -1; //ini state
 uint8_t i;
 
   while(!ccTalk.readyBuff) {
  vTaskDelay(10);
  taskYIELD();
  }

 for(i=0; i<ccTalk.readySymbols; i++){
  if(ccTalk.buffer[i] == 1)
   if(ccTalk.buffer[i+1] == 3)
    if(ccTalk.buffer[i+2] == 2){
       if(OldValue != ccTalk.buffer[i+4]){
         if(OldValue == -1){
           OldValue = (int16_t)ccTalk.buffer[i+4]; //initial value
           return;
         }
         else{
         ccTalk.gotMoney = 1;
         OldValue = (int16_t)ccTalk.buffer[i+4];
         }
           
       }
    }
 }
}

#define PROCESSING_DELAY 2
#define RUR_10          1
#define RUR_50          2
#define RUR_100         3
#define RUR_500         4
#define RUR_1000        5
#define RUR_5000        6

void ccTalkParseStatus(void){
 char Str[80];
 char StrShot[5];
 static int16_t OldCount =  -1;       // look on count if it goes to change...
 static uint8_t counter =   0;            // counter to processing
 static uint8_t OldBanknote = 0; //if we receive something new
 uint8_t i;


 memset(Str,0,sizeof(Str));

  while(!ccTalk.readyBuff) {
  vTaskDelay(10);
  taskYIELD();
  }

for(i=0; i<ccTalk.readySymbols; i++){
  if(ccTalk.buffer[i] == 1)
    if(ccTalk.buffer[i+1] == 11){
      if((ccTalk.buffer[i+6] == 0)&&(ccTalk.buffer[i+5]  > 0)&&(ccTalk.buffer[i+5]  < (RUR_5000 +1))){
    counter++;
    if(counter == PROCESSING_DELAY){
        if(OldCount != ccTalk.buffer[i+4] ){
          OldCount = ccTalk.buffer[i+4];

         switch(ccTalk.buffer[i+5] ){
          case RUR_10:
            Vend.inserted_funds += 10;  
            break;
          case RUR_50:
            Vend.inserted_funds += 50;  
            break;
          case RUR_100:
            Vend.inserted_funds += 100;  
            break;
          case RUR_500:
            Vend.inserted_funds += 500;  
            break; 
          case RUR_1000:
            Vend.inserted_funds += 1000;  
            break;
          case RUR_5000:
            Vend.inserted_funds += 5000;  
            break; 
         }
         ccTalk.gotMoney = 0;
        }
        counter = 0;
    }
   }
  }
}
  //i = ccTalkChecksum((uint8_t *)ccRequestIndAccepterCounter, sizeof(ccRequestIndAccepterCounter-1));
  lcd_clear();
  sprintf(Str,"%d",Vend.inserted_funds);

  lcd_puts((char const *)Str);
 return;
}

void ccTalkSendCMD(uint8_t Des){
volatile uint32_t tmpval;
  ccTalk.readyBuff = 0;
 // ccTalk.IndWR = 0;  
  __HAL_UART_DISABLE_IT(&huart5, UART_IT_RXNE);
  __HAL_UART_DISABLE_IT(&huart5, UART_IT_IDLE);
 
  
  switch(Des){    
    case CC_OPEN:
      HAL_UART_Transmit(&huart5,(uint8_t *)ccOpen, sizeof(ccOpen),10);
                  break;
    case CC_MASTER_READY:
      HAL_UART_Transmit(&huart5,(uint8_t *)ccMasterReady, sizeof(ccMasterReady),10);
                  break;   
    case CC_REQUEST:
      HAL_UART_Transmit(&huart5,(uint8_t *)ccRequest, sizeof(ccRequest),10);
                  break;  
    case CC_CLOSE:
      HAL_UART_Transmit(&huart5,(uint8_t *)ccClose, sizeof(ccClose),10);      
                  break;
    case CC_SIMPLEPOOL: 
      HAL_UART_Transmit(&huart5,(uint8_t *)SimplePool, sizeof(SimplePool),10);      
                  break;
    case CC_DISABLEESCROW: 
      HAL_UART_Transmit(&huart5,(uint8_t *)DisableEscrowMode, sizeof(DisableEscrowMode),10);      
                  break;
    case CC_READBUFFEREDBILL:              
      HAL_UART_Transmit(&huart5,(uint8_t *)ccReadBufferedBill, sizeof(ccReadBufferedBill),10);   
                  break;   
    case CC_REQ_ACC_COUNT:   
      HAL_UART_Transmit(&huart5,(uint8_t *)ccRequestAccepterCounter, sizeof(ccRequestAccepterCounter),10);  
                  break;
 //   case CC_REQ_IND_ACC_COUNT:
 //     HAL_UART_Transmit(&huart5,(uint8_t *)ccRequestIndAccepterCounter, sizeof(ccRequestIndAccepterCounter),10); 
 //                 break;
    }

     __HAL_UART_ENABLE_IT(&huart5, UART_IT_RXNE);
     __HAL_UART_ENABLE_IT(&huart5, UART_IT_IDLE);
     __HAL_UART_SEND_REQ(&huart5, UART_RXDATA_FLUSH_REQUEST); 

}

void UART5_IRQHandler(void)
{
volatile uint32_t tmpval;  //  

  if (__HAL_UART_GET_FLAG(&huart5, UART_FLAG_TC) != RESET) {
          if (__HAL_UART_GET_IT_SOURCE(&huart5, UART_IT_TC) != RESET) {
            __HAL_UART_CLEAR_FLAG(&huart5, UART_CLEAR_TCF);
          }
        }
        if (__HAL_UART_GET_FLAG(&huart5, UART_FLAG_ORE) != RESET) {
          //  if (__HAL_UART_GET_IT_SOURCE(&huart2, UART_IT_ORE) != RESET) {
          __HAL_UART_SEND_REQ(&huart5, UART_RXDATA_FLUSH_REQUEST);    
          __HAL_UART_CLEAR_FLAG(&huart5, UART_CLEAR_OREF);
         //   }
        }
        if(__HAL_UART_GET_FLAG(&huart5, UART_FLAG_IDLE) != RESET){
         // if (__HAL_UART_GET_IT_SOURCE(&huart5, UART_IT_IDLE) != RESET) {
        
           ccTalk.buffer[ccTalk.IndWR] = '\0';
           ccTalk.readyBuff = 1;
           ccTalk.readySymbols = ccTalk.IndWR;
           ccTalk.IndWR = 0;     
         //   tmpval = huart5.Instance->RDR;
          __HAL_UART_CLEAR_IT(&huart5, UART_CLEAR_IDLEF);
      //   }
        }
        if (__HAL_UART_GET_FLAG(&huart5, UART_FLAG_RXNE) != RESET) {
          if (__HAL_UART_GET_IT_SOURCE(&huart5, UART_IT_RXNE) != RESET) {
            tmpval = huart5.Instance->RDR;
            ccTalk.buffer[ccTalk.IndWR]= (uint8_t)tmpval;
            ccTalk.IndWR++;
            // protection
            if( !(ccTalk.IndWR < BUFLEN_CC))  // protect
              ccTalk.IndWR = 0;
            tmpval = huart5.Instance->RDR; // Clear RXNE bit
            UNUSED(tmpval);
            __HAL_UART_CLEAR_FLAG(&huart5, UART_FLAG_RXNE);
            __HAL_UART_SEND_REQ(&huart5, UART_RXDATA_FLUSH_REQUEST); 
          }
      }
}