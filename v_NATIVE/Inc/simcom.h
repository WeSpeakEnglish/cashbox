#ifndef _SIMCOM_H
#define _SIMCOM_H
#include <stdint.h>
#include "dma.h"
#include "cmsis_os.h"

#define RX_BUFFER_SIZE 256
#define SIMCOM_PWR_PORT GPIOA
#define SIMCOM_PWR_PIN  GPIO_PIN_4

#define CMD_Queue_size  16
#define CMD_Length      128 

#define MAX_HTTP_BODY_SIZE 	128 //100
#define MAX_LOCATION_SIZE	12

 typedef struct 
{
	int16_t rub;
	uint8_t cop;
}money;

typedef struct
{
	char latitude[MAX_LOCATION_SIZE];
	char longitude[MAX_LOCATION_SIZE];
}location;

typedef struct washing_holder
{
	uint8_t cost;
	uint8_t wm_id;
}wash_holder;

typedef struct {
 uint8_t powered : 1;  // is power on or off
 volatile uint8_t RX_Buffer1[RX_BUFFER_SIZE];  // double buffered RX
 volatile uint8_t RX_Buffer2[RX_BUFFER_SIZE];   
 volatile uint8_t * pRX_Buffer;   // point to current RX buffer
 volatile uint16_t * pRX_WR_index; // index of current RX buffer
 volatile uint16_t RX_WR_index1;  // how much of elements have been written to the first buffer
 volatile uint16_t RX_WR_index2;  // how much of elements have been written to the second buffer
 volatile uint8_t  bufferIsReady; // buffer is ready to parse 
 volatile uint8_t * pReadyBuffer;  // pointer to buffer which is ready to parse
 volatile uint16_t * pReadyIndex;
 // --------- Vars
 volatile uint8_t sim_ready;//when 0, launch queue_handler
 volatile uint8_t wash_until_update;//= 5;
 volatile uint8_t retry;
 volatile uint8_t clear_sms;
 volatile uint8_t clear_http;
 volatile uint8_t comm_index;// = 0;
 volatile uint8_t signal_quality;
 volatile money current_balance;
 volatile location current_location;
 volatile uint8_t upload_info_stat;
 volatile uint8_t upload_init_info_stat;
 volatile uint8_t check_price_stat;
 volatile uint8_t collection_stat;
 volatile uint8_t washing_stat;
 
}SIM800;

typedef struct {
  char * pMessage;
  uint16_t SizeOfMessage;
  uint16_t ParserID;        // id of parser for answer
}Message;

extern SIM800 Sim800;
extern xQueueHandle SIM800_CommandsQ;
extern SemaphoreHandle_t xSemaphore;

void SIM800_IniCMD(void);
void SIM800_SendCMD(void);
void SIM800_Ini(void);
void SIM800_PowerOnOff(void);
uint32_t SIM800_AddCMD(char * Msg, uint16_t Length, uint16_t ParserID);
#endif