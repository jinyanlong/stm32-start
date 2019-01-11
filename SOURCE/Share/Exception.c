#include "includes.h"
#include "stringRes.h"
#include "font.h"
#include "MessageLoop.h"
#include "RealTimer.h"
#include "Exception.h"
#include "halCPU.h"
#include "Form.h"
#include "CommDriver.h"
#include "RaceTask.h"
#include "MainFunc.h"

#define ERROR_RESET_INTERVAL 600000

void fns_Exception_doShow(void);

extern void fns_Comm_onMsg(MESSAGE* pMsg);
extern void fns_Form_onMsg(MESSAGE* pMsg);
//static CALLBACK_MSG _exceptionHandler[]={
//    {fns_Comm_onMsg,MSGID_COMM},         //串口通讯
//    {fns_Form_onMsg,MSGID_FORMSHOW|MSGID_CHAR|MSGID_TIMETICK},                    //界面显示
//};
static Exception m_first_exception;


void drv_Exception_init(void){
    memset(&m_first_exception,0,sizeof(Exception));
}


Exception* drv_Exception_get(void){
    if(m_first_exception.errType){
        return &m_first_exception;
    }else{
        return NULL;
    }
}

void drv_Exception_throwEx(UInt16 errType,UInt16 errCode, const char* desc){ 
    if(m_first_exception.errType!=0) return; //只保存第一个异常

    m_first_exception.errType=errType;
    m_first_exception.errCode=errCode;
    if(desc){
        strcpy(m_first_exception.desc,desc);
    }
    if(!drv_System_isRunning()) return;
    
    //抛出异常的位置,分为3种:在GUITask中，在RaceTask, 在其它Task中
    //1. 如果在GUITask中,这个函数不退出，就不能绘制界面,所以必须在这里直接绘制
    //2. 如果在RaceTask中,这个函数不退出，就不能通讯,所以必须在这里执行通讯操作
    //3. 如果在其它task中,只要向RaceTask 抛送异常事件即可
    
    if( drv_RaceTask_isCurrent() ){
#ifdef __GUI 
        event_GuiTask_raise(EVENT_GUI_EXCEPTION);
#endif 
        drv_RaceTask_doEvents(EVENT_TASK_ALL);
    }else{
        
 #ifdef __GUI        
        if( drv_GUITask_isCurrent() ){
            fns_Exception_doShow();
        }else{
            event_GuiTask_raise(EVENT_GUI_EXCEPTION);
        }
#endif        
        //死循环,直到断电退出
        while(1){
            delay_ms(100);
        }
    }
}

void drv_Exception_throw(UInt16 errType){
    drv_Exception_throwEx(errType,0,NULL);
}

typedef struct _EXCEPTIONFORM{
	FORM_HANDLERS const * handlers;
	struct _BASEFORM* owner;
	int timeTick;   //单位:ms,每当<=0时,触发一次时钟事件
    UInt32 resetTick;
} EXCEPTIONFORM;

void fns_ExceptionForm_onShow(BASEFORM* pThis,CDC* pFormCDC);
void fns_ExceptionForm_onTick(BASEFORM* pBase,UInt32 nowTick);
void fns_ExceptionForm_onCreate(BASEFORM* pBase);
void fns_ExceptionForm_onChar(BASEFORM* pThis,UInt32 keyValue);

const FORM_HANDLERS _hookExceptionForm={
    NULL,        
	fns_ExceptionForm_onShow,
	fns_ExceptionForm_onChar,
	fns_ExceptionForm_onTick,
	fns_ExceptionForm_onCreate,
    fns_NullForm_onDestroy,
    fns_NullForm_onFocus,
    NULL,
    NULL
};

void fns_ExceptionForm_create(void){
    static EXCEPTIONFORM _frmException={&_hookExceptionForm,NULL,-1}; //必须是静态/全局变量
    drv_Form_reset();
    drv_Form_createStatic((BASEFORM*)&_frmException,&_hookExceptionForm);
}
void fns_ExceptionForm_onCreate(BASEFORM* pBase){
    EXCEPTIONFORM* pThis=(EXCEPTIONFORM*)pBase;

    pThis->resetTick= drv_Time_getTick()+ERROR_RESET_INTERVAL;
}
void fns_ExceptionForm_onChar(BASEFORM* pBase,UInt32 keyValue){
    EXCEPTIONFORM* pThis=(EXCEPTIONFORM*)pBase;
    if(keyValue==KEYVALUE_CANCEL){
        drv_CPU_reset(0);
    }else if(keyValue==KEYVALUE_OK){
        pThis->resetTick= drv_Time_getTick()+ERROR_RESET_INTERVAL;
    }
}

