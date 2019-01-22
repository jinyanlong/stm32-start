#include <includes.h>
#include "TickTimer.h"


FUNCTION_TICK s_hookTick=NULL;
////////////定时器机制,过于复杂暂不用/////////////////////////////////
//static TICKTIMER*  _timerRoot=NULL;
//void fns_TickTimer_register(TICKTIMER* pTimer){
//    TICKTIMER** ppTm=&_timerRoot; 
//    taskDISABLE_INTERRUPTS(); 
//    ppTm=&_timerRoot;
//    while(*ppTm){
//        ppTm=&(*ppTm)->nextPtr;
//    }
//    *ppTm=pTimer;
//    taskENABLE_INTERRUPTS();
//}

////这个函数在fns_Tick_task中调用,
////已包含在taskDISABLE_INTERRUPTS,taskENABLE_INTERRUPTS中,所以这里不考虑同步问题
//void fns_TickTimer_onTick(int tickCount){
//    TICKTIMER** ppTm=&_timerRoot;
//    while(*ppTm){
//    if( (*ppTm)->tickCount > tickCount){
//          (*ppTm)->tickCount-=tickCount;
//        }else{
//            //超时以后直接从列表中去除
//            (*ppTm)->tickCount=0;
//            (*ppTm)=(*ppTm)->nextPtr;
//            if(NULL==(*ppTm)) break;
//        }
//        ppTm=&(*ppTm)->nextPtr;
//    }
//}
////检查TICKTIMER是否已超时,改函数不需要考虑同步
//bool fns_TickTimer_isOut(volatile TICKTIMER* pTm){
//    int tc=pTm->tickCount;
//    return (tc<=0);
//}
//
//void fns_TimerTick_delay(int _ms){
//    TICKTIMER timer;
//    timer.tickCount=_ms;
    
//    fns_TickTimer_register(&timer);
//    while( fns_TickTimer_isOut(&timer) ){
//        fns_Message_doEvents();
//    }
//}
////////////////////////////////////////////////////////////////////////////////////////
//////////////简化的定时器机制,只能在主消息循环中使用,不能嵌套调用//////////////////////
volatile int _singleton_delay;
void fns_TickTimer_start(int _ms){
    ATOMIC_SET(_singleton_delay,_ms);
}

bool fns_TickTimer_isover(void){
    return (_singleton_delay<=0);
}

void fns_TickTimer_wait(void){
    while(_singleton_delay>0){
		fns_Message_doEvents(MSGID_EXCEPTION|MSGID_TAG_DATA);
    }
}
void fns_TickTimer_stop(void){
    ATOMIC_SET(_singleton_delay,0);
}
void fns_TickTimer_delay(int _ms){
    fns_TickTimer_start(_ms);
    fns_TickTimer_wait();
}

////////////////////////////////////////////////////////////////////////////////////////

void fns_TickTimer_postMsg(UInt8 prior,UInt16 _ms){
    extern xQueueHandle s_mainQueue[MESSAGE_PRIOT_COUNT];

    MESSAGE msg;
    msg.nId = MSGID_TIMETICK;
    msg.p1=_ms;
    xQueueSend(s_mainQueue[prior],&msg,0);
}

void fns_TickTimer_task(void* pvParameters){ 
    // 使用当前时间 初始化xLastWakeTime 变量
    portTickType xLastWakeTime = xTaskGetTickCount();
    
    while(1){
        //等待下一个循环(10ms)
        vTaskDelayUntil( &xLastWakeTime, SYSTICK_MS );
        
        __disable_irq(); 
        if(_singleton_delay>=SYSTICK_MS){
            _singleton_delay-=SYSTICK_MS;
        }
        __enable_irq();

        //taskDISABLE_INTERRUPTS();      //屏蔽任务系统
        //taskENABLE_INTERRUPTS();       //屏蔽任务系统        
        if(s_hookTick) s_hookTick(SYSTICK_MS);

        //#ifndef __DEBUG
        IWDG_ReloadCounter();
        //#endif
    }
}
