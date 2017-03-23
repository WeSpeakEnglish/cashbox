#include "vend.h"
#include "statemachine.h"
#include "timer16.h"
#include "WASHERS.h"
#include "simcom.h"
#include "keyboard.h"
#include "LCDMenu.h"
#include "cctalk.h"
#include <string.h>
#include "core.h"
#include "sd_files.h"


#define RET_OK 0
#define RET_ERR 1
#define RET_ERR_1 2


//expiration time to return to ini state (milliseconds)
#define SELECT_WASHER_EXPIRATION                        20000
#define INSERT_FUNDS_EXPIRATION                         90000
#define START_WASHING_FAILED_EXPIRATION                 90000
#define WASHING_STARTED_SUCCESSFULLY_EXPIRATION         90000
#define SERVICE_MENU_EXPIRATION                         90000
#define SERVICE_MENU2_EXPIRATION                        90000
#define SERVICE_INFO1_EXPIRATION                        90000



volatile uint32_t simple_timeout_tmp = 0;


uint8_t CheckState(void){ // set the timer to exit, it returns time expiration (yes/no)as well 
  static SessionState_t OldCondition = INITIALIZATION; // here is the start condition 
  static uint64_t ExpirationTime = 0;
  static uint8_t Expiration_Set_Flag; // setFlag - meant that exp time was set (for expirate only once)
  
  if(OldCondition != p_session->current_state){ //what we do on entrance to this section (once)
    OldCondition = p_session->current_state; 
    switch(p_session->current_state ){ //and set the timer to exit
      
    case  SELECT_WASHER:
      Expiration_Set_Flag = 1;
      ExpirationTime = millis() + SELECT_WASHER_EXPIRATION;
        break;
    case  INSERT_FUNDS:
      Expiration_Set_Flag = 1;
      ExpirationTime = millis() + INSERT_FUNDS_EXPIRATION;
        break;   
    case  START_WASHING_FAILED:
      Expiration_Set_Flag = 1;
      ExpirationTime = millis() + START_WASHING_FAILED_EXPIRATION;
        break;       
    case  WASHING_STARTED_SUCCESSFULLY:
      Expiration_Set_Flag = 1;
      ExpirationTime = millis() + WASHING_STARTED_SUCCESSFULLY_EXPIRATION;
        break;        
    case  SERVICE_MENU:
      Expiration_Set_Flag = 1;
      ExpirationTime = millis() + SERVICE_MENU_EXPIRATION;
        break;    
    
    case  SERVICE_MENU2:
      Expiration_Set_Flag = 1;
      ExpirationTime = millis() + SERVICE_MENU2_EXPIRATION;
        break;       
    case  SERVICE_INFO1:
      Expiration_Set_Flag = 1;
      ExpirationTime = millis() + SERVICE_INFO1_EXPIRATION;
        break;          
    } 
  }
  else{
    if(Expiration_Set_Flag){
      if(millis() > ExpirationTime){
        Expiration_Set_Flag = 0;
        return 1; // means expiration
      }
    }
  }
  return 0;
}

