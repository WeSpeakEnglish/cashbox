#define SIMCOM_PWR_PORT GPIOA
#define SIMCOM_PWR_PIN  GPIO_PIN_4
#include "simcom.h"
#include "parse_sim800.h"
#include "stm32f303xc.h"
#include "stm32f3xx_hal.h"
#include <stdlib.h>
#include <string.h>
#include <usart.h>
#include "HTTP.h"
#include "calculations.h"
#include "sms.h"
#include "vend.h"
#include "core.h"

#define SIMCOM_PWR_PORT GPIOA
#define SIMCOM_PWR_PIN  GPIO_PIN_4
#define LV_SHIFTER_OE_Pin GPIO_PIN_1
#define LV_SHIFTER_OE_GPIO_Port GPIOC


uint8_t RX_Buffer2[RX_BUFFER_SIZE]; //receive buffer for incoming data from GSM module

char CMD_Bufer[CMD_Queue_size][CMD_Length];
uint16_t CMD_index = 0;

uint16_t rx_wr_index2; //RX_buffer position indicator

const char GSM_ATcmd[] = "AT"; // send "AT" string for gsm to set up its auto baud rade
const char GSM_ATcmd_Disable_Echo[] = "ATE0"; // disable command echo
//const char GSM_ATcmd_Enable_Echo[]="ATE1\r";                                // enable command echo
const char GSM_ATcmd_9600_Baud[] = "AT+IPR=9600"; // manually set a fixed buad rate of 9600bps
const char GSM_ATcmd_Erase_All_SMS[] = "AT+CMGD=1,4"; // erase all text messages from inbox
const char GSM_ATcmd_SIM_Query[] = "AT#QSS?";
const char creg_str[] = "AT+CREG?";
const char cgreg_str[] = "AT+CGREG?"; // Query SIM Status
const char GSM_ATcmd_terminate[] = "\r"; // terminate command
const char GSM_ATcmd_Reject_call[] = "ATH"; // Reject the incomming call
const char GSM_ATcmd_Signal[] = "AT+CSQ"; // Report signal quality
const char cusd_str[] = "AT+CUSD=1,\"*205#\""; // get number

const char contype_str[] = "AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"";
const char apn_str[] = "AT+SAPBR=3,1,\"APN\",\"internet\"";
const char user_str[] = "AT+SAPBR=3,1,\"USER\",\"gdata\"";
const char pwd_str[] = "AT+SAPBR=3,1,\"PWD\",\"gdata\"";
const char sapbr_str[] = "AT+SAPBR=1,1";
const char sapbr_close_str[] = "AT+SAPBR=0,1";
const char cgatt_str[] = "AT+CGATT?";
const char gsmloc_snd_str[] = "AT+CIPGSMLOC=1,1";
const char sapbr_contextIP_str[] = "AT+SAPBR=2,1";

const char vendweb_data[] = "http://vendweb.ru/api/0.1/data";
const char vendweb_command[] = "http://vendweb.ru/api/0.1/commands";
const char vendweb_collection[] = "http://vendweb.ru/api/0.1/collection";
const char vendweb_washing[] = "http://vendweb.ru/api/0.1/washing";

const char sig_str[] = "&signal=";
const char bal_str[] = "&balance=";

washing_holder wash_holder;
SIM800 Sim800;

