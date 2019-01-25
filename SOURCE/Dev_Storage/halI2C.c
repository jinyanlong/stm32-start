#include <includes.h>
#include "halI2c.h"

#define def_scl_delay()  delay_us(2)
#define def_byte_delay() 

static void def_scl_1(void){//SCL-PB10|SDA-PB11
	GPIOB_ODR_BIT(10)=1;
	def_scl_delay();
}

static void def_scl_0(void){
	GPIOB_ODR_BIT(10)=0;
	def_scl_delay();
}

static void def_sda_1(void){
	GPIOB_ODR_BIT(11)=1;
}

static void def_sda_0(void){
	GPIOB_ODR_BIT(11)=0;
}

static bool def_sda_get(void){
	GPIOB_ODR_BIT(11)=1;
	if( GPIOB_IDR_BIT(11) ){
		return true;
	}else{
		return false;
	}
}
void hal_I2C_config(void){//SCL-PB10|SDA-PB11
    GPIO_InitTypeDef  gpio_init;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    //I2C-SCL(AT24)
    gpio_init.GPIO_Speed=GPIO_Speed_50MHz;
    gpio_init.GPIO_Pin  = GPIO_Pin_10;
    gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &gpio_init);

    //I2C-SDA(AT24)
    gpio_init.GPIO_Speed=GPIO_Speed_50MHz;
    gpio_init.GPIO_Pin  = GPIO_Pin_11;
    gpio_init.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOB, &gpio_init);
}
void hal_I2C_unconfig(void){
    def_sda_1();
	def_scl_1();
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
    GPIO_InitTypeDef  gpio_init;
    
	def_sda_1();
	def_scl_1();
    gpio_init.GPIO_Speed=GPIO_Speed_50MHz;
    gpio_init.GPIO_Pin  = GPIO_Pin_10|GPIO_Pin_11;//@@1
    gpio_init.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOB, &gpio_init);
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
