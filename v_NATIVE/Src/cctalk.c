#include "stm32f303xc.h"
#include "stm32f3xx_hal.h"
#include "cctalk.h"
#include <stdint.h>
#include "usart.h"
#include "cmsis_os.h"
#include "lcd.h"
#include <stdio.h>
#include <string.h>

// several commands what we need for
const uint8_t ccOpen[]                  = {0,2,1,231,255,255,24};
const uint8_t ccMasterReady[]           = {0,1,1,228,255,27};
const uint8_t DisableEscrowMode[]       = {0,1,1,153,0,101};//
const uint8_t ccRequest[]               = {0,0,1,29,226};

const uint8_t SimplePool[]              = {2,0,1,254,255};





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

void ccTalkParseStatus(void){
 char Str[20];
 char StrShot[5];
 uint8_t i;
 
 memset(Str,0,sizeof(Str));

  while(!ccTalk.readyBuff) {
  vTaskDelay(10);
  taskYIELD();
  }
  lcd_goto(0);

for(i=0; i<11; i++){
  
   sprintf(StrShot,"%d,", ccTalk.buffer[i]);
   strcat(Str,StrShot);
}

 // sprintf(Str,"%d,%d,%d,%d,%d,%d,%d,%d",ccTalk.buffer[5],ccTalk.buffer[6],ccTalk.buffer[5],ccTalk.buffer[7],ccTalk.buffer[8],ccTalk.buffer[9],ccTalk.buffer[10],ccTalk.buffer[11]);
   
  lcd_puts((char const *)Str);
 return;
}

void ccTalkSendCMD(uint8_t Des){
  ccTalk.readyBuff = 0;
  ccTalk.IndWR = 0;  
  switch(Des){    
    case CC_OPEN:
      HAL_UART_Transmit(&huart5,(uint8_t *)ccOpen , sizeof(ccOpen),10);
                  break;
    case CC_MASTER_READY:
      HAL_UART_Transmit(&huart5,(uint8_t *)ccMasterReady , sizeof(ccMasterReady),10);
                  break;   
    case CC_REQUEST:
      HAL_UART_Transmit(&huart5,(uint8_t *)ccRequest , sizeof(ccRequest),10);
                  break;  
    case CC_CLOSE:
      HAL_UART_Transmit(&huart5,(uint8_t *)ccClose , sizeof(ccClose),10);      
                  break;
    case CC_SIMPLEPOOL: 
      HAL_UART_Transmit(&huart5,(uint8_t *)SimplePool , sizeof(SimplePool),10);      
                  break;
    case CC_DISABLEESCROW: 
      HAL_UART_Transmit(&huart5,(uint8_t *)SimplePool , sizeof(SimplePool),10);      
                  break;
    }
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
                __HAL_UART_CLEAR_FLAG(&huart5, UART_CLEAR_OREF);
         //   }
        }
        if(__HAL_UART_GET_FLAG(&huart5, UART_FLAG_IDLE) != RESET){
         // if (__HAL_UART_GET_IT_SOURCE(&huart5, UART_IT_IDLE) != RESET) {
        
          __HAL_UART_CLEAR_IT(&huart5, UART_CLEAR_IDLEF);
           ccTalk.buffer[ccTalk.IndWR] = '\0';
           ccTalk.readyBuff = 1;
           ccTalk.IndWR = 0;     
     
        
          tmpval = huart5.Instance->RDR;
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
          }
      }
}