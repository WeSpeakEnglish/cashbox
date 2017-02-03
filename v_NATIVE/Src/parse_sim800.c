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

uint8_t Ready = 0;

Result ResParse;


volatile uint16_t SIM800_ParserID = 0;

void SIM800_ParserOK(void){
  if(strstr((char const *)Sim800.pReadyBuffer, msg_ok_end_str)){
  ResParse.bits.OK = 1;
 
  }

}

void parse_Download(void){
  if(strstr((char const *)Sim800.pReadyBuffer, download_str)){
  ResParse.bits.OK = 1;
 
  }

}

void parse_CREG( void )
{
	char* tmpstr;
        
        tmpstr = strstr((char const *)Sim800.pReadyBuffer, ",");
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
  
    longitude = strstr((char const *)Sim800.pReadyBuffer, gsmloc_str);
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

void SIM800_parse_PhoneNumber(void){
 char* tmpstr = NULL;
 uint16_t i;
 uint16_t index = 0;
 tmpstr = strstr((char const *)Sim800.pReadyBuffer, "\"");
  if(tmpstr != NULL)
   for(i = (uint16_t)(tmpstr - (char*)Sim800.pReadyBuffer) + 1; i < (*Sim800.pReadyIndex); i+=4 ){
     if(Sim800.pReadyBuffer[i] == '\"'){
       Sim800.phone_number[index]= '\0';
       break;
     }
     
     if(Sim800.pReadyBuffer[i] == '0')
       if(Sim800.pReadyBuffer[i+1] == '0')
         if(Sim800.pReadyBuffer[i+2] == '3')
            {
             Sim800.phone_number[index++] = Sim800.pReadyBuffer[i+3];
           }     
     
     }
        
        
  return;
  
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
      case 4:
         SIM800_parse_PhoneNumber();
        break;
      case 5:
         parse_Download();
        break;
      case 6:
        //SIM800_pop_washing();
        break;
    
    }
    Sim800.parsed = 1;
  //  xSemaphoreGive(xSemaphoreParse); 
  }

}

