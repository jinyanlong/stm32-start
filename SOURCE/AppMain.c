#include <includes.h>
#include "halCPU.h"
#include "Keyboard.h"
#include "RealTimer.h"
#include "SFTypes.h"
#include "Led.h"
#include "hal2.4G.h"

extern volatile UInt32 s_led_activeTick;

void hook_Keyboard_onPress(UInt32 keyVal,UInt32 keyState){
    drv_Led_start(LED_2,1,500,0);
}

void drv_System_tick(UInt32 nowTick){ 

    hal_IDOG_flush();
    if(drv_CPU_checkReset(nowTick)){
        drv_CPU_reset(0);
    }  
    
    drv_Led_tick(nowTick);
    if(nowTick>s_led_activeTick){
        drv_Led_start(LED_1,1,500,0);
        s_led_activeTick=nowTick+2000;
    }

    drv_Keyboard_tick(nowTick);   
    
}


void drv_CPU_reset(int reason){
    (void) reason;
    hal_CPU_reset();
}

void appTaskStart(void){
    drv_TimeTick_init();
}

void drv_LF_init(void){}
void hal_LF_config(void){}
void appInit(void){   
    drv_Time_init();
    drv_Keyboard_init(hook_Keyboard_onPress);
    fns_Keyboard_enable();
    drv_LF_init();
}

void appConfig(void){
	hal_CPU_config();     
    hal_RTC_config();   
    hal_Led_config();
    hal_Keyboard_config();
    hal_24G_config();
    hal_LF_config();
}

int main(void){
    appConfig(); 
    appInit();
    appTaskStart();
    vTaskStartScheduler();
    while(1);
}

//空闲任务的空间分配
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize ){
	static StaticTask_t _task_tcb;
    static StackType_t _task_stack[configMINIMAL_STACK_SIZE];
    
    *ppxIdleTaskTCBBuffer=&_task_tcb;
    *ppxIdleTaskStackBuffer=_task_stack;
    *pulIdleTaskStackSize=sizeof(_task_stack)/sizeof(StackType_t);
}

