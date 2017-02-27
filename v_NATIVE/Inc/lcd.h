#ifndef _LCD_H
#define _LCD_H
#include <stdint.h>


#define LCD_LINE_SZ     20
#define LCD_LINES       4
#define LCD_SYMBS       (LCD_LINES * LCD_LINE_SZ)
#define LCD_BLOCK_SZ    5
#define LCD_BLOCKS      (LCD_SYMBS / LCD_BLOCK_SZ) 


typedef union {
 char line[LCD_LINES][LCD_LINE_SZ];
 char block[LCD_BLOCKS][LCD_BLOCK_SZ];
 char ravel[LCD_SYMBS];
} LCD_text_t;

typedef struct{
uint8_t init: 1; //initilized
}lcd;
/////////////////////////////////////////////
LCD_text_t *getLCDText(void);
char *getLCDLine(int line_num);
char *getLCDBlock(int block_num);
uint16_t whichBlocksWereChanged(void); // block = 5 symbs, 16 blocks, func. sets corresp. bit# in ret
void lcd_write_block(uint8_t block_num, char *string);
void lcd_write_byte(unsigned char byte, unsigned char A0_bit);				// 'byte' -- байт команды
void lcd_write_cmd(unsigned char byte);
void lcd_write_data(unsigned char byte);
void lcd_continue_block_writing(char *string);
////////////////////////////////////////////
void lcd_init(void);
void lcd_clear(void);
void lcd_puts(char const * s);
void lcd_goto(unsigned char line, unsigned char pos);
void lcd_putch(unsigned char c);

extern lcd LCD;

#endif