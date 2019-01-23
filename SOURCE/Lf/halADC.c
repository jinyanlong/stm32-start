#include <includes.h>
#include "halADC.h"
#define ADC_PORT ADC1

//va-pc0-ch10(���ߵ�ѹ)|vb-pc1-ch11|vm-pa6-ch6|vn-pc5-ch15|vcc1-pc2-ch12(��Դ��ѹ)|vcc2-pb0-ch8|
void hal_ADC_config(void){
    GPIO_InitTypeDef  gpio_init;
    ADC_InitTypeDef ADC_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_ADC1|RCC_APB2Periph_AFIO, ENABLE);

    gpio_init.GPIO_Speed=GPIO_Speed_50MHz;
    gpio_init.GPIO_Pin = GPIO_Pin_6;
    gpio_init.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &gpio_init);//ADC1-CH6-VM

    gpio_init.GPIO_Pin = GPIO_Pin_0;
    gpio_init.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOB, &gpio_init);//CH8

    gpio_init.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_5; //CH10|CH11|CH12|CH15
    gpio_init.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOC, &gpio_init);
    
    //���ò���ͨ��
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                  //��������ģʽ
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;                        //ɨ�跽ʽ
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;                  //����ת��
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //�ⲿ������ֹ
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;              //�����Ҷ���
    ADC_InitStructure.ADC_NbrOfChannel = 1;                             //����ת����ͨ����:1
    ADC_Init(ADC_PORT, &ADC_InitStructure);

    ADC_PORT->CR2 |= 0x07<<17;//ѡ���ⲿ������ʽΪ�������
    ADC_Cmd(ADC_PORT, ENABLE);

    ADC_ResetCalibration(ADC_PORT);
    while(ADC_GetResetCalibrationStatus(ADC_PORT));

    ADC_StartCalibration(ADC_PORT);
    while(ADC_GetCalibrationStatus(ADC_PORT));
}

void hal_ADC_stop(void){
    ADC_Cmd(ADC_PORT, DISABLE);
}


uint32_t hal_ADC_getValue(uint8_t ch){
    uint32_t battVolt;

    ADC_RegularChannelConfig(ADC_PORT, ch, 1, ADC_SampleTime_55Cycles5);

	ADC_SoftwareStartConvCmd(ADC_PORT, ENABLE);
	while(!(ADC_PORT->SR & ADC_FLAG_EOC)) __nop();//�ȴ�ת�����
    battVolt=ADC_PORT->DR;

    return battVolt;
}

uint32_t hal_ADC_getAverageVal(uint8_t ch,uint8_t times){
	uint32_t temp_val=0;
	uint8_t t;
	for(t=0;t<times;t++)
	{
		temp_val+=hal_ADC_getValue(ch);
		delay_ms(5);
	}
	return temp_val/times;
} 	



