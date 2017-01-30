#ifndef _SIMCOM_H
#define _SIMCOM_H
#include <stdint.h>
#include "dma.h"


#define RX_BUFFER_SIZE 200
#define SIMCOM_PWR_PORT GPIOA
#define SIMCOM_PWR_PIN  GPIO_PIN_4


typedef struct {
 uint8_t powered : 1;  // is power on or off
 volatile  uint8_t RX_Buffer1[200];  // double buffered RX
 volatile  uint8_t RX_Buffer2[200];   
 volatile  uint8_t * pRX_Buffer;   // point to current RX buffer
 volatile  uint16_t * pRX_WR_index; // index of current RX buffer
 volatile  uint16_t RX_WR_index1;  // how much of elements have been written to the first buffer
 volatile  uint16_t RX_WR_index2;  // how much of elements have been written to the second buffer
 volatile  uint8_t  bufferIsReady; // buffer is ready to parse 
 volatile  uint8_t * pReadyBuffer;  // pointer to buffer which is ready to parse
 
}SIM800;


extern SIM800 Sim800;

void sendCommand(void);
void SIM800_Ini(void);
void SIM800_PowerOnOff(void);

#endif