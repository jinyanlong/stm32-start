#ifndef FORM_H_0001
#define FORM_H_0001

#include "Keyboard.h"
#include "CDC.h"
#include "MessageLoop.h"
#include "FormCDC.h"

struct _BASEFORM;

typedef struct _FORM_HANDLERS{
    int titleId;
	void (*onShow)(struct _BASEFORM*,CDC* pFormCDC);
	void (*onChar)(struct _BASEFORM*,UInt32 keyValue);
	void (*onTick)(struct _BASEFORM*,UInt32 now);
	void (*onCreate)(struct _BASEFORM*);
	void (*onDestory)(struct _BASEFORM*);
	void (*onFocus)(struct _BASEFORM*,bool);
    void (*onUserMsg)(struct _BASEFORM*,MESSAGE* pMsg);
    void (*onUserEvents)(struct _BASEFORM*,UInt32 events);
} FORM_HANDLERS;


typedef struct _BASEFORM{
	FORM_HANDLERS const * handlers;
	struct _BASEFORM* owner;
	UInt32 timeTick;   //单位:ms,每当<=0时,触发一次时钟事件
} BASEFORM;

typedef void (*FORM_HANDLER_MSGCHAR)(BASEFORM*,MESSAGE* pMsg);

#define S_TITLE_DEFAULT NULL
#define	fns_NullForm_onDefault  NULL
#define	fns_NullForm_onCreate   NULL
#define	fns_NullForm_onShow     NULL
#define	fns_NullForm_onChar     NULL
    
#define	fns_NullForm_onTick     NULL
#define	fns_NullForm_onLoad         NULL
#define	fns_NullForm_onDestroy      NULL
#define	fns_NullForm_onFocus        NULL


CDC* drv_Form_getCDC(void);

void drv_Form_init(void);
void drv_Form_onEvents(UInt32 events);


void drv_Form_activeTimer(BASEFORM* pForm,UInt16 ms);

bool drv_Form_isDirty(void);
void drv_Form_redraw(void);
void drv_Form_doShow(void);
void drv_Form_doTick(UInt32 tick);
void drv_Form_doChar(MESSAGE* pMsg);
void drv_Form_onMsg(MESSAGE* pMsg);
BASEFORM* drv_Form_getRoot(void);
void drv_Form_close(void);
void drv_Form_reset(void);
void drv_Form_showTip(string strInfo);
// void drv_Form_showWTip(UInt32 resId);
void drv_Form_flush(void);

void drv_Form_onCharDefault(BASEFORM* pThis,UInt32 keyValue);

void drv_Form_createStatic(BASEFORM* pThis,FORM_HANDLERS const * pHandler);

void drv_Form_switchTop(BASEFORM* pThis,FORM_HANDLERS const * pHandler);
BASEFORM* drv_Form_getActive(void);
#endif
