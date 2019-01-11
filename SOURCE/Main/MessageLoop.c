#include <includes.h>
#include "MessageLoop.h"
#include "mem_alloc.h"
#include "halCPU.h"
#include "RealTimer.h"
#include "halLED.h"

FUNCTION_VOID s_hookOnInit=NULL;       //在所有任务建立之后，主任务之中执行
FUNCTION_VOID s_hookGuiOnLoad=NULL;       //在所有任务建立之后，主任务之中执行

FUNCTION_EVENTS s_guiOnEvents=NULL;

FUNCTION_MSG s_hookBeforeMsg=NULL;
FUNCTION_MSG s_hookAfterMsg=NULL;

static xQueueHandle s_mainQueue;
CALLBACK_MSG* _msgHooks;
UInt32 _hooksCount;
volatile UInt32 s_gui_activeTick;

static StaticEventGroup_t m_guiEvents_pcb;
static EventGroupHandle_t m_guiEvents;

static StaticTask_t m_guiTask_pcb;

////////////////////消息系统的内存分配///////////////////////////////////
//简化: 总认为内存能够及时释放,否则就扩大_msgBuffer
#define  MESSAGE_BUFFER_SIZE (2048-8)
static UInt32 _msgBuffer[MESSAGE_BUFFER_SIZE/4+2]={0,MESSAGE_BUFFER_SIZE};
void* fns_Message_alloc(UInt32 nSize){
    return fns_MessageMemory_alloc(_msgBuffer,nSize);
}
void fns_Message_free(void* p){
    fns_MessageMemory_free(p);
}


void event_GuiTask_raise(UInt32 event){
    xEventGroupSetBits(m_guiEvents,event);
}

////////////////////////////////////////////////////////////////////////////
void fns_Message_setHooks(CALLBACK_MSG* pHookList,UInt32 nCount){
    _msgHooks=pHookList;
    _hooksCount=nCount;
}
void fns_Message_clear(void){
    MESSAGE msg;
    
    while( pdTRUE==xQueueReceive(s_mainQueue,&msg,0) );
}


bool drv_GUI_doEvents(void){
    EventBits_t event;
    UInt32 j;
    
    if(drv_Exception_get()!=NULL){ //初始化阶段发现异常,补充一个异常事件
        event_GuiTask_raise(EVENT_GUI_EXCEPTION);
    }
    
    while(1){
        event= xEventGroupWaitBits(m_guiEvents,EVENT_TASK_ALL,pdTRUE,pdFALSE,portMAX_DELAY);
        if(event&EVENT_GUI_TICK){
            s_gui_activeTick=drv_Time_getTick();
        }
        if(event&EVENT_GUI_MSG){
            MESSAGE msg;
            while( xQueueReceive(s_mainQueue,&msg,0)==pdTRUE ){
                if(s_hookBeforeMsg) s_hookBeforeMsg(&msg);
                for(j=0;j<_hooksCount;j++){
                    if(_msgHooks[j].filter & msg.nId){
                        _msgHooks[j].handler(&msg);
                    }
                }
                if(s_hookAfterMsg) s_hookAfterMsg(&msg);                
            }

        }
        
        if(s_guiOnEvents){
            s_guiOnEvents( event&(~EVENT_GUI_MSG));
        }
    }
}


void drv_GuiTask_register(string taskName,FUNCTION_EVENTS onEvents, CALLBACK_MSG* pHookList,UInt32 nCount){
    static UInt32 _task_stack[STACKSIZE_GUITASK];

    _msgHooks=pHookList;
    _hooksCount=nCount;
    
    s_guiOnEvents=onEvents;
    s_gui_activeTick=drv_Time_getTick();
    
    m_guiEvents=xEventGroupCreateStatic(&m_guiEvents_pcb); 

    //创建任务
    xTaskCreateStatic( drv_GuiTask_run,taskName, STACKSIZE_GUITASK, NULL, TASK_PRIORITY_GUI, _task_stack,&m_guiTask_pcb);
}






void msg_GuiTask_post(MESSAGE* msg){
    xQueueSend(s_mainQueue,msg,0);
    event_GuiTask_raise(EVENT_GUI_MSG);
}

void msg_GuiTask_post0(UInt32 msgId){
    MESSAGE msg;
    msg.nId = msgId;
    msg_GuiTask_post(&msg);
}

void msg_GuiTask_post1(UInt32 msgId, UInt32 p1){
    MESSAGE msg;
    msg.nId = msgId;
    msg.p1=p1;
    msg_GuiTask_post(&msg);
}


//主消息循环,主要处理窗口事件,并存取数据
void drv_GuiTask_run(void* pvParameters){
    if( s_hookGuiOnLoad) s_hookGuiOnLoad();
    drv_GUI_doEvents();
}

bool drv_GUITask_isCurrent(void){
    return (&m_guiTask_pcb)==xTaskGetCurrentTaskHandle();
}
bool drv_System_isRunning(void){
    return s_hookOnInit==NULL;
}
int drv_System_run(void){
    static StaticQueue_t _queue_pcb;
    static UInt8 _queue_buffer[50*sizeof(MESSAGE)];

    //建立消息队列
    _hooksCount=0;
    _msgHooks=NULL;
    s_mainQueue = xQueueCreateStatic( 50, sizeof(MESSAGE), _queue_buffer,&_queue_pcb );
    
    s_hookOnInit();    //drv_System_init;
    s_hookOnInit=NULL; // 清除初始化回掉，表示初始化已经完成    
    
    //启动调度器
    vTaskStartScheduler();

    while(1);//调度器工作不正常时,利用WATCHDOG复位
}
