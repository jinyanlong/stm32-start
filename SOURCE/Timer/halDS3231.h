#ifndef  __DS3231_H
#define  __DS3231_H

#ifdef PIGEON_V7
void hal_DS3231_config(void);
void hal_DS3231_init(void);

void hal_DS3231_write(UInt16 _cldata);
UInt8 hal_DS3231_read(UInt8 _cldata);
void hal_DS3231_lock(void);
void hal_DS3231_unlock(void);
bool hal_DS3231_getTime(DATETIME *pTm);
bool hal_DS3231_setTime(DATETIME *pTm);
#endif

#endif
