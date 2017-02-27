
#ifndef LCDMENU_H
#define LCDMENU_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lcd.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CPY_LCD_LINE_FROM_PRGMEM(string_in_prgmem, line_num) \
	strncpy( getLCDLine(line_num), string_in_prgmem, LCD_LINE_SZ )

#define CPY_LCD_LINE_FROM_LINEBUF(lcd_line_buf, line_num) \
	strncpy( getLCDLine(line_num), lcd_line_buf, LCD_LINE_SZ )

uint16_t LCDTextUpdate(void);

void LCDMenu_Init (void);
uint16_t LCDMenu_Update (void);

char *LCDGetLineAsString(int line_num); // for debug purposes

#ifdef __cplusplus
}
#endif


#endif /* LCDMENU_H */