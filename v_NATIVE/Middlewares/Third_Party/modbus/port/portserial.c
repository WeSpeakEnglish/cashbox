/*
 * FreeModbus Libary: STR71x Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portserial.c,v 1.6 2006/06/15 15:41:20 wolti Exp $
 */

/* ----------------------- System includes ----------------------------------*/


/* ----------------------- FreeRTOS includes --------------------------------*/
#include "FreeRTOS.h"
#include "task.h"

/* ----------------------- STR71X includes ----------------------------------*/
#include "usart.h"

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- Defines ------------------------------------------*/


/* ----------------------- Static functions ---------------------------------*/

/* ----------------------- Start implementation -----------------------------*/

BOOL
xMBPortSerialInit( UCHAR ucPort, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
    BOOL            xResult = TRUE;

    (void)ucPort;
    (void)ulBaudRate;
    (void)ucDataBits;
    (void)eParity;
    MX_USART3_UART_Init();
    return xResult;
}
    
void vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
  if( xRxEnable ){
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
        __HAL_UART_ENABLE_IT(&huart3, UART_IT_RXNE); // enable interrupts
  }
    else
        __HAL_UART_DISABLE_IT(&huart3, UART_IT_RXNE); 

    if( xTxEnable ){
       HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
        __HAL_UART_ENABLE_IT(&huart3, UART_IT_TXE);
    }
    else
        __HAL_UART_DISABLE_IT(&huart3, UART_IT_TXE);
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    huart3.Instance->TDR = ucByte;
    return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    *pucByte = huart3.Instance->RDR;
    return TRUE;
}

void UART3_IRQHandler(void) {
  
  static BOOL     xTaskWokenReceive = FALSE;
  static BOOL     xTaskWokenTransmit = FALSE;\
    
		if((__HAL_UART_GET_IT(&huart3, UART_IT_RXNE) != RESET) && (__HAL_UART_GET_IT_SOURCE(&huart3, UART_IT_RXNE) != RESET)) {
			xTaskWokenReceive = pxMBFrameCBByteReceived();
			__HAL_UART_SEND_REQ(&huart3, UART_RXDATA_FLUSH_REQUEST);
                        portYIELD_FROM_ISR(xTaskWokenReceive);
                        

		}
		if((__HAL_UART_GET_IT(&huart3, UART_IT_TXE) != RESET) &&(__HAL_UART_GET_IT_SOURCE(&huart3, UART_IT_TXE) != RESET)) {
			xTaskWokenReceive = pxMBFrameCBTransmitterEmpty(  );
                         portYIELD_FROM_ISR(xTaskWokenTransmit);

		}
}
