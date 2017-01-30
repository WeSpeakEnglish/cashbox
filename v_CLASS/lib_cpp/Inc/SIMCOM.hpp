#ifndef _SIMCOM_H
#define _SIMCOM_H

#include "bitOperations.h"
#include <stdint.h>
#include <string>
#include "usart.h"

// #define SIM800       Serial2 // RXD2, TXD2
// #define SIM800_BAUD  9600


#define TERMINAL_UID	9


#define SIMCOM_DTR_PORT GPIOC

#define SIMCOM_PWR_PORT GPIOA

#define SIMCOM_DTR_PIN  GPIO_PIN_3 // PH3
#define SIMCOM_PWR_PIN  GPIO_PIN_4 // PH2

#define MSG_MAX_SIZE  		512
// #define MSG_SIZE_MASK 		63
#define QUEUE_MAX_SIZE  	32	//24
#define MAX_HTTP_BODY_SIZE 	128 //100
#define MAX_LOCATION_SIZE	12

const char    msg_ok_end_str[] = "\nOK\r\n"; //	"\nOK\r\n" won't work in some casese: OK\r\r
const char msg_error_end_str[] = "\nERROR\r\n"; //	"\nERROR\r\n"
const char      download_str[] = "DOWNLOAD";
const char    httpaction_str[] = "+HTTPACTION:";
const char           rdy_str[] = "RDY";
const char     sms_ready_str[] = "SMS Ready";
const char    call_ready_str[] = "Call Ready";
const char  sms_arrived1_str[] = "+CMTI: \"SM\",";
const char  sms_arrived2_str[] = "+CMTI: \"ME\",";
const char  ussd_arrived_str[] = "+CUSD:";
const char           csq_str[] = "+CSQ:";
const char        gsmloc_str[] = "+CIPGSMLOC:";
const char      httpinit_str[] = "AT+HTTPINIT";
const char      http_cid_str[] = "AT+HTTPPARA=\"CID\",1";
const char      http_url_str[] = "AT+HTTPPARA=\"URL\",\"";
const char  http_content_str[] = "AT+HTTPPARA=\"CONTENT\",\"application/x-www-form-urlencoded;charset=UTF-8\"";
// const char  http_content_str[] PROGMEM = "AT+HTTPPARA=\"CONTENT\",\"application/x-www-form-urlencoded;\"";
const char      http_act_str[] = "AT+HTTPACTION=";
const char     http_data_str[] = "AT+HTTPDATA=";
const char     http_term_str[] = "AT+HTTPTERM";
    
////////////////////////////
//char unsolicited_codes_0[] =
//char unsolicited_codes_1[] ={"SMS Ready"};

// const char          cmgf_str[] PROGMEM = "AT+CMGF=1";
// const char          cscs_str[] PROGMEM = "AT+CSCS=\"GSM\"";
// const char          cmgs_str[] PROGMEM = "AT+CMGS=\"";
// const char          ussd_str[] PROGMEM = "AT+USSD=1,\"";

#define MSG_OK_END_6(buf) 		strcpy(buf, msg_ok_end_str)
#define MSG_ERROR_END_9(buf) 	strcpy(buf, msg_error_end_str)
#define MSG_DOWNLOAD_9(buf)		strcpy(buf, download_str)
/* SIMCOM UNSOLICITED RESPONSES */
#define _HTTPACTION_13(buf)		strcpy(buf, httpaction_str)
#define _RDY_4(buf)			strcpy(buf, rdy_str)
#define _SMS_READY_10(buf)		strcpy(buf, sms_ready_str)
#define _CALL_READY_11(buf)		strcpy(buf, call_ready_str)
#define _SMS_ARRIVED1_13(buf)	strcpy(buf, sms_arrived1_str)
#define _SMS_ARRIVED2_13(buf) 	strcpy(buf, sms_arrived2_str)
#define _USSD_ARRIVED_7(buf)	strcpy(buf, ussd_arrived_str)
#define _CSQ_6(buf)		strcpy(buf, csq_str)
#define _GSMLOC_12(buf)		strcpy(buf, gsmloc_str)
#define _HTTPINIT_12(buf)   	strcpy(buf, httpinit_str)
#define _HTTP_CID_20(buf)   	strcpy(buf, http_cid_str)
#define _HTTP_URL_20(buf)   	strcpy(buf, http_url_str)
#define _HTTP_CONTENT_72(buf)  	strcpy(buf, http_content_str)
#define _HTTP_ACTION_15(buf)	strcpy(buf, http_act_str)
#define _HTTP_DATA_13(buf)	strcpy(buf, http_data_str)
#define _HTTP_TERM_12(buf)	strcpy(buf, http_term_str)

#define UPLOAD_READY		2
#define UPLOAD_STARTED		1
#define UPLOAD_NOT_READY 	0
//#define DEBUG Serial

//HTTP command status
#define EMPTY 				0
#define WAITING 			1
#define SENDING				2
#define WAITING_FOR_REPLY		3

