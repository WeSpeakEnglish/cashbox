#ifndef _SIMCOM_H
#define _SIMCOM_H
#include <stdint.h>
#include "dma.h"

#define RX_BUFFER_SIZE 256
#define SIMCOM_PWR_PORT GPIOA
#define SIMCOM_PWR_PIN  GPIO_PIN_4

#define CMD_Queue_size  16
#define CMD_Length      128 

#define MAX_HTTP_BODY_SIZE 	        128 //100
#define MAX_LOCATION_SIZE	        12
#define MAX_PHONE_NUMBER_LENGTH         30

#define TERMINAL_UID	8
#define MAX_WASHINGS    8

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

typedef struct 
{
	uint8_t cost;
	uint8_t wm_id;
}washing_holder;

extern washing_holder wash_holder;




typedef struct {
 uint8_t powered : 1;  // is power on or off
 volatile uint8_t parsed :  1;
 volatile uint8_t initialized : 1;
 volatile uint8_t flush_SMS : 1; // SMS is not expected
 volatile uint8_t SMS_received : 1;
 volatile uint8_t CGATT_READY: 1;
 volatile uint8_t gotInfoFromServer: 1;
 
 volatile uint8_t RX_Buffer1[RX_BUFFER_SIZE];  // double buffered RX
 volatile uint8_t RX_Buffer2[RX_BUFFER_SIZE];   
 volatile uint8_t http_body[MAX_HTTP_BODY_SIZE];
 volatile uint8_t phone_number[MAX_PHONE_NUMBER_LENGTH];
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



void SIM800_waitAnswer(uint8_t Cycles);
void SIM800_IniCMD(void); // farther commands to open gprs session
void SIM800_SendCMD(void); // the send CMD operation
void SIM800_Ini(void); // ini sequence
void SIM800_info_upload(void); //info upload
void SIM800_collection(void); //  collection

void SIM800_init_info_upload(void); //upload initialized info to the SERVER
void SIM800_PowerOnOff(void);
void SIM800_GPRS_open(void); // open GPRS session
void SIM800_GPRS_close(void); // close GPRS session
void SIM800_get_Balance(void);
void SIM800_get_Signal(void);
void SIM800_pop_washing(void);
void SIM800_command(void);
void SIM800_submit_washing( uint8_t wm, uint16_t cost ); // submit washing to the list
void parse_CGAT(void); 
uint32_t SIM800_AddCMD(char * Msg, uint16_t Length, uint16_t ParserID);
#endif