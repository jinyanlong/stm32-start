#include <includes.h>
#include "halBeep.h"
#define BEEP_TIMER  TIM1


/////////////////////////////////////////////////////////////////////////////
   //在pwm配置好之后再让PIN生效,否则会有杂音///
void hal_Beep_config(void){
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
	TIM_OCInitTypeDef TIM_OCStruct;

 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_TIM1|RCC_APB2Periph_GPIOB,ENABLE);

	TIM_TimeBaseStructInit(&TIM_TimeBaseStruct);//设置默认值
	TIM_TimeBaseInit(BEEP_TIMER,&TIM_TimeBaseStruct);//初始化
	TIM_OCStructInit(&TIM_OCStruct);
	TIM_OCStruct.TIM_OCMode = TIM_OCMode_PWM1;//配置为PWM1模式
	TIM_OCStruct.TIM_OutputNState = TIM_OutputNState_Enable; 
	TIM_OCStruct.TIM_Pulse = 0;
	TIM_OCStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	
	TIM_OC1Init(BEEP_TIMER,&TIM_OCStruct);	//使用通道1

	TIM_SetCounter(BEEP_TIMER, 0);//计数器清零
	TIM_PrescalerConfig(BEEP_TIMER, 36, TIM_PSCReloadMode_Immediate);
	TIM_Cmd(BEEP_TIMER, ENABLE);//开启计数器
	TIM_ARRPreloadConfig(BEEP_TIMER, ENABLE); // 这个记得要开
	
	hal_Beep_stop();

    {
        GPIO_InitTypeDef  gpio_init;
        //beep管脚在最后配置
        gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
        gpio_init.GPIO_Pin = GPIO_Pin_13;
        gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_Init(GPIOB, &gpio_init);
    }

}

void hal_Beep_stop(void){
     BEEP_TIMER->CCMR1 &= 0xFF8F;//冻结输出比较模式
     BEEP_TIMER->CCMR1 |= 0x0040;//强制拉低
     BEEP_TIMER->CR1 |=0x0080;
     BEEP_TIMER->EGR = 0x0001;
}

void hal_Beep_set(UInt16 _1Hz){  
    if(_1Hz==0){
        hal_Beep_stop();
        return;
    }
    
    BEEP_TIMER->BDTR|=TIM_BDTR_MOE;
    
    
	//PWM频率计算:2KHZ=1MHZ/500，所以重载寄存器TIM3->ARR应该设置为500
    BEEP_TIMER->ARR = 1000000/_1Hz;
    BEEP_TIMER->CCR1 = 1000000/_1Hz/2; // 通道2 捕获/比较寄存器,50%占空比
    BEEP_TIMER->CNT = 0;//计数器清零

    BEEP_TIMER->CCMR1 &= 0xFF8F;
    BEEP_TIMER->CCMR1 |=  TIM_OCMode_PWM1;//捕获/比较模式寄存器的OC1M位写入110(PWM模式1)
    BEEP_TIMER->CCMR1 |=  0x0008;//OC1PE使能相应的预装载寄存器
    BEEP_TIMER->CR1   |= 0x0080;//控制寄存器1中ARPE位使能预装载寄存器，TIM4_ARR被装入缓冲区

    BEEP_TIMER->EGR = 0x0001;//初始化计数器，产生一个更新事件	

}
