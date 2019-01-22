#include "includes.h"
#include "halLED.h"

#define LEDPIN_ON  1
#define LEDPIN_OFF 0

#ifdef __LED

#define LED_PORT1_PIN  GPIOB_ODR_BIT(12)
#define LED_PORT2_PIN  GPIOB_ODR_BIT(13)

void hal_Led_config(void){//pb12-red|pb13
    GPIO_InitTypeDef  gpio_init;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);
	LED_PORT1_PIN=0;
    LED_PORT2_PIN=0;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init.GPIO_Pin =GPIO_Pin_12|GPIO_Pin_13;
    gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &gpio_init);

}

void hal_Led_on(UInt8 pinNo){
    if(pinNo==LED_1){
        LED_PORT1_PIN=LEDPIN_ON;
    }else if(pinNo==LED_2){
        LED_PORT2_PIN= LEDPIN_ON;
    }
}
void hal_Led_off(UInt8 pinNo){
    if(pinNo==LED_1){
        LED_PORT1_PIN=LEDPIN_OFF;
    }else if(pinNo==LED_2){
        LED_PORT2_PIN=LEDPIN_OFF;
    }
}
bool hal_Led_isON(UInt8 pinNo){
    if(pinNo==LED_1){
        return LED_PORT1_PIN==LEDPIN_ON; 
    }else if(pinNo==LED_2){
        return LED_PORT2_PIN==LEDPIN_ON;
    }
    return false;
}
void hal_Led_toggle(UInt8 pinNo){
    if(pinNo==LED_1){
        if(LED_PORT1_PIN==LEDPIN_OFF){
            LED_PORT1_PIN=LEDPIN_ON;
        }else{
            LED_PORT1_PIN=LEDPIN_OFF;
        }
    }else if(pinNo==LED_2){
        if(LED_PORT2_PIN==LEDPIN_OFF){
            LED_PORT2_PIN=LEDPIN_ON;
        }else{
            LED_PORT2_PIN=LEDPIN_OFF;
        }            
    }
}

#endif

