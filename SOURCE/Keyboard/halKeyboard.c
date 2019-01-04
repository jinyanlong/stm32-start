#include  <includes.h>
#include  "halKeyBoard.h"

#define KEY_RAWVALUE()       (((GPIOB->IDR)>>7) & 0x0F)


void hal_Keyboard_config(void){//pb5-key1|pb14-key2   press-0
    GPIO_InitTypeDef  gpio_init;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
    
    gpio_init.GPIO_Speed=GPIO_Speed_50MHz;
    gpio_init.GPIO_Pin  = KEY1_PIN|KEY2_PIN;
    gpio_init.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
    GPIO_Init(GPIOB, &gpio_init);
							
}

UInt16 hal_Keyboard_get(void){
    if(GPIO_ReadInputDataBit(GPIOB, KEY1_PIN)==Bit_RESET){
        return KEY1_VALUE;
    }else if(GPIO_ReadInputDataBit(GPIOB, KEY2_PIN)==Bit_RESET){
        return KEY2_VALUE;
    }
    return KEYVALUE_NULL;
}
