#include "keyboard.h"

#include "stm32f3xx_hal.h"


#define ROW1_READ HAL_GPIO_ReadPin(Keypad_8_GPIO_Port,  Keypad_8_Pin)
#define ROW2_READ HAL_GPIO_ReadPin(Keypad_3_GPIO_Port,  Keypad_3_Pin)
#define ROW3_READ HAL_GPIO_ReadPin(Keypad_4_GPIO_Port,  Keypad_4_Pin)
#define ROW4_READ HAL_GPIO_ReadPin(Keypad_6_GPIO_Port,  Keypad_6_Pin)

#define COL1_WRITE(x) HAL_GPIO_WritePin(Keypad_7_GPIO_Port, Keypad_7_Pin,  x)
#define COL2_WRITE(x) HAL_GPIO_WritePin(Keypad_9_GPIO_Port, Keypad_9_Pin,  x)
#define COL3_WRITE(x) HAL_GPIO_WritePin(Keypad_5_GPIO_Port, Keypad_5_Pin, x)


KBD Keyboard = {0,0};
static uint8_t ScanLine = 0;

void ReadKeyboardValue(void){
  static uint8_t OldKey = 0x0F;
  static uint16_t CounterK = 0; // for how long pressed the button
  static uint8_t ReadValue; // we will read bits from the port 
  
  ReadValue = ROW1_READ;
  ReadValue += ROW2_READ << 1;
  ReadValue += ROW3_READ << 2;
  ReadValue += ROW4_READ << 3;
  
  
  if(ReadValue != OldKey && (ReadValue != 0x0F)){
    OldKey =   ReadValue;
    CounterK = 0;
  }
  else{
   if(ReadValue != 0x0F) 
     CounterK++;
  }
  
  if(CounterK == 10){
    Keyboard.keyCode = ReadValue;
    Keyboard.keyReady = 1;
  }
}

void WriteKeyboardLines(void){
 static uint8_t Counter = 0;
 
   COL1_WRITE(GPIO_PIN_SET); 
   COL2_WRITE(GPIO_PIN_SET); 
   COL3_WRITE(GPIO_PIN_SET); 
 
   switch(Counter%4){
   case 0:
     COL1_WRITE(GPIO_PIN_RESET); 
     ScanLine = 0;
        break;
   case 1:
     COL2_WRITE(GPIO_PIN_RESET); 
     ScanLine = 1;
        break;
   case 2:
     COL3_WRITE(GPIO_PIN_RESET); 
     ScanLine = 2;
        break;     
   case 3:
        break;        
    }
 Counter++;
  return;
}

void ScanKeyboard(void){
 static uint16_t SM_Counter = 0; //the state machine counter
  switch(SM_Counter){
     case 0: WriteKeyboardLines();
           break;
     case 10: ReadKeyboardValue();
            break;       
     case 20: WriteKeyboardLines();
            break;
     case 30: ReadKeyboardValue();
            break; 
     case 40: WriteKeyboardLines();
            break;
     case 50: ReadKeyboardValue();
            break;       
 }
 
 SM_Counter++;
 SM_Counter %= 60;
}

