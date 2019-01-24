#include <includes.h>
#include "halCPU.h"
#include "RealTimer.h"
#include "MainTask.h"
#include "Led.h"
#include "Lf.h"
#include "halMCP4018T.h"
#include "halAt24.h"
#include "flash.h"

bool _IsAppStarted;

void appTaskStart(void){
    UInt8 pro,val;
    bool result;
    uint8_t test1[3];
    uint8_t test[3]={'a','b','c'};
    drv_MainTask_init();
    drv_TimeTick_init();
    // pro=drv_adjust_LFCap(LF_ANTA);
    // pro=drv_adjust_LFCap(LF_ANTB);
    // pro=drv_adjust_LFCap(LF_ANTM);
    // pro=drv_adjust_LFCap(LF_ANTN);

    // result=hal_MCP4018_read(MCP4018T_1,&val,1);
    // result=hal_MCP4018_read(MCP4018T_2,&val,1);

   // result=drv_Flash_write(FLASHADDR_BASEINFO,&test,3);
    //result=drv_Flash_read(FLASHADDR_BASEINFO,&test1,3);
}

void appInit(void){  
    drv_CPU_init(); 
    drv_Time_init();
    hal_AT24C64_init();
    drv_LF_init();
}

void appConfig(void){
	hal_CPU_config();  
    hal_RTC_config();
    hal_AT24C64_config();   
    hal_Led_config();
    drv_LF_config();
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

//��������Ŀռ����
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize ){
	static StaticTask_t _task_tcb;
    static StackType_t _task_stack[configMINIMAL_STACK_SIZE];
    
    *ppxIdleTaskTCBBuffer=&_task_tcb;
    *ppxIdleTaskStackBuffer=_task_stack;
    *pulIdleTaskStackSize=sizeof(_task_stack)/sizeof(StackType_t);
}