void loop(void)
{
   static uint8_t i;
   static uint32_t InsertFundsFlag = 0; // we set it if we need to open receiver
   
        // poll all machines (which ones in use)        
        for( i = 0; i < WASHERS_MAX_COUNT; ++i )
            p_session->washers_in_use[ i ] = ( WASHER_ReadFeedback( i + 1 ) ) ? 1 : 0;

        // process tmp substates
        if ( (p_session->current_substate == TMP_SUBSTATE) || \
          (p_session->current_substate == TMP_SUBSTATE2) )
        {
            if (simpleTimeoutHasRunOut(&simple_timeout_tmp, p_session->tmp_substate_timeout))
                enter_substate(NO_SUBSTATE);
        }

       // state machine
        if((InsertFundsFlag == 1) && 
           (p_session->current_state != INSERT_FUNDS)){
              InsertFundsFlag = 0;
              disableCashInput();
           }
        switch ( p_session->current_state )
        {
            // initialization
            case INITIALIZATION: // must not be
                doAlmostNothing(WAIT_FOR_START); break;
            // user interface
            case WAIT_FOR_START:
                waitForSession(); break;
            case SELECT_WASHER:
              if(CheckState()){
                p_session->current_state = WAIT_FOR_START;
              }
                waitForSelection(); break;
            case INSERT_FUNDS:
              if(CheckState()){
                p_session->current_state = WAIT_FOR_START;
                
              }
                InsertFundsFlag =1;
                waitForFunds(); break;
            case START_WASHING:
                //inactivity_period_ms = 0;
                checkIfWashingWasStarted(); break;
            case START_WASHING_FAILED:
             if(CheckState()){
                p_session->current_state = WAIT_FOR_START;
              }
                doAlmostNothing(SELECT_WASHER); break;
            case WASHING_STARTED_SUCCESSFULLY:
              if(CheckState()){
                p_session->current_state = WAIT_FOR_START;
              }
                doAlmostNothing(WAIT_FOR_START); break;
            // service menu interface
            case ENTER_PASSWORD:
                waitForPassword(); break;
            case SERVICE_MENU:
              if(CheckState()){
                p_session->current_state = WAIT_FOR_START;
              }
                serviceMenu(); break;
            case SET_NEW_PASSWORD:
             //   inactivity_period_ms = 0;
                setNewPassword(); break;
            case SERVICE_MENU2:
              if(CheckState()){
                p_session->current_state = WAIT_FOR_START;
              }
                serviceMenu2(); break;
            case SERVICE_INFO1:
              if(CheckState()){
                p_session->current_state = WAIT_FOR_START;
              }
             //   inactivity_period_ms = (90 * 1000L); // another timeout set in on_enter (by TMP_SUBSTATE)
                doAlmostNothing(SERVICE_MENU2); break;
            default: break;
        }
        
          LCDMenu_Update();

}

int8_t switch_state(SessionState_t to)
{

    // check if already in needed state
    if (to == p_session->current_state) return RET_OK;
    // run on_exit_state method for the current state
    if (on_exit_state() != RET_OK)
    {
        // something went wrong
        // try to enter current_state again
        return RET_ERR_1 | on_enter_state(p_session->current_state);
    }
    // run on_enter_state method for the next state
    return on_enter_state(to); // if RET_OK current_state is switched in this func
}

void enter_substate(SessionSubState_t to)
{
    if ( (to == TMP_SUBSTATE) || (to == TMP_SUBSTATE2) ) simpleTimeoutInit(&simple_timeout_tmp);
    p_session->current_substate = to;
}

int8_t on_enter_state(SessionState_t to)
{
    int8_t success = 0;

    // do something on enter
    switch (to)
    {
        case INSERT_FUNDS:
            enableCashInput();
            success = 1; break;
        case START_WASHING:
            p_session->tmp_substate_timeout = (2500L); // ms
            enter_substate(TMP_SUBSTATE); // a little delay...
            // trying to activate the washer
            WASHER_SendStartSignal( p_session->selected_washer, 1); // the second number is regime
            p_session->inserted_funds -= washers_list[ p_session->selected_washer - 1 ].price;
            success = 1; break;
        case START_WASHING_FAILED:
            p_session->tmp_substate_timeout = (5000L); // ms
            enter_substate(TMP_SUBSTATE); // delay
            p_session->inserted_funds += washers_list[ p_session->selected_washer - 1 ].price;
            success = 1; break;
        case WASHING_STARTED_SUCCESSFULLY:
            p_session->tmp_substate_timeout = (12 * 1000L); // ms
            enter_substate(TMP_SUBSTATE); // delay
            success = 1; break;
        case SERVICE_INFO1:
            p_session->tmp_substate_timeout = (30 * 1000L); // ms
            enter_substate(TMP_SUBSTATE); // delay
            success = 1; break;
        default: success = 1; break;
    }
    if (success)
    {
        p_session->current_state = to;
        return RET_OK;
    }
    return RET_ERR;
}

int8_t on_exit_state(void)
{
    int8_t success = 0;
    // reset substate on any exit
    enter_substate(NO_SUBSTATE);
    // do something on exit
    switch (p_session->current_state)
    {
        case INSERT_FUNDS:
           success = 1; break;
        default: success = 1; break;
    }
    return success ? RET_OK : RET_ERR;
}