#define INFO_UPLOAD_PERIOD		300 //seconds
typedef enum {GET, POST, HEAD} HTTP_Method;    	// 'keys' for HTTP functions
typedef void ( *p_parse_func_t )( void );

typedef struct {
	char text[80];
	char mess_end[16];
	p_parse_func_t p_parse_func;
	uint8_t empty;
} Command_t;									//command for queue handler

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

const char       vendweb_data[] = "http://vendweb.ru/api/0.1/data";
const char    vendweb_command[] = "http://vendweb.ru/api/0.1/commands";
const char vendweb_collection[] = "http://vendweb.ru/api/0.1/collection";
const char    vendweb_washing[] = "http://vendweb.ru/api/0.1/washing";

// //HTTP URLS
#define VENDWEB_DATA_31(buf)		strcpy(buf, vendweb_data)
#define VENDWEB_COMMANDS_34(buf)	strcpy(buf, vendweb_command)
#define VENDWEB_COLLECTION_37(buf)	strcpy(buf, vendweb_collection)
#define VENDWEB_WASHING_35(buf)		strcpy(buf, vendweb_washing)

class SIMCOM
{
public:
	
	SIMCOM( UART_HandleTypeDef& _serial, uint32_t baudrate );

	void init( void );
	void reset(void);
	void powerON(void);
	void powerOFF(void);
	void sleepON(void);
	void sleepOFF(void);
	void reset_netework(void);
	string sendCommand( string command );
	void is_connected( void );
	string sendServiceInfo( void );
	void submitHTTPRequest( HTTP_Method method, char* p_http_link, char* p_post_body);
	
	void getSignalQuality  ( void );
	void getMobileNumber   ( void );
	void getCurrentBalance ( void );
	void getGSMCoordinates ( void );
	void parse_USSD ( char*);


	static void set_reset_state( void );
	static uint8_t network_reset;

	// make private in release
	static char recv_string[MSG_MAX_SIZE];
	Command_t comm_queue[QUEUE_MAX_SIZE];

	// Command queue handling
	uint8_t sim_ready;//when 0, launch queue_handler
	uint8_t wash_until_update;//= 5;
	static uint8_t retry;
	void queue_init();
	void queue_push( Command_t command );
	Command_t* queue_pop();
	uint8_t queue_is_empty();	
	void queue_handler( void );
	void run_next_cmd_from_queue( void );
	void queue_command( char *text_str, char *msg_end, p_parse_func_t func);
	void retry_command();
	static void Show( void );

	//washing stack
	static uint8_t wash_index;
	void push_washing( washing_holder );
	static washing_holder pop_washing( void );
	uint8_t washings_available( void );

	// SIMCOM module response parsers
	void parse_unsolicitedCodes ( void );
	void parse_AT_HTTPACTION    (  void );
	static void dummy_parser( void );	
	static uint8_t get_recv_HTTP_size( char* );
	static void launch_HTTPACTION( void );
	static void parse_CREG( void );
	static void parse_CGREG( void );
	static void free_command(Command_t *command);

	//SMS
	void sendSMS( char *reciever_number, char* message_body);
	static void parseSMS( void );
	static void parse_Balance( char* message);
	static void parse_ForceStart(char* message);
	static uint8_t clear_sms;
	static uint8_t clear_http;

	//USSD
	void send_USSD(char *ussd_code);
	uint8_t comm_index;// = 0;

	static money current_balance;
	
	//phone
	static char phone_number[13];

	//signal quality
	static uint8_t signal_quality;
	static void parse_SignalQuality( void );

	//location
	static location current_location;
	static void parse_Location( char* );

	//HTTP commands handling
	static uint8_t upload_info_stat;
	static uint8_t upload_init_info_stat;
	static uint8_t check_price_stat;
	static uint8_t collection_stat;
	static uint8_t washing_stat;

	//HTTP upload info
	void prepare_info( void );
	void prepare_init_info( void );
	void info_upload( void );
	void init_info_upload( void );

	void check_price_upload(void);
	void washing_upload(void);
	void collection_upload(void);

	static void confirm_washing( void );
	//HTTP commands parse
	static void parse_check_price(void);

	//for usage:
	void submit_washing(uint8_t w_num, uint16_t cost);
	void submit_collection(void);
	static void retry_washing();
	Command_t *cmd_waiting;
private:

	uint32_t sim_baudrate;
	UART_HandleTypeDef& sim_uart;
	char id_str[6];

	uint8_t upload_info_timer;  //=0;
	uint8_t queue_write_index;  // = 0;
	uint8_t queue_read_index;   // = 0;
	
	
	
	uint16_t recv_i;    //=0;	// receive buffer index	
	// char *unsolicited_codes[3]={"RDY","Call Ready","SMS Ready"};
        
	char * unsolicited_codes[2]; //={"Call Ready","SMS Ready"};

	//HTTP request handling
	static uint8_t http_parts_left;
	static char http_body[MAX_HTTP_BODY_SIZE];
	static void load_http_body_part( void );	
};
#endif

