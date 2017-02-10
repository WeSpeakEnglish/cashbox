#ifndef _KEYBOARD_H
#define _KEYBOARD_H
#include <stdint.h>

typedef struct{
 uint8_t keyReady;
 uint8_t keyCode;
} KBD;

void ScanKeyboard(void);


#endif