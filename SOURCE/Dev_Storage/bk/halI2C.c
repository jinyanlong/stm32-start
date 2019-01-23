#include <includes.h>
#include "halI2c.h"



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

void hal_I2C_reset(void){ // ��ʼ��
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
