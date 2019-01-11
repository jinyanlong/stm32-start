#ifndef __LED_H
#define __LED_H

#include "halLed.h"

#define LED_PHASE_COUNT  3
typedef struct _LEDSTRUCT{
    UInt32 nextTime;     //下一阶段的启动时刻
    int  step;           //分两段,循环次数的2倍,最低位表示所处阶段       
	int phase1;          //第1阶段延迟时间(ms),正值表示开启,负值表示关闭
	int phase2;          //第2阶段延迟时间(ms),正值表示开启,负值表示关闭

} LEDSTRUCT;
void drv_Led_start(UInt8 ledNo,UInt16 cycle,int phase1,int phase2);
void drv_Led_stop(UInt8 ledNo);

void drv_Led_init(void);
void drv_Led_tick(UInt32 now);

#endif
