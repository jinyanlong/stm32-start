#include <includes.h>
#include "halCPU.h"
#include "RealTimer.h"
#include "MainTask.h"
#include "Led.h"

bool _IsAppStarted;

void appTaskStart(void){
    drv_MainTask_init();
    drv_TimeTick_init();
}

void appInit(void){  
    drv_CPU_init(); 
    drv_Time_init();
    hal_Led_on(LED_RED);
    // hal_Led_on(LED_BLUE);
}

void appConfig(void){
	hal_CPU_config();  
    hal_RTC_config();   
    hal_Led_config();
}

void preAppStartInit(void){
    _IsAppStarted=false;
}

void afterAppStartInit(void){
    _IsAppStarted=true;
}

int main(void){
    preAppStartInit();
    appConfig(); 
    appInit();
    appTaskStart();
    afterAppStartInit();
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

