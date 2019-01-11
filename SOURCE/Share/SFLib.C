#include "includes.h"

#ifdef __TRACE
    char _trace_buffer[256];
#endif
//参考网址  http://zorc.breitbandkatze.de/crc.html , CCITT的衍生种类很多
/**************************************************************************
const UInt16 crc16_ccitt_table_h[256]={ // CRC余式表
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
	0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
	0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
	0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
	0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
	0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
	0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
	0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
	0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
	0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
	0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
	0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
	0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
	0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
	0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
	0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
	0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
	0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
	0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
	0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
	0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
	0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
	0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
	0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
	0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
	0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
	0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
	0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
	0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
	0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
	0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
	0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};
//低字节在前,高位在前(常规算法,但似乎不是标准)
UInt16 crc16_ccitt_h(UInt16 preset,void *ptr, UInt32 nLen) {
	UInt16 crc;
	UInt8 da;
	UInt8* pNext=ptr;

	crc=preset;
	while(nLen) {
		nLen--;
		da=(UInt8) (crc/256); // 以8位二进制数的形式暂存CRC的高8位
		crc<<=8; // 左移8位，相当于CRC的低8位乘以
		crc^=crc16_ccitt_table_h[da^(*pNext)]; // 高8位和当前字节相加后再查表求CRC ，再加上以前的CRC
		pNext++;
	}
	return(crc);
}
*************************************************************************************************/
const char HEXSTR[]="0123456789ABCDEF";
UInt16 crc16_ccitt_table_l[256] ={
	0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
	0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
	0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
	0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
	0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
	0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
	0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
	0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
	0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
	0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
	0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
	0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
	0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
	0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
	0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
	0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
	0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
	0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
	0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
	0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
	0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
	0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
	0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
	0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
	0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
	0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
	0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
	0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
	0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
	0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
	0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
	0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};
//初值为0,低位在前,低字节在前(ISO11784中使用)
//初值为0的CRC校验有很大的缺陷(对0的校验很弱),很多场合下采用初值FFFF
UInt16 crc16_ccitt_l(UInt16 preset,const void *pBuff, UInt32 nLen){
    unsigned short crc_reg = preset;
    UInt8* pNext=(UInt8*)pBuff;
	while (nLen--) {
        crc_reg = (crc_reg >> 8) ^ crc16_ccitt_table_l[(crc_reg ^ (*pNext) ) & 0xFF];
		pNext++;

	}
    return crc_reg;
}
UInt16 crc16_ccitt_calcbits(UInt16 preset,UInt16 val,UInt32 Bitcount){
	UInt16 nCrc;
	nCrc=preset;

	nCrc^=val;
	do{
		if(nCrc & 0x0001){
			nCrc>>=1;
			nCrc ^=CRC16_CCITT_POLYNOM_L;
		}else{
			nCrc>>=1;
		}
	}while(--Bitcount);
	return  nCrc;
}

const UInt8 CRC8TABLE[256]={0x00,0x1D,0x3A,0x27,0x74,0x69,0x4E,0x53,0xE8,0xF5,0xD2,0xCF,0x9C,0x81,0xA6,0xBB,
					0xCD,0xD0,0xF7,0xEA,0xB9,0xA4,0x83,0x9E,0x25,0x38,0x1F,0x02,0x51,0x4C,0x6B,0x76,
					0x87,0x9A,0xBD,0xA0,0xF3,0xEE,0xC9,0xD4,0x6F,0x72,0x55,0x48,0x1B,0x06,0x21,0x3C,
					0x4A,0x57,0x70,0x6D,0x3E,0x23,0x04,0x19,0xA2,0xBF,0x98,0x85,0xD6,0xCB,0xEC,0xF1,
					0x13,0x0E,0x29,0x34,0x67,0x7A,0x5D,0x40,0xFB,0xE6,0xC1,0xDC,0x8F,0x92,0xB5,0xA8,
					0xDE,0xC3,0xE4,0xF9,0xAA,0xB7,0x90,0x8D,0x36,0x2B,0x0C,0x11,0x42,0x5F,0x78,0x65,
					0x94,0x89,0xAE,0xB3,0xE0,0xFD,0xDA,0xC7,0x7C,0x61,0x46,0x5B,0x08,0x15,0x32,0x2F,
					0x59,0x44,0x63,0x7E,0x2D,0x30,0x17,0x0A,0xB1,0xAC,0x8B,0x96,0xC5,0xD8,0xFF,0xE2,
					0x26,0x3B,0x1C,0x01,0x52,0x4F,0x68,0x75,0xCE,0xD3,0xF4,0xE9,0xBA,0xA7,0x80,0x9D,
					0xEB,0xF6,0xD1,0xCC,0x9F,0x82,0xA5,0xB8,0x03,0x1E,0x39,0x24,0x77,0x6A,0x4D,0x50,
					0xA1,0xBC,0x9B,0x86,0xD5,0xC8,0xEF,0xF2,0x49,0x54,0x73,0x6E,0x3D,0x20,0x07,0x1A,
					0x6C,0x71,0x56,0x4B,0x18,0x05,0x22,0x3F,0x84,0x99,0xBE,0xA3,0xF0,0xED,0xCA,0xD7,
					0x35,0x28,0x0F,0x12,0x41,0x5C,0x7B,0x66,0xDD,0xC0,0xE7,0xFA,0xA9,0xB4,0x93,0x8E,
					0xF8,0xE5,0xC2,0xDF,0x8C,0x91,0xB6,0xAB,0x10,0x0D,0x2A,0x37,0x64,0x79,0x5E,0x43,
					0xB2,0xAF,0x88,0x95,0xC6,0xDB,0xFC,0xE1,0x5A,0x47,0x60,0x7D,0x2E,0x33,0x14,0x09,
					0x7F,0x62,0x45,0x58,0x0B,0x16,0x31,0x2C,0x97,0x8A,0xAD,0xB0,0xE3,0xFE,0xD9,0xC4} ;

