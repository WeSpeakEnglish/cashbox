#include "SIMCOM.hpp"
#include "TIMEOUT.h"
#include "cmsis_os.h"
#include <stdlib.h>
#include "calculations.h"
#include "usart.h"


char inicode_0[] = {"Call Ready"};
char inicode_1[] = {"SMS Ready"};

static wash_holder washings[16];

char SIMCOM::recv_string[MSG_MAX_SIZE];
char SIMCOM::http_body[MAX_HTTP_BODY_SIZE];	
uint8_t SIMCOM::http_parts_left;
money SIMCOM::current_balance;
uint8_t SIMCOM::signal_quality = 255; //?
location SIMCOM::current_location;
char SIMCOM::phone_number[13];

uint8_t SIMCOM::clear_sms = 0;
uint8_t SIMCOM::clear_http = 0;

uint8_t SIMCOM::retry = 0;
uint8_t SIMCOM::upload_info_stat=EMPTY;
uint8_t SIMCOM::check_price_stat=EMPTY;
uint8_t SIMCOM::collection_stat=EMPTY;
uint8_t SIMCOM::washing_stat = EMPTY;
uint8_t SIMCOM::upload_init_info_stat = SENDING;
uint8_t SIMCOM::wash_index = 0;
uint8_t SIMCOM::network_reset;


SIMCOM::SIMCOM( UART_HandleTypeDef& _serial, uint32_t baudrate ):sim_uart( _serial )
{
    uint8_t i;

    
    wash_until_update = 5; //initial values
    comm_index = 0;
    upload_info_timer =0;
    queue_write_index = 0;
    queue_read_index = 0;
 //   uint16_t recv_i = 0;
    unsolicited_codes[0] = inicode_0;
    unsolicited_codes[1] = inicode_1;
  //  unsolicited_codes[1] = {"SMS Ready"};

    
    /* Set Terminal UID */
    sprintf( id_str, "id=%d", TERMINAL_UID );
    memset(phone_number, 0, sizeof(phone_number));
	/* Set object parameters */
	sim_baudrate = baudrate;
        _serial.Init.BaudRate = sim_baudrate;
	//sim_uart//.begin(sim_baudrate);
    current_location.latitude[0] = '\0';
    current_location.longitude[0] = '\0';

    for(i = 0; i < 8; i++)
    {
        washings[i].cost = 0;
    }
    
    TIMEOUT_Init();
    TIMEOUT_Start(INFO_UPLOAD_PERIOD,0);
}
/*
 * Initiate SIMCOM SIM800 Module
 */

const char contype_str[] = "AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"";
const char apn_str[] = "AT+SAPBR=3,1,\"APN\",\"internet\"";
const char user_str[] = "AT+SAPBR=3,1,\"USER\",\"gdata\"";
const char pwd_str[] = "AT+SAPBR=3,1,\"PWD\",\"gdata\"";
const char sapbr_str[] = "AT+SAPBR=1,1";
//const char sapbr_close_str[] = "AT+SAPBR=0,1";
const char cgatt_str[] = "AT+CGATT?";

#define _CONTYPE_30(buf) strcpy(buf, contype_str)
#define _APN_30(buf)    strcpy(buf, apn_str)
#define _USER_30(buf)   strcpy(buf, user_str)
#define _PWD_27(buf)    strcpy(buf, pwd_str)
#define _SAPBR_13(buf)  strcpy(buf, sapbr_str)
#define _SAPBR_CLOSE_13(buf) strcpy(buf, sapbr_close_str)
#define _CGATT_10(buf)  strcpy(buf, cgatt_str)

