#include <includes.h>
#include "Beep.h"

volatile static BEEPSTRUCT _beep;

void drv_Beep_init(void){
     _beep.repeat=-1;
     drv_Beep_stop();
}

void drv_Beep_begin(UInt16 repeat){
    _beep.repeat=(short)repeat;
    _beep.nextTime=0;
    _beep.rowStep=0;

}

void drv_Beep_stop(void){
    _beep.repeat=-1;
    hal_Beep_stop();
}


bool drv_Beep_isStop(void){
    return (_beep.repeat<=0);
}
void drv_Beep_gotoStep(UInt16 step){
    if( _beep.rowStep<_beep.rowCount ){
        _beep.rowStep=step;
        hal_Beep_set(_beep.rowFreq[step]);   
    }
}

void drv_Beep_tick(UInt32 now){
    if( _beep.repeat<0 ){
        return;
    }

    if( now >= _beep.nextTime){
        int i;
        //进入下一轮循环
        _beep.repeat--;
        if(_beep.repeat<0){ //确保处在最后一个频率
            drv_Beep_stop();
            return;
        }
        
        //计算下一轮
        _beep.nextTime=now;
        for( i=0;i<_beep.rowCount;i++){
            _beep.nextTime+=_beep.rowTick[i];
        }
        
        drv_Beep_gotoStep(0);
    }else {   //本轮循环之内
        int i;
        UInt32 tick=0;
        for(i=_beep.rowStep+1;i<_beep.rowCount;i++){
            tick+=_beep.rowTick[i];
        }
        if( (now+tick) >= _beep.nextTime ){//需要切换频率
            drv_Beep_gotoStep(_beep.rowStep+1);    
        }
    }
}


void drv_Beep_warn(UInt16 freq,UInt16 nCount,UInt16 on_ms,UInt16 off_ms){
    _beep.rowFreq[0]=freq;
    _beep.rowTick[0]=on_ms;
    _beep.rowFreq[1]=0;
    _beep.rowTick[1]=off_ms;
    _beep.rowCount=2;
    drv_Beep_begin(nCount);
}

void fns_Beep_warn(UInt16 freq,UInt16 nCount,UInt16 on_ms,UInt16 off_ms){
    taskDISABLE_INTERRUPTS();
    drv_Beep_warn(freq,nCount,on_ms,off_ms);
    taskENABLE_INTERRUPTS();
}

void drv_Beep_ok(UInt16 nCount){
    _beep.rowFreq[3]=FREQ_M4;
    _beep.rowTick[3]=150;
    _beep.rowFreq[2]=FREQ_M3;
    _beep.rowTick[2]=150;
    _beep.rowFreq[1]=FREQ_L4;
    _beep.rowTick[1]=80;
    _beep.rowFreq[0]=FREQ_L3;
    _beep.rowTick[0]=100;
    _beep.rowCount=4;
    drv_Beep_begin(nCount);

}


void fns_Beep_poweron(UInt16 nCount){  
    taskDISABLE_INTERRUPTS();
    drv_Beep_ok(nCount);
    taskENABLE_INTERRUPTS();
}


void fns_Beep_ok(UInt16 nCount){
    taskDISABLE_INTERRUPTS();
    drv_Beep_ok(nCount);
    taskENABLE_INTERRUPTS();

}

void fns_Beep_stop(void){
    taskDISABLE_INTERRUPTS();
    drv_Beep_stop();
    taskENABLE_INTERRUPTS();
}
