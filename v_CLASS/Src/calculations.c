#include "calculations.h"
#ifdef __cplusplus
extern "C"{
#endif
char * Utoa(uint16_t Number, char * StrDst){ // convert int into string

  char * pStrDst = &StrDst[0];
  uint16_t Tmp;
  uint8_t Iliminate = 1;
  
  Tmp = Number/10000;
  if(Tmp)*pStrDst++ = Tmp + 0x30, Iliminate = 0;
    Number -= Tmp*10000;
  Tmp = Number/1000;
  if(Tmp) *pStrDst++ = Tmp + 0x30, Iliminate = 0;
  else 
    if (!Iliminate) *pStrDst++ = 0x30;
  Number -= Tmp*1000;
  Tmp = Number/100;
  if(Tmp) *pStrDst++ = Tmp + 0x30, Iliminate = 0;
    else 
    if (!Iliminate) *pStrDst++ = 0x30;
  Number -= Tmp*100;
  Tmp = Number/10;
  if(Tmp) *pStrDst++ = Tmp + 0x30, Iliminate = 0;
    else 
    if (!Iliminate) *pStrDst++ = 0x30;
  Tmp = Number%10;
  *pStrDst++ = Tmp + 0x30;
  *pStrDst = '\0';
return StrDst;
}

uint16_t sizeOfstr(char * str){
  uint16_t Ret = 0;
  while(*str++ != 0)
    Ret++;
 
  return Ret;  
}


char * Itoa(int16_t Number, char * StrDst){ // convert int into string
  
  char * pStrDst = &StrDst[0];
  uint16_t Tmp;
  uint8_t Iliminate = 1;

  if(Number < 0) *pStrDst++ = '-';
    
  Tmp = Number/10000;
  if(Tmp)*pStrDst++ = Tmp + 0x30, Iliminate = 0;
    Number -= Tmp*10000;
  Tmp = Number/1000;
  if(Tmp) *pStrDst++ = Tmp + 0x30, Iliminate = 0;
  else 
    if (!Iliminate) *pStrDst++ = 0x30;
  Number -= Tmp*1000;
  Tmp = Number/100;
  if(Tmp) *pStrDst++ = Tmp + 0x30, Iliminate = 0;
    else 
    if (!Iliminate) *pStrDst++ = 0x30;
  Number -= Tmp*100;
  Tmp = Number/10;
  if(Tmp) *pStrDst++ = Tmp + 0x30, Iliminate = 0;
    else 
    if (!Iliminate) *pStrDst++ = 0x30;
  Tmp = Number%10;
  *pStrDst++ = Tmp + 0x30;
  *pStrDst = '\0';
return StrDst;
}
#ifdef __cplusplus
}
#endif