void waitForSession(void)
{
    char c;


    c = readKey();


    if ( c == '#' )
    {
        // In this place there is no need to update EEPROM,
        // because no funds are inserted yet
        switch_state(SELECT_WASHER);
    }
    else if ( c == '*' )
    {
        switch_state(ENTER_PASSWORD);
    }
}


void waitForSelection(void)
{
    char c;
    uint8_t washer_num;
    
    c = readKey();

    washer_num = c - 0x30;


    if ( IS_WASHER_VALID(washer_num) )
        p_session->selected_washer = washer_num;
  
   
    if ( c == '#' )
    {
      if(p_session->selected_washer) {
        // selected washer is not in use then go on with cash inserted
        if ( (p_session->washers_in_use[ p_session->selected_washer - 1 ] == 0) )
        {
            switch_state(INSERT_FUNDS);
        }
        else
        {
            // goto substate where we say "this washer is already in use"
            p_session->tmp_substate_timeout = (5000L);
            enter_substate(TMP_SUBSTATE);
        }
      }
    }
    else if ( c == '*' )
    {
        if (p_session->current_substate != NO_SUBSTATE) enter_substate(NO_SUBSTATE);
        else switch_state(WAIT_FOR_START);
    }
}

void waitForFunds(void)
{
    char c;

    c = readKey();

        enter_substate(NO_SUBSTATE);
        if(ccTalk.TransferFlag){
         if (!ccTalk.gotMoney) {
            ccTalkSendCMD(CC_REQ_ACC_COUNT);
            ccTalkParseAccCount();
         } 
         else {
            ccTalkSendCMD(CC_READBUFFEREDBILL);
            ccTalkParseStatus();
         }
         ccTalk.TransferFlag = 0;
        }

    if ( p_session->inserted_funds >= washers_list[p_session->selected_washer - 1].price )
    {

       // switch to START_WASHING state
        
        switch_state(START_WASHING);
        return;
    }
    else if ( c == '*' )
    {
        if (1) // cancel anyway! why not?
        {
            switch_state(SELECT_WASHER);
            
            return;
        }
    }
}

void checkIfWashingWasStarted(void)
{
 
    // wait for washer to start (about 2.5 s for sure)
    if (p_session->current_substate == TMP_SUBSTATE) return; // just wait a little...

    // wait for user to read that washing was started successfully
    if (p_session->current_substate == TMP_SUBSTATE2) return; // just wait a little...

    // check washer feedback

    if (WaitForStartWasher(p_session->selected_washer))
    {
    p_session->washers_in_use[ p_session->selected_washer - 1 ] = \
      ( WASHER_ReadFeedback( p_session->selected_washer ) ) ? 1 : 0;
        // increment clients count and save to eeprom

        VendSession_RAMIncrementClientsCount(p_session->selected_washer);

        S_push(SIM800_pop_washing);   
        SD_SetSession();
        switch_state(WASHING_STARTED_SUCCESSFULLY);
    //    M_push
    }
    else
    {
        // if washer was not started then go to fail-state for a while
        // (and then back to washer selection)
        switch_state(START_WASHING_FAILED);
    }
}

void doAlmostNothing(SessionState_t after)
{
    char c;
    c = readKey();
    if
    (
        ( (c >= '0') && (c <= '9') )
    ||
        ( c == '#' )
    ||
        ( c == '*' )
    ||
        ( p_session->current_substate == NO_SUBSTATE )
    )
        switch_state(after);
}

