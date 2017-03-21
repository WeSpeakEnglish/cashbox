#include "parse_sim800.h"
#include "simcom.h"
#include <string.h>
#include "vend.h"
#include "sd_files.h"
#include "calculations.h"
#include "core.h"
#include "modbus.h"

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

void SIM800_Parse_CGAT(void){
  Sim800.CGATT_READY = 0;
  if(strstr((char const *)Sim800.pReadyBuffer,"CGATT: 1")){
    Sim800.CGATT_READY = 1;
  }
    
}

void parse_Location(void)//possible optimisation (for)
{
  uint8_t i;
  uint8_t CommaCount = 0; // we count commas in valid case for take the time and date
  date_time_t dt;
  uint32_t TempVar;
 // char String_To_parse[10]; // parse numbers
    //string format: +CIPGSMLOC: 0,37.735329,55.785633,2016/11/10,13:19:54
  char *longitude = NULL;
  char *latitude = NULL;
  
    longitude = strstr((char const *)Sim800.pReadyBuffer, gsmloc_str);
    if(longitude){
   // get the time and date
     for(i = 0; i < *(Sim800.pReadyIndex); i++ ){
       if(Sim800.pReadyBuffer[i] == ','){
         CommaCount++;
       
       switch(CommaCount){
          case  3:
            dt.day = 0;
            dt.hours = 0;
            dt.minutes = 0;
            dt.seconds = 0;
            dt.month = 0;
            dt.weekday = 0;
            dt.year = 0;
                GetDate((char *)(&Sim800.pReadyBuffer[i+1]), &dt);
               
                break;
          case  4:
                 GetTime((char *)(&Sim800.pReadyBuffer[i+1]), &dt);
                 TempVar = DateToUNIX(&dt);
                 DataTime = dt;
                 RTC_seconds = TempVar;
                break;         
        }
       }
      }     
    ////
 
    longitude = strstr(longitude,",")+1;//potints to longtitude
    latitude = strstr(longitude,",");

    if(latitude){  *latitude++='\0';//cut longtitude from recv_string
      memset((void *)Sim800.current_location.longitude,0,MAX_LOCATION_SIZE);
      strcpy((char *)Sim800.current_location.longitude,longitude);

    *(strstr(latitude,","))='\0';//cut latitude from recv_string
    memset((void *)Sim800.current_location.latitude,0,MAX_LOCATION_SIZE);
    strcpy((char *)Sim800.current_location.latitude,latitude);
    
    /// try to get the time and date in this case

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

void  SIM800_parse_Signal(void){  //parser of signal level
  char * pParse;
  uint16_t Signal;
  pParse = strstr((char const *)Sim800.pReadyBuffer, ",");
  if(pParse != NULL){
    pParse--;
    Signal = *(pParse--) - 0x30;
    if(*pParse != ' ')Signal += (*(pParse) - 0x30)*10;
  }
  if(Signal == 99) Signal = 0;
 Signal = (Signal * 100) / 31;
 Sim800.signal_quality = Signal;
return;
}

void  SIM800_parse_Balance(void){
  
 char* tmpstr = NULL;
 uint16_t i;
 uint16_t index = 0;
 tmpstr = strstr((char const *)Sim800.pReadyBuffer, "0440002E"); // rubles

 if(tmpstr != NULL){
   tmpstr -=4;
        if(tmpstr[0] == '0')
         if(tmpstr[1] == '0')
          if(tmpstr[2] == '3')
            {
             Sim800.current_balance.cop = tmpstr[3] - 0x30;
           } 
   tmpstr -=4;
        if(tmpstr[0] == '0')
         if(tmpstr[1] == '0')
          if(tmpstr[2] == '3')
            {
             Sim800.current_balance.cop += (tmpstr[3] - 0x30)*10;
           } 
   tmpstr -=8;
   index = 1;
   Sim800.current_balance.rub = 0;
   for(i = 0; i < 5; i++){
     
     
        if(tmpstr[0] == '0')
         if(tmpstr[1] == '0')
          if(tmpstr[2] == '3')
            {
             Sim800.current_balance.rub += (tmpstr[3] - 0x30)*index;
           }
        if(*(tmpstr-1) == '\"') break;
        index *=10;
        tmpstr -= 4;
   }
 }
 return;
}

void  SIM800_Parse_WM(void){
uint8_t wm;   
uint8_t    i = 0;
uint8_t    j = 0;
uint8_t i2 = 1;
uint8_t mul = 1;
uint8_t SetWM = 0;
uint8_t cleanFlag = 0; //wash away all the prices
volatile  uint8_t * str = Sim800.pReadyBuffer;
while(str[i] != '\0'){
  if(str[i++] == 'w')     
    if(str[i++] == 'm'){          
     wm = str[i] - 0x30 -1;
     if(!cleanFlag){
      cleanFlag = 1;
      for(j = 0; j < WASHERS_MAX_COUNT; j++){
        WL[j].enable = 0;
      }
     }
    }
  if(str[i] == ','){
    //WL[wm].start =str[i+1] - 0x30;
    if(str[i+1] - 0x30){
      //  Machine.SetCoil = 1;//wm + 1; 
      //  Machine.SlaveAddr = wm + 1;
      WASHER_SendStartSignal(wm + 1, str[i+1] - 0x30);
    } 
    WL[wm].price = 0;
    i2 = 1;  
    mul = 1;
  while((str[i-i2] < 0x3A)&&( i > i2)){     
     WL[wm].price += (str[i-i2] - 0x30)*mul;
      //    WL[wm].index = wm + 1; //existed indexes
     mul*=10;
     i2++;
   }      
   SetWM =1;
   WL[wm].enable = 1;
  }
 }
if(SetWM){
 if(CheckPriceChanges())SD_SetData();
 }
 return;
}

void   SIM800_ParseAnswers(void){ //parsers selector
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
        SIM800_parse_Signal();
        break;
      case 7:
        SIM800_Parse_CGAT();
        break;
      case 8:
        break;
    
    }
    Sim800.parsed = 1;
  }

}