#define NextCRC8(curCrc,val)  CRC8TABLE[(UInt8)((curCrc)^(val))]

//CRC8=X^8 + X^4 + X^3 + X^2 + 1
UInt8 crc8_byte_calc(UInt8 preset,const void* pDataBuf,UInt32 Length){
	UInt32 i;
	UInt8 Crc=preset;
	for (i=0;i<Length;i++)
		Crc=NextCRC8(Crc,((UInt8*)pDataBuf)[i]);
  	return Crc;
}

UInt8 crc8_bit_calc(UInt8 preset,UInt8 val,UInt32 Bitcount){
	UInt8 bCrc;
	bCrc=preset;

	bCrc^=val;
	do{
		if(bCrc & 0x80){
			bCrc<<=1;
			bCrc ^=CRC8_POLYNOM;
		}else{
			bCrc<<=1;
		}
	}while(--Bitcount);
	return  bCrc;
}
void crc8_append(UInt8 preset,const void* pDataBuff,UInt32 nLength){
    ((UInt8*)pDataBuff)[nLength]=crc8_byte_calc(preset,pDataBuff,nLength);
}

UInt8 BitReverse(UInt8 bytVal){
	UInt8 bytRet=0;
	if(bytVal & 0x01){
		bytRet|=0x80;
	}
	if(bytVal & 0x02){
		bytRet|=0x40;
	}
	if(bytVal & 0x04){
		bytRet|=0x20;
	}

	if(bytVal & 0x08){
		bytRet|=0x10;
	}

	if(bytVal & 0x10){
		bytRet|=0x08;
	}

	if(bytVal & 0x20){
		bytRet|=0x04;
	}

	if(bytVal & 0x40){
		bytRet|=0x02;
	}

	if(bytVal & 0x80){
		bytRet|=0x01;
	}
	return bytRet;
}
void BitReverseEx(UInt8* pSrc,UInt8* pTrg,UInt32 nCount){
	UInt32 i;
	for(i=0;i<nCount;i++){
		pTrg[i]=BitReverse(pSrc[i]);
	}
}

bool MCBit_Parse(UInt32* pSampleBits,UInt32* pDataBits, UInt32 nCount){
	UInt32 i;
	for(i=0;i<nCount;i++){
		if( (pSampleBits[2*i]==1) && (pSampleBits[2*i+1]==0) ){
			pDataBits[i]=1;
		}else if((pSampleBits[2*i]==0) && (pSampleBits[2*i+1]==1) ){
			pDataBits[i]=0;
		}else{
			return false;
		}
	}
	return true;
}

//循环解析
bool MCBit_ParseEx(UInt32* pSampleBits,UInt16 nSOF,UInt32 nBuffSize,UInt32* pDataBits,UInt32 nCount){
	UInt32 i;
	for(i=0;i<nCount;i++){
		if( (pSampleBits[(nSOF+2*i)%nBuffSize]==1) && (pSampleBits[(nSOF+2*i+1)%nBuffSize]==0) ){
			pDataBits[i]=1;
		}else if((pSampleBits[(nSOF+2*i)%nBuffSize]==0) && (pSampleBits[(nSOF+2*i+1)%nBuffSize]==1) ){
			pDataBits[i]=0;
		}else{
			return false;
		}
	}
	return true;
}


