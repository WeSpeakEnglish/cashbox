
#include "LCDMenu.h"

#include "VendSession.h"
#include "WASHERS.h"
#include "Vend.h"
#include <stdint.h>


// Abstraction display refresh event
// Write static char strings into progmem

// 20 symbols strings
// space filled string
const char space20_str[]  = "                    ";

// initialization
const char init_str0[]  = "   Laundry cube     ";
//
//
const char init_str3[]  = "  Инициализация...  ";

// welcome screen strings
//
const char welcome_str1[] = "    Здравствуйте!   ";
const char welcome_str2[] = "Нажмите # для старта";
//

// select washer screen strings
const char select_str0[] = "  Выберите  машину  ";
const char select_str1[] = "    и  нажмите #    ";
// payment screen strings
const char payment_str2[] = "Нажмите * для выбора";
const char payment_str3[] = "   другой машины    ";
// start washing screen strings

const char eating_str2[] = "  Идет прием купюр, ";
const char eating_str3[] = "    подождите...    ";

const char cannot_str0[] = "    Вы не можете    ";
const char cannot_str1[] = "   выбрать данную   ";
const char cannot_str2[] = "  машину, т.к. она  ";
const char cannot_str3[] = "  уже используется  ";

const char fail_str0[] = "  К сожалению, не   ";
const char fail_str1[] = " удалось  запустить ";
//                                  "машину _, попробуйте"
const char fail_str3[] = "   выбрать другую   ";

//                                  " Машина _ включена, "
const char success_str1[] = "   можете начать    ";
const char success_str2[] = "      стирку        ";
//

// enter to service menu screen strings
const char enter_service_str0[] = "  Сервисный  режим  ";
const char enter_service_str1[] = "   Введите пароль:  ";

const char set_new_pwd_str0[] = "   Введите новый    ";
const char set_new_pwd_str1[] = "  пароль два раза:  ";

const char service_str0[] = "  Сервисный  режим  ";
const char service_str1[] = " 0 Инкассация|9 Еще ";
const char service_str2[] = " 1-8 Старт машины   ";
const char service_str3[] = " *Выход|#Нов.пароль ";

const char exit_only_str[] = " * Выход            ";

const char service2_str0[] = "  Сервисный режим 2 ";
const char service2_str1[] = " 5 Информация       ";
const char service2_str2[] = "6 Обнул.текущ.баланс";
const char service2_str3[] = " * Выход | 9 Назад  ";


// // buffer string       
char lcd_line_buf[LCD_LINE_SZ<<3]; // *2 for a good reserve

// initialization
static void LCDMenu_Initialization(void);
// user menu
static void LCDMenu_WelcomeScreen    (void);
static void LCDMenu_WasherSelection  (void);
static void LCDMenu_AwaitingPayment  (void);
static void LCDMenu_StartWashing     (void);
static void LCDMenu_StartWashingFailed (void);
static void LCDMenu_WashingStartedSuccessfully (void);
// service menu
static void LCDMenu_EnterPassword    (void);
static void LCDMenu_SetNewPassword   (void);
static void LCDMenu_ServiceMenu      (void);
static void LCDMenu_ServiceMenu2     (void);
static void LCDMenu_ServiceInfo1     (void);

char *LCDGetLineAsString(int line_num) // for debug purposes
{
	strncpy( lcd_line_buf, getLCDLine(line_num), LCD_LINE_SZ );
	lcd_line_buf[LCD_LINE_SZ] = '\x0';
	return lcd_line_buf;
}

void LCDMenu_Init(void)
{
	lcd_init();
}

uint16_t LCDTextUpdate(void)
{
	uint8_t i;
	uint8_t may_be_continued = 0;
	uint16_t changes;
	uint16_t ret;
	ret = whichBlocksWereChanged();
	changes = ret;
	if (changes)
	{
		for (i = 0; i < LCD_BLOCKS; i++)
		{
			if ( (i % (LCD_BLOCKS/LCD_LINES))==0 ) may_be_continued = 0;
			if ((changes & 0x0001))
			{
				if (may_be_continued)
					//@@@ lcd_continue_block_writing((char *)(&(p_txt->ravel[LCD_BLOCK_SZ*i])));
					lcd_continue_block_writing(getLCDBlock(i));
				else
				{
					//@@@ lcd_write_block(i, (char *)(&(p_txt->ravel[LCD_BLOCK_SZ*i])));
					lcd_write_block(i, getLCDBlock(i));
					may_be_continued = 1;
				}
			}
			else
				may_be_continued = 0;
			//---
			changes >>= 1;
		}
	}
	return ret;
}

