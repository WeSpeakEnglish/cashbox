#include "simcom.h"
#include "parse_sim800.h"
#include "stm32f303xc.h"
#include "stm32f3xx_hal.h"
#include <stdlib.h>
#include <string.h>
#include <usart.h>
#include "HTTP.h"
#include "calculations.h"


#define SIMCOM_PWR_PORT GPIOA
#define SIMCOM_PWR_PIN  GPIO_PIN_4
#define LV_SHIFTER_OE_Pin GPIO_PIN_1
#define LV_SHIFTER_OE_GPIO_Port GPIOC


SemaphoreHandle_t xSemaphoreUART2 = NULL;

uint8_t RX_Buffer2[RX_BUFFER_SIZE];                                                        //receive buffer for incoming data from GSM module

char CMD_Bufer[CMD_Queue_size][CMD_Length];
uint16_t CMD_index = 0;

uint16_t rx_wr_index2;                                                          //RX_buffer position indicator

const char GSM_ATcmd[]="AT";                                               // send "AT" string for gsm to set up its auto baud rade
const char GSM_ATcmd_Disable_Echo[]="ATE0";                                // disable command echo
//const char GSM_ATcmd_Enable_Echo[]="ATE1\r";                                // enable command echo
const char GSM_ATcmd_9600_Baud[]="AT+IPR=9600";                            // manually set a fixed buad rate of 9600bps
const char GSM_ATcmd_Erase_All_SMS[]="AT+CMGD=1,4";                        // erase all text messages from inbox
const char GSM_ATcmd_SIM_Query[]="AT#QSS?";   
const char creg_str[] = "AT+CREG?";
const char cgreg_str[] = "AT+CGREG?";                                        // Query SIM Status
const char GSM_ATcmd_terminate[]="\r";                                       // terminate command
const char GSM_ATcmd_Reject_call[]="ATH";                                  // Reject the incomming call
const char GSM_ATcmd_Signal[]="AT+CSQ";                                    // Report signal quality
const char cusd_str[] = "AT+CUSD=1,\"*205#\"";

const char contype_str[] = "AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"";
const char apn_str[] = "AT+SAPBR=3,1,\"APN\",\"internet\"";
const char user_str[] = "AT+SAPBR=3,1,\"USER\",\"gdata\"";
const char pwd_str[] = "AT+SAPBR=3,1,\"PWD\",\"gdata\"";
const char sapbr_str[] = "AT+SAPBR=1,1";
const char sapbr_close_str[] = "AT+SAPBR=0,1";
const char cgatt_str[] = "AT+CGATT?";
const char gsmloc_snd_str[] = "AT+CIPGSMLOC=1,1";
const char sapbr_contextIP_str[] = "AT+SAPBR=2,1";

const char       vendweb_data[] = "http://vendweb.ru/api/0.1/data";
//const char       vendweb_data[] = "http://wow-only.ru/";
const char    vendweb_command[] = "http://vendweb.ru/api/0.1/commands";
const char vendweb_collection[] = "http://vendweb.ru/api/0.1/collection";
const char    vendweb_washing[] = "http://vendweb.ru/api/0.1/washing"; 

const char sig_str[]  = "&signal=";
const char bal_str[] = "&balance=";

xQueueHandle SIM800_CommandsQ;

SIM800 Sim800;

void SemaphoreUART2wait(void ){
  if( xSemaphoreTake( xSemaphoreUART2, 10000) == pdTRUE ){
                             vTaskDelay(10);
                             xSemaphoreGive(xSemaphoreUART2);
  }

}



