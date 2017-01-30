#include "simcom.h"
#include "stm32f303xc.h"
#include "stm32f3xx_hal.h"
#include <stdlib.h>
#include <string.h>
#include <usart.h>

#define SIMCOM_PWR_PORT GPIOA
#define SIMCOM_PWR_PIN  GPIO_PIN_4
#define LV_SHIFTER_OE_Pin GPIO_PIN_1
#define LV_SHIFTER_OE_GPIO_Port GPIOC




uint8_t RX_Buffer2[RX_BUFFER_SIZE];                                                        //receive buffer for incoming data from GSM module

char CMD_Bufer[CMD_Queue_size][CMD_Length];
uint16_t CMD_index = 0;

uint16_t rx_wr_index2;                                                          //RX_buffer position indicator

const char GSM_ATcmd[]="AT\r";                                               // send "AT" string for gsm to set up its auto baud rade
const char GSM_ATcmd_Disable_Echo[]="ATE0\r";                                // disable command echo
const char GSM_ATcmd_9600_Baud[]="AT+IPR=9600\r";                            // manually set a fixed buad rate of 9600bps
const char GSM_ATcmd_Erase_All_SMS[]="AT+CMGD=1,4\r";                        // erase all text messages from inbox
const char GSM_ATcmd_SIM_Query[]="AT#QSS?\r";   
const char GSM_ATcmd_SIM_Status[]="AT+CREG?\r";                                   // Query SIM Status
const char GSM_ATcmd_terminate[]="\r";                                       // terminate command
const char GSM_ATcmd_Reject_call[]="ATH\r";                                  // Reject the incomming call
const char GSM_ATcmd_Signal[]="AT+CSQ\r";                                    // Report signal quality

xQueueHandle SIM800_CommandsQ;

//struct {
//  uint8_t powered : 1;
//}SIM800;

SIM800 Sim800;

void SIM800_SendCMD(void){
static Message Msg;
 

  //Message  = 
    if(xQueueReceive(SIM800_CommandsQ, &Msg, 10) == pdPASS){
                               HAL_UART_Transmit_DMA(&huart2,(uint8_t *)Msg.pMessage , Msg.SizeOfMessage-1);
    }
  
  vTaskDelay(10);
   // HAL_UART_Transmit_DMA(&huart2, (uint8_t *)GSM_ATcmd_Disable_Echo, sizeof(GSM_ATcmd_Disable_Echo)-1);

   // HAL_UART_Transmit_DMA(&huart2, (uint8_t *)GSM_ATcmd_9600_Baud, sizeof(GSM_ATcmd_9600_Baud)-1);

   // HAL_UART_Transmit_DMA(&huart2, (uint8_t *)GSM_ATcmd_SIM_Query, sizeof(GSM_ATcmd_SIM_Query)-1);

}



uint32_t SIM800_AddCMD(char * Msg, uint16_t Length){ // add new commad to the Queue
Message Cmd;

  strcpy(&CMD_Bufer[CMD_index][0], Msg); 
  Cmd.pMessage =  &CMD_Bufer[CMD_index][0];
  Cmd.SizeOfMessage = Length;
 // Msg->SizeOfMessage
  CMD_index++;
  CMD_index %= CMD_Queue_size;
  return xQueueSendToBack(SIM800_CommandsQ, &Cmd, 100);
}

