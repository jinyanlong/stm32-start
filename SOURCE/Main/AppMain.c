#include <includes.h>
#include "halCPU.h"
#include "RealTimer.h"
#include "MainTask.h"
#include "Led.h"
#include "Lf.h"
#include "halMCP4018T.h"
#include "halAt24.h"
#include "flash.h"
#include "PcComm.h"

bool _IsAppStarted;
UInt8  ROM_WORKMODE;
UInt8  ROM_PROXYID[2];

UInt8 s_systemId[4]={0,0,0,0};  //前2字节为代理商编号,后2字节为学校编号
UInt32 s_systemMark=0;
UInt8 s_systemKey[8];

UInt16 m_antIdA=0x0011;
UInt16 m_antIdB=0x0011;
UInt16 m_antIdM=0x0011;
UInt16 m_antIdN=0x0011;

extern UartPacketHandler _pcCommUart;

void appTaskStart(void){
    UInt8 pro,val;
//    UInt8 vcc=0x15;
//    UInt8 vcc1=0x26;
     bool result;
  //  uint8_t test1[3];
  //  uint8_t test[3]={'a','b','c'};

    drv_MainTask_init();
    drv_TimeTick_init();
    
    // pro=drv_adjust_LFCap(LF_ANTA);
    // pro=drv_adjust_LFCap(LF_ANTB);
    // pro=drv_adjust_LFCap(LF_ANTM);
    // pro=drv_adjust_LFCap(LF_ANTN);

    // hal_MCP4018_write(MCP4018T_1,&vcc, 1);
//     hal_MCP4018_write(MCP4018T_2,&vcc1, 1);

    result=hal_MCP4018_read(MCP4018T_1,&val,1);
    result=hal_MCP4018_read(MCP4018T_2,&val,1);

//    result=drv_Flash_write(FLASHADDR_BASEINFO,&test,3);
    // result=drv_Flash_read(FLASHADDR_BASEINFO,&test1,3);


}

void appInit(void){  
    drv_CPU_init(); 
    drv_Time_init();
    hal_AT24C64_init();
    hal_MCPI2C_init();
    drv_LF_init();
}

void appConfig(void){
	hal_CPU_config();  
    hal_RTC_config();
    hal_AT24C64_config();  
    hal_MCP4018_config(); 
    hal_PcComm_config();
    hal_Led_config();
    drv_LF_config();
}

void preAppStartInit(void){
    _IsAppStarted=false;
}

void afterAppStartInit(void){
    ROM_WORKMODE=~(*(u8*)(ROM_ADDRESS_START+ROM_PAGESIZE*ROM_CONFIG_PAGE) );
    ROM_PROXYID[0]=~(*(u8*)(ROM_ADDRESS_START+ROM_PAGESIZE*ROM_CONFIG_PAGE+2) );
    ROM_PROXYID[1]=~(*(u8*)(ROM_ADDRESS_START+ROM_PAGESIZE*ROM_CONFIG_PAGE+3) );//@@1为什么放在两页

    fns_Flash_getSystemData(s_systemId,&s_systemMark,s_systemKey);

    m_antIdA=fns_Flash_getAntId(LF_ANTA);
    m_antIdB=fns_Flash_getAntId(LF_ANTB);
    m_antIdM=fns_Flash_getAntId(LF_ANTM);
    m_antIdN=fns_Flash_getAntId(LF_ANTN);

    drv_Comm_startRx(&_pcCommUart);

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

