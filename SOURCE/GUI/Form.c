#include <includes.h>
#include "Lcd.h"
#include "Form.h"
#include "mem_alloc.h"
#include "RealTimer.h"
#include "halCPU.h"
#include "stringRes.h"
extern const FONTHANDLER FONT_SMALL_FIXED;
extern const FONTHANDLER FONT_NORMAL_AUTO;
extern const FONTHANDLER FONT_NORMAL_FIXED;

static BASEFORM* _pActiveForm;
CDC _formCDC;
bool _formDirty;
UInt8 s_lcdBuff[(LCD_Y_MAX+1)/8][LCD_X_MAX+1];

CDC* drv_Form_getCDC(void){
    return &_formCDC;
}
bool drv_Form_isDirty(void){
    return _formDirty;
}

void drv_Form_redraw(void){
    _formDirty=true;
}

void drv_Form_init(void){
    _pActiveForm=NULL;
    _formDirty=false;
    _formCDC.color=COLOR_BLACK;
    _formCDC.font=&FONT_NORMAL_AUTO;
    _formCDC.lcdBuff=s_lcdBuff;

    drv_FormCDC_reset(&_formCDC);
}



void drv_Form_activeTimer(BASEFORM* pForm,UInt16 ms){
    pForm->timeTick=drv_Time_getTick()+ms;
}

void drv_Form_flush(void){
    _formDirty=false;
    if(!hal_Lcd_flush(_formCDC.lcdBuff)){//刷屏
        drv_Lcd_init();
        hal_Lcd_flush(_formCDC.lcdBuff);
    }
}

void drv_Form_doShow(void){
    drv_FormCDC_reset(&_formCDC);
    if(_pActiveForm->handlers->titleId){
        drv_FormCDC_drawWStringCenter(&_formCDC,0,LoadWString(_pActiveForm->handlers->titleId));
    }
    if(_pActiveForm->handlers->onShow){
        _pActiveForm->handlers->onShow(_pActiveForm,&_formCDC);         //执行绘制操作
	}
    drv_Form_flush();
}


//onTimetick入口
void drv_Form_doTick(UInt32 tick){
    if(_pActiveForm->timeTick==0) return;

    if(tick > _pActiveForm->timeTick){
        _pActiveForm->timeTick=0; //关闭定时器
        if( _pActiveForm->handlers->onTick ){
            _pActiveForm->handlers->onTick(_pActiveForm,tick);
        }
    }
}

void drv_Form_doChar(MESSAGE* pMsg){    
    if(_pActiveForm->handlers->onChar){
        _pActiveForm->handlers->onChar(_pActiveForm,pMsg->p1);
	}
}



void drv_Form_doUserMsg(MESSAGE* pMsg){
    if(_pActiveForm->handlers->onUserMsg){
		_pActiveForm->handlers->onUserMsg(_pActiveForm,pMsg);
	}
}
void drv_Form_onUserEvents(UInt32 events){
        if(_pActiveForm->handlers->onUserEvents){
		_pActiveForm->handlers->onUserEvents(_pActiveForm,events);
	}
}
void drv_Form_onEvents(UInt32 events){        
    if(events & EVENT_GUI_TICK){
        drv_Form_doTick(drv_Time_getTick());
    }
    events&=~EVENT_GUI_TICK;
    if(events){
        drv_Form_onUserEvents(events );
    }
}

void drv_Form_onMsg(MESSAGE* pMsg){
    switch(pMsg->nId){
    case MSGID_GUI_CHAR:
        drv_Form_doChar(pMsg);
        break;
    default:
        drv_Form_doUserMsg(pMsg);
        break;
    }
}


BASEFORM* drv_Form_getRoot(void){
    BASEFORM* pRoot=_pActiveForm;
    if(pRoot){
        while(pRoot->owner){
            pRoot=pRoot->owner;
        }
    }
    return pRoot;
}

//#define FORM_STACK_SIZE (512-8)
//static UInt32 _FormStack[FORM_STACK_SIZE/4+2]={0,FORM_STACK_SIZE};

//BASEFORM* drv_Form_alloc(int size){
//    return (BASEFORM*)drv_Stack_alloc(&_FormStack,size);
//}
//void drv_Form_free(BASEFORM* pThis){
//    drv_Stack_free(&_FormStack,pThis);
//}

void drv_Form_createStatic(BASEFORM* pThis,FORM_HANDLERS const * pHandler){    
    //原窗口失去焦点
    if(_pActiveForm && _pActiveForm->handlers->onFocus){
        _pActiveForm->handlers->onFocus(_pActiveForm,false);
    }
    
    pThis->handlers=pHandler;
    pThis->owner=_pActiveForm;  //当前窗口成为父窗口
    pThis->timeTick=0;         //默认情况下,禁止timeTick
    
    _pActiveForm=pThis;

    //创建新窗口
    if(_pActiveForm->handlers->onCreate){
        _pActiveForm->handlers->onCreate(_pActiveForm);
    }

    //新窗口获得焦点
    if(_pActiveForm->handlers->onFocus){
        _pActiveForm->handlers->onFocus(_pActiveForm,true);
    }
    
    //新窗口绘制
    drv_Form_redraw();

}


//顶层窗口切换
void drv_Form_switchTop(BASEFORM* pThis,FORM_HANDLERS const * pHandler){
    if(drv_Form_getRoot()->handlers!=pHandler){
        drv_Form_reset();
        drv_Form_createStatic(pThis,pHandler);
    }
}

void def_Form_close(void){
    _pActiveForm=_pActiveForm->owner;
}

void drv_Form_close(void){
	if(_pActiveForm){
        //不允许关闭最后顶层界面,否则应使用drv_Form_reset
        if(_pActiveForm->owner==NULL) return;
        
        //原窗口失去焦点
        if(_pActiveForm->handlers->onFocus){
            _pActiveForm->handlers->onFocus(_pActiveForm,false);
        }
    
        //销毁
        if(_pActiveForm->handlers->onDestory){
    	    _pActiveForm->handlers->onDestory(_pActiveForm);
        }
        
        def_Form_close();
        
        //请求新窗口绘制
        drv_Form_redraw();
    }
}

void drv_Form_reset(void){
 	if(_pActiveForm){
        if(_pActiveForm->handlers->onFocus){   //当前窗口失去焦点
            _pActiveForm->handlers->onFocus(_pActiveForm,false);
        }
    
        //销毁
        if(_pActiveForm->handlers->onDestory){
    	    _pActiveForm->handlers->onDestory(_pActiveForm);
        }
        
        def_Form_close();
    }

    while(_pActiveForm){
        if(_pActiveForm->handlers->onDestory){
    	    _pActiveForm->handlers->onDestory(_pActiveForm);
        }
       def_Form_close();
    }
}
BASEFORM* drv_Form_getActive(void){
    return _pActiveForm;
}

// void drv_Form_showWTip(UInt32 resId){
// 	drv_FormCDC_reset(&_formCDC);
//     drv_FormCDC_drawWStringCenter(&_formCDC,1,LoadWString(resId));
// 	drv_Form_flush();
// }

void drv_Form_showTip(string strInfo){
	drv_FormCDC_reset(&_formCDC);
    drv_FormCDC_drawStringCenter(&_formCDC,1,strInfo);
	drv_Form_flush();
}

void drv_Form_onCharDefault(BASEFORM* pThis,UInt32 keyValue){
    int keyCode=keyValue;
	if(keyCode==KEYVALUE_CANCEL){//默认处理
    	drv_Form_close();
    }
}