void SIM800_SendCMD(void){
static Message Msg;
 
 //Transmit command to the SIM800
 if(xQueueReceive(SIM800_CommandsQ, &Msg, 10) == pdPASS){
                               Msg.pMessage[Msg.SizeOfMessage -1 ] = '\r';
                               HAL_UART_Transmit_DMA(&huart2,(uint8_t *)Msg.pMessage , Msg.SizeOfMessage);
                               SIM800_ParserID = Msg.ParserID;
    }
  
  vTaskDelay(10);
}
void SIM800_info_upload(void)
{
    char post_body[35];
    char id_str[8] ={0,0,0,0,0,0,0,0};

    sprintf( id_str, "id=%d", TERMINAL_UID );
    ///
    Sim800.signal_quality = 90;
    Sim800.current_balance.rub = 50;
    Sim800.current_balance.cop = 19;
    ///
    memset(post_body,0,25);
    strcat(post_body,id_str);
    strcpy(post_body+strlen(post_body), sig_str);
    Utoa((uint16_t)(Sim800.signal_quality), post_body+strlen(post_body));
    strcpy(post_body+strlen(post_body), bal_str);
    Utoa((uint16_t)(Sim800.current_balance.rub), post_body+strlen(post_body));
    strcat(post_body,".");
    Utoa((uint16_t)(Sim800.current_balance.cop), post_body+strlen(post_body));
    submitHTTPRequest(POST, (char*)vendweb_data, post_body);
}

void SIM800_init_info_upload(void)
{
    char post_body[64];
    char id_str[8] ={0,0,0,0,0,0,0,0};
    
 //   char url[31];
 //   strcpy(url, vendweb_data);
    sprintf( id_str, "id=%d", TERMINAL_UID );
    memset(post_body,0,64);
    strcat(post_body,id_str);
    strcat(post_body,"&lat=");
    strcat(post_body, (char const *)Sim800.current_location.latitude);
    strcat(post_body,"&lon=");
    strcat(post_body, (char const *)Sim800.current_location.longitude);
    strcat(post_body,"&phone=");
    strcat(post_body, (char const *)Sim800.phone_number);
    Sim800.upload_init_info_stat = SENDING;
    submitHTTPRequest(POST, (char *)vendweb_data, post_body);
}

uint32_t SIM800_AddCMD(char * Msg, uint16_t Length, uint16_t ParserID){ // add new commad to the Queue
Message Cmd;

  strcpy(&CMD_Bufer[CMD_index][0], Msg); 
  Cmd.pMessage =  &CMD_Bufer[CMD_index][0];
  Cmd.SizeOfMessage = Length;
  Cmd.ParserID = ParserID;
 // Msg->SizeOfMessage
  CMD_index++;
  CMD_index %= CMD_Queue_size;
  Sim800.parsed = 0;
  return xQueueSendToBack(SIM800_CommandsQ, &Cmd, 100);
}

void SIM800_Ini(void){
  
 SIM800_PowerOnOff();
 HAL_GPIO_WritePin(LV_SHIFTER_OE_GPIO_Port, LV_SHIFTER_OE_Pin, GPIO_PIN_SET);
  
 Sim800.pRX_Buffer = Sim800.RX_Buffer1;   // point to current RX buffer
 Sim800.pRX_WR_index = &(Sim800.RX_WR_index1); // index of current RX buffer
 Sim800.RX_WR_index1 = 0;  // how much of elements have been written to the first buffer
 Sim800.RX_WR_index2 = 0;  // how much of elements have been written to the second buffer
 Sim800.bufferIsReady = 0; // buffer is ready to parse 
 Sim800.pReadyBuffer = Sim800.RX_Buffer1;  // pointer to buffer which is ready to parse
 Sim800.pReadyIndex = &Sim800.RX_WR_index1;
 Sim800.parsed = 0;
 Sim800.initialized = 0;
 Sim800.retry = 0;
 
  SIM800_CommandsQ = xQueueCreate(20, sizeof(Message));
 }

void SIM800_waitAnswer(uint8_t Cycles){
  while(Cycles--){
  Sim800.parsed = 0;
   while (Sim800.parsed == 0){
    vTaskDelay(100);
    taskYIELD();
  }
  }
}
 
