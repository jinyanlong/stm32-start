
#ifndef __HAL_LF_H
#define __HAL_LF_H

void hal_LF_config(void);
void readyLfSendData(u16 data);
void drv_LF_init(void);
bool lfSendStart(uint16_t count);
#endif
