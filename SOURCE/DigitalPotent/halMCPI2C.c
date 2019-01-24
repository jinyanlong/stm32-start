#include <includes.h>
#include "halMCPI2c.h"

//SCL1-PA0|SDA1-PA1|SCL2-PB14|SDA2-PB15
#define MCP1_SCL_PIN 0
#define MCP1_SDA_PIN 1
#define MCP2_SCL_PIN 14
#define MCP2_SDA_PIN 15

#define mcp_scl_delay()  delay_us(10)
#define mcp_byte_delay() 

__inline void mcp1_scl_1(void){
    GPIOA_ODR_BIT(MCP1_SCL_PIN)=1;
	mcp_scl_delay();
}
__inline void mcp1_scl_0(void){
    GPIOA_ODR_BIT(MCP1_SCL_PIN)=0;
	mcp_scl_delay();
}
__inline void mcp1_sda_1(void){
    GPIOA_ODR_BIT(MCP1_SDA_PIN)=1;
}
__inline void mcp1_sda_0(void){
    GPIOA_ODR_BIT(MCP1_SDA_PIN)=0;
}
__inline bool mcp1_sda_get(void){
    GPIOA_ODR_BIT(MCP1_SDA_PIN)=1; 
	if(GPIOA->IDR & (0x0001<<MCP1_SDA_PIN)){
		return true;
	}else{
		return false;
	}
}

__inline void mcp2_scl_1(void){
    GPIOA_ODR_BIT(MCP2_SCL_PIN)=1;
	mcp_scl_delay();
}
__inline void mcp2_scl_0(void){
    GPIOA_ODR_BIT(MCP2_SCL_PIN)=0;
	mcp_scl_delay();
}
__inline void mcp2_sda_1(void){
    GPIOA_ODR_BIT(MCP2_SDA_PIN)=1;
}
__inline void mcp2_sda_0(void){
    GPIOA_ODR_BIT(MCP2_SDA_PIN)=0;
}
__inline bool mcp2_sda_get(void){
    GPIOA_ODR_BIT(MCP2_SDA_PIN)=1; 
	if(GPIOA->IDR & (0x0001<<MCP1_SDA_PIN)){
		return true;
	}else{
		return false;
	}
}

void hal_MCPI2C_config(void){//SCL1-PA0|SDA1-PA1|SCL2-PB14|SDA2-PB15
    GPIO_InitTypeDef  gpio_init;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);

	//I2C-SCL(时钟)
    GPIOA_ODR_BIT(MCP1_SCL_PIN)=1;
    gpio_init.GPIO_Speed=GPIO_Speed_50MHz;
	gpio_init.GPIO_Pin  = (0x0001<<MCP1_SCL_PIN);
    gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &gpio_init);
	//I2C-SDA(时钟)
    GPIOA_ODR_BIT(MCP1_SDA_PIN)=1;
    gpio_init.GPIO_Speed=GPIO_Speed_50MHz;
	gpio_init.GPIO_Pin  = (0x0001<<MCP1_SDA_PIN);
    gpio_init.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOA, &gpio_init);

	//I2C-SCL(时钟)
    GPIOA_ODR_BIT(MCP2_SCL_PIN)=1;
    gpio_init.GPIO_Speed=GPIO_Speed_50MHz;
	gpio_init.GPIO_Pin  = (0x0001<<MCP2_SCL_PIN);
    gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &gpio_init);
	//I2C-SDA(时钟)
    GPIOA_ODR_BIT(MCP2_SDA_PIN)=1;
    gpio_init.GPIO_Speed=GPIO_Speed_50MHz;
	gpio_init.GPIO_Pin  = (0x0001<<MCP2_SDA_PIN);
    gpio_init.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOB, &gpio_init);   
}
//SCL为高时，拉低SDA表示启动
void hal_MCPI2C_start(uint8_t val){
	if(val==MCP4018T_1){
		mcp1_sda_1();
		mcp1_scl_1();
		mcp1_sda_get();
		mcp1_sda_0();
		mcp1_scl_0();
	}else if(val==MCP4018T_2){
		mcp2_sda_1();
		mcp2_scl_1();
		mcp2_sda_get();
		mcp2_sda_0();
		mcp2_scl_0();
	}

}

//SCL为高时，拉高SDA表示停止
void hal_MCPI2C_stop(uint8_t val){
	if(val==MCP4018T_1){
		mcp1_sda_0();
		mcp1_scl_1();
		mcp1_sda_1();
	}else if(val==MCP4018T_2){
		mcp2_sda_0();
		mcp2_scl_1();
		mcp2_sda_1();
	}
}

void hal_MCPI2C_ack(uint8_t val){
	if(val==MCP4018T_1){
		mcp1_sda_0();
		mcp1_scl_1();
		mcp1_scl_0();
		mcp1_sda_1();
	}else if(val==MCP4018T_2){
		mcp2_sda_0();
		mcp2_scl_1();
		mcp2_scl_0();
		mcp2_sda_1();
	}
}	
void hal_MCPI2C_noack(uint8_t val){
	if(val==MCP4018T_1){
		mcp1_sda_1();
		mcp1_scl_1();
		mcp1_scl_0();
	}else if(val==MCP4018T_2){
		mcp2_sda_1();
		mcp2_scl_1();
		mcp2_scl_0();
	}
}

void hal_MCPI2C_init(uint8_t val){ // 初始化
	if(val==MCP4018T_1){
		mcp1_sda_1();
		mcp1_scl_1();
	}else if(val==MCP4018T_2){
		mcp2_sda_1();
		mcp2_scl_1();	
	}

}

bool hal_MCPI2C_send(uint8_t val, unsigned char bytData){
	bool ret=false;
	unsigned char i;
	if(val==MCP4018T_1){
		for(i=0;i<8;i++){
			if( bytData & (0x80>>i) ){
				mcp1_sda_1();
			}else{
				mcp1_sda_0();
			}
			mcp1_scl_1();
			mcp1_scl_0();
		}
		//查看ACK应答
		mcp1_sda_1();
		mcp1_scl_1();
		if(mcp1_sda_get()){
			ret= false;		//读到为低电平表示为ACK应答
		}else{
			ret= true;
		}
		mcp1_scl_0();
	}else if(val==MCP4018T_2){
		for(i=0;i<8;i++){
			if( bytData & (0x80>>i) ){
				mcp2_sda_1();
			}else{
				mcp2_sda_0();
			}
			mcp2_scl_1();
			mcp2_scl_0();
		}
		//查看ACK应答
		mcp2_sda_1();
		mcp2_scl_1();
		if(mcp2_sda_get()){
			ret= false;		//读到为低电平表示为ACK应答
		}else{
			ret= true;
		}
		mcp2_scl_0();
	}
        
	return ret;
}

unsigned char hal_MCPI2C_receive(uint8_t val){//跟at24不一样，需要等回复
	unsigned char i,bytData;
	if(val==MCP4018T_1){
		bytData=0;
		mcp1_sda_1();
		mcp1_scl_1();
		mcp_byte_delay();	
		for(i=0;i<8;i++){
			mcp1_scl_1();
			if(mcp1_sda_get()){
				bytData|=(0x80>>i);
			}
			mcp1_scl_0();
		}
	}else if(val==MCP4018T_2){
		bytData=0;
		mcp2_sda_1();
		mcp2_scl_1();
		mcp_byte_delay();	
		for(i=0;i<8;i++){
			mcp2_scl_1();
			if(mcp2_sda_get()){
				bytData|=(0x80>>i);
			}
			mcp2_scl_0();
		}
	}
	return bytData;
}
