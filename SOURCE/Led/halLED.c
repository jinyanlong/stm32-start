#include "includes.h"
#include "halLED.h"

#define LEDPIN_ON  0
#define LEDPIN_OFF 1

#ifdef __LED

#define LED_PORT1_PIN  GPIOB_ODR_BIT(0)
#define LED_PORT2_PIN  GPIOB_ODR_BIT(1)

void hal_Led_config(void){
    GPIO_InitTypeDef  gpio_init;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	LED_PORT1_PIN=1;
	LED_PORT2_PIN=1;        
    gpio_init.GPIO_Pin =GPIO_Pin_0|GPIO_Pin_1;
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

