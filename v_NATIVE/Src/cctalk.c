#include "stm32f303xc.h"
#include "stm32f3xx_hal.h"
#include "cctalk.h"
#include <stdint.h>
#include "usart.h"

// several commands what we need for
const uint8_t ccOpen[]         = {0,2,1,231,255,255,24};
const uint8_t ccMasterReady[]  = {0,1,1,228,255,27};
const uint8_t ccRequest[]      = {0,0,1,29,226};
const uint8_t ccClose[]        = {0,1,1,231,0,0};



CC_struct ccTalk;

uint8_t ccTalkParseOK(void){ //if command receive
 if(ccTalk.readyBuff){
  if(ccTalk.buffer[3] == 0x00)
    return 1; // one is OK
  else 
    return 0;
  }
 return 0;
}

void ccTalkParseStatus(void){

 return;
}

void ccTalkSendCMD(uint8_t Des){
  ccTalk.readyBuff = 0;
  ccTalk.IndWR = 0;  
  switch(Des){
    case CC_INIT:
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
    }
}

void UART5_IRQHandler(void)
{
volatile uint8_t tmpval;  //  
        if (__HAL_UART_GET_FLAG(&huart5, UART_FLAG_TC) != RESET) {
            __HAL_UART_CLEAR_IT(&huart5, UART_FLAG_TC);
        }
        if(__HAL_UART_GET_FLAG(&huart5, UART_FLAG_IDLE) != RESET){
        
        __HAL_UART_CLEAR_IT(&huart5, UART_FLAG_IDLE);

        tmpval = huart5.Instance->RDR;
              ccTalk.readyBuff = 1;
              ccTalk.IndWR = 0;        
        }
        if (__HAL_UART_GET_FLAG(&huart5, UART_FLAG_RXNE) != RESET) {
            tmpval = huart5.Instance->RDR& 0x00FF;
            ccTalk.buffer[ccTalk.IndWR++];
            if( !(ccTalk.IndWR < BUFLEN_CC))  // protect
              ccTalk.IndWR = 0;
          //  tmpval = huart5.Instance->RDR& 0x00FF;// Clear RXNE bit
            __HAL_UART_SEND_REQ(&huart5, UART_RXDATA_FLUSH_REQUEST); 
  }
}