uint16_t LCDMenu_Update(void)
{

	switch (p_session->current_state)
	{
		// initialization
		case INITIALIZATION : LCDMenu_Initialization();   break;
		// user interface
		case WAIT_FOR_START : LCDMenu_WelcomeScreen();    break;
		case SELECT_WASHER  : LCDMenu_WasherSelection();  break;
		case INSERT_FUNDS   : LCDMenu_AwaitingPayment();  break;
		case START_WASHING	: LCDMenu_StartWashing();     break;
		case START_WASHING_FAILED: LCDMenu_StartWashingFailed(); break;
		case WASHING_STARTED_SUCCESSFULLY: LCDMenu_WashingStartedSuccessfully(); break;
		// service menu interface
		case ENTER_PASSWORD : LCDMenu_EnterPassword();    break;
		case SERVICE_MENU   : LCDMenu_ServiceMenu();      break;
		case SERVICE_MENU2  : LCDMenu_ServiceMenu2();     break;
		case SERVICE_INFO1  : LCDMenu_ServiceInfo1();     break;
		case SET_NEW_PASSWORD: LCDMenu_SetNewPassword();  break;

		default : break;
	}

	// update screen
	return LCDTextUpdate();
}

void LCDMenu_Initialization(void)
{
	CPY_LCD_LINE_FROM_PRGMEM(init_str0, 0);
	sprintf( lcd_line_buf, " Сборка %s           ", __DATE__);
	CPY_LCD_LINE_FROM_LINEBUF(lcd_line_buf, 1);
	sprintf( lcd_line_buf, "        %s           ", __TIME__);
	CPY_LCD_LINE_FROM_LINEBUF(lcd_line_buf, 2);
	CPY_LCD_LINE_FROM_PRGMEM(init_str3, 3);
}

void LCDMenu_WelcomeScreen(void)
{
	CPY_LCD_LINE_FROM_PRGMEM(space20_str, 0);
	CPY_LCD_LINE_FROM_PRGMEM(welcome_str1, 1);
	CPY_LCD_LINE_FROM_PRGMEM(welcome_str2, 2);
	CPY_LCD_LINE_FROM_PRGMEM(space20_str, 3);
}

void LCDMenu_WasherSelection(void){

	if (p_session->current_substate == NO_SUBSTATE)
	{
		CPY_LCD_LINE_FROM_PRGMEM(select_str0, 0);
		CPY_LCD_LINE_FROM_PRGMEM(select_str1, 1);

		sprintf( lcd_line_buf, "Машина: %1d (%3d руб.)",
				        		 p_session->selected_washer,
								 WL[(p_session->selected_washer)-1].price);
		CPY_LCD_LINE_FROM_LINEBUF(lcd_line_buf, 2);
		sprintf( lcd_line_buf, "Внесено: %3d руб.   ", p_session->inserted_funds );
		CPY_LCD_LINE_FROM_LINEBUF(lcd_line_buf, 3);
	}
	else
	{
		CPY_LCD_LINE_FROM_PRGMEM(cannot_str0, 0);
		CPY_LCD_LINE_FROM_PRGMEM(cannot_str1, 1);
		CPY_LCD_LINE_FROM_PRGMEM(cannot_str2, 2);
		CPY_LCD_LINE_FROM_PRGMEM(cannot_str3, 3);
	}
}

void LCDMenu_AwaitingPayment(void)
{

	sprintf( lcd_line_buf, "Машина: %1d (%3d руб.)",
							 p_session->selected_washer,
							 WL[(p_session->selected_washer)-1].price);
	CPY_LCD_LINE_FROM_LINEBUF(lcd_line_buf, 0);
	sprintf( lcd_line_buf, "Внесено: %3d руб.   ", p_session->inserted_funds );
	CPY_LCD_LINE_FROM_LINEBUF(lcd_line_buf, 1);

	if (p_session->current_substate == PROCESSING_SUBSTATE)
	{
		CPY_LCD_LINE_FROM_PRGMEM(eating_str2, 2);
		CPY_LCD_LINE_FROM_PRGMEM(eating_str3, 3);
	}
	else
	{
		CPY_LCD_LINE_FROM_PRGMEM(payment_str2, 2);
		CPY_LCD_LINE_FROM_PRGMEM(payment_str3, 3);
	}
}

void LCDMenu_StartWashing(void)
{
	CPY_LCD_LINE_FROM_PRGMEM(space20_str, 0);
	sprintf( lcd_line_buf, "Машина %1d включается,", p_session->selected_washer);
	CPY_LCD_LINE_FROM_LINEBUF(lcd_line_buf, 1);
	CPY_LCD_LINE_FROM_PRGMEM(eating_str3, 2);
	CPY_LCD_LINE_FROM_PRGMEM(space20_str, 3);
}

