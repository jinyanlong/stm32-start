#include <includes.h>
#include "Led.h"

volatile static LEDSTRUCT _led[LED_COUNT];

void drv_Led_init(void){ 
    drv_Led_stop(LED_1); 
    drv_Led_stop(LED_2);
}

void drv_Led_nextStep_(UInt8 pinNo){
    _led[pinNo].step--;

    if( (_led[pinNo].step & 0x01) == 1){  //第一阶段
        if(_led[pinNo].phase1>0){
            _led[pinNo].nextTime+=(UInt32)(_led[pinNo].phase1);
            hal_Led_on(pinNo);
        }else{
            _led[pinNo].nextTime+=(UInt32)(-_led[pinNo].phase1);
            hal_Led_off(pinNo);
        }
    }else{ //第二阶段
        if(_led[pinNo].phase2>0){
            _led[pinNo].nextTime+=(UInt32)(_led[pinNo].phase2);
            hal_Led_on(pinNo);
        }else{
            _led[pinNo].nextTime+=(UInt32)(-_led[pinNo].phase2);
            hal_Led_off(pinNo);
        }
    }
}
void drv_Led_start(UInt8 pinNo,UInt16 repeat,int phase1,int phase2){
    _led[pinNo].nextTime=0;
    _led[pinNo].step=repeat<<1;
    _led[pinNo].phase1=phase1;
    _led[pinNo].phase2=phase2;
   
    if(phase1>0){
        hal_Led_on(pinNo);
    }else{
        hal_Led_off(pinNo);
    }

}

void drv_Led_stop(UInt8 pinNo){
    _led[pinNo].step=0;
    _led[pinNo].phase1=0;
    _led[pinNo].phase2=0;
    _led[pinNo].nextTime=0;
    hal_Led_off(pinNo);
}

void drv_Led_doEvent(UInt8 pinNo,UInt32 now){
    if(_led[pinNo].step==0){
        return;
    }
    if( now > _led[pinNo].nextTime ){
        if(_led[pinNo].nextTime==0){ //修正启动时刻
            _led[pinNo].nextTime=now;
        }
        drv_Led_nextStep_(pinNo);//切换到下一阶段
    }

}
void drv_Led_tick(UInt32 now){
    drv_Led_doEvent(LED_1,now);
    drv_Led_doEvent(LED_2,now);
}

