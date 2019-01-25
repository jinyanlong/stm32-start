
#ifndef __LF_H
#define __LF_H
#include "halLf.h"
#include "halMCPI2C.h"
#include "halMCP4018T.h"

void drv_LF_config(void);
UInt8 drv_adjust_LFCap(UInt8 ant);
void drv_open_LFQVAL(UInt8 ant);
void drv_close_LFQVAL(UInt8 ant);
uint32_t drv_GET_LFPOWVCC(UInt8 pNO);
void drv_SET_LFPOWVAL(UInt8 pNO,UInt8 data);
UInt8 drv_GET_LFPOWVAL(UInt8 pNO);
void drv_LFset_LFQVAL(UInt8 ant,UInt8 val);
#endif
