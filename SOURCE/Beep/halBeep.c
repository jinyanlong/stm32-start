#include <includes.h>
#include "halBeep.h"
#define BEEP_TIMER  TIM1


/////////////////////////////////////////////////////////////////////////////
   //��pwm���ú�֮������PIN��Ч,�����������///
void hal_Beep_config(void){
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
	TIM_OCInitTypeDef TIM_OCStruct;

 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_TIM1|RCC_APB2Periph_GPIOB,ENABLE);

	TIM_TimeBaseStructInit(&TIM_TimeBaseStruct);//����Ĭ��ֵ
	TIM_TimeBaseInit(BEEP_TIMER,&TIM_TimeBaseStruct);//��ʼ��
	TIM_OCStructInit(&TIM_OCStruct);
	TIM_OCStruct.TIM_OCMode = TIM_OCMode_PWM1;//����ΪPWM1ģʽ
	TIM_OCStruct.TIM_OutputNState = TIM_OutputNState_Enable; 
	TIM_OCStruct.TIM_Pulse = 0;
	TIM_OCStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	
	TIM_OC1Init(BEEP_TIMER,&TIM_OCStruct);	//ʹ��ͨ��1

	TIM_SetCounter(BEEP_TIMER, 0);//����������
	TIM_PrescalerConfig(BEEP_TIMER, 36, TIM_PSCReloadMode_Immediate);
	TIM_Cmd(BEEP_TIMER, ENABLE);//����������
	TIM_ARRPreloadConfig(BEEP_TIMER, ENABLE); // ����ǵ�Ҫ��
	
	hal_Beep_stop();

    {
        GPIO_InitTypeDef  gpio_init;
        //beep�ܽ����������
        gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
        gpio_init.GPIO_Pin = GPIO_Pin_13;
        gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_Init(GPIOB, &gpio_init);
    }

}

void hal_Beep_stop(void){
     BEEP_TIMER->CCMR1 &= 0xFF8F;//��������Ƚ�ģʽ
     BEEP_TIMER->CCMR1 |= 0x0040;//ǿ������
     BEEP_TIMER->CR1 |=0x0080;
     BEEP_TIMER->EGR = 0x0001;
}

void hal_Beep_set(UInt16 _1Hz){  
    if(_1Hz==0){
        hal_Beep_stop();
        return;
    }
    
    BEEP_TIMER->BDTR|=TIM_BDTR_MOE;
    
    
	//PWMƵ�ʼ���:2KHZ=1MHZ/500���������ؼĴ���TIM3->ARRӦ������Ϊ500
    BEEP_TIMER->ARR = 1000000/_1Hz;
    BEEP_TIMER->CCR1 = 1000000/_1Hz/2; // ͨ��2 ����/�ȽϼĴ���,50%ռ�ձ�
    BEEP_TIMER->CNT = 0;//����������

    BEEP_TIMER->CCMR1 &= 0xFF8F;
    BEEP_TIMER->CCMR1 |=  TIM_OCMode_PWM1;//����/�Ƚ�ģʽ�Ĵ�����OC1Mλд��110(PWMģʽ1)
    BEEP_TIMER->CCMR1 |=  0x0008;//OC1PEʹ����Ӧ��Ԥװ�ؼĴ���
    BEEP_TIMER->CR1   |= 0x0080;//���ƼĴ���1��ARPEλʹ��Ԥװ�ؼĴ�����TIM4_ARR��װ�뻺����

    BEEP_TIMER->EGR = 0x0001;//��ʼ��������������һ�������¼�	

}