void SIM800_info_upload(void) // upload info to the server
{
    char post_body[35];
    char id_str[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    sprintf(id_str, "id=%d", terminal_UID);

    memset(post_body, 0, 25);
    strcat(post_body, id_str);
    strcpy(post_body + strlen(post_body), sig_str);
    Utoa((uint16_t) (Sim800.signal_quality), post_body + strlen(post_body));
    strcpy(post_body + strlen(post_body), bal_str);
    //SIM800_get_Balance();
    Utoa((uint16_t) (Sim800.current_balance.rub), post_body + strlen(post_body));
    strcat(post_body, ".");
    Utoa((uint16_t) (Sim800.current_balance.cop), post_body + strlen(post_body));
    SIM800_GPRS_open();
    submitHTTP_init();
    submitHTTPRequest(POST, (char*) vendweb_data, post_body, 0);
    submitHTTP_terminate();
    SIM800_GPRS_close();
}

void SIM800_collection(void) // upload info to the server
{
    char post_body[35];
    char id_str[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    sprintf(id_str, "id=%d", terminal_UID);

    memset(post_body, 0, 25);
    strcat(post_body, id_str);
    
    SIM800_GPRS_open();
    submitHTTP_init();
    submitHTTPRequest(POST, (char*) vendweb_collection, post_body, 0);
    submitHTTP_terminate();
    SIM800_GPRS_close();
}
void SIM800_command(void) {
    char post_body[35];
    char id_str[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    sprintf(id_str, "id=%d", terminal_UID);

    memset(post_body, 0, 25);
    strcat(post_body, id_str);
    SIM800_GPRS_open();
    submitHTTP_init();
    submitHTTPRequest(POST, (char*) vendweb_command, post_body, 1);
    SIM800_Parse_WM(); // read the answer from HTTP
    submitHTTP_terminate();
    SIM800_GPRS_close();
}

void SIM800_get_Signal(void) {
    SIM800_AddCMD((char *) GSM_ATcmd_Signal, sizeof (GSM_ATcmd_Signal), 6);
    SIM800_waitAnswer(1);
    return;
}

void SIM800_init_info_upload(void) {
    char post_body[64];
    char id_str[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    sprintf(id_str, "id=%d", terminal_UID);
    memset(post_body, 0, 64);
    strcat(post_body, id_str);
    strcat(post_body, "&lat=");
    strcat(post_body, (char const *) Sim800.current_location.latitude);
    strcat(post_body, "&lon=");
    strcat(post_body, (char const *) Sim800.current_location.longitude);
    strcat(post_body, "&phone=");
    strcat(post_body, (char const *) Sim800.phone_number);
    Sim800.upload_init_info_stat = SENDING;
    SIM800_GPRS_open();
    submitHTTP_init();
    submitHTTPRequest(POST, (char *) vendweb_data, post_body, 0);
    submitHTTP_terminate();
    SIM800_GPRS_close();
}

uint32_t SIM800_AddCMD(char * Msg, uint16_t Length, uint16_t ParserID) { // add new commad to the Queue
  //  Cmd.pMessage = &CMD_Bufer[CMD_index][0];
 //   Cmd.SizeOfMessage = Length+1;
 // //  Cmd.ParserID = ParserID;
  //  CMD_index++;
 //   CMD_index %= CMD_Queue_size;
  uint8_t i;
  static char Cmd[128];
  
  for(i = 0; i < Length; i++)
    Cmd[i] = Msg[i];
  
    Sim800.parsed = 0;
  
    Cmd[Length-1] = '\r';
    if(HAL_UART_Transmit_DMA(&huart2, (uint8_t *) Cmd, Length) == HAL_OK)
            SIM800_ParserID = ParserID;
    return 0; //xQueueSendToBack(SIM800_CommandsQ, &Cmd, 100);
}

void SIM800_Ini(void) {

    SIM800_PowerOnOff();
    HAL_GPIO_WritePin(LV_SHIFTER_OE_GPIO_Port, LV_SHIFTER_OE_Pin, GPIO_PIN_SET);

    Sim800.pRX_Buffer = Sim800.RX_Buffer1; // point to current RX buffer
    Sim800.pRX_WR_index = &(Sim800.RX_WR_index1); // index of current RX buffer
    Sim800.RX_WR_index1 = 0; // how much of elements have been written to the first buffer
    Sim800.RX_WR_index2 = 0; // how much of elements have been written to the second buffer
    Sim800.bufferIsReady = 0; // buffer is ready to parse 
    Sim800.pReadyBuffer = Sim800.RX_Buffer1; // pointer to buffer which is ready to parse
    Sim800.pReadyIndex = &Sim800.RX_WR_index1;
    Sim800.parsed = 0;
    Sim800.initialized = 0;
    Sim800.retry = 0;
    Sim800.flush_SMS = 1;
    Sim800.CGATT_READY = 0;

    //SIM800_CommandsQ = xQueueCreate(20, sizeof (Message));
}

#define LONG_WAITING 100

uint8_t SIM800_waitAnswer(uint8_t Cycles) {
  uint8_t wait = 0;
    while (Cycles--) {
        Sim800.parsed = 0;
        while (Sim800.parsed == 0) {
            Delay_ms_OnFastQ(100);
            M_pull()();
            wait++;
            if(wait ==  LONG_WAITING)
              return LONG_WAITING;
        }
        Delay_ms_OnFastQ(100);
     //   M_pull()();
    }
return 0;    
}

void SIM800_GPRS_open(void) {
    uint8_t i;
    SIM800_AddCMD((char *) contype_str, sizeof (contype_str), 0);
    SIM800_waitAnswer(1);
    SIM800_AddCMD((char *) apn_str, sizeof (apn_str), 0);
    SIM800_waitAnswer(1);
    SIM800_AddCMD((char *) user_str, sizeof (user_str), 0);
    SIM800_waitAnswer(1);
    SIM800_AddCMD((char *) pwd_str, sizeof (pwd_str), 0);
    SIM800_waitAnswer(1);
    SIM800_AddCMD((char *) sapbr_str, sizeof (sapbr_str), 0);
    SIM800_waitAnswer(1);

    for (i = 0; i < 5; i++) {
        if (Sim800.CGATT_READY)
            break;
        SIM800_AddCMD((char *) cgatt_str, sizeof (cgatt_str), 7);
        SIM800_waitAnswer(1);
    }
}

void SIM800_GPRS_close(void) {
    SIM800_AddCMD((char *) sapbr_close_str, sizeof (sapbr_close_str), 0);
    SIM800_waitAnswer(1);
}

void SIM800_IniCMD(void) {
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE); //Enable IDLE
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); //Enable IDLE
    ResParse.byte = 0; //reset parsed bits
    Delay_ms_OnMediumQ(3000);
    SIM800_AddCMD((char *) GSM_ATcmd, sizeof (GSM_ATcmd), 0); // AT to sinhronize
    Delay_ms_OnMediumQ(4000);

    SIM800_AddCMD((char *) GSM_ATcmd_Disable_Echo, sizeof (GSM_ATcmd_Disable_Echo), 1);
    
    if(SIM800_waitAnswer(1)){
      SIM800_PowerOnOff();
      Delay_ms_OnMediumQ(3000);
      SIM800_AddCMD((char *) GSM_ATcmd, sizeof (GSM_ATcmd), 0); // AT to sinhronize
      Delay_ms_OnMediumQ(4000);
      SIM800_AddCMD((char *) GSM_ATcmd_Disable_Echo, sizeof (GSM_ATcmd_Disable_Echo), 1);
      SIM800_waitAnswer(1);
   }
     
     Delay_ms_OnMediumQ(1000);
     
    SIM800_get_Signal();
    SIM800_AddCMD((char *) creg_str, sizeof (creg_str), 2);
    SIM800_waitAnswer(1);

    SIM800_AddCMD((char *) cgreg_str, sizeof (cgreg_str), 2);
    SIM800_waitAnswer(1);
    SIM800_GPRS_open();
    SIM800_AddCMD((char *) gsmloc_snd_str, sizeof (gsmloc_snd_str), 3);
    SIM800_waitAnswer(1);
    SIM800_AddCMD((char *) cusd_str, sizeof (cusd_str), 0);
    Delay_ms_OnFastQ(100);
    SIM800_waitAnswer(2);
    SIM800_parse_PhoneNumber(); // the phone number will be received later
    Delay_ms_OnFastQ(100);

    SIM800_GPRS_close();

}


void SIM800_GetTimeAndLoc(void) { // it gets time and location
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE); //Enable IDLE
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); //Enable IDLE
    ResParse.byte = 0; //reset parsed bits
    Delay_ms_OnMediumQ(3000);
    SIM800_AddCMD((char *) GSM_ATcmd, sizeof (GSM_ATcmd), 0); // AT to sinhronize
    Delay_ms_OnMediumQ(4000);

    SIM800_AddCMD((char *) GSM_ATcmd_Disable_Echo, sizeof (GSM_ATcmd_Disable_Echo), 0);
    
    if(SIM800_waitAnswer(1)){
      SIM800_PowerOnOff();
      Delay_ms_OnMediumQ(3000);
      SIM800_AddCMD((char *) GSM_ATcmd, sizeof (GSM_ATcmd), 0); // AT to sinhronize
      Delay_ms_OnMediumQ(4000);
      SIM800_AddCMD((char *) GSM_ATcmd_Disable_Echo, sizeof (GSM_ATcmd_Disable_Echo), 0);
   }
      
    SIM800_waitAnswer(2);  
    SIM800_get_Signal();
    SIM800_AddCMD((char *) creg_str, sizeof (creg_str), 2);
    SIM800_waitAnswer(1);

    SIM800_AddCMD((char *) cgreg_str, sizeof (cgreg_str), 2);
    SIM800_waitAnswer(1);
    SIM800_GPRS_open();
    SIM800_AddCMD((char *) gsmloc_snd_str, sizeof (gsmloc_snd_str), 3);
    SIM800_waitAnswer(1);
    SIM800_AddCMD((char *) cusd_str, sizeof (cusd_str), 0);
    Delay_ms_OnFastQ(100);
    SIM800_waitAnswer(2);
    SIM800_parse_PhoneNumber(); // the phone number will be received later
    Delay_ms_OnFastQ(100);

    SIM800_GPRS_close();

}


