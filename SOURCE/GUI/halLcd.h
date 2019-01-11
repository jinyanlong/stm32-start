#ifndef __HALLCD_H
#define __HALLCD_H

#define LCDCOLOR    UInt8
#define LCDSIZETYPE UInt8

#define LCD_X_MAX       127
#define LCD_Y_MAX       31
#define LCD_Y_MAXPAGE   (LCD_Y_MAX/8)



void hal_Lcd_config(void);
void hal_Lcd_clear(UInt8 Mask);
bool hal_Lcd_flush(void* pBuff);
void hal_Lcd_open(void);
void hal_Lcd_lightOn(void);
void hal_Lcd_lightOff(void);

#endif

