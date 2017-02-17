#ifndef _LCD_H
#define _LCD_H
#include <stdint.h>

typedef struct{
uint8_t init: 1; //initilized
}lcd;


void lcd_init(void);
void lcd_clear(void);
void lcd_puts(char const * s);
void lcd_goto(unsigned char line, unsigned char pos);
void lcd_putch(unsigned char c);

extern lcd LCD;

#endif