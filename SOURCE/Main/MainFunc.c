#include <includes.h>
#include "MainFunc.h"
#include "Led.h"
#include "Keyboard.h"
#include "halCPU.h"
#include "RaceTask.h"


UInt32 mLedTick;

void hook_Keyboard_onPress(UInt32 keyVal,UInt32 keyState){
    drv_Led_start(LED_2,1,500,0);
}

UInt32 lftick;
void drv_System_tick(UInt32 nowTick){ //软件定时器tick
    UInt32 flag;
    
    hal_IDOG_flush();
    if(drv_CPU_checkReset(nowTick)){
        drv_CPU_reset(0);
    }  
    
    drv_Led_tick(nowTick);
    if(nowTick>mLedTick){
        drv_Led_start(LED_1,1,500,0);
        mLedTick=nowTick+2000;
    }

    drv_Keyboard_tick(nowTick);   

    flag=fns_Event_test(EVENT_RX_NRFCOMM|EVENT_RX_PCCOMM);
    event_RaceTask_raise(flag|EVENT_RACE_TICK);

    // if(nowTick>lftick){
    //     lftick=nowTick+50;
    //     lfSendStart(1);
    // }
}

void drv_CPU_reset(int reason){
#ifdef __GUI 
    char strBuff[64];    
    sprintf(strBuff,"System reset(%d) ...",reason);
    drv_Form_init();
    fns_Form_showStringCenter(2,strBuff);    
    drv_LCD_flush(); //showSplash时消息系统尚未启动,所以直接刷屏
    delay_us(1000000);
#endif

    hal_CPU_reset();
}
