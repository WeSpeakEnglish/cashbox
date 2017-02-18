#ifndef _MODBUS_H
#define _MODBUS_H
#include <stdint.h>

#define BUFLEN_MODBUS 32

typedef struct{
 volatile uint8_t  buffer[BUFLEN_MODBUS];    // easy without twice buffering awaiting while ready
 volatile uint8_t  readyBuff;            // buffer is ready
 volatile uint8_t  readySymbols;         // number of symbols in the buffer
 volatile uint16_t IndWR;               //current write index
 volatile uint8_t  TransferFlag;        //we put the command  
}MODBUS_struct;

void TranmitSlaveCmd(uint8_t Cmd);
void ModbusBufferFree(void);
void USART3_IRQHandler(void);
#endif