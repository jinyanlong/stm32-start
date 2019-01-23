#ifndef __HALAT24_001
#define __HALAT24_001

#include "halI2C.h"

#define AT24_PAGE 0xA0  

void hal_AT24C16_config(void);
bool hal_AT24C16_read(UInt8 Page,UInt16 uAddr,void * pData,UInt16 len);
bool hal_AT24C16_check(UInt8 Page,UInt16 uAddr,void * pData,UInt16 nLen);
bool hal_AT24C16_write(UInt8 Page,UInt16 uAddr,void * pData,UInt16 nLen);

#define hal_AT24C16_wdelay()  delay_us(5000)

#endif

