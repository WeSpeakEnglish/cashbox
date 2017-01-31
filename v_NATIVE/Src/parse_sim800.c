#include "parse_sim800.h"
#include "simcom.h"
#include <string.h>

//SemaphoreHandle_t xSemaphoreParse = NULL;

const char    msg_ok_end_str[] = "OK"; //	"\nOK\r\n" won't work in some casese: OK\r\r
const char msg_error_end_str[] = "ERROR"; //	"\nERROR\r\n"


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
uint8_t Ready = 0;

Result ResParse;


volatile uint16_t SIM800_ParserID = 0;

void SIM800_ParserOK(){
  if(strstr((char const *)Sim800.pReadyBuffer, msg_ok_end_str)){
  ResParse.bits.OK = 1;
 
  }

}

void parse_CREG( void )
{
	char* tmpstr;
        
        tmpstr = strstr((char const *)Sim800.pRX_Buffer, ",");
	// if(tmpstr!=-1)
    if(tmpstr!=NULL)
	{
		if(*(tmpstr+1)!='1')
		{
			Sim800.retry = 1;
		}
	}
}

void parse_Location(void)//possible optimisation (for)
{
    //string format: +CIPGSMLOC: 0,37.735329,55.785633,2016/11/10,13:19:54
  char *longitude = NULL;
  char *latitude = NULL;
  
    longitude = strstr((char const *)Sim800.pRX_Buffer, gsmloc_str);
    if(longitude){
    longitude = strstr(longitude,",")+1;//potints to longtitude
    latitude = strstr(longitude,",");

    if(latitude){  *latitude++='\0';//cut longtitude from recv_string
      memset((void *)Sim800.current_location.longitude,0,MAX_LOCATION_SIZE);
      strcpy((char *)Sim800.current_location.longitude,longitude);

    *(strstr(latitude,","))='\0';//cut latitude from recv_string
    memset((void *)Sim800.current_location.latitude,0,MAX_LOCATION_SIZE);
    strcpy((char *)Sim800.current_location.latitude,latitude);
    }
 }
} 

void   SIM800_ParseAnswers(void){
  if(Sim800.bufferIsReady == 1){
Sim800.pReadyBuffer[*(Sim800.pReadyIndex)] = '\0';
      switch(SIM800_ParserID){
      case 1:
        SIM800_ParserOK();
        break;
      case 2:
        parse_CREG();
        break;
      case 3:
        parse_Location();
        break;
    
    }
  //  xSemaphoreGive(xSemaphoreParse); 
  }

}

