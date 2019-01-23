#ifndef __HALAT24_001
#define __HALAT24_001

#include "halI2C.h"

#define AT24_PAGE 0xA0

#define AT24_PAGESIZE 32

void hal_AT24C64_config(void);
void hal_AT24C64_init(void);
bool hal_AT24C64_read(UInt8 Page,UInt16 uAddr,void* pData,UInt16 len);
bool hal_AT24C64_check(UInt8 Page,UInt16 uAddr,const void* pData,UInt16 nLen);
bool hal_AT24C64_write(UInt8 Page,UInt16 uAddr,const void* pData,UInt16 nLen);

#define drv_AT24C64_config hal_AT24C64_config

#endif

