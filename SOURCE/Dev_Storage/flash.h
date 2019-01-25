#ifndef __FLASH_001
#define __FLASH_001

#define FLASHADDR_BASEINFO      		0x0000	//基本信息
	#define BASEOFFSET_SYSTEMID		0x00   
	#define BASEOFFSET_SYSTEMMAC	0x04
	#define BASEOFFSET_SYSTEMKEY	0x08	//8字节
	#define BASEOFFSET_ANTIDA   	0x10	//2字节
	#define BASEOFFSET_ANTIDB   	0x12	
	#define BASEOFFSET_ANTIDM   	0x14	
	#define BASEOFFSET_ANTIDN   	0x16	
	
#define FLASHADDR_CMDBASE				0x0020
	#define CMDBASEOFFSET_QVALA		0x00
	#define CMDBASEOFFSET_QVALB		0x01
	#define CMDBASEOFFSET_QVALM		0x02
	#define CMDBASEOFFSET_QVALN		0x03
	#define CMDBASEOFFSET_LFCA		0x04
	#define CMDBASEOFFSET_LFCB		0x05
	#define CMDBASEOFFSET_LFCM		0x06
	#define CMDBASEOFFSET_LFCN		0x07


bool drv_Flash_check(UInt16 uAddr,void* pBuff,UInt32 nLen);
bool drv_Flash_read(UInt16 uAddr,void* pBuff,UInt32 nLen);
bool drv_Flash_write(UInt16 uAddr,void* pBuff,UInt32 nLen);

void fns_Flash_setSystemData(UInt32 sysId,UInt32 sysMac,UInt64 sysKey);
void fns_Flash_getSystemData(UInt8* sysId,UInt32* sysMac,UInt8* sysKey);
void fns_Flash_setAntId(UInt8 ant,UInt16 value);
UInt16 fns_Flash_getAntId(UInt8 ant);

void fns_Flash_setQVAL(UInt8 ant,UInt8 val);
UInt8 fns_Flash_getQVAL(UInt8 ant);
void fns_Flash_setLFC(UInt8 ant,UInt8 val);
UInt8 fns_Flash_getLFC(UInt8 ant);

#endif

