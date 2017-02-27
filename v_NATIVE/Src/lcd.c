#include "lcd.h"
#include "stm32f3xx_hal.h"
#include "cmsis_os.h"
#include "DbgInfo.h"

union {

    struct {
        uint8_t b0 : 1;
        uint8_t b1 : 1;
        uint8_t b2 : 1;
        uint8_t b3 : 1;
        uint8_t b4 : 1;
        uint8_t b5 : 1;
        uint8_t b6 : 1;
        uint8_t b7 : 1;
    } bits;
    unsigned char byte;
} DataByte;

lcd LCD = {1};

const unsigned char lcd_chars[] ={//таблица с кодами русских символов
    0x41, 0xA0, 0x42, 0xA1, 0xE0, 0x45, 0xA3, 0xA4, 0xA5, 0xA6, 0x4B, 0xA7, 0x4D, 0x48, 0x4F, 0xA8,
    0x50, 0x43, 0x54, 0xA9, 0xAA, 0x58, 0xE1, 0xAB, 0xAC, 0xE2, 0xAD, 0xAE, 0x62, 0xAF, 0xB0, 0xB1,
    0x61, 0xB2, 0xB3, 0xB4, 0xE3, 0x65, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0x6F, 0xBE,
    0x70, 0x63, 0xBF, 0x79, 0xE4, 0x78, 0xE5, 0xC0, 0xC1, 0xE6, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
    0xA2, 0xB5
};

void LCD_SetData(unsigned char Data) {
    DataByte.byte = Data;
    HAL_GPIO_WritePin(LCD_D0_GPIO_Port, LCD_D0_Pin, (GPIO_PinState) DataByte.bits.b0);
    HAL_GPIO_WritePin(LCD_D1_GPIO_Port, LCD_D1_Pin, (GPIO_PinState) DataByte.bits.b1);
    HAL_GPIO_WritePin(LCD_D2_GPIO_Port, LCD_D2_Pin, (GPIO_PinState) DataByte.bits.b2);
    HAL_GPIO_WritePin(LCD_D3_GPIO_Port, LCD_D3_Pin, (GPIO_PinState) DataByte.bits.b3);
    HAL_GPIO_WritePin(LCD_D4_GPIO_Port, LCD_D4_Pin, (GPIO_PinState) DataByte.bits.b4);
    HAL_GPIO_WritePin(LCD_D5_GPIO_Port, LCD_D5_Pin, (GPIO_PinState) DataByte.bits.b5);
    HAL_GPIO_WritePin(LCD_D6_GPIO_Port, LCD_D6_Pin, (GPIO_PinState) DataByte.bits.b6);
    HAL_GPIO_WritePin(LCD_D7_GPIO_Port, LCD_D7_Pin, (GPIO_PinState) DataByte.bits.b7);

}


LCD_text_t *getLCDText(void)
{
	static LCD_text_t lcd_txt = { 0 };
	return &lcd_txt;
}

char *getLCDLine(int line_num)
{
	LCD_text_t *p_txt = getLCDText();
	return (char *)(p_txt->line[line_num]);
}

char *getLCDBlock(int block_num)
{
	LCD_text_t *p_txt = getLCDText();
	return (char *)(p_txt->block[block_num]);
}

uint16_t whichBlocksWereChanged(void) // block = 5 symbs, 16 blocks, func. sets corresp. bit# in ret
{
	uint8_t i;
	uint16_t ret = 0;
	LCD_text_t *p_txt;

	static LCD_text_t lcd_txt_cpy = { 0 };
	
	p_txt = getLCDText();

	for (i = 0; i < LCD_BLOCKS; i++)
		ret |= \
		  (
		  	(
	  	      (uint16_t)(cmp_(getLCDBlock(i), \
	                          (char *)(lcd_txt_cpy.block[i]), \
	                          LCD_BLOCK_SZ)
	  	    ^
	  	      0x0001)
		  	) << i
		  );
	if (ret)
		cpy_
	    (
		  (char *)(p_txt),
		  (char *)(&lcd_txt_cpy),
		  sizeof(LCD_text_t)
		);
	return ret;
}

