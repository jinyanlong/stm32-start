#ifndef __FLASH_001
#define __FLASH_001

#define FLASHADDR_BASEINFO      		0x0000	//基本信息
	#define BASEOFFSET_DATAVERSION		0x00    //数据版本
	#define BASEOFFSET_BAUD				0x09

	
#define FLASHADDR_CMDBASE				0x0020
	#define CMDBASEOFFSET_DATAVERSION		0x00
	#define CMDBASEOFFSET_REV				0x02
	#define CMDBASEOFFSET_DEVICEADDR		0x04	//设备地址（2B）
	#define CMDBASEOFFSET_OUTPUTFORMAT		0x06	//输出格式，十进制或十六进制（2B）
	#define CMDBASEOFFSET_OUTPUTMODE		0x08	//输出模式，主动输出和被动输出（2B）
	#define CMDBASEOFFSET_BUZZER			0x0A	//蜂鸣器控制（2B）
	#define CMDBASEOFFSET_TAGTYPE			0x0C	//读卡类型(1B)
	#define CMDBASEOFFSET_TIMEOVER			0x0E	//无卡超时时间(4B)
	#define CMDBASEOFFSET_TTFBAUD			0x12	//TTF输出波特率(1B)
	#define CMDBASEOFFSET_TTFMODE			0x14	//TTF模式(1B)
	#define CMDBASEOFFSET_OUTFREQ			0x16	//输出频率(4B)
	#define CMDBASEOFFSET_REPEADOUTPUT		0x1B	//是否重复输出(1B)
	#define CMDBASEOFFSET_NOTAGFLAG			0x1D	//是否是否输出NOTAG标志(1B)

bool drv_Flash_check(UInt16 uAddr,void* pBuff,UInt32 nLen);
bool drv_Flash_read(UInt16 uAddr,void* pBuff,UInt32 nLen);
bool drv_Flash_write(UInt16 uAddr,void* pBuff,UInt32 nLen);

void fns_Flash_setBaud(UInt8 baud);
UInt8 fns_Flash_getBaud(void);
void fns_Flash_setDeviceAddr(UInt8 addr);
UInt8 fns_Flash_getDeviceAddr(void);	
void fns_Flash_setOutputFormat(UInt8 fmt);
UInt8 fns_Flash_getOutputFormat(void);
void fns_Flash_setOutputMode(UInt8 mode);
UInt8 fns_Flash_getOutputMode(void);
void fns_Flash_setBuzzer(UInt8 buz);
UInt8 fns_Flash_getBuzzer(void);

void fns_Flash_setTagType(UInt8 tagtype);
UInt8 fns_Flash_getTagType(void);
void fns_Flash_setNoTagTimeout(UInt16 time);
UInt16 fns_Flash_getNoTagTimeout(void);
void fns_Flash_setTTFBaud(UInt8 baud);
UInt8 fns_Flash_getTTFBaud(void);
void fns_Flash_setTTFMode(UInt8 mode);
UInt8 fns_Flash_getTTFMode(void);
void fns_Flash_setOutFreq(UInt16 freq);
UInt16 fns_Flash_getOutFreq(void);

#endif

