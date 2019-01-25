#include "includes.h"
#include "halCPU.h"
#include "MainTask.h"
#include "MainData.h"
#include "RealTimer.h"
#include "Led.h"
#include "halLf.h"
#include "PcComm.h"
#include "CommDriver.h"
#include "lf.h"

UInt8 _systemRnd[8];
DESKEYSTRUCT desKey; //desKey实际上是局部变量,提升至全局,是为了减少栈空间消耗

volatile UInt32 s_main_activeTick;
static xQueueHandle m_mainQueue;
static EventGroupHandle_t m_mainEvents;
static UInt32 _mainTask_stack[STACKSIZE_MAINTASK];
static StaticTask_t _mainTask_tcb;
static StaticEventGroup_t m_mainEvents_pcb;

bool m_MainTask_ready=false;
bool drv_MainTask_isReady(void){
    return m_MainTask_ready;
}
void event_RaceTask_raise(UInt32 event){
    xEventGroupSetBits(m_mainEvents,event);
}

extern UartPacketHandler _pcCommUart;
void drv_MainTask_onTick(void){
    static UInt32 tickOut=0;
    UInt32 nowTick;

    uint32_t powv;
    static UInt8 vcc=0;

    s_main_activeTick=nowTick=drv_Time_getTick();

    if(nowTick>tickOut){

        drv_Led_start(LED_RED,1,200,0);

        // drv_LF_send(0x0011);

        vcc++;
        if(vcc>0x7F){vcc=0;} 
            
        hal_MCP4018_write(MCP4018T_1,&vcc, 1);
        powv=drv_GET_LFPOWVCC(MCP4018T_1);//天线电源电压

        
        tickOut=nowTick+1000;
    }


}

void drv_MainTask_onMsg(MESSAGE* pMsg){
    switch(pMsg->nId){
    default:
        break;
    }
}
extern UartPacketHandler _pcCommUart;
void drv_mainTask_run(void *p){
    EventBits_t event;
    m_MainTask_ready=true;

    while(1){
        event= xEventGroupWaitBits(m_mainEvents,EVENT_TASK_ALL,pdTRUE,pdFALSE,portMAX_DELAY);

        if(event&EVENT_MAIN_TICK){
            drv_MainTask_onTick();
        }

        if(event&EVENT_RX_PCCOMM){            
            drv_PcComm_onEvent();
            drv_Comm_startRx(&_pcCommUart);          
        }
                        
        if(event&EVENT_MAIN_MSG){
            //检查消息队列,如果有则执行
            MESSAGE msg;
    
            while( xQueueReceive(m_mainQueue,&msg,0)==pdTRUE ){//xQueuePeek(m_mainQueue,&msg,0)==pdTRUE
                drv_MainTask_onMsg(&msg);
            }
        }
    }
}
void drv_MainTask_init(void){
    static StaticQueue_t _queue_pcb;
    static UInt8 _queue_buffer[50*sizeof(MESSAGE)];

    s_main_activeTick=drv_Time_getTick();

    //建立消息队列
    m_mainQueue = xQueueCreateStatic(sizeof(_queue_buffer)/sizeof(MESSAGE), sizeof(MESSAGE), _queue_buffer,&_queue_pcb );
    
    m_mainEvents=xEventGroupCreateStatic(&m_mainEvents_pcb);   
    xTaskCreateStatic(drv_mainTask_run, "MAIN", STACKSIZE_MAINTASK, NULL,TASK_PRIORITY_MAIN, _mainTask_stack, &_mainTask_tcb );    

    
}
