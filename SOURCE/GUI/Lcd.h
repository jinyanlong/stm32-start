#ifndef _LCD_H
#define _LCD_H

#include "halLcd.h"
//����LCDӲ����صĴ����������������ִ��
void drv_Lcd_init(void);
void drv_Lcd_lightOn(void);
void drv_Lcd_lightOff(void);
bool drv_Lcd_isLightOn(void);


#define fns_Lcd_lightOn     drv_Lcd_lightOn     
#define fns_Lcd_lightOff    drv_Lcd_lightOff

#endif
