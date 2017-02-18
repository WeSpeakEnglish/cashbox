/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 *
 * Copyright (c) 2017 STMicroelectronics International N.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted, provided that the following conditions are met:
 *
 * 1. Redistribution of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of STMicroelectronics nor the names of other
 *    contributors to this software may be used to endorse or promote products
 *    derived from this software without specific written permission.
 * 4. This software, including modifications and/or derivative works of this
 *    software, must execute solely and exclusively on microcontroller or
 *    microprocessor devices manufactured by or for STMicroelectronics.
 * 5. Redistribution and use of this software other than as permitted under
 *    this license is void and will automatically terminate your rights under
 *    this license.
 *
 * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
 * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
 * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */
#include "usart.h"
#include "stm32f3xx_hal.h"
#include "simcom.h"
#include "parse_sim800.h"
#include "sd_files.h"
#include "ccTalk.h"
#include "lcd.h"
#include "keyboard.h"
#include "modbus.h"
/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/
osThreadId defaultTaskHandle;
osThreadId keybScanTaskHandle;
osThreadId parserTaskHandle;
osThreadId sim800_TaskHandle;
osThreadId main_TaskHandle;
//osThreadId nv_9_TaskHandle;
/* USER CODE BEGIN Variables */
/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void StartDefaultTask(void const * argument);
void MainTask(void const * argument);
void KeybScanTask(void const * argument);
void ParserExecuteTask(void const * argument);
void SIM800_IniTask(void const * argument);
//void NV_9_Task(void const * argument);

extern void MX_FATFS_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

/* Hook prototypes */

/* Init FreeRTOS */

void MX_FREERTOS_Init(void) {
    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
    /* USER CODE END RTOS_MUTEX */

    /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
    /* USER CODE END RTOS_SEMAPHORES */

    /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
    /* USER CODE END RTOS_TIMERS */

    /* Create the thread(s) */

    /* USER CODE BEGIN RTOS_THREADS */
    /* definition and creation of defaultTask */
    osThreadDef(defaultTask, StartDefaultTask, osPriorityIdle, 0, 256);
    defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

    /* definition and creation of keybScanTask */
    osThreadDef(keybScanTask, KeybScanTask, osPriorityIdle, 0, 256);
    keybScanTaskHandle = osThreadCreate(osThread(keybScanTask), NULL);


    /* definition and creation of ParserExecute */
    osThreadDef(parserExecuteTask, ParserExecuteTask, osPriorityIdle, 0, 128);
    parserTaskHandle = osThreadCreate(osThread(parserExecuteTask), NULL);

    osThreadDef(sim800_IniTask, SIM800_IniTask, osPriorityBelowNormal, 0, 128);
    sim800_TaskHandle = osThreadCreate(osThread(sim800_IniTask), NULL);


    osThreadDef(main_Task, MainTask, osPriorityIdle, 0, 512);
    main_TaskHandle = osThreadCreate(osThread(main_Task), NULL);


    //  osThreadDef(nv_9_Task, NV_9_Task, osPriorityIdle, 0, 512);
    //  nv_9_TaskHandle = osThreadCreate(osThread(nv_9_Task), NULL);
    /* USER CODE END RTOS_THREADS */

    /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
    /* USER CODE END RTOS_QUEUES */
}

/* StartDefaultTask function */
void StartDefaultTask(void const * argument) {
    /* init code for FATFS */
    MX_FATFS_Init();
    lcd_init();
    ModbusBufferFree();

    /* USER CODE BEGIN StartDefaultTask */
    /* Infinite loop */
    for (;;) {
        SIM800_SendCMD();
        osDelay(10);
    }
    /* USER CODE END StartDefaultTask */
}

/* KeybScanTask function */
void KeybScanTask(void const * argument) {

    /* Infinite loop */
    for (;;) {
        if (LCD.init) {
            if (Keyboard.keyReady) {
                lcd_putch(Keyboard.keyCode);
                Keyboard.keyReady = 0;
            }
        }

        vTaskDelay(10);
    }

}

void ParserExecuteTask(void const * argument) {

    xSemaphoreUART2 = xSemaphoreCreateBinary();

    for (;;) {
        if (xSemaphoreTake(xSemaphoreUART2, 60000) == pdTRUE) {
            SIM800_ParseAnswers();
        }
    }
}

void SIM800_IniTask(void const * argument) {



    vTaskDelay(800);

    for (;;) {
        SIM800_IniCMD();
        SIM800_init_info_upload();
        SIM800_info_upload();
        SIM800_command();
        SIM800_pop_washing();
        Sim800.initialized = 1;
        vTaskDelete(NULL);
    }

}

void MainTask(void const * argument) {
    uint8_t counter = 0;

    vTaskDelay(40000);


    if (!Sim800.initialized)
        SD_GetData();
    else
        SD_SetData();
    /* Infinite loop */

    ccTalkSendCMD(CC_OPEN);
    vTaskDelay(500);
    ccTalkSendCMD(CC_MASTER_READY);
    vTaskDelay(500);
    ccTalkSendCMD(CC_DISABLEESCROW);
    vTaskDelay(500);
    ccTalk.gotMoney = 0;
    for (;;) {
        if (!ccTalk.gotMoney) {
            taskENTER_CRITICAL();
            {
                ccTalkSendCMD(CC_REQ_ACC_COUNT);
            }
            taskEXIT_CRITICAL();

            ccTalkParseAccCount();
        } else {
            taskENTER_CRITICAL();
            {
                ccTalkSendCMD(CC_READBUFFEREDBILL);
            }
            taskEXIT_CRITICAL();
            ccTalkParseStatus();
        }
        counter++;
        
        TranmitSlaveCmd(0);
        
        vTaskDelay(490);
    }

}

/* USER CODE END KeybScanTask */
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
