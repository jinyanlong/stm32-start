#ifndef  __DS1302_H
#define  __DS1302_H


void hal_DS1302_config(void);

void hal_DS1302_write(UInt16 _cldata);
UInt8 hal_DS1302_read(UInt8 _cldata);
void hal_DS1302_lock(void);
void hal_DS1302_unlock(void);
bool hal_DS1302_getTime(DATETIME *pTm);
void hal_DS1302_setTime(DATETIME *pTm);


#endif
