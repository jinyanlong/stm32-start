#include <includes.h>
#include "halMCP4018T.h"

#define MCP4018_ADDR_WRITE  0x5E
#define MCP4018_ADDR_READ   0x5F

void hal_MCP4018_config(void){
	hal_MCPI2C_config();
}

//I2C-ADRESS|MCP4018-0101111
//DATA- S|A6|A5|A4|A3|A2|A1|A0|R(1)/W(0)|ACK(0)/NA(1)
bool hal_MCP4018_read(UInt8 mcpNO,void * pData,UInt16 nLen){
	UInt16 i;
	bool ret;

	hal_MCPI2C_start(mcpNO);
	ret=hal_MCPI2C_send(mcpNO,MCP4018_ADDR_READ); //写地址操作
	if(!ret){
		return false;
	}
	
	for(i=0;i<nLen-1;i++){
		((UInt8*)pData)[i]=hal_MCPI2C_receive(mcpNO);
		hal_MCPI2C_ack(mcpNO);
	}
	((UInt8*)pData)[i]=hal_MCPI2C_receive(mcpNO);
	hal_MCPI2C_noack(mcpNO);
	hal_MCPI2C_stop(mcpNO);
	return true;
}

//I2C-ADRESS|MCP4018-0101111
//DATA- S|A6|A5|A4|A3|A2|A1|A0|R(1)/W(0)|ACK(0)/NA(1)
bool hal_MCP4018_write(UInt8 mcpNO,void * pData,UInt16 nLen){
	UInt16 i;
	taskDISABLE_INTERRUPTS();//操作不可被打断
	
	hal_MCPI2C_start(mcpNO);
	if(!hal_MCPI2C_send(mcpNO,MCP4018_ADDR_WRITE)){ 
		return false;
	}

	for(i=0;i<nLen;i++){
		if( !hal_MCPI2C_send(mcpNO,((u8*)pData)[i]) ){
			return false;
		}
	}
	hal_MCPI2C_stop(mcpNO);
	hal_MCP4018_wdelay();
	return true;
}
