#include <includes.h>
#include "halCPU.h"
#include "MainFunc.h"
#include "Keyboard.h"
#include "RealTimer.h"
#include "SFTypes.h"
#include "Led.h"
#include "halNrf.h"
#include "halLf.h"
#include "Lcd.h"
#include "halAT45DB.h"
#include "RaceTask.h"
#include "CommDriver.h"
#include "PcComm.h"
#include "Form.h"
#include "Beep.h"

extern bool _IsAppStarted;

void appTaskStart(void){
    drv_RaceTask_init();
    // drv_GuiTask_init(app_GUI_onBeforeMsg,app_GUI_onAfterMsg);
    drv_TimeTick_init();
    
}

void appInit(void){  
    drv_CPU_init(); 
    drv_Form_init();
    drv_Time_init();
    drv_Lcd_init();
    drv_Beep_init();
    drv_Keyboard_init(hook_Keyboard_onPress);
    fns_Keyboard_enable();
    drv_LF_init();
    drv_NRF_init();
}

void appConfig(void){
	hal_CPU_config();  
    hal_Lcd_config();
    hal_RTC_config();   
    hal_Led_config();
    hal_Beep_config();      
    hal_Keyboard_config();
    hal_LF_config();
    hal_PcComm_config();
    hal_Nrf_config();
    hal_AT45DB_config();
    
}

void preAppStartInit(void){
    _IsAppStarted=false;
    drv_Exception_init(app_Exception_onRaise);
    drv_Led_start(LED_1,1,0,100);
    hal_Led_on(LED_1);
}

void afterAppStartInit(void){
    fns_Beep_ok(1);
    app_GUI_showSplash();
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

