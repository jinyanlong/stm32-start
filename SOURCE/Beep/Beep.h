#ifndef __BEEP_001
#define __BEEP_001


#include "SFTypes.h"
#include "halBeep.h"

#define BEEPROW_COUNT 10
typedef struct _BEEPSTRUCT{
    UInt32 nextTime;               //��һ�ֵ�����ʱ��
    short repeat;                  //�ظ�����
    UInt16 rowCount;
    UInt16 rowStep;                 //��ʱ�������,����ָʾ��ǰִ�е��Ǹ�Ƶ�ʽ׶�
    UInt16 rowFreq[BEEPROW_COUNT];  //�ý׶�Ƶ��
    UInt16 rowTick[BEEPROW_COUNT];  //�ý׶��ӳ�ʱ��
}BEEPSTRUCT;

#define FREQ_L1 262
#define FREQ_L2 294
#define FREQ_L3 330
#define FREQ_L4 349
#define FREQ_L5 392
#define FREQ_L6 440
#define FREQ_L7 494

#define FREQ_M1 523
#define FREQ_M2 578
#define FREQ_M3 659
#define FREQ_M4 698
#define FREQ_M5 784
#define FREQ_M6 880
#define FREQ_M7 988


#define FREQ_H1 1046
#define FREQ_H2 1175
#define FREQ_H3 1318
#define FREQ_H4 1397
#define FREQ_H5 1568
#define FREQ_H6 1760
#define FREQ_H7 1976



//������API
void drv_Beep_init(void);
#define drv_Beep_config    hal_Beep_config
#define drv_Beep_unconfig  hal_Beep_unconfig
void drv_Beep_begin(UInt16 repeat);
void drv_Beep_stop(void);
bool drv_Beep_isStop(void);
void drv_Beep_tick(UInt32 now);
#define drv_Beep_sleep       drv_Beep_stop

void drv_Beep_warn(UInt16 freq,UInt16 nCount,UInt16 _on_ms,UInt16 _off_ms);
void drv_Beep_poweron(UInt16 nCount);
void drv_Beep_ok(UInt16 nCount);

//Ӧ�ò�API
void fns_Beep_warn(UInt16 freq,UInt16 nCount,UInt16 _on_ms,UInt16 _off_ms);
void fns_Beep_poweron(UInt16 nCount);
void fns_Beep_ok(UInt16 nCount);
void fns_Beep_stop(void);
#define fns_Beep_isStop  drv_Beep_isStop

#endif

