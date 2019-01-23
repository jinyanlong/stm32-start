#include <includes.h>
#include "halDelay.h"
#include "halI2C.h"
#include "halAT24.h"

void hal_AT24C16_config(void){
}
///////////////////////////////////////////////////////////////////////////////
bool hal_AT24C16_read(UInt8 Page,UInt16 uAddr,void * pData,UInt16 nLen){
	UInt16 i;
	bool ret;

	hal_I2C_start();
	ret=hal_I2C_send(Page | (uAddr>>7) & 0xFE); //写地址操作
	if(!ret){
		return false;
	}
	ret=hal_I2C_send((u8)uAddr);
	if(!ret){
		return false;
	}

	hal_I2C_start();
	ret=hal_I2C_send(Page | uAddr>>7 | 0x01); //读操作
	if(!ret){
		return false;
	}
	
	for(i=0;i<nLen-1;i++){
		((UInt8*)pData)[i]=hal_I2C_receive();
		hal_I2C_ack();
	}
	((UInt8*)pData)[i]=hal_I2C_receive();
	hal_I2C_noack();
	hal_I2C_stop();
	return true;	
}

bool hal_AT24C16_check(UInt8 Page,UInt16 uAddr,void * pData,UInt16 nLen){
	UInt16 i;
	bool ret;
	hal_I2C_start();
	
	ret=hal_I2C_send(Page | (uAddr>>7) & 0xFE); //写操作
	if(!ret)
		return false;
	ret=hal_I2C_send((u8)uAddr);
	if(!ret)
		return false;

	hal_I2C_start();
	ret=hal_I2C_send(Page | 0x01); //读操作
	if(!ret)
		return false;
        
	for(i=0;i<nLen-1;i++){
		if(	((UInt8*)pData)[i]!=hal_I2C_receive() )	{
			hal_I2C_noack();
			hal_I2C_stop();
			return false;
		}
		hal_I2C_ack();
	}
	if( ((UInt8*)pData)[i]!=hal_I2C_receive() )
	{
		hal_I2C_noack();
		hal_I2C_stop();
		return false;
	}else{
		hal_I2C_noack();
		hal_I2C_stop();
		return true;
	}
}

bool hal_AT24C16_write(UInt8 Page,UInt16 uAddr,void * pData,UInt16 nLen){
	UInt16 i;
	taskDISABLE_INTERRUPTS();//操作不可被打断
	
	hal_I2C_start();
	if(!hal_I2C_send(Page | (uAddr>>7) & 0xFE)){ //写高地址
		return false;
	}
	if(!hal_I2C_send( (UInt8)uAddr )){ //写低地址
		return false;
	}

	for(i=0;i<nLen-1;i++){
		if( !hal_I2C_send(((u8*)pData)[i]) ){
			return false;
		}
	}
	if( !hal_I2C_send(((u8*)pData)[i]) ){ //最后一个字节，写完发送stop命令，然后结束
		return false;	
	}
	hal_I2C_stop();

	hal_AT24C16_wdelay();
	return true;
}