bool ACBit_Parse(UInt32* pSampleBits,UInt32* pDataBits, UInt32 nCount){
	UInt32 i;
	for(i=0;i<nCount;i++){
		if( (pSampleBits[4*i]==1) && (pSampleBits[4*i+1]==0) && (pSampleBits[4*i+2]==1) && (pSampleBits[4*i+3]==0) ){
			pDataBits[i]=1;
		}else if((pSampleBits[4*i]==1) && (pSampleBits[4*i+1]==1) && (pSampleBits[4*i+2]==0) && (pSampleBits[4*i+3]==0) ){
			pDataBits[i]=0;
		}else{
			return false;
		}
	}
	return true;
}

bool BCBit_Parse(UInt32* pSampleBits,UInt32* pDataBits, UInt32 nCount){
	UInt32 i;
	for(i=0;i<nCount;i++){
		if(pSampleBits[i*2]==pSampleBits[i*2+1]){
			pDataBits[i]=1;
		}else{
			pDataBits[i]=0;
		}
	}
    return true;
}

BYTE get_bcc(void * pBuff, UInt32 num){
	BYTE bcc;
	UInt32 i;
	bcc=0;
	for(i=0;i<num;i++)
		bcc^=((BYTE*)pBuff)[i];
	return bcc;
}

UInt16 ToUInt16(UInt8 bh, UInt8 bl){
	u16 wRet;
	wRet = (UInt16)bl | ((UInt16)bh << 8);
	return(wRet);
}
UInt16 ByteSwap(UInt16 wSwW){
    UInt8 bTemp;
    UInt16 wRet;
    bTemp = (UInt8)(wSwW & 0xff);
    wRet =  (wSwW >> 8) | ((UInt16)bTemp << 8);
    return(wRet);
}


////////////////////////////////时钟系统////////////////////////////////////////
const UInt8 s_month_days[]={31,28,31,30,31,30,31,31,30,31,30,31};
const UInt8 s_month_days_r[]={31,29,31,30,31,30,31,31,30,31,30,31};

void def_time_adjust(DATETIME* pDateTime){
    pDateTime->Year%=50;
    if(pDateTime->Month==0){
        pDateTime->Month=12;
    }else if(pDateTime->Month>12){
        pDateTime->Month=1;
    }

    if( pDateTime->Year%4==0 ){
        if(pDateTime->Day==0){
            pDateTime->Day=s_month_days_r[pDateTime->Month-1];
        }else if(pDateTime->Day>s_month_days_r[pDateTime->Month-1]){
            pDateTime->Day=1;
        }
    }else{
        if(pDateTime->Day==0){
            pDateTime->Day=s_month_days[pDateTime->Month-1];
        }else if(pDateTime->Day>s_month_days[pDateTime->Month-1]){
            pDateTime->Day=1;
        }
    }
    pDateTime->Hour%=24;
    pDateTime->Minute%=60;
    pDateTime->Second%=60;
}

//提取某一年内的日期索引
//1月1日 对应 0
//12月31日 对应 364
UInt32 def_year_getdays(UInt8 nYear,UInt8 nMonth,UInt8 nDay){
	UInt32 nDays,i;
	nDays=0;
	if(nYear%4==0){//润年
		for(i=1;i<nMonth;i++){
			nDays+=s_month_days_r[i-1];
		}
	}else{
		for(i=1;i<nMonth;i++){
			nDays+=s_month_days[i-1];
		}
	}
	nDays+=nDay-1;
	return nDays;
}

//0   对应2000-1-1(润年)
//366 对应2001-1-1
UInt8 def_year_parse(UInt8 nStartYear,UInt32* pDays){
	UInt8 nYear;
	
	nYear=nStartYear;
	while(nYear<100){
		if(nYear%4==0){
			if( *pDays<366){
				return nYear;
			}else{
				*pDays-=366;
				nYear++;
			}
		}else{
			if( *pDays<365 ){
				return nYear;
			}else{
				*pDays-=365;
				nYear++;
			}
		}
	}
	return nYear;
}

UInt8 def_month_parse(UInt8 nYear,UInt32* pYearDays){
	int i;
	if(nYear%4==0){
		for(i=0;i<12;i++){
			if(*pYearDays<s_month_days_r[i]){
				return i+1;
			}else{
				*pYearDays-=s_month_days_r[i];
			}
		}
	}else{
		for(i=0;i<12;i++){
			if(*pYearDays<s_month_days[i]){
				return i+1;
			}else{
				*pYearDays-=s_month_days[i];
			}
		}
	}
	return 1;
}

