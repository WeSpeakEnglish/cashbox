#include "calculations.h"
#include <math.h>
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

uint8_t byteDistance(uint8_t First, uint8_t Second){ // overflow byte calculate distance
 uint8_t retVal;
  if(Second < First){
     retVal = 0xFF - First;
     retVal += Second + 1;  
  }
  else 
    retVal = Second - First;
  
   return retVal;
}


uint16_t Atou(char * str){ // ONLY terminated strings be carifully!
  char * pLead = str; // first symbol
  char * pRead = str;
  uint16_t ReturnVal = 0;
  uint16_t Multiplier = 1;
 
  while((*pRead > 0x2F)&&(*pRead < 0x3A)){ // including ANY comma or something else (NOT NUMBER) like terminator
    pRead++;
  }
  if(pRead > pLead){
   pRead--;
   while(pRead + 1 > pLead ){
     ReturnVal += ((uint16_t)(*pRead) - 0x30) * ((uint16_t)Multiplier);
    // ReturnVal += (uint16_t)(*pRead)*Multiplier;
     Multiplier *= 10;
     pRead--; 
   } 
  
  }
  //find the end of str;
  return      ReturnVal;
}

void GetDate(char * str, date_time_t * dt){ // get date from string DD/MM/DD
  if(((*(str + 4)) =='/') && ((*(str + 7)) =='/')){
  dt->year = (uint8_t)(Atou(str) - 2000);
  dt->month = (uint8_t)Atou(str + 5);
  dt->day = (uint8_t)Atou(str + 8);
  }
  return;
}

void GetTime(char * str, date_time_t * dt){ // get date from string DD/MM/DD
  if(((*(str + 2)) ==':') && ((*(str + 5)) ==':')){
  dt->hours = (uint8_t)(Atou(str));
  dt->minutes = (uint8_t)Atou(str + 3);
  dt->seconds = (uint8_t)Atou(str + 6);
   }
  return;
}

const unsigned char numofdays[12]={31,28,31,30,31,30,31,31,30,31,30,31};


void UNIXToDate(volatile uint32_t utc, date_time_t * dt){
  uint32_t n,d,i;
  uint16_t year;
  dt->seconds = (uint8_t)(utc % 60); utc /= 60;
  dt->minutes = (uint8_t)(utc % 60); utc /= 60;
  dt->hours = (uint8_t)(utc % 24); utc /= 24;
  dt->weekday = (uint8_t)((utc + 4) % 7);
  year = (uint8_t)((1970 + utc / 1461 * 4)); utc %= 1461;
  n = ((utc >= 1096) ? utc - 1 : utc) / 365;
  year += n;
  dt->year = (uint8_t)(year - 2000);
  utc -= n * 365 + (n > 2 ? 1 : 0);
  for (i = 0; i < 12; i++) {
    d = numofdays[i];
    if (i == 1 && n == 2) d++;
    if (utc < d) break;
    utc -= d;
   }
  dt->month = (uint8_t)(1 + i);
  dt->day = (uint8_t)(1 + utc);
}

uint32_t DateToUNIX(date_time_t * dt){
uint32_t yDay = 0;
uint8_t i;

for(i = 0; i < (dt->month) - 1; i++){
   yDay += numofdays[i];
}

yDay += dt->day;

 return 31536000 * ((dt->year) + 2000 - 1970) + ((yDay) + (uint32_t)(floor((double)(((dt->year) + 2000 - 1972)/4))))*86400 + (dt->hours) * 3600 + (dt->minutes)*60 + (dt->seconds);

}

unsigned short dayofweek(unsigned short year, unsigned short month, unsigned short day)
{
 unsigned short t1, t2, t2r, t3;
 const unsigned short m[12] = {0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5};
  t1 = year - 2000;
  t2 = t1 / 100;
  t2r = t1 % 100;
  t3 = t1 + (t1 >> 2) - t2 + (t2 >> 2) - 1;
  if ( ((t1 & 0x0003 == 0) && ((t2r != 0) || (t2 & 0x0003 == 0))) && (month < 2) ) t3--;
 return (t3 + m[month] + day) % 7;
}
