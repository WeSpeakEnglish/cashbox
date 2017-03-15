#include "modbus.h"
#include "stm32f3xx_hal.h"
#include "usart.h"


#define AWAITDELAY_TRANSMIT 100

MODBUS_struct Modbus;


Packet packets[TOTAL_NO_OF_PACKETS];
unsigned int regs[10];

unsigned int SetCoil; // set coil

#define TOTAL_NO_OF_REGISTERS 10
// Masters register array
unsigned int regs[TOTAL_NO_OF_REGISTERS];

void ModbusBufferFree(void){
  uint16_t i;
  for(i = 0; i< sizeof(Modbus.buffer); i++){
   Modbus.buffer[i] = 0;
  }
return;
}

void TranmitSlaveCmd(uint8_t Cmd){
  //we need to prepare buffer here
  switch(Cmd){
    case 0:
      HAL_UART_Transmit_DMA(&huart3, (uint8_t *) Modbus.buffer, sizeof(Modbus.buffer));
      break;
    case 1:  
      
      break;
  }
}

void ReceiveSlaveCmd(void){
  

}

void USART3_IRQHandler(void) {
    static uint32_t tmpval; //  
    if (__HAL_UART_GET_FLAG(&huart3, UART_FLAG_TC) != RESET) {
        if (__HAL_UART_GET_IT_SOURCE(&huart3, UART_IT_TC) != RESET) {
            __HAL_UART_CLEAR_FLAG(&huart3, UART_CLEAR_TCF);
        }
    }
    if (__HAL_UART_GET_FLAG(&huart3, UART_FLAG_ORE) != RESET) {

        __HAL_UART_SEND_REQ(&huart3, UART_RXDATA_FLUSH_REQUEST);
        __HAL_UART_CLEAR_FLAG(&huart3, UART_CLEAR_OREF);

    }
    if (__HAL_UART_GET_FLAG(&huart3, UART_FLAG_IDLE) != RESET) {
        if (__HAL_UART_GET_IT_SOURCE(&huart3, UART_IT_IDLE) != RESET) {
            Modbus.buffer[Modbus.IndWR] = '\0';
            Modbus.readyBuff = 1;
            Modbus.readySymbols = Modbus.IndWR;
            Modbus.IndWR = 0;
            //   tmpval = huart5.Instance->RDR;
            __HAL_UART_CLEAR_IT(&huart3, UART_CLEAR_IDLEF);
        }
    }

    if (__HAL_UART_GET_FLAG(&huart3, UART_FLAG_RXNE) != RESET) {
        if (__HAL_UART_GET_IT_SOURCE(&huart3, UART_IT_RXNE) != RESET) {
            tmpval = huart3.Instance->RDR;
            Modbus.buffer[Modbus.IndWR] = (uint8_t) tmpval;
            Modbus.IndWR++;
            // protection
            if (Modbus.IndWR > BUFLEN_MODBUS - 2) // protect
                Modbus.IndWR = 0;
            tmpval = huart3.Instance->RDR; // Clear RXNE bit
            UNUSED(tmpval);
            __HAL_UART_CLEAR_FLAG(&huart3, UART_FLAG_RXNE);
            __HAL_UART_SEND_REQ(&huart3, UART_RXDATA_FLUSH_REQUEST);
        }
    }
}