void SIMCOM::init( void )
{
    network_reset = 0;
    sim_ready = 2;//call ready, sms ready
    char msg[6];
    MSG_OK_END_6(msg);

    char comm_buffer[30];
	this->queue_init();
   // this->powerOFF();
    this->powerON();
  //  this->reset();
  //  this->sleepOFF();
    this->is_connected();
    // GPRS Config
    _CONTYPE_30(comm_buffer);
	this->queue_command(comm_buffer, msg, NULL);
    _APN_30(comm_buffer);
	this->queue_command(comm_buffer, msg, NULL);
    _USER_30(comm_buffer);
	this->queue_command(comm_buffer, msg, NULL);
    _PWD_27(comm_buffer);
	this->queue_command(comm_buffer, msg, NULL);
    _SAPBR_13(comm_buffer);
    this->queue_command(comm_buffer, msg, NULL); // Enable bearer
    _CGATT_10(comm_buffer);
	this->queue_command(comm_buffer, msg, NULL);
    prepare_init_info();

}
/*
 * Wait for reply for the command from SIMCOM module and return it
 * returns the reply (String object)
 */
 
 void SIMCOM::reset_netework(void)
 {
    TIMEOUT_Stop();
    init();
    memset(this->recv_string, 0, sizeof(this->recv_string));
    this->recv_i = 0;
 }
 void SIMCOM::set_reset_state(void)
 {
    network_reset = 1;
 }
/*
 * Turn SIMCOM Power ON
 */
void SIMCOM::powerON(void)
{
    //    pinMode(A0, OUTPUT);//gsm modem gnd transistor pin
    //    digitalWrite(A0, HIGH);//turned on
    //    delay(5000);
    //#ifdef DEBUG_ENABLED
    //    DEBUG.println(F("SIMCOM Power ON"));
    //#endif
  
  HAL_GPIO_WritePin(SIMCOM_PWR_PORT, SIMCOM_PWR_PIN, GPIO_PIN_SET);
  HAL_Delay(5000);
  HAL_GPIO_WritePin(SIMCOM_PWR_PORT, SIMCOM_PWR_PIN, GPIO_PIN_RESET);
}

/*
 * Turn SIM800 Power OFF
 */
void SIMCOM::powerOFF(void)
{
    //    pinMode(A0, OUTPUT);//gsm modem gnd transistor pin
    //    digitalWrite(A0, LOW);//turned off
    //    delay(5000);
    //#ifdef DEBUG_ENABLED
    //    DEBUG.println(F("SIMCOM Power OFF"));
    //#endif
    
 // HAL_GPIO_WritePin(SIMCOM_PWR_PORT, SIMCOM_PWR_PIN, GPIO_PIN_SET);
 // HAL_Delay(1200);
 // HAL_GPIO_WritePin(SIMCOM_PWR_PORT, SIMCOM_PWR_PIN, GPIO_PIN_RESET);
}
/*
 * Put SIM800 in Sleep Mode by pulling DTR line HIGH
 */

void SIMCOM::sleepON(void)
{
  //  SET_BIT_(SIMCOM_DTR_DDR,  SIMCOM_DTR_PIN);
  //  SET_BIT_(SIMCOM_DTR_PORT, SIMCOM_DTR_PIN);
//  HAL_GPIO_WritePin(SIMCOM_DTR_PORT, SIMCOM_DTR_PIN, GPIO_PIN_SET);
}
/*
 * Put SIM800 out of Sleep Mode by pulling DTR line LOW
 */
void SIMCOM::sleepOFF(void)
{
 //   SET_BIT_(SIMCOM_DTR_DDR,  SIMCOM_DTR_PIN);
 //   CLR_BIT_(SIMCOM_DTR_PORT, SIMCOM_DTR_PIN); 
//  HAL_GPIO_WritePin(SIMCOM_DTR_PORT, SIMCOM_DTR_PIN, GPIO_PIN_RESET);
 //   osDelay(100);
}
void SIMCOM::reset(void)
{
 //   SET_BIT_(SIMCOM_RST_DDR,  SIMCOM_RST_PIN);
 //   CLR_BIT_(SIMCOM_RST_PORT, SIMCOM_RST_PIN);
    HAL_GPIO_WritePin(SIMCOM_PWR_PORT, SIMCOM_PWR_PIN, GPIO_PIN_RESET);
    osDelay(500);
    //SET_BIT(SIMCOM_PWR_PORT, SIMCOM_PWR_PIN);
    HAL_GPIO_WritePin(SIMCOM_PWR_PORT, SIMCOM_PWR_PIN, GPIO_PIN_SET);
    osDelay(3000);
}


