#include "SIMCOM.hpp"
#include "WASHERS.h"
#include "VendSession.h"



void SIMCOM::parse_check_price(void)
{
    //format: wm1=150,0;wm2=100,0OK
	char wm_num[4];
    char price_buf[4];
    char state_buf[2];

//	volatile VendSession_t *p_session;
	WasherSettings_t *washers_list;

    // DEBUG.println( "Started parsing costs" );

//	p_session = VendSession_RAMGetSession();
	washers_list = WASHERS_RAMGetAll();
    //check if http body correct
    if(strstr(SIMCOM::http_body,"wm")==NULL)
        return;
       
    for(int i = 0; i < WASHERS_MAX_COUNT; i++)
    {
        memset( wm_num,    0, 4 );
        memset( price_buf, 0, 4 );
        memset( state_buf, 0, 2 );

        // DEBUG.println( "Iteration" );
        // DEBUG.println( i );

    	sprintf( wm_num, "wm%d", i + 1 );
	    char *wm_index_p = strstr(SIMCOM::http_body, wm_num);

    	if (wm_index_p == NULL)
    	{
            // DEBUG.println( "wm:" );
            // DEBUG.println( wm_num );
    		//in use - can be used
    		// p_session->washers_in_use[ i ] = 1;//if force stat from sms, or price == 0
    		washers_list[ i ].price = 0;//no sms
    		continue;
        }

        // find start and end of the price and calculate price length in bytes
	    char *wm_price_start_p = strstr(wm_index_p, "=") + 1;
        char *wm_price_end_p   = strstr(wm_index_p, ",");
        uint16_t price_len = wm_price_end_p - wm_price_start_p;
        // copy these 'len' characters to buffer
        strncpy( price_buf, wm_price_start_p,   price_len );
        strncpy( state_buf, wm_price_end_p + 1, 1 );
        // DEBUG.println( price_buf );
        // DEBUG.println( atoi( price_buf ) );

        washers_list[ i ].price = atoi( price_buf );
        // DEBUG.println("=====");
        // DEBUG.println( atoi( state_buf ) );
        // DEBUG.println("=====");
        switch( atoi( state_buf ) )
        {
            case 0 : break;
            case 1 : {
                WASHER_SendStartSignal( i + 1 );
                // p_session->washers_in_use[ i ] = 1;
            }; break;
            default : break;
        }
    }
    // WASHERS_EEMEMUpdateAll();
    // VendSession_EEMEMUpdateAll();
}

void SIMCOM::parse_unsolicitedCodes( void )
{
    char msg[6];
    char str_1[13];
    char str_2[13];
    char *unsol_string;
    MSG_OK_END_6(msg);
	
    //System codes
	for(int i=0;i<2;i++)
	{
		unsol_string = strstr(SIMCOM::recv_string, this->unsolicited_codes[i]);
		if(unsol_string!=NULL)//found unsolicited in recv_string
		{
            this->sim_ready--;
            //DEBUG.print("unsol: ");
          //  DEBUG.println(unsol_string);

			memset(unsol_string,0,strlen(unsol_string));//remove unsolicited
            this->recv_i = unsol_string - SIMCOM::recv_string;//move recv_i back
		}
	}
    //SMS codes
    
    _SMS_ARRIVED1_13(str_1);
    _SMS_ARRIVED2_13(str_2);

    if(strstr(SIMCOM::recv_string, str_1)!=NULL || 
        strstr(SIMCOM::recv_string, str_2)!=NULL)
    {
        //2 types of sms notification
        unsol_string = strstr(SIMCOM::recv_string, str_1);

        if(unsol_string==NULL)
            unsol_string = strstr(SIMCOM::recv_string, str_2);

        if(strstr(unsol_string,"\r\n")==NULL)
            return;

        char *sms_index = strstr(unsol_string,",");
        sms_index++;
        char recv_sms_command[16] = "AT+CMGR=";
        strcat(recv_sms_command, sms_index);
        queue_command(recv_sms_command, msg, NULL);
        queue_command(recv_sms_command, msg, SIMCOM::parseSMS);
        //remove message after parsing

        memset(unsol_string,0,strlen(unsol_string));//remove unsolicited
        this->recv_i = unsol_string - SIMCOM::recv_string; //move recv_i back
    }
    //USSD codes
    _USSD_ARRIVED_7(str_1);
    if ( strstr(SIMCOM::recv_string, str_1) != NULL )
    {
        unsol_string = strstr(SIMCOM::recv_string, str_1);
        //check if USSD body loaded
        if(strstr(unsol_string,"\r\n")==NULL)
            return;

        //save what needs to be removed
        uint16_t remove_count = strlen(unsol_string);

        char *USSD_body;
        USSD_body = strstr(unsol_string,"\"")+1;

        *(strstr(USSD_body,"\""))='\0';//separate body from rest of the message
        parse_USSD(USSD_body);

        memset( unsol_string, 0, remove_count);
        this->recv_i = unsol_string - SIMCOM::recv_string; //move recv_i back
    }
    //HTTP codes
    _HTTPACTION_13(str_1);
    if ( strstr( SIMCOM::recv_string, str_1 ) != NULL )
    {
        unsol_string = strstr(SIMCOM::recv_string, str_1);
        // 1)check if HTTP body loaded
        if(strstr(unsol_string,"\r\n")==NULL)
            return;
        //save what needs to be removed            
        uint16_t remove_count = strlen(unsol_string);

        //char *action_type = strstr(SIMCOM::recv_string, ":")+1;
        SIMCOM::recv_i = strstr(unsol_string,":") - SIMCOM::recv_string+1;
        parse_AT_HTTPACTION( );

        memset( unsol_string, 0, remove_count);
        this->recv_i = unsol_string - SIMCOM::recv_string;
    }
}


