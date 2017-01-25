/**
  ******************************************************************************
  * File Name          : main.h
  * Description        : This file contains the common defines of the application
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H
  /* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define RESERVED_10_Pin GPIO_PIN_2
#define RESERVED_10_GPIO_Port GPIOE
#define RESERVED_11_Pin GPIO_PIN_3
#define RESERVED_11_GPIO_Port GPIOE
#define RESERVED_12_Pin GPIO_PIN_4
#define RESERVED_12_GPIO_Port GPIOE
#define RESERVED_13_Pin GPIO_PIN_5
#define RESERVED_13_GPIO_Port GPIOE
#define RESERVED_14_Pin GPIO_PIN_6
#define RESERVED_14_GPIO_Port GPIOE
#define RESERVED_15_Pin GPIO_PIN_13
#define RESERVED_15_GPIO_Port GPIOC
#define RESERVED_16_Pin GPIO_PIN_9
#define RESERVED_16_GPIO_Port GPIOF
#define RESERVED_17_Pin GPIO_PIN_10
#define RESERVED_17_GPIO_Port GPIOF
#define RESERVED_18_Pin GPIO_PIN_0
#define RESERVED_18_GPIO_Port GPIOC
#define LV_SHIFTER_OE_Pin GPIO_PIN_1
#define LV_SHIFTER_OE_GPIO_Port GPIOC
#define SIMCOM_MCU_R1_Pin GPIO_PIN_2
#define SIMCOM_MCU_R1_GPIO_Port GPIOC
#define MCU_SIMCOM_DTR_Pin GPIO_PIN_3
#define MCU_SIMCOM_DTR_GPIO_Port GPIOC
#define SIMCOM_DCD_Pin GPIO_PIN_2
#define SIMCOM_DCD_GPIO_Port GPIOF
#define SIMCOM_STATUS_Pin GPIO_PIN_4
#define SIMCOM_STATUS_GPIO_Port GPIOF
#define SD_CARD_DETECT_Pin GPIO_PIN_4
#define SD_CARD_DETECT_GPIO_Port GPIOA
#define RESERVED_19_Pin GPIO_PIN_5
#define RESERVED_19_GPIO_Port GPIOA
#define RESERVED_20_Pin GPIO_PIN_6
#define RESERVED_20_GPIO_Port GPIOA
#define RESERVED_21_Pin GPIO_PIN_7
#define RESERVED_21_GPIO_Port GPIOA
#define RESERVED_22_Pin GPIO_PIN_4
#define RESERVED_22_GPIO_Port GPIOC
#define RESERVED_23_Pin GPIO_PIN_5
#define RESERVED_23_GPIO_Port GPIOC
#define RESERVED_24_Pin GPIO_PIN_0
#define RESERVED_24_GPIO_Port GPIOB
#define RESERVED_25_Pin GPIO_PIN_1
#define RESERVED_25_GPIO_Port GPIOB
#define Keypad_9_Pin GPIO_PIN_2
#define Keypad_9_GPIO_Port GPIOB
#define Keypad_8_Pin GPIO_PIN_7
#define Keypad_8_GPIO_Port GPIOE
#define Keypad_7_Pin GPIO_PIN_8
#define Keypad_7_GPIO_Port GPIOE
#define Keypad_6_Pin GPIO_PIN_9
#define Keypad_6_GPIO_Port GPIOE
#define Keypad_5_Pin GPIO_PIN_10
#define Keypad_5_GPIO_Port GPIOE
#define Keypad_4_Pin GPIO_PIN_11
#define Keypad_4_GPIO_Port GPIOE
#define Keypad_3_Pin GPIO_PIN_12
#define Keypad_3_GPIO_Port GPIOE
#define Keypad_2_Pin GPIO_PIN_13
#define Keypad_2_GPIO_Port GPIOE
#define Keypad_1_Pin GPIO_PIN_14
#define Keypad_1_GPIO_Port GPIOE
#define Keypad_0_Pin GPIO_PIN_15
#define Keypad_0_GPIO_Port GPIOE
#define USART3_Rx_LED_Pin GPIO_PIN_12
#define USART3_Rx_LED_GPIO_Port GPIOB
#define USART3_Tx_LED_Pin GPIO_PIN_13
#define USART3_Tx_LED_GPIO_Port GPIOB
#define RESERVED_29_Pin GPIO_PIN_15
#define RESERVED_29_GPIO_Port GPIOB
#define LCD_D7_Pin GPIO_PIN_8
#define LCD_D7_GPIO_Port GPIOD
#define LCD_D6_Pin GPIO_PIN_9
#define LCD_D6_GPIO_Port GPIOD
#define LCD_D5_Pin GPIO_PIN_10
#define LCD_D5_GPIO_Port GPIOD
#define LCD_D4_Pin GPIO_PIN_11
#define LCD_D4_GPIO_Port GPIOD
#define LCD_D3_Pin GPIO_PIN_12
#define LCD_D3_GPIO_Port GPIOD
#define LCD_D2_Pin GPIO_PIN_13
#define LCD_D2_GPIO_Port GPIOD
#define LCD_D1_Pin GPIO_PIN_14
#define LCD_D1_GPIO_Port GPIOD
#define LCD_D0_Pin GPIO_PIN_15
#define LCD_D0_GPIO_Port GPIOD
#define LCD_E_Pin GPIO_PIN_6
#define LCD_E_GPIO_Port GPIOC
#define LCD_RW_Pin GPIO_PIN_7
#define LCD_RW_GPIO_Port GPIOC
#define LCD_RS_Pin GPIO_PIN_8
#define LCD_RS_GPIO_Port GPIOC
#define RESERVED_1_Pin GPIO_PIN_9
#define RESERVED_1_GPIO_Port GPIOC
#define RESERVED_2_Pin GPIO_PIN_6
#define RESERVED_2_GPIO_Port GPIOF
#define RESERVED_3_Pin GPIO_PIN_11
#define RESERVED_3_GPIO_Port GPIOC
#define PAYMENTS_1_Pin GPIO_PIN_0
#define PAYMENTS_1_GPIO_Port GPIOD
#define PAYMENTS_2_Pin GPIO_PIN_1
#define PAYMENTS_2_GPIO_Port GPIOD
#define PAYMENTS_3_Pin GPIO_PIN_3
#define PAYMENTS_3_GPIO_Port GPIOD
#define PAYMENTS_4_Pin GPIO_PIN_4
#define PAYMENTS_4_GPIO_Port GPIOD
#define RESERVED_4_Pin GPIO_PIN_5
#define RESERVED_4_GPIO_Port GPIOD
#define SD_LED_Pin GPIO_PIN_6
#define SD_LED_GPIO_Port GPIOD
#define SD_DETECT_Pin GPIO_PIN_7
#define SD_DETECT_GPIO_Port GPIOD
#define CD_CS_Pin GPIO_PIN_6
#define CD_CS_GPIO_Port GPIOB
#define RESERVED_5_Pin GPIO_PIN_7
#define RESERVED_5_GPIO_Port GPIOB
#define RESERVED_6_Pin GPIO_PIN_8
#define RESERVED_6_GPIO_Port GPIOB
#define RESERVED_7_Pin GPIO_PIN_9
#define RESERVED_7_GPIO_Port GPIOB
#define RESERVED_8_Pin GPIO_PIN_0
#define RESERVED_8_GPIO_Port GPIOE
#define RESERVED_9_Pin GPIO_PIN_1
#define RESERVED_9_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

/**
  * @}
  */ 

/**
  * @}
*/ 

#endif /* __MAIN_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
