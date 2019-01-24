#ifndef __HALMCP4018T_
#define __HALMCP4018T_

#include "halMCPI2C.h"
void hal_MCP4018_config(void);
bool hal_MCP4018_read(UInt8 mcpNO,void * pData,UInt16 nLen);
bool hal_MCP4018_write(UInt8 mcpNO,void * pData,UInt16 nLen);

#define hal_MCP4018_wdelay()  delay_us(5000)

#endif