const char creg_str[] = "AT+CREG?";
const char cgreg_str[] = "AT+CGREG?";
#define _CREG_9(buf) strcpy(buf, creg_str)
#define _CGREG_10(buf) strcpy(buf, cgreg_str)
void SIMCOM::is_connected(void)
{
    char msg[6];
    char cmd[10];
    MSG_OK_END_6(msg);
    _CREG_9(cmd);
    this->queue_command(cmd, msg, SIMCOM::parse_CREG);
    _CGREG_10(cmd);
    this->queue_command(cmd, msg, SIMCOM::parse_CGREG);//gprs registrated?    
}

//WASH QUEUE
void SIMCOM::push_washing(washing_holder w_hldr)
{
    wash_index++;    
    washings[wash_index] = w_hldr;
  //  DEBUG.print("wash:");
  //  DEBUG.println(washings[wash_index].cost);
   // DEBUG.println(washings[wash_index].wm_id);
}

washing_holder SIMCOM::pop_washing()
{
  washing_holder RetVaue = washings[wash_index];
 //   DEBUG.print("ppw:");
    wash_index--;
    return RetVaue;
    //DEBUG.println(wash_index);
}

uint8_t SIMCOM::washings_available( void )
{
    if(wash_index > 0) return 1;
    return 0;
}
void SIMCOM::retry_washing( void )
{
    wash_index++;
}

void SIMCOM::init_info_upload()
{
    char post_body[64];
    
    char url[31];
    VENDWEB_DATA_31(url);
    memset(post_body,0,64);
    strcat(post_body,id_str);
    strcat(post_body,"&lat=");
    strcat(post_body, SIMCOM::current_location.latitude);
    strcat(post_body,"&lon=");
    strcat(post_body, SIMCOM::current_location.longitude);
    strcat(post_body,"&phone=");
    strcat(post_body, phone_number);
    upload_init_info_stat = SENDING;
    submitHTTPRequest(POST, url, post_body);
}

const char sig_str[]  = "&signal=";
const char bal_str[] = "&balance=";
#define _SIG_9(buf) strcpy(buf, sig_str)
#define _BAL_10(buf) strcpy(buf, bal_str)

void SIMCOM::info_upload()
{
    char post_body[35];
    char url[31];
    VENDWEB_DATA_31(url);
    memset(post_body,0,25);
    strcat(post_body,id_str);
    _SIG_9(post_body+strlen(post_body));
    Utoa((uint16_t)(SIMCOM::signal_quality), post_body+strlen(post_body));
    _BAL_10(post_body+strlen(post_body));
    Utoa((uint16_t)(SIMCOM::current_balance.rub), post_body+strlen(post_body));
    strcat(post_body,".");
    Utoa((uint16_t)(SIMCOM::current_balance.cop), post_body+strlen(post_body));
    
    submitHTTPRequest(POST, url, post_body);
}

void SIMCOM::check_price_upload(void)
{
    char url[64];
    VENDWEB_COMMANDS_34(url);
    submitHTTPRequest(POST, url, id_str);
}
void SIMCOM::submit_washing( uint8_t wm, uint16_t cost )
{
    washing_holder w_h;
    w_h.wm_id=wm;
    w_h.cost=cost;
    //DEBUG.print("w:");
   // DEBUG.print(wm);
   // DEBUG.print(",c:");
   // DEBUG.println(cost);
    push_washing(w_h);

    TIMEOUT_Stop();
    TIMEOUT_Start(INFO_UPLOAD_PERIOD/2,0);
}

void SIMCOM::submit_collection(void)
{
    collection_stat = WAITING;
}

const char wm_str[] = "&wm=";
const char cost_str[] = "&cost=";
const char wash_stat_str[]  = "&status=true";
#define _WM_5(buf)         strcpy(buf, wm_str)
#define _COST_7(buf)       strcpy(buf, cost_str)
#define _WASH_STAT_13(buf) strcpy(buf, wash_stat_str)

void SIMCOM::washing_upload(void)
{
    char post_body[32]="\0";
    char url[35];

 //   char buf[12];
    char msg[9];
    MSG_OK_END_6(msg);
    VENDWEB_WASHING_35(url);
    strcat(post_body,id_str);
    _WM_5(post_body+strlen(post_body));
    Utoa((uint16_t)washings[wash_index].wm_id, post_body+strlen(post_body));
    _COST_7(post_body+strlen(post_body));
    Utoa((uint16_t)washings[wash_index].cost, post_body+strlen(post_body));
    _WASH_STAT_13(post_body+strlen(post_body));


    submitHTTPRequest(POST, url, post_body);

}

