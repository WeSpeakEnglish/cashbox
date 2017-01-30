#include "parse_sim800.h"
#include "simcom.h"

uint8_t Ready = 0;

//void Catch(void){
//  Ready

//}

void   SIM800_ParseAnswers(void){
static uint16_t count = 0;

  if(Sim800.bufferIsReady == 1){
     count++;
     Sim800.bufferIsReady = 0;

     if (count == 4){
       Sim800.bufferIsReady;
     }
  }

}

