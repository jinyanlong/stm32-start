#ifndef __HAL_LF_H
#define __HAL_LF_H

#define LF_ANTA  0x01
#define LF_ANTB  0x02         
#define LF_ANTM  0x03 
#define LF_ANTN  0x04 
#define LF_ANTA_DOUTPIN           14
#define LF_ANTB_DOUTPIN           12
#define LF_ANTM_DOUTPIN           10
#define LF_ANTN_DOUTPIN           7

void hal_LF_config(void);
void drv_LF_init(void);
void drv_LF_send(uint16_t coinId);
void hal_OpenLF(uint8_t lfNo);
void hal_CloseLF(uint8_t lfNo);
void hal_LF_SetCapPin(UInt8 ant,uint8_t val);
void hal_open_LFQVAL(UInt8 ant);
void hal_close_LFQVAL(UInt8 ant);

#endif
