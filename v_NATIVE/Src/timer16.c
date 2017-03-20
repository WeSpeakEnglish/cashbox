#include "timer16.h"
#include "stm32f3xx_hal.h"
//#include "sdcard.hpp"
#include <stdint.h>
#include <iwdg.h>
#include "lcd.h"
#include "core.h"
#include "statemachine.h"
#include "cctalk.h"
#include "vend.h"
#include "modbus.h"

volatile uint64_t milliseconds = 0;
volatile uint32_t TicksGlobal_mS = 0;

void TIM16_Start(uint32_t Prescaler, uint32_t Count) {

    TIM16->PSC = Prescaler - 1;
    TIM16->ARR = Count;
    TIM16->DIER |= TIM_DIER_UIE;
    TIM16->CR1 |= TIM_CR1_CEN; //Counter enable
    TIM16->CNT = 0;
    NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);
}

void TIM16_Stop(void) {
    TIM16->DIER &= ~TIM_DIER_UIE;
    TIM16->CR1 &= ~TIM_CR1_CEN; //Counter enable
    NVIC_DisableIRQ(TIM1_UP_TIM16_IRQn);
}

void TIM16_Reset(void) {
    TIM16->CNT = 0;
}

uint64_t millis(void){

return milliseconds;
}

void simpleTimeoutInit(volatile uint32_t *simpleTimeout_instance)
{
        // timeout_instance must be static volatile
        *simpleTimeout_instance = millis();
}

uint8_t simpleTimeoutHasRunOut(volatile uint32_t *simpleTimeout_instance, uint32_t milliseconds)
{
        if (milliseconds == 0) return 0; // never run out, timer is off
        return ( ( millis() - (*simpleTimeout_instance) ) >= milliseconds );
}

void TIM1_UP_TIM16_IRQHandler(void) {
//static uint8_t Test = 0;
 static date_time_t dt;
 
  if (TIM16->SR & TIM_SR_UIF) {
    TIM16->SR &= ~TIM_SR_UIF; // сбрасываю флаг прерывани€
    TIM16->CNT = 0; // обнул€ю счетчик... 

    //EXTI->IMR|=EXTI_IMR_MR0; //разрешаю внешние прерывани€
    // HAL_IWDG_Refresh(&hiwdg);
    milliseconds++;
    TicksGlobal_mS++;
    
    if(!(milliseconds % 100)){
       if(LCD.init) M_push(loop);
       

    }
    if(!(milliseconds % 150)){
//        F_push(modbus_update);

    }
    if(!(milliseconds % 1000)){
       dt = DataTime;
       UNIXToDate(++RTC_seconds, &dt);
       DataTime = dt;
       if((DataTime.seconds == 0) && (!(DataTime.minutes % 5)))
         S_push(SIM800_command);
       if((DataTime.seconds == 0) && (DataTime.minutes == 0)){
         S_push(SIM800_get_Balance);
         S_push(SIM800_info_upload);
       }
    }
    if(p_session->current_state == INSERT_FUNDS){
     if(!(milliseconds % 490)){
        ccTalk.TransferFlag = 1;
     }
   }
  }
}

