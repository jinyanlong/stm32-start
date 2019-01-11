#ifndef __TICKTIMER_001
#define __TICKTIMER_001

typedef struct _TICKTIMER{
    int tickCount;
    struct _TICKTIMER*  nextPtr;
}TICKTIMER;

//void fns_TickTimer_register(TICKTIMER* pTimer);
//void fns_TickTimer_onTick(int tickCount);
//bool fns_TickTimer_isOut(volatile TICKTIMER* pTm);
void fns_TickTimer_postMsg(UInt8 prior,UInt16 _ms); //发送时间片消息
void fns_TickTimer_task(void* pvParameters);
void fns_TickTimer_start(int _ms);
bool fns_TickTimer_isover(void);
void fns_TickTimer_wait(void);
void fns_TickTimer_stop(void);
void fns_TickTimer_delay(int _ms);


#endif

