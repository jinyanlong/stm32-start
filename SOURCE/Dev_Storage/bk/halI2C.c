#include <includes.h>
#include "halI2c.h"



//SCL为高时，拉低SDA表示启动
void hal_I2C_start(void){
	def_sda_1();
	def_scl_1();
	
	def_sda_get();
	
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

void hal_I2C_reset(void){ // 初始化
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

	//查看ACK应答
	def_sda_1();
	def_scl_1();
	if(def_sda_get()){
		ret= false;		//读到为低电平表示为ACK应答
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
