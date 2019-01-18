#ifndef _MESSAGELOOP_H
#define _MESSAGELOOP_H



typedef struct _MESSAGE{
    UInt32 nId;
    UInt32 p1;
    UInt32 p2;
    UInt32 p3;
} MESSAGE;

typedef void (*FUNCTION_PVOID)(void*);
typedef void (*FUNCTION_VOID)(void);
typedef void (*FUNCTION_INT)(int);
typedef void (*FUNCTION_MSG)(MESSAGE* pMsg);
typedef void (*FUNCTION_EVENTS)(UInt32 events);

typedef struct _CALLBACK_MSG{
    FUNCTION_MSG handler;
    UInt32 filter;
}CALLBACK_MSG;





void* fns_Message_alloc(UInt32 nSize);
void fns_Message_free(void* p);

void event_GuiTask_raise(UInt32 event);

//将消息发送到FORM队列
void msg_GuiTask_post(MESSAGE* msg);
void msg_GuiTask_post0(UInt32 msgId);
void msg_GuiTask_post1(UInt32 msgId, UInt32 p1);


void fns_Message_onBefore(MESSAGE* pMsg);
void fns_Message_onAfter(MESSAGE* pMsg);
void drv_GuiTask_register(string taskName,FUNCTION_EVENTS onEvents,CALLBACK_MSG* pHookList,UInt32 nCount);
void fns_Message_setHooks(CALLBACK_MSG* pHookList,UInt32 nCount);
void fns_Message_clear(void);
bool fns_Message_fetch(MESSAGE* pMsg);
void fns_Message_doEvents(UInt32 filter);




//主循环,主要处理窗口事件,并存取数据
bool drv_GUITask_isCurrent(void);
void drv_GuiTask_run(void* pvParameters);
int drv_System_run(void);
bool drv_System_isRunning(void);

void drv_GuiTask_init(FUNCTION_MSG app_GUI_onBeforeMsg,FUNCTION_MSG app_GUI_onAfterMsg);
#endif