void SIM800_IniCMD(void){
  __HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);  //Enable IDLE
  ResParse.byte = 0; //reset parsed bits
  vTaskDelay(3000);
  SIM800_AddCMD((char *)GSM_ATcmd,sizeof(GSM_ATcmd),0); // AT to sinhronize
  vTaskDelay(4000);
  SIM800_AddCMD((char *)GSM_ATcmd_Disable_Echo,sizeof(GSM_ATcmd_Disable_Echo),1);
  SIM800_waitAnswer(1); 
     
   SIM800_AddCMD((char *)creg_str,sizeof(creg_str),2);
   SIM800_waitAnswer(1);

   SIM800_AddCMD((char *)cgreg_str,sizeof(cgreg_str),2);
   SIM800_waitAnswer(1);

   SIM800_AddCMD((char *)contype_str,sizeof(contype_str),0);
   SIM800_waitAnswer(1);
   SIM800_AddCMD((char *)apn_str,sizeof(apn_str),0);
   SIM800_waitAnswer(1);
   SIM800_AddCMD((char *)user_str,sizeof(user_str),0);
   SIM800_waitAnswer(1);
   SIM800_AddCMD((char *)pwd_str,sizeof(pwd_str),0);
   SIM800_waitAnswer(1);
   SIM800_AddCMD((char *)sapbr_str,sizeof(sapbr_str),0);   
   SIM800_waitAnswer(1);
   SIM800_AddCMD((char *)cgatt_str,sizeof(cgatt_str),0); 
   SIM800_waitAnswer(1);
  SIM800_AddCMD((char *)gsmloc_snd_str,sizeof(gsmloc_snd_str),3);
   SIM800_waitAnswer(1);
  SIM800_AddCMD((char *)cusd_str,sizeof(cusd_str),0);
   SIM800_waitAnswer(2);
   vTaskDelay(1000);
  SIM800_parse_PhoneNumber(); // the phone number will be received later
  vTaskDelay(100);
  Sim800.initialized = 1;

  vTaskDelete(NULL);   //  error 
   // }
}


void SIM800_pop_washing(void){

 return;
}

void SIM800_PowerOnOff(void){
  if(!Sim800.powered)Sim800.powered = 0;
  else Sim800.powered = 1;

  HAL_GPIO_WritePin(SIMCOM_PWR_PORT, SIMCOM_PWR_PIN, GPIO_PIN_SET);
   HAL_Delay(1200);
   HAL_GPIO_WritePin(SIMCOM_PWR_PORT, SIMCOM_PWR_PIN, GPIO_PIN_RESET);
   HAL_Delay(3000);
  
   return;
}



void USART2_IRQHandler(void)
{
volatile uint8_t tmpval;  //  
static portBASE_TYPE xHigherPriorityTaskWoken;

        if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC) != RESET) {
            __HAL_UART_CLEAR_IT(&huart2, UART_FLAG_TC);
        }
        if(__HAL_UART_GET_FLAG(&huart2, UART_FLAG_IDLE) != RESET){
        
        __HAL_UART_CLEAR_IT(&huart2, UART_FLAG_IDLE);
          xSemaphoreGiveFromISR( xSemaphoreUART2, &xHigherPriorityTaskWoken );
          tmpval = huart2.Instance->RDR;
              Sim800.bufferIsReady = 1;
              if(Sim800.pRX_Buffer == Sim800.RX_Buffer1){ // switch to the second buffer
                Sim800.pRX_Buffer = Sim800.RX_Buffer2;
                Sim800.RX_WR_index2 = 0;
                Sim800.pRX_WR_index = &(Sim800.RX_WR_index2);
                Sim800.pReadyBuffer = Sim800.RX_Buffer1;
                Sim800.pReadyIndex = &Sim800.RX_WR_index1;
              }
              else{ 
                // switch to the first buffer 
                Sim800.pRX_Buffer = Sim800.RX_Buffer1;
                Sim800.RX_WR_index1 = 0;
                Sim800.pRX_WR_index = &(Sim800.RX_WR_index1); 
                Sim800.pReadyBuffer = Sim800.RX_Buffer2;
                Sim800.pReadyIndex = &Sim800.RX_WR_index2;
              }
        
        }
        if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE) != RESET) {
            tmpval = (uint8_t)(USART2->RDR & 0x00FF);
              
            Sim800.pRX_Buffer[(*Sim800.pRX_WR_index)] = tmpval;
            *Sim800.pRX_WR_index +=1;
            // protection
            if (*(Sim800.pRX_WR_index) == RX_BUFFER_SIZE){
            *(Sim800.pRX_WR_index) = 0;
              }
            tmpval = huart2.Instance->RDR; // Clear RXNE bit
            __HAL_UART_SEND_REQ(&huart2, UART_RXDATA_FLUSH_REQUEST); 
            portYIELD_FROM_ISR( xHigherPriorityTaskWoken );    
}
 
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