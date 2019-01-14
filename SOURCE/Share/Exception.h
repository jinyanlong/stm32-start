#ifndef __EXCEPTION_H_001
#define __EXCEPTION_H_001

//#include "MessageLoop.h"
#define ERROR_PARAM_OVER    0x0101

#define ERROR_TIMER_INIT      0x1001
#define ERROR_TIMER_SET       0x1002
#define ERROR_TIMER_GET       0x1003
#define ERROR_TIMER_FORMAT    0x1004
#define ERROR_TIMER_INVALIDATE   0x1005

#define ERROR_FLASH        0x1101

#define ERROR_TYPE_FILE         0x1200
#define ERROR_FILE_ROVERSIZE    0x1201 //¶ÁÒç³ö
#define ERROR_FILE_FORMAT       0x1202
#define ERROR_FILE_SYSID        0x1203
#define ERROR_FILE_DATA         0x1204
#define ERROR_FILE_WOVERSIZE    0x1205  //Ð´Òç³ö

#define ERROR_HTTP_MATCH    0x1301
#define ERROR_SMD_ERROR    0x1302


#define ERROR_BUG_RACESTATE   0x2001
#define ERROR_BUG_OS          0x2003
#define ERROR_BUG_TAGNO       0x2004

#define EXCEPTION_DESC_SIZE   62

typedef struct _ExceptionStruct{
    UInt16 errType;
    UInt16 errCode;    
    char desc[EXCEPTION_DESC_SIZE];
}Exception;


void drv_Exception_init(void);

void fns_ExceptionForm_create(void);

Exception* drv_Exception_get(void);
void drv_Exception_throw(UInt16 errType);
void drv_Exception_throwEx(UInt16 errType,UInt16 errCode, const char* desc);

#endif
