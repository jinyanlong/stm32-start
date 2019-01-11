#include  "includes.h"
#include  "lcd.h"
static bool _Lcd_lightOn;

void drv_Lcd_init(void){
    hal_Lcd_open();
    _Lcd_lightOn=true;
}
void drv_Lcd_lightOn(void){
    if(!_Lcd_lightOn){
        _Lcd_lightOn=true;
        //hal_Lcd_init();
        hal_Lcd_lightOn();
    }
}
void drv_Lcd_lightOff(void){
    if(_Lcd_lightOn){
        _Lcd_lightOn=false;
        hal_Lcd_lightOff();
    }
}
bool drv_Lcd_isLightOn(void){
    return _Lcd_lightOn;
}


