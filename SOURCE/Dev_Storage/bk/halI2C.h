#ifndef __HALI2C_001
#define __HALI2C_001

#include "haldelay.h"


__INLINE void hal_I2C_config(void){
    GPIO_InitTypeDef  gpio_init;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	//I2C-SCL( ±÷”°¢AT24)
    GPIOA_ODR_BIT(1)=1;
    gpio_init.GPIO_Speed=GPIO_Speed_50MHz;
	gpio_init.GPIO_Pin  = GPIO_Pin_1;
    gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &gpio_init);


	//I2C-SDA( ±÷”°¢AT24)
    GPIOA_ODR_BIT(2)=1;
    gpio_init.GPIO_Speed=GPIO_Speed_50MHz;
	gpio_init.GPIO_Pin  = GPIO_Pin_2;
    gpio_init.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOA, &gpio_init);
}

#define def_scl_delay()  delay_us(2)
#define def_byte_delay() 

__INLINE void def_scl_1(void){
    GPIOA_ODR_BIT(1)=1;
	def_scl_delay();
}

__INLINE void def_scl_0(void){
    GPIOA_ODR_BIT(1)=0;
	def_scl_delay();
}

__INLINE void def_sda_1(void){
    GPIOA_ODR_BIT(2)=1;
}

__INLINE void def_sda_0(void){
    GPIOA_ODR_BIT(2)=0;
}

__INLINE bool def_sda_get(void){
	GPIOA_ODR_BIT(2)=1;
	if( GPIOA_IDR_BIT(2) ){
		return true;
	}else{
		return false;
	}
}

void hal_I2C_reset(void);
void hal_I2C_start(void);
void hal_I2C_stop(void);
void hal_I2C_ack(void);
void hal_I2C_noack(void);
bool hal_I2C_send(unsigned char bytData);
unsigned char hal_I2C_receive(void);

#endif



