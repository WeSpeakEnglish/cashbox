#include "parse_sim800.h"
#include "simcom.h"

uint8_t Ready = 0;

//void Catch(void){
//  Ready

//}
volatile uint16_t SIM800_ParserID = 0;

uint8_t SIM800_ParserOK(){
 if((Sim800.pReadyBuffer[0] == 'O')&&(Sim800.pReadyBuffer[1]=='K')) 
   return 0;
 else 
   return 1;
}

void   SIM800_ParseAnswers(void){
static uint16_t count = 0;

  if(Sim800.bufferIsReady == 1){
  
    switch(SIM800_ParserID){
      case 1:
        SIM800_ParserOK();
        break;
    
    }

  }

}