//计算的基点是2000-1-1 00:00:00,单位秒
UInt32 TimeToSeconds(DATETIME* pTm){
    UInt32 _seconds;
    UInt32 _days;

    _days=pTm->Year*365+(pTm->Year+3)/4;
	_days+=def_year_getdays(pTm->Year,pTm->Month,pTm->Day);
	_seconds=pTm->Hour*3600+pTm->Minute*60+pTm->Second;
    return _seconds + _days*3600*24;
}

void SecondsToTime(UInt32 seconds,DATETIME* pTm){
	UInt32 nDayCount;
	
	nDayCount= seconds/(3600*24);
	pTm->Year=def_year_parse(0,&nDayCount);
	pTm->Month=def_month_parse(pTm->Year,&nDayCount);
	pTm->Day=nDayCount+1;
	
	seconds %= (24*60*60);
	pTm->Hour =seconds/(60*60);
	seconds %= (60*60);
	pTm->Minute =seconds/(60);
	seconds %= (60);
	pTm->Second =seconds;
}

Int64 parseTime(DATETIME* pTm,short timeZone){
    Int64 val;
    val=TimeToSeconds(pTm);
    val+=UTC_S_2000+timeZone*60;
    val*=1000;
    return val;
}
Int64 parseTimeEx(DATETIME_EX* pTm,short timeZone){
    Int64 val;
    val=parseTime((DATETIME*)pTm,timeZone);
    val+=pTm->MilliSecond;
    return val;
    
}

//zone:分钟
void printTime(Int64 time,short zone,DATETIME* dt){
    if(time<=UTC_MS_2000){
        memset(dt,0,sizeof(DATETIME));
        return;
    }
        
    time+=(int)zone*60*1000;
    SecondsToTime((UInt32)(time/1000-UTC_S_2000),(DATETIME*)dt);
}
void printTimeEx(Int64 time,short zone,DATETIME_EX* dt){
    if(time<=UTC_MS_2000){
        memset(dt,0,sizeof(DATETIME_EX));
        return;
    }
    time+=(int)zone*60*1000;
    SecondsToTime((UInt32)(time/1000-UTC_S_2000),(DATETIME*)dt);
    dt->MilliSecond=(UInt16)(time%1000);
}

UInt32 utcToSeconds(Int64 utc,UInt32 base){
    return (UInt32)(utc/1000-base);
}

Int64 secondsToUTC(UInt32 seconds,UInt32 base){
    Int64 ret;
    ret=seconds;
    ret+=base;
    ret*=1000;
    return ret;
}



bool checkTime(DATETIME *pTm){
    if( pTm->Year>=100 ){
        return false;
    }
    if( (pTm->Month<1) || (pTm->Month>12) ){
        return false;
    }
    if( (pTm->Day<1) || (pTm->Day>31)){
        return false;
    }
    if( pTm->Hour>23){
        return false;
    }
    if( pTm->Minute>59){
        return false;
    }
    if( pTm->Second>59){
        return false;
    }

    return true;
}


void BinToHex(const void* pBin,void* pHex,UInt32 nCount){
    UInt8* pBuff=(UInt8*)pBin;
    StringBuffer pText=pHex;
    UInt32 i;
    for(i=0;i<nCount;i++){
        pText[i*2]=HEXSTR[pBuff[i]>>4];
        pText[i*2+1]=HEXSTR[pBuff[i]&0x0F];
    }
    pText[nCount*2]=0;
}

UInt32 HexToBin(const void* pHex,void* pBin,UInt32 nCount){
	UInt32 i;
    BYTE bytVal_h,bytVal_l;
	char ch;
    UInt8* pBuff=(UInt8*)pBin;
    String pText=(String)pHex;
    
    for(i=0;i<nCount;i++){
		ch=pText[i*2];
		if(	(ch>='0') && (ch<='9') ){
			bytVal_h=ch-'0';
		}else if(ch>='A' && ch<='F'){
			bytVal_h=ch-'A'+10;			
		}else if(ch>='a' && ch<='f'){
			bytVal_h=ch-'a'+10;
		}else{
			return i;
		}

        ch=pText[i*2+1];
		if(	(ch>='0') && (ch<='9') ){
			bytVal_l=ch-'0';
		}else if(ch>='A' && ch<='F'){
			bytVal_l=ch-'A'+10;			
		}else if(ch>='a' && ch<='f'){
			bytVal_l=ch-'a'+10;
		}else{
			return i;
		}

		pBuff[i]=bytVal_h*0x10+bytVal_l;
	}
	return nCount;
}

