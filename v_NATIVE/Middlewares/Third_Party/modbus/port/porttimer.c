
/* ----------------------- System includes ----------------------------------*/
//#include "assert.h"

/* ----------------------- FreeRTOS includes --------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#

/* ----------------------- STR71X includes ----------------------------------*/

#include "timer6.h"
#include "tim.h"

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "stm32f3xx_hal.h"
/* ----------------------- Defines ------------------------------------------*/

/* Timer ticks are counted in multiples of 50us. Therefore 20000 ticks are
 * one second.
 */
#define MB_TIMER_TICKS          ( 20000UL )

/* ----------------------- Static variables ---------------------------------*/
static uint16_t timeout = 0;
static uint16_t downcounter = 0;

/* ----------------------- Static functions ---------------------------------*/
/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortTimersInit( USHORT usTim1Timerout50us )
{

   TIM6_Start(8,450); 

    return TRUE;
}


void TIM6_DAC_IRQHandler(void) {
 static portBASE_TYPE xTaskSwitch = pdFALSE;

    if (TIM6->SR & TIM_SR_UIF) {
        TIM6->SR &= ~TIM_SR_UIF;
        TIM6->CNT = 0; // reset counter 
        



    if (!--downcounter)
         xTaskSwitch |= pxMBPortCBTimerExpired(  );
      }
  portYIELD_FROM_ISR(xTaskSwitch);
}

inline void vMBPortTimersEnable(  )
{
	downcounter = timeout;
	HAL_TIM_Base_Start_IT(&htim6);
}

inline void vMBPortTimersDisable(  )
{
    /* We can always clear both flags. This improves performance. */
   HAL_TIM_Base_Stop_IT(&htim6);
}