void SIMCOM::confirm_washing( void )
{
    pop_washing();
}

void SIMCOM::collection_upload(void)
{
    char url[37];
    char msg[9];
    MSG_OK_END_6(msg);

    // _HTTP_TERM_12(buf);
    // queue_command(buf, msg, NULL);
    //DEBUG.println("collect");

    VENDWEB_COLLECTION_37(url);
    submitHTTPRequest(POST, url, id_str);
}
void SIMCOM::prepare_info()
{    
    getSignalQuality();    
    getCurrentBalance();
}

void SIMCOM::prepare_init_info()
{
    getGSMCoordinates();
    getMobileNumber();
}
/*
 * Send HTTP request
 * input arguments: enum HTTPMethod -- GET, POST or HEAD
 *                  String request -- request string of structure http://server_address:server_port/path
 *                  uint8_t  *http_status_code -- returned http code by pointer
 * returned value:  String -- server response body
 */
void SIMCOM::submitHTTPRequest(HTTP_Method method, char* http_link, char* post_body)
{

    char http_link_buf[78];

    char msg[9];
    MSG_OK_END_6(msg);

    // _HTTP_TERM_12(http_link_buf);
    // queue_command(http_link_buf, msg, NULL);

    _HTTPINIT_12(http_link_buf);
    queue_command(http_link_buf,  msg, NULL);

    _HTTP_CID_20(http_link_buf);
    queue_command(http_link_buf, msg, NULL);	
    
    _HTTP_URL_20(http_link_buf);
	strcat(http_link_buf,http_link);
	strcat(http_link_buf,"\"");
    queue_command(http_link_buf, msg, NULL);

    _HTTP_CONTENT_72(http_link_buf);//charset utf-8
    queue_command(http_link_buf, msg, NULL);
    
    switch ( method )
    {
        case GET:

            _HTTP_ACTION_15(http_link_buf);
            strcat(http_link_buf,"0");
            queue_command(http_link_buf, msg, NULL);
            break;

        case POST:
            _HTTP_DATA_13(http_link_buf);
            Itoa(strlen(post_body), http_link_buf + strlen(http_link_buf));
            strcat(http_link_buf, ",");
            sprintf( http_link_buf + strlen(http_link_buf), "%ld", 1200000 );
            // itoa(120000 ,http_link_buf + strlen(http_link_buf),10);

            MSG_DOWNLOAD_9(msg);
            queue_command(http_link_buf, msg, NULL);

            MSG_OK_END_6(msg);
            queue_command(post_body, msg, NULL);
            
            _HTTP_ACTION_15(http_link_buf);
            strcat(http_link_buf,"1");
            //DEBUG.println(http_link_buf);
            if(strstr(post_body,"wm")!=NULL)
            {
         //       DEBUG.println("wm_snd");
                queue_command(http_link_buf, msg, SIMCOM::confirm_washing);
            }
            else
                queue_command(http_link_buf, msg, NULL);

            // _HTTP_TERM_12(http_link_buf);
            // queue_command(http_link_buf, msg, NULL);
            break;

        case HEAD:
            _HTTP_ACTION_15(http_link_buf);
            strcat(http_link_buf,"2");
            queue_command(http_link_buf, msg, NULL);
            break;

        default:
            break;
    }
    // _SAPBR_CLOSE_13(http_link_buf);
    // queue_command(http_link_buf, msg, NULL);

    //this->queue_command("AT+SAPBR=0,1", MSG_OK_END, NULL, NULL); // disable bearer
    
}

const char    cmgf_str[] = "AT+CMGF=1";
const char    cmgs_str[] = "AT+CMGS=\"";
const char    cscs_str[] = "AT+CSCS=\"GSM\"";
#define _CMGF_10(buf)       strcpy(buf, cmgf_str)
#define _CMGS_10(buf)       strcpy(buf, cmgs_str)
#define _CSCS_14(buf)       strcpy(buf, cscs_str)


