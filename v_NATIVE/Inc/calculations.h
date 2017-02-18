#ifndef _CALCULATIONS_H
#define _CALCULATIONS_H
#include <stdint.h>
#ifdef __cplusplus
extern "C"{
#endif
 char * Utoa(uint16_t Number, char * StrDst);
 char * Itoa(int16_t Number, char * StrDst);
 uint16_t sizeOfstr(char * str);
 uint8_t byteDistance(uint8_t First, uint8_t Second);

#ifdef __cplusplus 
  }
#endif
#endif