void waitForPassword(void)
{
    char c;
    static uint8_t pwd_position = 0;
    char *pwd_input_buffer, *pwd_eemem_buffer;
    
    pwd_input_buffer = VendSession_GetTypedPwd();    

    c = readKey();

    if ( (c >= '0') && (c <= '9') )
    {
        pwd_input_buffer[pwd_position++] = c;
        if ( pwd_position == VendSession_PwdSize )
        {
            pwd_eemem_buffer = VendSession_GetPwd();
            if (memcmp(pwd_input_buffer, pwd_eemem_buffer, VendSession_PwdSize) == 0)
                switch_state(SERVICE_MENU);
            else
                switch_state(WAIT_FOR_START);
            
            pwd_position = 0;
            memset(pwd_input_buffer, ' ', VendSession_PwdSize); // set symbols back to spaces
            return;
        }
    }
    else if ( c == '*' )
    {
        pwd_position = 0;
        memset(pwd_input_buffer, ' ', VendSession_PwdSize); // set symbols back to spaces
        switch_state(WAIT_FOR_START);
    }
    else if ( c == '#' )
    {
        // secret combination for all devices
        if ( (pwd_position == 5) && (memcmp((char *)pwd_input_buffer, "12242", 5) == 0) )
        {
            // show password and type it (user need only retype the last pwd symbol)
            strncpy( (char *)pwd_input_buffer, (char *)VendSession_GetPwd(), VendSession_PwdSize );
        }
    }
}

void setNewPassword(void)
{
    char c, i;

    static uint8_t pwd_position = 0;
    static uint8_t repeat = 0;
    char *pwd_input_buffer;
    static char pwd_copy[VendSession_PwdSize + 1];


    pwd_input_buffer = VendSession_GetTypedPwd();    

    c = readKey();

    if ( p_session->current_substate != NO_SUBSTATE ) return;

    if ( (c >= '0') && (c <= '9') )
    {
        pwd_input_buffer[pwd_position++] = c;
        if ( pwd_position == VendSession_PwdSize )
        {
            if (repeat == 0)
            {
                //---
                repeat = 1;
                pwd_position = 0;
                // store pwd_input_buffer to pwd_copy
                for(i=0; i < VendSession_PwdSize; i++)
                  pwd_copy[i] = pwd_input_buffer[i];
   
                // reset input buffer
                memset(pwd_input_buffer, '*', VendSession_PwdSize); // "hide" password
            }
            else
            {
                //---
                repeat = 0;
                pwd_position = 0;
                // compare input passwords
                if (memcmp(pwd_input_buffer, pwd_copy, VendSession_PwdSize) == 0)
                {
                    // save new password to eeprom
                    VendSession_UpdPwd(pwd_copy);
                    // reset input password
                    memset(pwd_input_buffer, ' ', VendSession_PwdSize);
                    // switch back to service menu
                    switch_state(SERVICE_MENU);
                }
                else
                {
                    // wrong password, do not save to eeprom, let user enter password again
                    memset(pwd_input_buffer, ' ', VendSession_PwdSize);
                }
            }
        }
    }
    else if ( c == '*' )
    {
        pwd_position = 0;
        repeat = 0;
        memset(pwd_input_buffer, ' ', VendSession_PwdSize); // set symbols back to spaces
        switch_state(SERVICE_MENU);
    }
}

void serviceMenu(void)
{
    char c;
    uint8_t washer_num = 0;

    c = readKey();

    washer_num = c - 0x30; // 1...WASHERS_MAX_COUNT

    
    switch( c )
    {
        //case '#': switch_state(SET_NEW_PASSWORD); break;
        case '*': switch_state(WAIT_FOR_START); break;
        case '#': switch_state(SERVICE_MENU2); break;
        case '0': {
            S_push(SIM800_collection);
            CashBOX = 0;
            SD_SetSession();
            switch_state(WAIT_FOR_START);
        }; break;
        default: break;
    }

    if ( ( washer_num >= 1 ) && ( washer_num <= WASHERS_MAX_COUNT ) )
    {
        WASHER_SendStartSignal( washer_num, 1); // the second number is regime
    }
}

void serviceMenu2(void)
{
    char c;

    c = readKey();

    switch( c )
    {
        case '4': switch_state(SET_NEW_PASSWORD); break;
        case '*': switch_state(WAIT_FOR_START); break;
        case '9': switch_state(SERVICE_MENU); break;
        case '5': switch_state(SERVICE_INFO1); break; // DBG
        case '6': p_session->inserted_funds = 0; break; // DBG
        case '0': 
          break; 
        default: break;
    }
}

char readKey(void)
{
  char ret_c = 0;
    if (Keyboard.keyReady) {
              //  lcd_putch(Keyboard.keyCode);
                Keyboard.keyReady = 0;
                ret_c = Keyboard.keyCode;
            }
  return ret_c;
}