#ifndef _CALCULATIONS_H
#define _CALCULATIONS_H
#include <stdint.h>
#ifdef __cplusplus
extern "C"{
#endif
  
typedef struct
{
 uint8_t seconds;    // 0 to 59
 uint8_t minutes;    // 0 to 59
 uint8_t hours;      // 0 to 23  (24-hour time)
 uint8_t day;        // 1 to 31
 uint8_t weekday;    // 0 = Sunday, 1 = Monday, etc.
 uint8_t month;      // 1 to 12
 uint8_t year;       // 00 to 99
}date_time_t;

  
 char * Utoa(uint16_t Number, char * StrDst);
 char * Itoa(int16_t Number, char * StrDst);
 uint16_t sizeOfstr(char * str);
 uint8_t byteDistance(uint8_t First, uint8_t Second);


uint16_t Atou(char * str);

void GetDate(char * str, date_time_t * dt);
void GetTime(char * str, date_time_t * dt);
//date to the UNIX time and back translations  
uint32_t DateToUNIX(date_time_t * dt);
void UNIXToDate(volatile uint32_t utc, date_time_t * dt);

unsigned short dayofweek(unsigned short year, unsigned short month, unsigned short day);

#ifdef __cplusplus 
  }
#endif
#endif