void SIMCOM::sendSMS( char* phone_number, char* message_body)
{
    //this->queue_command("AT+SAPBR=1,1", MSG_OK_END, NULL, NULL); // Enable bearer
    //Set SMS system into text mode
    char msg[6];
    char message[32];
    MSG_OK_END_6(msg);

    _CMGF_10(message);
    queue_command(message,msg,NULL);

    //Send SMS
    _CSCS_14(message);
    queue_command(message, msg, NULL);
    
    _CMGS_10(message);
    strcat(message,phone_number);
    strcat(message,"\"\n>");
    strcat(message,message_body);
    strcat(message,"\x1A");

    queue_command(message, msg, NULL); 
}
/*
 * Get current signal strength
 */

const char    acsq_str[] = "AT+CSQ";
#define _CSQ_7(buf)       strcpy(buf, acsq_str)

void SIMCOM::getSignalQuality(void)
{
    char msg[6];
    char cmd[7];
    _CSQ_7(cmd);
    MSG_OK_END_6(msg);
    queue_command(cmd, msg,SIMCOM::parse_SignalQuality);    
}


// const char    ussd_str[] PROGMEM = "AT+USSD=1,\"";
// #define _USSD_13(buf)       strcpy(buf, csq_str)
const char  cusd_str[] = "AT+CUSD=1,\"*205#\"";
#define _CUSD_20(buf)       strcpy(buf, cusd_str)
void SIMCOM::send_USSD(char *ussd_code)
{
    char msg[6];
    char cmd[20];
    MSG_OK_END_6(msg);

    // queue_command("AT", msg, NULL);
    // char ussd_buf[32]={"AT+USSD=1,\""};
    // strcat(ussd_buf,ussd_code);
    // strcat(ussd_buf,"\"");
    _CUSD_20(cmd);
    queue_command(cmd,msg,NULL);
}
/*
 * Get current balance
 */
const char balance_str[] = "000100\0B";
#define _BALANCE_CODE_10(buf)       strcpy(buf, balance_str)
void SIMCOM::getCurrentBalance(void)
{
    char bal_code[10];
    _BALANCE_CODE_10(bal_code);
    sendSMS(bal_code, bal_code);
}

void SIMCOM::getMobileNumber(void)
{
    send_USSD("*205#");
}
const char    gsmloc_snd_str[] = "AT+CIPGSMLOC=1,1";
#define _GSMLOC_SND_17(buf)       strcpy(buf, gsmloc_snd_str)
void SIMCOM::getGSMCoordinates(void)
{
    char msg[6];
    MSG_OK_END_6(msg);
    char cmd[17];
    _GSMLOC_SND_17(cmd);
    queue_command(cmd,msg,( p_parse_func_t )SIMCOM::parse_Location);
}

//Print all characters in recv_string
void SIMCOM::Show( void )
{
    //Shows all symbols in recv_string
    int i;

    Time_t* tm = TIMEOUT_GetTime();
    //DEBUG.print(tm->minutes); 
   // DEBUG.print(":");
   // DEBUG.println(tm->seconds);    
  //  DEBUG.print("rc:[");
    for(i=0; i<MSG_MAX_SIZE; i++)
    {
        if(SIMCOM::recv_string[i] == '\r')
        {
  //          DEBUG.print("\\r");
            continue;
        }
        if(SIMCOM::recv_string[i] == '\n')
        {
   //         DEBUG.print("\\n");
            continue;
        }
        if(SIMCOM::recv_string[i] != '\0')
        {
     //       DEBUG.write(SIMCOM::recv_string[i]);
        }
    }
  //  DEBUG.println("]");
  //  DEBUG.print("w");
  //  DEBUG.println(washing_stat);
  //  DEBUG.print("p");
  //  DEBUG.println(check_price_stat);
  //  DEBUG.print("c");
   // DEBUG.println(collection_stat);
   // DEBUG.print("i");
  //  DEBUG.println(upload_info_stat);
}
    
