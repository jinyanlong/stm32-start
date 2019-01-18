#ifndef __REALTIMER_001
#define __REALTIMER_001

#include "SFTypes.h"

#include "halRTC.h"

#define TIMEZONE_UTC 0
#define SYSTICK_MS  10
typedef void (*FUNCTION_TICK)(UInt32 now);


void drv_Time_init(void);
bool drv_Time_isValid(void);

void drv_Time_set(Int64 value);     //ֻ����ʱ������������ִ��,����ָ����RACETASK
void drv_Time_load(void);         //������ʱ�����������ж���ִ��,��ֹʱ�䳤��֮��RTCʱ����DS1302֮��������

UInt32 isr_Time_getTick(void);
UInt32 drv_Time_getTick(void);      //ֻ���ڶ�ʱ��hook��ִ��

Int64 fns_Time_get(void);           //�����ڵ����ȼ�������ִ��
Int64 fns_Time_flush(void);
void drv_TimeTick_init(void);

#endif

