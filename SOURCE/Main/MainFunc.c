#include <includes.h>
#include "MainFunc.h"
#include "Led.h"
#include "Keyboard.h"
#include "halCPU.h"
#include "RaceTask.h"
#include "Form.h"
#include "MessageLoop.h"
#include "Lcd.h"
#include "Beep.h"

extern volatile UInt32 s_race_activeTick;


void hook_Keyboard_onPress(UInt32 keyVal,UInt32 keyState){
    drv_Led_start(LED_2,1,500,0);
}


void drv_System_tick(UInt32 nowTick){ //软件定时器tick
    UInt32 flag;
    
    hal_IDOG_flush();
    if(drv_CPU_checkReset(nowTick)){
        drv_CPU_reset(0);
    }  
    
    drv_Keyboard_tick(nowTick);
    drv_Led_tick(nowTick);
    drv_Beep_tick(nowTick);

    if( drv_Exception_get()){
        //异常
        return;
    }

    if(drv_RaceTask_isReady()){
#ifndef __DEBUG
        //检查每一个TASK最后的刷新时刻
        if( nowTick > (s_race_activeTick+TASK_RACE_OVERTIME) ){
            drv_CPU_reset(-1);
        }
#endif

    flag=fns_Event_test(EVENT_RX_NRFCOMM|EVENT_RX_PCCOMM);
    event_RaceTask_raise(flag|EVENT_RACE_TICK);

    }
}

void drv_CPU_reset(int reason){
    char strBuff[64];    
    sprintf(strBuff,"System reset(%d) ...",reason);
    drv_Form_init();
    drv_Form_showTip(strBuff);    
    delay_us(2000000);

    hal_CPU_reset();
}

//异常处理函数
void app_Exception_onRaise(Exception* exception){
     if(!drv_System_isRunning()){
        //初始化尚未完成
         fns_ExceptionForm_showTip(exception);
         //启动有限的串口通讯
         ///...
         while(1);
     }else{
        //抛出异常的位置,分为3种:在GUITask中，在RaceTask, 在其它Task中
        //1. 如果在GUITask中,这个函数不退出，就不能绘制界面,所以必须在这里直接绘制
        //2. 如果在RaceTask中,这个函数不退出，就不能通讯,所以必须在这里执行通讯操作
        //3. 如果在其它task中,只要向GuiTask 抛送异常事件即可
        
        if( drv_RaceTask_isCurrent() ){
            drv_RaceTask_doEvents(EVENT_TASK_ALL);
        }else{
            if( drv_GUITask_isCurrent() ){
                fns_ExceptionForm_showTip(exception);
            }else{
                event_GuiTask_raise(EVENT_GUI_EXCEPTION);
            }
            
            //死循环,直到断电退出
            while(1){
                delay_ms(100);
            }
        }
    }
}

void app_GUI_onBeforeMsg(MESSAGE* pMsg){   
    // switch(pMsg->nId){
    // case MSGID_GUI_CHAR:
    //     //复位按键的处理
    //     if( (pMsg->p1==KEYVALUE_RESET) && (pMsg->p2&KEYSTATE_LONG) ){
    //         //恢复出厂设置
    //         OS_LOCK();
    //         drv_Race_format(1);
    //         OS_UNLOCK();
    //         drv_CPU_reset(0);
    //     }


    //     ///////////////////////////////
    //     if( pMsg->p1!=KEYVALUE_RESET ){
    //         fns_Beep_warn(FREQ_H4,1,50,50);
    //     }
    //     break;
    // default:
    //     break;
    // }
}

void app_GUI_onAfterMsg(MESSAGE* pMsg){

}


void app_GUI_showSplash(void){
    extern CDC _formCDC;
    extern const FONTHANDLER FONT_NORMAL_AUTO;
    extern const FONTHANDLER FONT_NORMAL_FIXED;
    
    char strBuff[32];
    drv_FormCDC_reset(&_formCDC);
    // drv_FormCDC_drawWString(&_formCDC,SCREEN_PAGE1+1,20,LoadWString(S_COMPANY));
    
    _formCDC.font=&FONT_NORMAL_FIXED;
    sprintf(strBuff,"V%X.%02X",DEV_VER>>8,DEV_VER&0xFF);
    drv_FormCDC_drawString(&_formCDC,SCREEN_PAGE1+1,78,strBuff);
    drv_FormCDC_drawStringCenter(&_formCDC,SCREEN_PAGE1+2,"www.yz-online.com");
    _formCDC.font=&FONT_NORMAL_AUTO;
	drv_Form_flush();

    // char strBuff[32];
    // sprintf(strBuff,"V%X.%02X",DEV_VER>>8,DEV_VER&0xFF);
    // if(!hal_Lcd_flush(strBuff)){//刷屏
    //     drv_Lcd_init();
    //     hal_Lcd_flush(strBuff);
    // }

}
