#ifndef _MODBUS_H
#define _MODBUS_H
#include <stdint.h>
#include "SimpleModbusMaster.h"

#define BUFLEN_MODBUS 32

typedef struct{
 volatile uint8_t  buffer[BUFLEN_MODBUS];    // easy without twice buffering awaiting while ready
 volatile uint8_t  readyBuff;            // buffer is ready
 volatile uint8_t  readySymbols;         // number of symbols in the buffer
 volatile uint16_t IndWR;               //current write index
 volatile uint8_t  TransferFlag;        //we put the command  
}MODBUS_struct;

enum
{
  PACKET1,
  PACKET2,
  TOTAL_NO_OF_PACKETS // leave this last entry
};

void TranmitSlaveCmd(uint8_t Cmd);
void ModbusBufferFree(void);
void USART3_IRQHandler(void);
extern MODBUS_struct Modbus;
extern Packet packets[TOTAL_NO_OF_PACKETS];
extern unsigned int regs[10];
#endif