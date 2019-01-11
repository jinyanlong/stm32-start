#include <includes.h>
#include "halI2C.h"

#define def_scl_delay()  delay_us(2)
#define def_sda_delay()  delay_us(1)

__inline void def_scl_1(void){
    GPIOC_ODR_BIT(11)=1;
	def_scl_delay();
}

__inline void def_scl_0(void){
    GPIOC_ODR_BIT(11)=0;
	def_scl_delay();
}

__inline void def_sda_1(void){
    GPIOC_ODR_BIT(12)=1;
    def_sda_delay();
}

__inline void def_sda_0(void){
    GPIOC_ODR_BIT(12)=0;
    def_sda_delay();
}

__inline bool def_sda_get(void){
	if(GPIOC->IDR & GPIO_Pin_12){
		return true;
	}else{
		return false;
	}
}

void hal_I2C_config(void){
    GPIO_InitTypeDef  gpio_init;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    //I2C-SCL
    gpio_init.GPIO_Speed=GPIO_Speed_2MHz;
    gpio_init.GPIO_Pin  = GPIO_Pin_11;
    gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &gpio_init);

    //I2C-SDA
    gpio_init.GPIO_Speed=GPIO_Speed_2MHz;
    gpio_init.GPIO_Pin  = GPIO_Pin_12;
    gpio_init.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOC, &gpio_init);
}

//SCL为高时，拉低SDA表示启动
void hal_I2C_start(void){
	def_sda_1();
	def_scl_1();
	
	def_sda_0();
	def_scl_0();
}

//SCL为高时，拉高SDA表示停止
void hal_I2C_stop(void){
	def_sda_0();
	def_scl_1();
	def_sda_1();
}

void hal_I2C_ack(void){
	def_sda_0();
	def_scl_1();
	def_scl_0();
	def_sda_1();
}	
void hal_I2C_noack(void){
	def_sda_1();
	def_scl_1();
	def_scl_0();
}

void hal_I2C_init(void){ // 初始化
	def_sda_1();
	def_scl_1();

}


//void hal_I2C_datInput(void){
//    GPIO_InitTypeDef  gpio_init;

//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

//    //I2C-SDA
//    gpio_init.GPIO_Speed=GPIO_Speed_50MHz;
//    gpio_init.GPIO_Pin  = GPIO_Pin_12;
//    gpio_init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//    GPIO_Init(GPIOC, &gpio_init);
//}

//void hal_I2C_datOutput(void){
//    GPIO_InitTypeDef  gpio_init;

//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

//    //I2C-SDA
//    gpio_init.GPIO_Speed=GPIO_Speed_50MHz;
//    gpio_init.GPIO_Pin  = GPIO_Pin_12;
//    gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
//    GPIO_Init(GPIOC, &gpio_init);
//}

bool hal_I2C_send(unsigned char bytData){
	bool ret;
	unsigned char i;
	for(i=0;i<8;i++){
	    if( bytData & (0x80>>i) ){
			def_sda_1();
		}else{
			def_sda_0();
		}
		def_scl_1();
		def_scl_0();
	}

    //hal_I2C_datInput();
    
	//查看ACK应答
	def_sda_1();
	def_scl_1();
	if(def_sda_get()){
		ret= false;		//读到为低电平表示为ACK应答
	}else{
		ret= true;
	}
	def_scl_0();
    //delay_us(2);
    //hal_I2C_datOutput();
        
	return ret;
}

//unsigned char hal_I2C_receive(void){
//	unsigned char i,bytData;

//	bytData=0;
//	def_sda_1();
//	def_scl_1();
//	for(i=0;i<8;i++){
//		def_scl_1();
//		if(def_sda_get()){
//	        bytData|=(0x80>>i);
//		}
//		def_scl_0();
//	}
//	return bytData;
//}
