#ifndef __REALTIMER_001
#define __REALTIMER_001

#include "SFTypes.h"

#include "halRTC.h"

#define TIMEZONE_UTC 0
#define SYSTICK_MS  10
typedef void (*FUNCTION_TICK)(UInt32 now);


void drv_Time_init(void);
bool drv_Time_isValid(void);

void drv_Time_set(Int64 value);     //只能在时钟驱动例程中执行,现在指的是RACETASK
void drv_Time_load(void);         //必须在时钟驱动例程中定期执行,防止时间长了之后RTC时钟与DS1302之间产生误差

UInt32 isr_Time_getTick(void);
UInt32 drv_Time_getTick(void);      //只能在定时器hook中执行

Int64 fns_Time_get(void);           //可以在低优先级例程中执行
Int64 fns_Time_flush(void);
void drv_TimeTick_init(void);

#endif