void lcd_continue_block_writing(char *string)
{
	uint8_t i;
	for (i = 0; i < LCD_BLOCK_SZ; i++) lcd_write_data(string[i]);
}

void lcd_write_block(uint8_t block_num, char *string)
{
	// свичкейсом устанавливаем курсор в нужное положение
	switch(block_num / (LCD_BLOCKS/LCD_LINES))
	{
		case 0 : lcd_write_cmd(0x80 + (LCD_BLOCK_SZ*(block_num % (LCD_BLOCKS/LCD_LINES)))); break;
		case 1 : lcd_write_cmd(0x80 + 0x40 + (LCD_BLOCK_SZ*(block_num % (LCD_BLOCKS/LCD_LINES)))); break;
		case 2 : lcd_write_cmd(0x80 + 0x14 + (LCD_BLOCK_SZ*(block_num % (LCD_BLOCKS/LCD_LINES)))); break;
		case 3 : lcd_write_cmd(0x80 + 0x54 + (LCD_BLOCK_SZ*(block_num % (LCD_BLOCKS/LCD_LINES)))); break;
		default : break;
	}
	lcd_continue_block_writing(string);
}

void lcd_clear_screen(void)
{	
      lcd_clear();
}

void lcd_write(unsigned char Data) {

    LCD_SetData(Data);
    osDelay(1);
    HAL_GPIO_WritePin(LCD_E_GPIO_Port, LCD_E_Pin, GPIO_PIN_SET);
    //  osDelay(1);
    HAL_GPIO_WritePin(LCD_E_GPIO_Port, LCD_E_Pin, GPIO_PIN_RESET);
    osDelay(1);
}

void lcd_putch(unsigned char c) //symbol output
{
    HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_SET);
    if (c < 127) lcd_write(c);
    else
        if (c < 208) lcd_write(lcd_chars[c - 128]);
    else
        lcd_write(lcd_chars[c - 128 - 48]);
}

void lcd_clear(void) //clear display
{
    HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_RESET);
    osDelay(1);
    lcd_write(0x01);
    osDelay(3); //3ms
}

void lcd_puts(char const * s) //print string
{
    HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_SET);
    while (*s) lcd_write(*s++);
}

//if we have 4-line display
//lines is 0,1,2,3

void lcd_goto(unsigned char line, unsigned char pos) //set cursor position
{
    //first line:  0..20
    //second line: 40..60
    switch (line) { // an addition var, that will be added to the position
        case 1:
            pos += 64;
            break;
        case 2:
            pos += 20;
            break;
        case 3:
            pos += 84;
            break;
    }
    HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_RESET);
    lcd_write(0x80 + pos);
}

void lcd_write_byte(unsigned char byte, unsigned char A0_bit)				// 'byte' -- байт команды
{
  switch(A0_bit){
    case 0:
          HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_RESET);  //cmd here
             break;
    case 1:
          HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_SET);  //cmd here
             break;             
  }
  lcd_write(byte);
}

void lcd_write_cmd(unsigned char byte)
{
	lcd_write_byte(byte, 0);
}
// write data (not cmd)
void lcd_write_data(unsigned char byte)
{
	lcd_write_byte(byte, 1);
}

void lcd_init(void) {
    HAL_GPIO_WritePin(LCD_RW_GPIO_Port, LCD_RW_Pin, GPIO_PIN_RESET);
    osDelay(30);
    HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_RESET);
    osDelay(2);
    lcd_write(0x30);
    lcd_write(0x30);
    lcd_write(0x30);
    lcd_write(0x3A); //set 8 bits mode
    lcd_write(0x08); //disp off
    lcd_clear(); //clear display
    lcd_write(0x0C); //switch off the cursor
    osDelay(20);
}






