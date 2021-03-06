#include "http.h"
#include "simcom.h"
#include <string.h>
#include <stdio.h>
#include "calculations.h"
#include "core.h"

const char      httpinit_str[] = "AT+HTTPINIT";
const char      http_cid_str[] = "AT+HTTPPARA=\"CID\",1";
const char      http_url_str[] = "AT+HTTPPARA=\"URL\",\"";
const char  http_content_str[] = "AT+HTTPPARA=\"CONTENT\",\"application/x-www-form-urlencoded;charset=UTF-8\"";
// const char  http_content_str[] PROGMEM = "AT+HTTPPARA=\"CONTENT\",\"application/x-www-form-urlencoded;\"";
const char      http_act_str[] = "AT+HTTPACTION=";
const char     http_data_str[] = "AT+HTTPDATA=";
const char     http_term_str[] = "AT+HTTPTERM";
const char     http_read_str[] = "AT+HTTPREAD";

const char GSM_ATcmd_Enable_Echo[] = "ATE1";

// enable command echo

void submitHTTP_init(void) {
    SIM800_AddCMD((char *) httpinit_str, sizeof (httpinit_str), 1);
    SIM800_waitAnswer(1);
};

void submitHTTP_terminate(void) {
    SIM800_AddCMD((char *) http_term_str, sizeof (http_term_str), 0);
    SIM800_waitAnswer(1);
};

void submitHTTPRequest(HTTP_Method method, char* http_link, char* post_body, char read) {
    char http_link_buf[80];
    
      SIM800_AddCMD((char *)http_cid_str,sizeof(http_cid_str), 0);
      SIM800_waitAnswer(1); 
      strcpy(http_link_buf, http_url_str);
      strcat(http_link_buf,http_link);
      strcat(http_link_buf,"\"\r");
      SIM800_AddCMD((char *)http_link_buf,sizeof(http_link_buf), 0);
      SIM800_waitAnswer(1); 
      SIM800_AddCMD((char *)http_content_str,sizeof(http_content_str), 0);
      SIM800_waitAnswer(1);
    
    switch ( method )
    {
        case GET:
            strcpy(http_link_buf, http_act_str);
            strcat(http_link_buf,"0\r");
            SIM800_AddCMD((char *)http_link_buf,strlen(http_link_buf), 0);
            SIM800_waitAnswer(1);
            break;

        case POST:
            strcpy(http_link_buf, http_data_str);
              
            Itoa(strlen(post_body), http_link_buf + strlen(http_link_buf));
            strcat(http_link_buf, ",");
            sprintf( http_link_buf + strlen(http_link_buf), "%ld", 1200000 );
            SIM800_AddCMD((char *)http_link_buf,strlen(http_link_buf), 0);
            SIM800_waitAnswer(1);
            SIM800_AddCMD((char *)post_body,strlen(post_body) + 1, 1);
            SIM800_waitAnswer(1);
            strcpy(http_link_buf, http_act_str);
            strcat(http_link_buf,"1");
            if(strstr(post_body,"wm")!=NULL)
            {
                SIM800_AddCMD((char *)http_link_buf,strlen(http_link_buf) + 1, 6);
            }
            else
                SIM800_AddCMD((char *)http_link_buf,strlen(http_link_buf) + 1, 0);
            break;

        case HEAD:
            strcpy(http_link_buf, http_act_str);
            strcat(http_link_buf,"2");
            SIM800_AddCMD((char *)http_link_buf,strlen(http_link_buf) + 1, 0);
            SIM800_waitAnswer(1);
            break;

        default:
            break;
    }
    
     SIM800_waitAnswer(2);
    if(read){
      SIM800_AddCMD((char *)http_read_str,sizeof(http_read_str), 0);
      SIM800_waitAnswer(1);
    }
}

 