#define _HTTPREAD_18(buf)        strcpy(buf, httpread_str)
const char    httpread_str[] = "AT+HTTPREAD=0,200";

void SIMCOM::parse_AT_HTTPACTION()
{
   //char http_stat[4]={0};
    char msg[6];
    MSG_OK_END_6(msg);
    char *HTTPACTION_args = SIMCOM::recv_string + SIMCOM::recv_i + 1;//<Method>,<StatusCode>,<DataLen>
   	if(strstr(strstr(HTTPACTION_args,","),"200")==NULL)
   	{   
    //    DEBUG.print("phf:");
    //    DEBUG.println(HTTPACTION_args);
   		//this->reset_netework();
    	//RETRY HTTP request
    	if(check_price_stat > WAITING)
    		check_price_stat = WAITING;

    	if(washing_stat > WAITING)
    		washing_stat = WAITING;

    	if(collection_stat > WAITING)
    		collection_stat = WAITING;

    	if(upload_info_stat > WAITING)
    		upload_info_stat = WAITING;

    	return;
	}

    uint8_t body_size = SIMCOM::get_recv_HTTP_size(SIMCOM::recv_string + recv_i + 1);

    char http_buf[18];
    _HTTPREAD_18(http_buf);
    //SIM800 ring buffer size = 200
    switch(*HTTPACTION_args)//points 
    {
        case '0'://GET                
            SIMCOM::http_parts_left = 0; 
            if(body_size==0)
            {
                check_price_stat=EMPTY;
            }
            if(body_size>0)
                queue_command(http_buf, msg, SIMCOM::load_http_body_part);
            break;

        case '1'://POST
            SIMCOM::http_parts_left = 0;
            if(body_size==0)
            {
                check_price_stat=EMPTY;
            }
            if(body_size>0)
                queue_command(http_buf,msg, SIMCOM::load_http_body_part);

            if(check_price_stat==SENDING)
            {
                check_price_stat=WAITING_FOR_REPLY;
            }

            if(upload_info_stat==SENDING)
            {
                upload_info_stat=WAITING_FOR_REPLY;
            }


            if(collection_stat==SENDING)
            {
                collection_stat=WAITING_FOR_REPLY;
            }

            if(washing_stat==SENDING)
            {
                washing_stat=WAITING_FOR_REPLY;
            }

            break;
        case '2'://HEAD

            break;
    }

    _HTTP_TERM_12(http_buf);
    queue_command(http_buf, msg, NULL);

}

uint8_t SIMCOM::get_recv_HTTP_size( char *str )
{
    char convert_to_int[5];//rename
    memset(convert_to_int,0,sizeof(convert_to_int));
    uint8_t http_body_size = 0;
    char *p = strstr(str,",")+1;
    char *p2 = strstr(p,",")+1;
    *strstr(p2,"\r")='\0';
    http_body_size = atoi(p2);
    return http_body_size;
}

void SIMCOM::parse_CREG( void )
{
	char* tmpstr = strstr(SIMCOM::recv_string, ",");
	// if(tmpstr!=-1)
    if(tmpstr!=NULL)
	{
		if(*(tmpstr+1)!='1')
		{
			SIMCOM::retry = 1;
		}
	}
}

void SIMCOM::parse_CGREG( void )
{
	char* tmpstr = strstr(SIMCOM::recv_string, ",");
	// if(tmpstr!=-1)
    if(tmpstr!=NULL)
	{
		if(*(tmpstr+1)!='1')
		{
			SIMCOM::retry = 1;
		}
	}
}


#define _PHONE_PARSE_13(buf)     strcpy(buf, phone_parse_str)
const char    phone_parse_str[] = "041204300448";

