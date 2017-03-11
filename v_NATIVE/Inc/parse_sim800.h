#ifndef _PARSE_SIM800_H
#define _PARSE_SIM800_H
#include <stdint.h>

extern volatile uint16_t SIM800_ParserID;

typedef union{
struct{
    uint8_t OK :        1;
    uint8_t ERROR :     1;
    uint8_t DOWNLOAD :  1;
}bits;
uint8_t byte;
}Result;
  
//extern SemaphoreHandle_t xSemaphoreParse;
extern Result ResParse;

void    SIM800_ParseAnswers(void);
void    SIM800_parse_PhoneNumber(void);
void    SIM800_parse_Signal(void);
void    SIM800_parse_Balance(void);
void    SIM800_Parse_CGAT(void);
void    SIM800_Parse_WM(void);
#endif