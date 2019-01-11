#include <includes.h>
#include "halRTC.h"

//�����RTCû�к󱸵�Դ����,����ÿ�ζ��Ǵ�0��ʼ����
void hal_RTC_config(void){
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP,ENABLE);//�����������ʹ��RTC
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);//��Դ������ʹ��
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

    //�ȴ�HSEʹ��
    RCC_HSEConfig(RCC_HSE_ON);
    while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET);

    RCC_RTCCLKConfig(RCC_RTCCLKSource_HSE_Div128);  //�ø�Ƶ������Ϊ��Դ
	RCC->BDCR |= 0x8100;    //��������ƼĴ��� BDCR-bit8|9=11-HSE/128 @@1
	RTC_WaitForSynchro();   //���RSF�Ĵ���ͬ����־��Ҫ�����κεĶ�����֮ǰ���û��������ȴ�RSFλ��Ӳ����1
    RTC_WaitForLastTask();  //�ȴ���һ�ζ�RTC�Ĵ�����д�������
    //RTC_SetPrescaler(125-1);//��2ms��Ƶ(8M����)
    RTC_SetPrescaler(62-1);//��1ms��Ƶ(8M����),��������(=62.5),���Զ�ȡʱ��Ҫ�����������
    RTC_WaitForLastTask();  //�ȴ���һ�ζ�RTC�Ĵ�����д�������
	//RTC_SetCounter(startTime/2);
    RTC_SetCounter(startTime-startTime/125);
       
    RTC_WaitForLastTask();  //�ȴ���һ�ζ�RTC�Ĵ�����д�������
    //PWR_BackupAccessCmd(DISABLE);

}

//��������40��֮��,�����,����һ��ʱ��֮��,��Ҫ��λ
UInt32 hal_RTC_getCount(void){
    UInt32 count;
    //PWR_BackupAccessCmd(ENABLE);
    RTC_WaitForSynchro();       //���RSF�Ĵ���ͬ����־��Ҫ�����κεĶ�����֮ǰ���û��������ȴ�RSFλ��Ӳ����1
    count= RTC_GetCounter();      //��1ms��Ƶ,���غ�����
    count=count+count/124;        //�������
    //PWR_BackupAccessCmd(DISABLE);
    return count;
}

//RTCʱ�ӽ����ϵ�ʱִ��һ�γ�ʼ��,�Ժ�ֻ���Զ�����,��Զ���ᱻ�Ķ�
//void hal_RTC_setCount(UInt32 tickCount){
//    //PWR_BackupAccessCmd(ENABLE);
//    RTC_SetCounter(tickCount/2);
//    RTC_WaitForLastTask();  //�ȴ���һ�ζ�RTC�Ĵ�����д�������
//    //PWR_BackupAccessCmd(DISABLE);
//}


