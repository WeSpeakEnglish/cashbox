#include "sdcard.hpp"
#include "gpio.h"

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


