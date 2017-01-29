#include "sdcard.hpp"
#include "gpio.h"

class SD_Card{
  private:  
    bool LedCondition;
  public:
    void Led_OnOff(bool OnOff);  
};

void SD_Card::Led_OnOff(bool OnOff){
  switch(OnOff){
  case 0:
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, GPIO_PIN_RESET);
    break;
  case 1:
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, GPIO_PIN_SET);
    break;
  }
};

SD_Card SD;


extern "C" void SD_Card_On_Off(uint8_t OnOff){
  switch(OnOff){
    case 0:
       SD.Led_OnOff(0);
      break;
    case 1:
      SD.Led_OnOff(1);
      break;
  }
  
 return;
}