// id=2&wm=105&cost=22&status=true

void SIM800_pop_washing(void) {
    char post_body[64];
    char str[8]; // an addition str to add
    char id_str[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    sprintf(id_str, "id=%d", terminal_UID);
    memset(post_body, 0, 64);
    strcat(post_body, id_str);
    strcat(post_body, "&wm=");
    strcat(post_body, Utoa(p_session->selected_washer, str));
    strcat(post_body, "&cost=");
    strcat(post_body, Utoa(WL[p_session->selected_washer - 1].price, str));
    strcat(post_body, "&status=true");
    Sim800.upload_init_info_stat = SENDING;
    SIM800_GPRS_open();
    submitHTTP_init();
    submitHTTPRequest(POST, (char *) vendweb_washing, post_body, 0);
    submitHTTP_terminate();
    SIM800_GPRS_close();

    return;
}

void SIM800_PowerOnOff(void) {
    if (!Sim800.powered)Sim800.powered = 0;
    else Sim800.powered = 1;

    HAL_GPIO_WritePin(SIMCOM_PWR_PORT, SIMCOM_PWR_PIN, GPIO_PIN_SET);
    HAL_Delay(1200);
    HAL_GPIO_WritePin(SIMCOM_PWR_PORT, SIMCOM_PWR_PIN, GPIO_PIN_RESET);
    HAL_Delay(3000);

    return;
}

void USART2_IRQHandler(void) {
    uint32_t tmpval; //  
  //  static portBASE_TYPE xHigherPriorityTaskWoken;

    if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC) != RESET) {
        if (__HAL_UART_GET_IT_SOURCE(&huart2, UART_IT_TC) != RESET) {
            __HAL_UART_CLEAR_FLAG(&huart2, UART_CLEAR_TCF);
        }
    }
    if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_ORE) != RESET) {
        //  if (__HAL_UART_GET_IT_SOURCE(&huart2, UART_IT_ORE) != RESET) {
        __HAL_UART_CLEAR_FLAG(&huart2, UART_CLEAR_OREF);
        //   }
    }
    if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_IDLE) != RESET) {
        if (__HAL_UART_GET_IT_SOURCE(&huart2, UART_IT_IDLE) != RESET) {

            __HAL_UART_CLEAR_IT(&huart2, UART_CLEAR_IDLEF);
            if (Sim800.flush_SMS) {
                if ((strstr((char const *) Sim800.pRX_Buffer, "+CMTI:")) != NULL) {
                    Sim800.SMS_received = 1; // stop flood 
                }
             //   else xSemaphoreGiveFromISR(xSemaphoreUART2, &xHigherPriorityTaskWoken);

            }
            else {
                if ((strstr((char const *) Sim800.pRX_Buffer, "+CMTI:")) != NULL) {
                    Sim800.SMS_received = 1; // stop flood 
                }
           //     xSemaphoreGiveFromISR(xSemaphoreUART2, &xHigherPriorityTaskWoken);
            }

            tmpval = huart2.Instance->RDR;
            Sim800.bufferIsReady = 1;
            if (Sim800.pRX_Buffer == Sim800.RX_Buffer1) { // switch to the second buffer
                Sim800.pRX_Buffer = Sim800.RX_Buffer2;
                Sim800.RX_WR_index2 = 0;
                Sim800.pRX_WR_index = &(Sim800.RX_WR_index2);
                Sim800.pReadyBuffer = Sim800.RX_Buffer1;
                Sim800.pReadyIndex = &Sim800.RX_WR_index1;
            } else {
                // switch to the first buffer 
                Sim800.pRX_Buffer = Sim800.RX_Buffer1;
                Sim800.RX_WR_index1 = 0;
                Sim800.pRX_WR_index = &(Sim800.RX_WR_index1);
                Sim800.pReadyBuffer = Sim800.RX_Buffer2;
                Sim800.pReadyIndex = &Sim800.RX_WR_index2;
            }
            F_push(SIM800_ParseAnswers);
        }
    }
    if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE) != RESET) {
        if (__HAL_UART_GET_IT_SOURCE(&huart2, UART_IT_RXNE) != RESET) {
            tmpval = huart2.Instance->RDR;

            Sim800.pRX_Buffer[(*Sim800.pRX_WR_index)] = (uint8_t) tmpval;
            *Sim800.pRX_WR_index += 1;
            // protection
            if (*(Sim800.pRX_WR_index) == RX_BUFFER_SIZE) {
                *(Sim800.pRX_WR_index) = 0;
            }
            tmpval = huart2.Instance->RDR; // Clear RXNE bit
            UNUSED(tmpval);
            __HAL_UART_CLEAR_FLAG(&huart2, UART_FLAG_RXNE);
            __HAL_UART_SEND_REQ(&huart2, UART_RXDATA_FLUSH_REQUEST);

        }
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    RX_Buffer2[rx_wr_index2++] = USART2->RDR & (uint8_t) 0x00FF;
    if (rx_wr_index2 == RX_BUFFER_SIZE)
        rx_wr_index2 = 0;
}

void serial_clear(UART_HandleTypeDef * usart) {
    __HAL_UART_CLEAR_IT(usart, UART_FLAG_TC);
    __HAL_UART_SEND_REQ(usart, UART_RXDATA_FLUSH_REQUEST);
}
