#ifndef _PARSE_SIM800_H
#define _PARSE_SIM800_H
#include <stdint.h>

extern volatile uint16_t SIM800_ParserID;

void   SIM800_ParseAnswers(void);

#endif