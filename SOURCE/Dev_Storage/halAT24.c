#include <includes.h>
#include "halAT24.h"

///////////////////////////////////////////////////////////////////////////////
void hal_AT24C64_config(void){
    hal_I2C_config();
}
void hal_AT24C64_init(void){
    hal_I2C_init();
}
bool hal_AT24C64_read(UInt8 Page,UInt16 uAddr,void * pData,UInt16 nLen){
	UInt16 i;
	bool ret;

	hal_I2C_start();
	
	ret=hal_I2C_send(Page & 0xFE); //Ğ´²Ù×÷
	if(!ret)
		return false;

    ret=hal_I2C_send((UInt8)(uAddr>>8));
	if(!ret)
		return false;    
	ret=hal_I2C_send((UInt8)uAddr);
	if(!ret)
		return false;


	hal_I2C_start();
	ret=hal_I2C_send(Page | 0x01); //¶Á²Ù×÷
	if(!ret)
		return false;
	
	for(i=0;i<nLen-1;i++){
		((UInt8*)pData)[i]=hal_I2C_receive();
		hal_I2C_ack();
	}
	((UInt8*)pData)[i]=hal_I2C_receive();
	hal_I2C_noack();
	hal_I2C_stop();
	return true;	
}

bool hal_AT24C64_check(UInt8 Page,UInt16 uAddr,const void * pData,UInt16 nLen){
	UInt16 i;
	bool ret;
	hal_I2C_start();
	
	ret=hal_I2C_send(Page & 0xFE); //Ğ´²Ù×÷
	if(!ret)
		return false;

    ret=hal_I2C_send(uAddr>>8);
	if(!ret)
		return false;
	ret=hal_I2C_send((UInt8)uAddr );
	if(!ret)
		return false;


	hal_I2C_start();
	ret=hal_I2C_send(Page | 0x01); //¶Á²Ù×÷
	if(!ret)
		return false;
	for(i=0;i<nLen-1;i++){
		if(	((UInt8*)pData)[i]!=hal_I2C_receive() ){
			hal_I2C_noack();
			hal_I2C_stop();
			return false;
		}
		hal_I2C_ack();
	}
	if( ((UInt8*)pData)[i]!=hal_I2C_receive() ){
		hal_I2C_noack();
		hal_I2C_stop();
		return false;
	}else{
		hal_I2C_noack();
		hal_I2C_stop();
		return true;
	}
}

bool hal_AT24C64_write(UInt8 Page,UInt16 uAddr,const void * pData,UInt16 nLen){
	UInt16 i;
	bool ret;


	hal_I2C_start();
	ret=hal_I2C_send(Page & 0xFE); //Ğ´²Ù×÷
	if(!ret)
		return false;
    
	ret=hal_I2C_send( (UInt8)(uAddr>>8));
	if(!ret)
		return false;
	ret=hal_I2C_send((UInt8)uAddr);
	if(!ret)
		return false;

	for(i=0;i<nLen;i++){
		if( !hal_I2C_send( ((UInt8*)pData)[i]) )
			return false;
	}
	hal_I2C_stop();
    delay_us(5000);
	return true;
}
