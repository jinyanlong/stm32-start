#ifndef __LED_H
#define __LED_H

#include "halLed.h"

#define LED_PHASE_COUNT  3
typedef struct _LEDSTRUCT{
    UInt32 nextTime;     //��һ�׶ε�����ʱ��
    int  step;           //������,ѭ��������2��,���λ��ʾ�����׶�       
	int phase1;          //��1�׶��ӳ�ʱ��(ms),��ֵ��ʾ����,��ֵ��ʾ�ر�
	int phase2;          //��2�׶��ӳ�ʱ��(ms),��ֵ��ʾ����,��ֵ��ʾ�ر�

} LEDSTRUCT;
void drv_Led_start(UInt8 ledNo,UInt16 cycle,int phase1,int phase2);
void drv_Led_stop(UInt8 ledNo);

void drv_Led_init(void);
void drv_Led_tick(UInt32 now);

#endif
