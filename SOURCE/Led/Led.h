#ifndef __LED_H
#define __LED_H

#include "halLed.h"

#define LED_REPEAT_ALL   0xFFFF
typedef struct _LEDSTRUCT{
    int repeat;     //重复次数,递减
	int onTick;     //开启长度,ms
	int offTick;    //关闭长度,ms
	int curStep;
} LEDSTRUCT;
void drv_Led_init(void);
void drv_Led_tick(UInt16 _ms);

void drv_Led_start(UInt8 pinNo,UInt16 cycle,UInt16 on,UInt16 off);
void drv_Led_stop(UInt8 pinNo);
bool drv_Led_isStopped(UInt8 pinNo);
void drv_Led_on(UInt8 pinNo);
void drv_Led_off(UInt8 pinNo);
void fns_Led_start(UInt8 pinNo,UInt16 repeat,UInt16 on,UInt16 off);
void fns_Led_stop(UInt8 pinNo);
void fns_Led_on(UInt8 pinNo);
void fns_Led_toggle(UInt8 pinNo);

#endif
