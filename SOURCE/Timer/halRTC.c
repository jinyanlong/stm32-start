#include <includes.h>
#include "halRTC.h"

//这里的RTC没有后备电源保护,所以每次都是从0开始计数
void hal_RTC_config(void){
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP,ENABLE);//备份域控制器使能RTC
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);//电源控制器使能
}

u32 hal_RTC_GetPrescaler(void){
    u32 tmp = 0x00;
    tmp = ((u32)RTC->PRLH & (u32)0x000F) << 0x10;
    tmp |= RTC->PRLL;
    return tmp;
}



void hal_RTC_init(UInt32 startTime){
    PWR_BackupAccessCmd(ENABLE);
	RTC_ClearFlag(RTC_FLAG_RSF);
    BKP_DeInit();

    //等待HSE使能
    RCC_HSEConfig(RCC_HSE_ON);
    while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET);

    RCC_RTCCLKConfig(RCC_RTCCLKSource_HSE_Div128);  //用高频晶振作为震荡源
	RCC->BDCR |= 0x8100;    //备份域控制寄存器 BDCR-bit8|9=11-HSE/128 @@1
	RTC_WaitForSynchro();   //检查RSF寄存器同步标志。要进行任何的读操作之前，用户程序必须等待RSF位被硬件置1
    RTC_WaitForLastTask();  //等待上一次对RTC寄存器的写操作完成
    //RTC_SetPrescaler(125-1);//按2ms分频(8M晶振)
    RTC_SetPrescaler(62-1);//按1ms分频(8M晶振),不能整除(=62.5),所以读取时需要进行误差修正
    RTC_WaitForLastTask();  //等待上一次对RTC寄存器的写操作完成
	//RTC_SetCounter(startTime/2);
    RTC_SetCounter(startTime-startTime/125);
       
    RTC_WaitForLastTask();  //等待上一次对RTC寄存器的写操作完成
    //PWR_BackupAccessCmd(DISABLE);

}

//连续工作40天之后,会溢出,所以一段时间之后,需要置位
UInt32 hal_RTC_getCount(void){
    UInt32 count;
    //PWR_BackupAccessCmd(ENABLE);
    RTC_WaitForSynchro();       //检查RSF寄存器同步标志。要进行任何的读操作之前，用户程序必须等待RSF位被硬件置1
    count= RTC_GetCounter();      //按1ms分频,返回毫秒数
    count=count+count/124;        //误差修正
    //PWR_BackupAccessCmd(DISABLE);
    return count;
}

//RTC时钟仅在上电时执行一次初始化,以后只会自动递增,永远不会被改动
//void hal_RTC_setCount(UInt32 tickCount){
//    //PWR_BackupAccessCmd(ENABLE);
//    RTC_SetCounter(tickCount/2);
//    RTC_WaitForLastTask();  //等待上一次对RTC寄存器的写操作完成
//    //PWR_BackupAccessCmd(DISABLE);
//}