//==========QUEUE==========
    void SIMCOM::queue_init()//good
    {
        for(int i=0; i< QUEUE_MAX_SIZE; i++)
        {
            comm_queue[i].empty=1;
        }
        queue_write_index = 0;
        queue_read_index = 0;
    }

    uint8_t SIMCOM::queue_is_empty()//good
    {
    	for(int i=0; i< QUEUE_MAX_SIZE; i++)
        {
            if (comm_queue[i].empty!=1)
                return 0;
        }
        return 1;
    }

    void SIMCOM::queue_push(Command_t command)//
    {	
        if(this->comm_queue[this->queue_write_index].empty == 0)
        {
            network_reset = 1;
            return;
        }
        this->comm_queue[this->queue_write_index] = command;
        this->queue_write_index++;
    	this->queue_write_index %= QUEUE_MAX_SIZE;        
    }

    Command_t *SIMCOM::queue_pop()
    {
    	if ( queue_is_empty() )
    	{
    		return NULL;
    	}
    	Command_t *temp = &this->comm_queue[this->queue_read_index];
    	this->comm_queue[this->queue_read_index++].empty = 1;
    	this->queue_read_index %= QUEUE_MAX_SIZE;
    	return temp;
    	//retuns NULL if queue is empty
    	//retuns Next command if not empty
    }

    void SIMCOM::run_next_cmd_from_queue( void )
    {
        this->cmd_waiting = queue_pop();
        if(this->cmd_waiting==NULL)
            return;
       // sim_uart.println(this->cmd_waiting->text);
        HAL_UART_Transmit(&sim_uart, (uint8_t*) this->cmd_waiting->text, sizeOfstr(this->cmd_waiting->text) ,100);
    }

    void SIMCOM::queue_command( char *text_str, char *msg_end, p_parse_func_t p_f)
    { 
        Command_t command;
        strcpy(command.text,text_str);
        strcpy(command.mess_end, msg_end);
        command.p_parse_func = p_f;
        command.empty = 0;

        queue_push( command );
        if(network_reset == 0)
        {
  //          DEBUG.print("qu:");
   //         DEBUG.println(text_str);
        }
        else
        {
    //        DEBUG.println("NetReset");
        }
    }

    void SIMCOM::retry_command( void )
    {
        this->recv_i = 0;
        memset( SIMCOM::recv_string, 0, MSG_MAX_SIZE );
    //    sim_uart.println( this->cmd_waiting->text );
          HAL_UART_Transmit(&sim_uart, (uint8_t*) this->cmd_waiting->text, sizeOfstr(this->cmd_waiting->text) ,100);
    }

    void SIMCOM::free_command(Command_t *command)
    {
        memset(command->text,0,sizeof(command->text));
        memset(command->mess_end,0,sizeof(command->mess_end));
    }
    //--QUEUEUE_HANDLER--
        // 1)recieve text from sim (SIMCOM::recv_string)
        // 2)if new message appear, wait for (cmd_waiting->mess_end) to appear in recv_string
        // 3)if no message, check recv_string for unsolicited_codes


    const char    cmgd_str[] = "AT+CMGD=1,3";
    #define _CMGD_12(buf)       strcpy(buf, cmgd_str)
    void SIMCOM::queue_handler( void )
    {
        //handle http
        //EMPTY=0, WAITING=1, SENDING=2, WAITING_FOR_REPLY=3 
        if(network_reset == 1)
        {
            network_reset = 0;
            reset_netework();
        }
        char msg[9];
        char ReceivedSym;

        if(upload_init_info_stat==EMPTY)
        {
            if(washings_available() == 1 && washing_stat == EMPTY)
            {
                washing_stat = WAITING;
            }
            //washing_stat
            if(washing_stat==WAITING && check_price_stat<SENDING && collection_stat<SENDING && upload_info_stat<SENDING)
            {            
                washing_upload();
                washing_stat=SENDING;
                Show();
            }

            //CHECK PRICE
            if(check_price_stat==WAITING && washing_stat<SENDING && collection_stat<SENDING && upload_info_stat<SENDING)
            {
                check_price_upload();
                check_price_stat=SENDING;
                Show();
            }

            //collection_stat
            if(collection_stat==WAITING && washing_stat<SENDING && check_price_stat<SENDING && upload_info_stat<SENDING)
            {
                collection_upload();
                collection_stat=SENDING;
                Show();
            }

            // UPLOAD INFO
            if(upload_info_stat==WAITING && check_price_stat<SENDING && collection_stat<SENDING && washing_stat<SENDING)
            {

                if(phone_number[0]!=0)//default is 0
                {
                    info_upload();
                    
                    phone_number[0]=0;
                    upload_info_stat = SENDING;
                    Show();
                }
                else
                    upload_info_stat = EMPTY;

                if(phone_number[0]==0)
                    prepare_info();                                
            }

            if( TIMEOUT_HasRanOut())
            {
                TIMEOUT_Stop();
                
                if(upload_info_timer==0)
                {
                    if(upload_info_stat < WAITING)
                        upload_info_stat = WAITING;
                }

                if(check_price_stat < WAITING)
                    check_price_stat = WAITING;

                upload_info_timer++;
                upload_info_timer%=144;      //144
                TIMEOUT_Start(INFO_UPLOAD_PERIOD,0);
            }
        }
        
        if ( this->cmd_waiting == NULL )
            run_next_cmd_from_queue();

  //  	if ( sim_uart.available() == 0)
  //  		return;
        HAL_UART_Receive(&sim_uart, (uint8_t * )&ReceivedSym, 1, 10);
        SIMCOM::recv_string[ this->recv_i++ ] = ReceivedSym;
        parse_unsolicitedCodes();

        if(this->sim_ready != 0)//wait until device is ready
            return;

        if ( this->cmd_waiting == NULL )
            return;
    	
    	if ( strstr( SIMCOM::recv_string, this->cmd_waiting->mess_end ) != NULL )
    	{
    		if(this->cmd_waiting->p_parse_func!=NULL)
    		{
    			this->cmd_waiting->p_parse_func();

                if(clear_sms!=0)
                {
                    char cmd[12];
                    clear_sms = 0;
                    MSG_OK_END_6(msg);
                    _CMGD_12(cmd);
                    queue_command(cmd,msg,NULL);
                }


    		}
    	}
        // #if 0
        else
        {
            MSG_ERROR_END_9(msg);
            if(strstr(SIMCOM::recv_string, msg)==NULL)//retry
            {
                return;
            } 
            else
            {
        //        DEBUG.println("err");
            }
        }
        // #endif

        if(SIMCOM::retry == 1)
        {
            retry_command();
            SIMCOM::retry = 0;
            return;
        }

    //    DEBUG.print("cmd:");
    //    DEBUG.println(this->cmd_waiting->text);
     //   DEBUG.print("end:");
     //   DEBUG.println(this->cmd_waiting->mess_end);
     //   DEBUG.print("rcv:");
        Show();

        free_command(this->cmd_waiting);
        this->cmd_waiting=NULL;

    	memset( SIMCOM::recv_string, 0, MSG_MAX_SIZE );
        this->recv_i = 0;
    	// DEBUG.println("Buffer cleared.");    	
    }