//忽略空格
#define ignoreBlank(i,str)      while(str[i]==' ') i++

//功能把字符转换成数字(10进制数)
UInt32  parseUInt32(String pdata,UInt8 nSize){
    UInt32 ret=0,temp=0;
    UInt8  i=0;

    ignoreBlank(i,pdata);
    if(pdata[i]=='+'){
        i++;
    }
    
    for(;i<nSize;i++){
        if((pdata[i]>='0')&&(pdata[i]<='9')){
            temp=(pdata[i]-'0');
//        }else  if((pdata[i]>='A')&&(pdata[i]<='F')){	//一定是大写
//            temp=(pdata[i]-'A'+10);
        }else{
            break;
        }
       
        ret=10*ret+temp;
    }
    return ret;
}
int parseInt(String pdata,UInt8 nMaxSize){
    int flag=1,ret=0,temp=0;
    UInt8  i=0;
    if(pdata==NULL) return -1;
    
    ignoreBlank(i,pdata);
    
    if(pdata[i]=='-'){
        flag=-1;
        i++;
    }else if(pdata[i]=='+'){
        i++;
    }else if( (pdata[i]<'0') || (pdata[i]>'9') ){
        //不是数字出错
        return -1;
    }
    
    for(;i<nMaxSize;i++){
        if((pdata[i]>='0')&&(pdata[i]<='9')){
            temp=(pdata[i]-'0');
//        }else  if((pdata[i]>='A')&&(pdata[i]<='F')){	//一定是大写
//            temp=(pdata[i]-'A'+10);
        }else{
            break;
        }
       
        ret=10*ret+temp;
    }
    return ret*flag;
}

void memnor(void* pVoid,UInt32 nSize){
    UInt8* buffer=(UInt8*)pVoid;
    UInt32 i;
    //执行一次取反操作,使得擦除后,数据出事值为0
    for(i=0;i<nSize;i++){
        buffer[i]=~buffer[i];
    }
}

int findByte(UInt8* buff,int count,UInt8 ch){
    int i;
    for(i=0;i<count;i++){
        if(buff[i]==ch) return i;
    }
    return -1;
}
int findChar(String str,char ch){
    int i;
    for(i=0;str[i]!=0;i++){
        if(str[i]==ch) return i;
    }
    return -1;
}

void delay_1us(void){
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();

    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();

    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
}

void delay(UInt32 tick){
    UInt32 i;
	for(i=0;i<tick;i++){
		__nop();
	}
}
void delay_us(UInt32 _1us){
	UInt32 i;
	for(i=0;i<_1us;i++){
		delay_1us();
	}
}

void delay_ms(UInt32 _1ms){
    vTaskDelay((portTickType)_1ms);
}

//********************************************************************
//** 函数名称: distance_jw
//** 功能描述: 根据经度纬度计算距离
//** 参    数:  经度纬度
//** 返 回 值:  距离
//********************************************************************* 
float computeDistance(int GpsLo1,int GpsLa1,int GpsLo2,int GpsLa2){
	float   PI     =3.1425926;
	float   Earth_R=6378137.0;	 //m
	float   delta_lon=0.0;
	float   hudu_lo1,hudu_lo2;	
	float   hudu_la1,hudu_la2;
	float   temp_under_sqr,temp_under_div; 
	float   temp_dis;

    hudu_lo1=GpsLo1*PI/3600000.0/180.0;
    hudu_lo2=GpsLo2*PI/3600000.0/180.0;
    hudu_la1=GpsLa1*PI/3600000.0/180.0;
	hudu_la2=GpsLa2*PI/3600000.0/180.0;
    delta_lon=fabs(hudu_lo1-hudu_lo2);			 //经度差
	temp_under_sqr=(cos(hudu_la2)*cos(hudu_la2)*sin(delta_lon)*sin(delta_lon))+\
		(cos(hudu_la1)*sin(hudu_la2)-sin(hudu_la1)*cos(hudu_la2)*cos(delta_lon))*\
		(cos(hudu_la1)*sin(hudu_la2)-sin(hudu_la1)*cos(hudu_la2)*cos(delta_lon));
	temp_under_div=sin(hudu_la1)*sin(hudu_la2)+cos(hudu_la1)*cos(hudu_la2)*cos(delta_lon);
	temp_dis=Earth_R*atan(sqrt(temp_under_sqr)/temp_under_div);
    if(temp_dis<0){
        temp_dis=temp_dis*(-1);
    }
	return temp_dis;
}
