#include "http.h"
#include "simcom.h"
#include <string.h>
#include <stdio.h>
#include "calculations.h"

const char      httpinit_str[] = "AT+HTTPINIT\r";
const char      http_cid_str[] = "AT+HTTPPARA=\"CID\",1\r";
const char      http_url_str[] = "AT+HTTPPARA=\"URL\",\"\r";
const char  http_content_str[] = "AT+HTTPPARA=\"CONTENT\",\"application/x-www-form-urlencoded;charset=UTF-8\"\r";
// const char  http_content_str[] PROGMEM = "AT+HTTPPARA=\"CONTENT\",\"application/x-www-form-urlencoded;\"";
const char      http_act_str[] = "AT+HTTPACTION=\r";
const char     http_data_str[] = "AT+HTTPDATA=\r";
const char     http_term_str[] = "AT+HTTPTERM\r";

void submitHTTPRequest( HTTP_Method method, char* http_link, char* post_body){


    char http_link_buf[80];

    //char msg[9];
   
      SIM800_AddCMD((char *)httpinit_str,sizeof(httpinit_str),0);
      while (Sim800.parsed == 0){
       vTaskDelay(100);
       taskYIELD();
       }


    SIM800_AddCMD((char *)http_cid_str,sizeof(http_cid_str),0);
      while (Sim800.parsed == 0){
       vTaskDelay(100);
       taskYIELD();
       }
    strcpy(http_link_buf, http_url_str);
    SIM800_AddCMD((char *)http_url_str,sizeof(http_url_str),0);
      while (Sim800.parsed == 0){
       vTaskDelay(100);
       taskYIELD();
       }
	strcat(http_link_buf,http_link);
	strcat(http_link_buf,"\"\r");
    SIM800_AddCMD((char *)http_link_buf,sizeof(http_url_str),0);
      while (Sim800.parsed == 0){
       vTaskDelay(100);
       taskYIELD();
       }

    SIM800_AddCMD((char *)http_content_str,sizeof(http_content_str),0);
      while (Sim800.parsed == 0){
       vTaskDelay(100);
       taskYIELD();
       }
    
    switch ( method )
    {
        case GET:
            strcpy(http_link_buf, http_act_str);
            strcat(http_link_buf,"0\r");
            SIM800_AddCMD((char *)http_link_buf,strlen(http_link_buf),0);
             while (Sim800.parsed == 0){
               vTaskDelay(100);
               taskYIELD();
               }
            break;

        case POST:
            strcpy(http_link_buf, http_data_str);
              
            Itoa(strlen(post_body), http_link_buf + strlen(http_link_buf));
            strcat(http_link_buf, ",");
            sprintf( http_link_buf + strlen(http_link_buf), "%ld", 1200000 );
            // itoa(120000 ,http_link_buf + strlen(http_link_buf),10);

           // MSG_DOWNLOAD_9(msg);
            SIM800_AddCMD((char *)http_link_buf,strlen(http_link_buf),0);
             while (Sim800.parsed == 0){
               vTaskDelay(100);
               taskYIELD();
               }
             
              // MSG_OK_END_6(msg);
           // queue_command(post_body, msg, NULL);
            SIM800_AddCMD((char *)post_body,strlen(post_body),1);
             while (Sim800.parsed == 0){
               vTaskDelay(100);
               taskYIELD();
               }
            
            strcpy(http_link_buf, http_act_str);
            strcat(http_link_buf,"1\r");
            //DEBUG.println(http_link_buf);
            if(strstr(post_body,"wm")!=NULL)
            {
         //       DEBUG.println("wm_snd");
                SIM800_AddCMD((char *)http_link_buf,strlen(http_link_buf),6);
            }
            else
                SIM800_AddCMD((char *)http_link_buf,strlen(http_link_buf),0);
          
            while (Sim800.parsed == 0){
               vTaskDelay(100);
               taskYIELD();
               }
            // _HTTP_TERM_12(http_link_buf);
            // queue_command(http_link_buf, msg, NULL);
            break;

        case HEAD:
            strcpy(http_link_buf, http_act_str);
            strcat(http_link_buf,"2\r");
            SIM800_AddCMD((char *)http_link_buf,strlen(http_link_buf),0);
             while (Sim800.parsed == 0){
               vTaskDelay(100);
               taskYIELD();
               }
            break;

        default:
            break;
    }
    // _SAPBR_CLOSE_13(http_link_buf);
    // queue_command(http_link_buf, msg, NULL);

    //this->queue_command("AT+SAPBR=0,1", MSG_OK_END, NULL, NULL); // disable bearer
    
} 