void SIMCOM::parse_USSD(char *USSD_body)
{
 //   DEBUG.print("ussd: ");
 //   DEBUG.println(USSD_body);
    char buf[13];
    _PHONE_PARSE_13(buf);
    if(strstr(USSD_body, buf)!=NULL) //041204300448 = "Ваш"
    {
        //parse phone number
    //    DEBUG.println("phn");
        USSD_body+=(strlen(USSD_body)-1);//set pointer to end
        for(int i = 10; i>=0;i--)
        {

            this->phone_number[i] = *USSD_body;
            //DEBUG.print(this->phone_number[i]);
            USSD_body-=4;
        }
    }
    if(this->phone_number[0] != 0 && current_location.latitude[0] != '\0' && current_location.longitude[0] != '\0')
    {
        init_info_upload();
    }
    else
    {
        prepare_init_info();
    }
}


void SIMCOM::parse_SignalQuality( void )
{
    //string format: +CSQ: quality,ber
    char csq_buf[6];
    _CSQ_6(csq_buf);
   // DEBUG.println("SIG_PTR:");

    char *sig_ptr = strstr(SIMCOM::recv_string, csq_buf);

    //DEBUG.println(sig_ptr);
    sig_ptr = strstr(sig_ptr," ")+1;

    //DEBUG.println(sig_ptr);
    *(strstr(SIMCOM::recv_string, ",")) = '\0';
    //DEBUG.println(sig_ptr);
    SIMCOM::signal_quality = atoi(sig_ptr) % 99;//set 0 if value = 99
  //  DEBUG.println(signal_quality);

    //2*(100-2*sig quality) in %

}

void SIMCOM::parse_Location(char *str )//possible optimisation (for)
{
    //string format: +CIPGSMLOC: 0,37.735329,55.785633,2016/11/10,13:19:54
    char gsmloc_buf[12];
    _GSMLOC_12(gsmloc_buf);
    char *longitude = strstr(SIMCOM::recv_string, gsmloc_buf);
    longitude = strstr(longitude,",")+1;//potints to longtitude
    char *latitude = strstr(longitude,",");

    *latitude++='\0';//cut longtitude from recv_string
    memset(SIMCOM::current_location.longitude,0,MAX_LOCATION_SIZE);
    strcpy(SIMCOM::current_location.longitude,longitude);

    *(strstr(latitude,","))='\0';//cut latitude from recv_string
    memset(SIMCOM::current_location.latitude,0,MAX_LOCATION_SIZE);
    strcpy(SIMCOM::current_location.latitude,latitude);
}

void SIMCOM::parseSMS( void )
{
    char msg[6];
    MSG_OK_END_6(msg);
    //Get raw sms body inside recv_string 
    //1) Remove MSG_OK ending
    char *sms_body = strstr(SIMCOM::recv_string, msg);
    sms_body = '\0';

    //2)find message body
    sms_body = strstr(SIMCOM::recv_string,"\"\r\n")+3;
    //DEBUG.print("sms:");
    //DEBUG.println(sms_body);
    //DEBUG.println(SIMCOM::recv_string);
    //3)parse sms
    if(strstr(sms_body,"wm")!=NULL||(*sms_body == 'w' && *(sms_body+1) == 'm'))
	{
		parse_ForceStart(sms_body);
	}

    if(strstr(sms_body,"0440")!=NULL)
    {
    	parse_Balance(sms_body);
    }
    if(strstr(sms_body,"4C07")!=NULL)
    {
        network_reset = 1;
    }
    
    SIMCOM::clear_sms = 1;


}
//SMS parsers

void SIMCOM::parse_Balance( char* message)
{

    money r_mon={0,0};
    char chr_money[64];
    message+=2;
    //DEBUG.println("W1");
    while(*message!='2' && *(message+1)!='E')
    {
        if(*message=='3')
        {
            r_mon.rub=r_mon.rub * 10 + *(message+1) - '0';
        }

        else if (strstr(message,"2D"))
        {
            r_mon.rub*=-1;
        }
        message+=4;
    }
    //DEBUG.println("W2");
    while(*message!='4'&&*(message+1)!='0')
    {
        if(*message=='3')
        {
            r_mon.cop=r_mon.cop * 10 + *(message + 1) - '0';
        }
        message+=4;
    }
    //DEBUG.println("Wnd");
    SIMCOM::current_balance = r_mon;
    
}

void SIMCOM::parse_ForceStart(char* message)
{
	char wm_num[4];
	wm_num[3] = '\0';

	volatile VendSession_t *p_session;
	WasherSettings_t *washers_list;

	p_session = VendSession_RAMGetSession();
	washers_list = WASHERS_RAMGetAll();

    for(int i = 0; i < WASHERS_MAX_COUNT; i++)
    {
        memset( wm_num, 0, 4 );
    	sprintf( wm_num, "wm%d", i + 1 );
	    char *wm_index_p = strstr(message, wm_num);
		char *wm_state_p = strstr(wm_index_p, "=");

        wm_state_p++;//points to state, sets line ending for wm_price
        
		if(*wm_state_p == '1')
        {
        	WASHER_SendStartSignal( i + 1 );
        	// p_session->washers_in_use[ i ] = 1;
        }
    }
    // WASHERS_EEMEMUpdateAll();
    // VendSession_EEMEMUpdateAll();
}