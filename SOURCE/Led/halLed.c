#include <includes.h>
#include "Led.h"


void hal_Led_config(void){//led1-pb9|led2-pb15  0-open
	GPIO_InitTypeDef  gpio_init;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO| RCC_APB2Periph_GPIOB,ENABLE);

	gpio_init.GPIO_Speed=GPIO_Speed_50MHz;
   
    GPIOB_ODR_BIT(LED_1_PIN)=GPIOB_ODR_BIT(LED_2_PIN)=1;
	gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio_init.GPIO_Pin  = 1<<LED_1_PIN|1<<LED_2_PIN;
	GPIO_Init(GPIOB, &gpio_init);
}

void hal_Led_set(UInt8 ledNo,UInt8 val){
     switch(ledNo){
    case LED_1:
        GPIOB_ODR_BIT(LED_1_PIN)=val;
        break;
    case LED_2:
        GPIOB_ODR_BIT(LED_2_PIN)=val;
        break;     
    }
}

void hal_Led_on(UInt8 ledNo){
    hal_Led_set(ledNo,LED_ON);
}

void hal_Led_off(UInt8 ledNo){
    hal_Led_set(ledNo,LED_OFF);
}