const char httpread_str[] = "+HTTPREAD:";
#define _HTTPREAD_11(buf)       strcpy(buf, httpread_str)

    void SIMCOM::load_http_body_part( void )
    {
        char *body_p;
        //append http_body with recieved body part
        char msg[6];
        char httpread_buf[12];
        _HTTPREAD_11(httpread_buf);
        MSG_OK_END_6(msg);

        body_p = strstr(SIMCOM::recv_string, msg);
        *body_p = 0;

        body_p = strstr(SIMCOM::recv_string, httpread_buf);
        body_p = strstr(body_p,"\r\n")+2;    
        strcat(SIMCOM::http_body, body_p);

        if(SIMCOM::http_parts_left == 0)
        {
            //HTTP BODY PARSERS
            //ALWAYS assign empty to status!

            if(check_price_stat==WAITING_FOR_REPLY)
            {
                SIMCOM::parse_check_price();
                check_price_stat=EMPTY;
            }
            if(washing_stat == WAITING_FOR_REPLY)
            {
                washing_stat = EMPTY;
            }
            if(collection_stat == WAITING_FOR_REPLY)
            {
                collection_stat = EMPTY;
            }
            if(upload_info_stat == WAITING_FOR_REPLY)
            {
                upload_info_stat = EMPTY;
            }
            upload_init_info_stat = EMPTY;

            //clear http_body
            memset(SIMCOM::http_body, 0, sizeof(SIMCOM::http_body));
            Show();
            return;
        }

        SIMCOM::http_parts_left--;

    }