void LCDMenu_StartWashingFailed(void)
{
	CPY_LCD_LINE_FROM_PRGMEM(fail_str0, 0);
	CPY_LCD_LINE_FROM_PRGMEM(fail_str1, 1);
	sprintf( lcd_line_buf, "машину %1d, попробуйте", p_session->selected_washer);
	CPY_LCD_LINE_FROM_LINEBUF(lcd_line_buf, 2);
	CPY_LCD_LINE_FROM_PRGMEM(fail_str3, 3);
}

void LCDMenu_WashingStartedSuccessfully(void)
{
	sprintf( lcd_line_buf, " Машина %1d включена, ", p_session->selected_washer);
	CPY_LCD_LINE_FROM_LINEBUF(lcd_line_buf, 0);
	CPY_LCD_LINE_FROM_PRGMEM(success_str1, 1);
	CPY_LCD_LINE_FROM_PRGMEM(success_str2, 2);
	CPY_LCD_LINE_FROM_PRGMEM(space20_str, 3);
}

void LCDMenu_EnterPassword(void)
{
	char *pwd_buf = VendSession_GetTypedPwd();
	
	CPY_LCD_LINE_FROM_PRGMEM(enter_service_str0, 0);
	CPY_LCD_LINE_FROM_PRGMEM(enter_service_str1, 1);

	sprintf( lcd_line_buf, "      \"%6s\"      ", pwd_buf );
	CPY_LCD_LINE_FROM_LINEBUF(lcd_line_buf, 2);

	CPY_LCD_LINE_FROM_PRGMEM(exit_only_str, 3);
}

void LCDMenu_SetNewPassword(void)
{
	char *pwd_buf = VendSession_GetTypedPwd();

	CPY_LCD_LINE_FROM_PRGMEM(set_new_pwd_str0, 0);
	CPY_LCD_LINE_FROM_PRGMEM(set_new_pwd_str1, 1);
	sprintf( lcd_line_buf, "      \"%6s\"      ", pwd_buf );
	CPY_LCD_LINE_FROM_LINEBUF(lcd_line_buf, 2);
	CPY_LCD_LINE_FROM_PRGMEM(exit_only_str, 3);
}

void LCDMenu_ServiceMenu(void)
{
	CPY_LCD_LINE_FROM_PRGMEM(service_str0, 0);
	CPY_LCD_LINE_FROM_PRGMEM(service_str1, 1);
	CPY_LCD_LINE_FROM_PRGMEM(service_str2, 2);
	CPY_LCD_LINE_FROM_PRGMEM(service_str3, 3);
}

void LCDMenu_ServiceMenu2(void)
{
	CPY_LCD_LINE_FROM_PRGMEM(service2_str0, 0);
	CPY_LCD_LINE_FROM_PRGMEM(service2_str1, 1);
	CPY_LCD_LINE_FROM_PRGMEM(service2_str2, 2);
	CPY_LCD_LINE_FROM_PRGMEM(service2_str3, 3);
}

void LCDMenu_ServiceInfo1(void)
{
    volatile uint32_t *p_cashbox;
    volatile uint16_t *p_clients_count;
    char buffer_to_print[10] = {0};
    uint8_t i; // for indexing inside the cycles

    p_cashbox = &CashBOX;
    p_clients_count = VendSession_RAMGetClientsCount();
    strcpy(lcd_line_buf, " Цены x50: ");
    for(i = 0; i <  WASHERS_MAX_COUNT; i++){
      sprintf(buffer_to_print, "%d", WL[i].price / 50);
      strcat(lcd_line_buf, buffer_to_print);
    }
	CPY_LCD_LINE_FROM_LINEBUF(lcd_line_buf, 0);
        strcpy(lcd_line_buf," В работе: ");
        for(i = 0; i <  WASHERS_MAX_COUNT; i++){
          sprintf(buffer_to_print, "%d", p_session->washers_in_use[i]);
          strcat(lcd_line_buf, buffer_to_print);
         }
	CPY_LCD_LINE_FROM_LINEBUF(lcd_line_buf, 1);
	sprintf( lcd_line_buf, " В кассе: %d           ", *p_cashbox);
	CPY_LCD_LINE_FROM_LINEBUF(lcd_line_buf, 2);
	sprintf( lcd_line_buf, " Запусков: %d           ", *p_clients_count);
	CPY_LCD_LINE_FROM_LINEBUF(lcd_line_buf, 3);
}
