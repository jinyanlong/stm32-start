#ifndef  __RTC_H
#define  __RTC_H

void hal_RTC_config(void);
void hal_RTC_init(UInt32 startTime);
UInt32 hal_RTC_getCount(void);
//void hal_RTC_setCount(UInt32 nTickCount);
#endif