void SIM800_Ini(void){

// SIM800_PowerOnOff();
 HAL_GPIO_WritePin(LV_SHIFTER_OE_GPIO_Port, LV_SHIFTER_OE_Pin, GPIO_PIN_SET);
  
 Sim800.pRX_Buffer = Sim800.RX_Buffer1;   // point to current RX buffer
 Sim800.pRX_WR_index = &(Sim800.RX_WR_index1); // index of current RX buffer
 Sim800.RX_WR_index1 = 0;  // how much of elements have been written to the first buffer
 Sim800.RX_WR_index2 = 0;  // how much of elements have been written to the second buffer
 Sim800.bufferIsReady = 0; // buffer is ready to parse 
 Sim800.pReadyBuffer = Sim800.RX_Buffer1;  // pointer to buffer which is ready to parse
 
 SIM800_CommandsQ = xQueueCreate(20, sizeof(Message));
  
  SIM800_AddCMD((char *)GSM_ATcmd,sizeof(GSM_ATcmd));
  SIM800_AddCMD((char *)GSM_ATcmd_Disable_Echo,sizeof(GSM_ATcmd_Disable_Echo));
  SIM800_AddCMD((char *)GSM_ATcmd_SIM_Status,sizeof(GSM_ATcmd_SIM_Status));
 
 }

void SIM800_PowerOnOff(void){
  if(!Sim800.powered)Sim800.powered = 0;
  else Sim800.powered = 1;
  HAL_GPIO_WritePin(SIMCOM_PWR_PORT, SIMCOM_PWR_PIN, GPIO_PIN_RESET);
   HAL_Delay(1200);
   HAL_GPIO_WritePin(SIMCOM_PWR_PORT, SIMCOM_PWR_PIN, GPIO_PIN_SET);
   HAL_Delay(1200);
   HAL_GPIO_WritePin(SIMCOM_PWR_PORT, SIMCOM_PWR_PIN, GPIO_PIN_RESET);
   HAL_Delay(1200);
  
   return;
}



void USART2_IRQHandler(void)
{
volatile uint8_t tmpval;  //  
static uint8_t OldChar = '\0';

        if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC) != RESET) {
      //      irq_handler(serial_irq_ids[id], TxIrq);
            __HAL_UART_CLEAR_IT(&huart2, UART_FLAG_TC);
        }
        if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE) != RESET) {
            tmpval = (uint8_t)(USART2->RDR & 0x00FF);
             if((tmpval == '\r') &&(OldChar != '\r')&&(OldChar != '\n')){
              Sim800.bufferIsReady = 1;
              if(Sim800.pRX_Buffer == Sim800.RX_Buffer1){ // switch to the second buffer
                Sim800.pRX_Buffer = Sim800.RX_Buffer2;
                Sim800.RX_WR_index2 = 0;
                Sim800.pRX_WR_index = &(Sim800.RX_WR_index2);
                Sim800.pReadyBuffer = Sim800.RX_Buffer1;
              }
              else{ 
                // switch to the first buffer 
                Sim800.pRX_Buffer = Sim800.RX_Buffer1;
                Sim800.RX_WR_index1 = 0;
                Sim800.pRX_WR_index = &(Sim800.RX_WR_index1); 
                Sim800.pReadyBuffer = Sim800.RX_Buffer2;
              }
            }
            
            else{
              if((tmpval != '\n')&& (tmpval != '\r' )){
              
            Sim800.pRX_Buffer[(*Sim800.pRX_WR_index)] = tmpval;
            *Sim800.pRX_WR_index +=1;

            // protection
            if (*(Sim800.pRX_WR_index) == RX_BUFFER_SIZE){
            *(Sim800.pRX_WR_index) = 0;
              }
             }
            }
            OldChar = tmpval; 
            tmpval = huart2.Instance->RDR; // Clear RXNE bit
   //         __HAL_UART_SEND_REQ(&huart2, UART_RXDATA_FLUSH_REQUEST);      
}
__HAL_UART_SEND_REQ(&huart2, UART_RXDATA_FLUSH_REQUEST);      
 
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
  RX_Buffer2[rx_wr_index2++] = USART2->RDR & (uint8_t)0x00FF;
  if (rx_wr_index2 == RX_BUFFER_SIZE)
  rx_wr_index2 = 0;
}


void serial_clear(UART_HandleTypeDef * usart)
{
    __HAL_UART_CLEAR_IT(usart, UART_FLAG_TC);
    __HAL_UART_SEND_REQ(usart, UART_RXDATA_FLUSH_REQUEST);
}