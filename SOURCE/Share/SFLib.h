#ifndef __SFLIB_H
#define __SFLIB_H

#define CRC8_PRESET  0xFF
#define CRC8_POLYNOM 0x1D

#define CRC16_CCITT_POLYNOM_H 0x1021
#define CRC16_CCITT_POLYNOM_L 0x8408

void crc8_append(UInt8 preset,const void* pDataBuff,UInt32 nLength);
UInt16 crc16_ccitt_h(void *ptr, UInt32 nLen);
UInt16 crc16_ccitt_l(UInt16 preset,const void *pBuff, UInt32 nLen);
UInt8 crc8_byte_calc(UInt8 preset,const void* pDataBuf,UInt32 Length);
UInt8 crc8_bit_calc(UInt8 preset,UInt8 val,UInt32 Bitcount);
UInt8 BitReverse(UInt8 bytVal);
void BitReverseEx(UInt8* pSrc,UInt8* pTrg,UInt32 nCount);
BYTE get_bcc(void * pBuf, UInt32 num);

bool MCBit_Parse(UInt32* pSampleBits,UInt32* pDataBits, UInt32 nCount);
bool ACBit_Parse(UInt32* pSampleBits,UInt32* pDataBits, UInt32 nCount);
bool BCBit_Parse(UInt32* pSampleBits,UInt32* pDataBits, UInt32 nCount);


UInt16 ToUInt16(UInt8 bh, UInt8 bl);
UInt16 ByteSwap(UInt16 wSwW);

#define UTC_S_2000  946684800
#define UTC_MS_2000 946684800000ull

void SecondsToTime(UInt32 seconds,DATETIME* pTm);
UInt32 TimeToSeconds(DATETIME* pTm);

void printTime(Int64 utc,short zone,DATETIME* dt);
void printTimeEx(Int64 time,short zone,DATETIME_EX* dt);
Int64 parseTime(DATETIME* pTm,short zone);
Int64 parseTimeEx(DATETIME_EX* pTm,short zone);
UInt32 utcToSeconds(Int64 utc,UInt32 base);
Int64 secondsToUTC(UInt32 seconds,UInt32 base);
bool checkTime(DATETIME *pTm);

    
UInt32 HexToBin(const void* pHex,void* pBin,UInt32 nCount);
void BinToHex(const void* pBin,void* pHex,UInt32 nCount);
UInt32  parseUInt32(String pdata,UInt8 nSize);
int parseInt(String pdata,UInt8 nMaxSize);
    
#define STRING_EQUAL(str1,str2) (strcmp(str1,str2)==0)
#define STRING_MATCH(strline,strsub) (memcmp(strline,strsub,strlen(strsub))==0)

__INLINE bool STRING_IMATCH(String strLine,String strMatch){
    int count,i;
    count=strlen(strMatch);
    for(i=0;i<count;i++){
        if(toupper(strLine[i])!=toupper(strMatch[i]) )
            return false;
    }
    return true;
}

//__INLINE void delay_ms(UInt32 _1ms){
//    vTaskDelay((portTickType)_1ms);
//}

void delay(UInt32 tick);
void delay_us(UInt32 us);
void delay_ms(UInt32 ms);

void memnor(void* pVoid,UInt32 nSize);
int findByte(UInt8* buff,int count,UInt8 ch);
int findChar(String str,char ch);

float computeDistance(int GpsLo1,int GpsLa1,int GpsLo2,int GpsLa2);
static __INLINE bool memEquals(UInt8* buffer,UInt8 val, size_t count ){
    size_t i;
    for(i=0;i<count;i++){
        if(buffer[i]!=val) return false;
    }
    return true;
}

#ifdef __TRACE
    #define trace(fmt,args...)   do{  \
                                     extern char _trace_buffer[256];          \
                                     DATETIME_EX tm;                        \
                                     printTimeEx(fns_Time_get(),0,&tm);                    \
                                     sprintf(_trace_buffer,"(%02d-%02d-%02dT%02d:%02d:%02d.%03dZ)",tm.Year,tm.Month,tm.Day,tm.Hour,tm.Minute,tm.Second,tm.MilliSecond); \
                                     hal_Uart_sendString(USART1,_trace_buffer);    \
                                     sprintf(_trace_buffer,fmt,args);    \
                                     hal_Uart_sendString(USART1,_trace_buffer);    \
                                 }while(0)
    
    #define traceTime(t)        do{  \
                                     extern char _trace_buffer[256];       \
                                     DATETIME_EX tm;                        \
                                     printTimeEx(t,0,&tm);                    \
                                     sprintf(_trace_buffer,"%02d-%02d-%02dT%02d:%02d:%02d.%03dZ",tm.Year,tm.Month,tm.Day,tm.Hour,tm.Minute,tm.Second,tm.MilliSecond); \
                                     hal_Uart_sendString(USART1,_trace_buffer);    \
                                }while(0)
#else
    #define trace(fmt,args...)
    #define traceTime(t)
#endif
                                 
#endif
