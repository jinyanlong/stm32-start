#include "includes.h"
#include "Led.h"

#ifdef __LED

volatile static LEDSTRUCT _led[LED_MAXCOUNT];

void drv_Led_init(void){
    UInt8 pinNo;
    for(pinNo=0;pinNo<LED_MAXCOUNT;pinNo++){
        drv_Led_stop(pinNo);                
    }
}

void drv_Led_start(UInt8 pinNo,UInt16 repeat,UInt16 on,UInt16 off){
    _led[pinNo].repeat=repeat;
    _led[pinNo].onTick=_led[pinNo].curStep=on;
    _led[pinNo].offTick=off;
    _led[pinNo].offTick=-_led[pinNo].offTick;
    hal_Led_on(pinNo);
}

void drv_Led_stop(UInt8 pinNo){
    _led[pinNo].repeat=0;
    _led[pinNo].onTick=0;
    _led[pinNo].offTick=0;
    _led[pinNo].curStep=0;
    hal_Led_off(pinNo);
}

void drv_Led_tick(UInt16 _ms){
    UInt8 pinNo;
    for(pinNo=0;pinNo<LED_MAXCOUNT;pinNo++){
        if(_led[pinNo].repeat==0){
            continue;
        }
        if(_led[pinNo].curStep>_led[pinNo].offTick){
            _led[pinNo].curStep-=_ms;
        }else{
            if(_led[pinNo].repeat!=LED_REPEAT_ALL){
                _led[pinNo].repeat--;
            }
            if(_led[pinNo].repeat){
                _led[pinNo].curStep=_led[pinNo].onTick;
                hal_Led_on(pinNo);
            }
        }
        
        if(_led[pinNo].curStep<0){
            hal_Led_off(pinNo);
        }
    }
}
void drv_Led_toggle(UInt8 pinNo){
    _led[pinNo].repeat=0;
    hal_Led_toggle(pinNo);
}

void drv_Led_on(UInt8 pinNo){
    _led[pinNo].repeat=0;
    hal_Led_on(pinNo);
}

void drv_Led_off(UInt8 pinNo){
    _led[pinNo].repeat=0;
    hal_Led_off(pinNo);
}

bool drv_Led_isStopped(UInt8 pinNo){
    return _led[pinNo].repeat==0;
}
///////////////////////////////////////////////////////////////////////////////////////
void fns_Led_start(UInt8 pinNo,UInt16 repeat,UInt16 on,UInt16 off){
    taskDISABLE_INTERRUPTS();       //屏蔽任务系统
    drv_Led_start(pinNo,repeat,on,off);
    taskENABLE_INTERRUPTS();
}
void fns_Led_on(UInt8 pinNo){
    taskDISABLE_INTERRUPTS();       //屏蔽任务系统
    _led[pinNo].repeat=0;
    taskENABLE_INTERRUPTS();
    hal_Led_on(pinNo);
}
void fns_Led_stop(UInt8 pinNo){
    taskDISABLE_INTERRUPTS();       //屏蔽任务系统
    drv_Led_stop(pinNo);
    taskENABLE_INTERRUPTS();
}
void fns_Led_toggle(UInt8 pinNo){
    taskDISABLE_INTERRUPTS();
    drv_Led_toggle(pinNo);
    taskENABLE_INTERRUPTS();
}

#endif