void fns_ExceptionForm_onTick(BASEFORM* pBase,UInt32 nowTick){
    EXCEPTIONFORM* pThis=(EXCEPTIONFORM*)pBase;
    if(nowTick > pThis->resetTick){
        drv_CPU_reset(1);
    }
    pBase->timeTick= nowTick+1000;
}

void fns_ExceptionForm_onShow(BASEFORM* pBase,CDC* pFormCDC){
    char strText[64];  
    Exception* exception=drv_Exception_get();
    
    if(exception==NULL) return;
    sprintf(strText,"ERROR: %04X-%04X",exception->errType,exception->errCode);
    drv_FormCDC_drawStringCenter(pFormCDC,1,strText);
}

void fns_ExceptionForm_showTip(Exception* exception){
    char strText[64];
    if(exception==NULL) return;        
    sprintf(strText,"ERROR: %04X-%04X",exception->errType,exception->errCode);
    
    drv_Form_init();
    drv_Form_showTip(strText);
}

#ifdef __GUI 
    extern CDC _formCDC;
#endif

typedef struct _EXCEPTION_DESC{
    UInt16 errType;
    UInt16 stringId;
}EXCEPTION_DESC;

const EXCEPTION_DESC _exception_list[]={
    {ERROR_PARAM_OVER,S_ERROR_PARAM_OVER},
    {ERROR_TIMER_INIT,S_ERROE_TIMER},
    {ERROR_TIMER_SET,S_ERROE_TIMER},
    {ERROR_TIMER_GET,S_ERROE_TIMER},
    {ERROR_TIMER_FORMAT,S_ERROE_TIMER_FORMAT},
    {ERROR_TIMER_INVALIDATE,S_ERROE_TIMER_INVALIDATE},
    {ERROR_FLASH,S_ERROE_FLASH},
    {ERROR_FILE_ROVERSIZE,S_ERROE_FILE_OVERSIZE},
    {ERROR_FILE_WOVERSIZE,S_ERROE_FILE_OVERSIZE},
    {ERROR_FILE_FORMAT,S_ERROE_FILE_FORMAT},
    {ERROR_FILE_SYSID,S_ERROE_SYSTEM},
    {ERROR_BUG_OS,S_ERROE_OS},
    {ERROR_BUG_TAGNO,S_ERROE_TAG}
};
UInt16 fns_ExceptionForm_getId(UInt16 errType){
    int i;
    for(i=0;i<sizeof(_exception_list)/sizeof(EXCEPTION_DESC);i++){
        if( _exception_list[i].errType == errType ){
            return _exception_list[i].stringId;
        }
    }
    return S_ERROE_UNKNOWN;
}


void fns_Exception_doShow(void){
    
#ifdef __GUI    
    char strText[64];
    Exception* exception=drv_Exception_get();
    if(exception==NULL) return;
    
    fns_Form_clearLine(0);
	fns_Form_clearLine(1);
	fns_Form_clearLine(2);
	fns_Form_clearLine(3);
	fns_Form_clearLine(4);
        
    sprintf(strText,"ERROR: %04X-%04X",exception->errType,exception->errCode);
    fns_CDC_drawString(&_formCDC,(LCD_X_MAX-strlen(strText)*FONT_CHAR_WIDTH)/2,2*FONT_CHAR_HEIGHT,strText);
    fns_Form_showUStringIdCenter(1,fns_ExceptionForm_getId(exception->errType));
    
    //显示更详细的错误信息
    if(strlen(exception->desc)*FONT_CHAR_WIDTH >=LCD_X_MAX ){
        fns_CDC_drawString(&_formCDC,0,3*FONT_CHAR_HEIGHT,exception->desc);
    }else{
        fns_CDC_drawString(&_formCDC,(LCD_X_MAX-strlen(exception->desc)*FONT_CHAR_WIDTH)/2,3*FONT_CHAR_HEIGHT,exception->desc);
    }
    drv_LCD_flush();
    drv_LCD_invalidate();
#endif    

}
    

