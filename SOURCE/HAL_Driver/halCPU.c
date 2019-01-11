#include "includes.h"
#include "halCPU.h"
#include "RealTimer.h"

//ʱ�ӵ�ص�ѹ����
void hal_Battery_config(void){
#ifdef __BOARD_PIGEON_V7
	GPIO_InitTypeDef  gpio_init;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);
	gpio_init.GPIO_Speed=GPIO_Speed_50MHz;
    gpio_init.GPIO_Pin  = GPIO_Pin_5;
    gpio_init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOE, &gpio_init);
#endif
}

bool hal_Battery_isLower(void){
#ifdef __BOARD_PIGEON_V7
    //PE5      ��������     I/O�Ǹ�   ��ص�ѹ����2.6V     I/O�ǵ�    ��ص�ѹ���ͻ���ûװ���
    return GPIOE_IDR_BIT(5)?false:true;
#else
    return false;
#endif
}

void  hal_CPU_config (void){
    RCC_DeInit();
	
	//ʹ��HSE
    RCC_HSEConfig(RCC_HSE_ON);
    RCC_WaitForHSEStartUp();
	
	//AHB=SYSCLK
    RCC_HCLKConfig(RCC_SYSCLK_Div1);
	//APB2=AHB
    RCC_PCLK2Config(RCC_HCLK_Div1);
	//APB1=AHB/2
    RCC_PCLK1Config(RCC_HCLK_Div2);
	//ADCCLK=APB2/6
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	//USBCLK=PLL/1.5
	RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);

	//Flash
    FLASH_SetLatency(FLASH_Latency_2);
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

	//PLL=HSE*9
	RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
    RCC_PLLCmd(ENABLE);
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

 	//SYSCLK=PLL
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    while (RCC_GetSYSCLKSource() != 0x08);
	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

#ifdef __DFU3000
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x3000);   
#endif

    //2λ�����ȼ�,2λ�����ȼ�
    NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);
}


void hal_CPU_getId(UInt8* uid){
	UInt8 i;
	for(i=0;i<12;i++)
	{
		uid[i]=*(UInt8*)(ROM_UID_OFFSET+i);
	}
}

extern DESKEYSTRUCT desKey; //desKeyʵ�����Ǿֲ�����,������ȫ��,��Ϊ�˼���ջ�ռ�����

void drv_CPU_getDeviceId(void* deviceID){
	UInt8 uid[12];
	UInt8 mac[8];
	UInt8 key[8];
	UInt8 i;
	memcpy(key,FOFIAKEY,8);
	for(i=0;i<8;i++)
	{
		key[i]=key[i]<<1;
	}
	hal_CPU_getId(uid);
	drv_DES_createSubKey(key,&desKey);
	drv_DES_decrypt(mac,uid,&desKey);
	for(i=0;i<8;i++)
		mac[i]^=uid[4+i];
	drv_DES_decrypt(mac,uid+4,&desKey);
	memcpy(deviceID,mac,4);

}

void hal_IDOG_enable(void){
#ifdef __WATCHDOG
    if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET){
       RCC_ClearFlag();
    }
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(IWDG_Prescaler_32);
    IWDG_SetReload(349);
    IWDG_ReloadCounter();
    IWDG_Enable();
#endif
}

void hal_IDOG_flush(void){
#ifdef __WATCHDOG
    IWDG_ReloadCounter();
#endif
}

//////////////////////////////////////////////////////////////////////////////////////
//����ϵͳ��λ
static volatile UInt32 _resetTick=0;
UInt32 s_deviceID;
void drv_CPU_init(void){
    hal_IDOG_enable();
    
    drv_CPU_getDeviceId(&s_deviceID);
}

void drv_CPU_requestReset(UInt32 resetTick){
    _resetTick = drv_Time_getTick()+resetTick;
}
bool drv_CPU_checkReset(UInt32 nowTick){
    if( (_resetTick>0) && (nowTick>_resetTick)){
        return true;
    }else{
        return false;
    }
}





//////////////////////////////////////���ж�ģ��/////////////////////////////////////////////
///////////////���Թ�ʹ��Ӳ��IO���ж�,�������д������ɹ������//////////////////////////////
UInt32 s_event_flags=0; //�յ�����ָ�������Ϊ1
void isr_Event_raise(UInt32 flag){
    ATOMIC_OR(s_event_flags,flag);
}

UInt32 fns_Event_test(UInt32 flagMask){
    UInt32 raiseFlag;
    __disable_irq();
    raiseFlag=s_event_flags&flagMask;
    s_event_flags&=~raiseFlag;
    __enable_irq();
    return raiseFlag;
}




