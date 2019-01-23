#include <includes.h>
#include "halI2c.h"

#define def_scl_delay()  delay_us(2)
#define def_byte_delay() 

__inline void def_scl_1(void){
    GPIOA_ODR_BIT(1)=1;
	def_scl_delay();
}

__inline void def_scl_0(void){
    GPIOA_ODR_BIT(1)=0;
	def_scl_delay();
}

__inline void def_sda_1(void){
    GPIOA_ODR_BIT(2)=1;
}

__inline void def_sda_0(void){
    GPIOA_ODR_BIT(2)=0;
}

__inline bool def_sda_get(void){
    GPIOA_ODR_BIT(2)=1; 
	if(GPIOA->IDR & GPIO_Pin_2){
		return true;
	}else{
		return false;
	}
}

void hal_I2C_config(void){
    GPIO_InitTypeDef  gpio_init;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	//I2C-SCL(ʱ�ӡ�AT24)
    GPIOA_ODR_BIT(1)=1;
    gpio_init.GPIO_Speed=GPIO_Speed_50MHz;
	gpio_init.GPIO_Pin  = GPIO_Pin_1;
    gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &gpio_init);

	//I2C-SDA(ʱ�ӡ�AT24)
    GPIOA_ODR_BIT(2)=1;
    gpio_init.GPIO_Speed=GPIO_Speed_50MHz;
	gpio_init.GPIO_Pin  = GPIO_Pin_2;
    gpio_init.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOA, &gpio_init);
    
}
//SCLΪ��ʱ������SDA��ʾ����
void hal_I2C_start(void){
	def_sda_1();
	def_scl_1();
	
	def_sda_get();
	
	def_sda_0();
	def_scl_0();
}

//SCLΪ��ʱ������SDA��ʾֹͣ
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

void hal_I2C_init(void){ // ��ʼ��
	def_sda_1();
	def_scl_1();
}

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

	//�鿴ACKӦ��
	def_sda_1();
	def_scl_1();
	if(def_sda_get()){
		ret= false;		//����Ϊ�͵�ƽ��ʾΪACKӦ��
	}else{
		ret= true;
	}
	def_scl_0();
        
	return ret;
}

unsigned char hal_I2C_receive(void){
	unsigned char i,bytData;

	bytData=0;
	def_sda_1();
	def_scl_1();
	def_byte_delay();	
	for(i=0;i<8;i++){
		def_scl_1();
		if(def_sda_get()){
	        bytData|=(0x80>>i);
		}
		def_scl_0();
	}
	return bytData;
}
