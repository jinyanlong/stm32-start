#ifndef __FLASH_001
#define __FLASH_001

#define FLASHADDR_BASEINFO      		0x0000	//������Ϣ
	#define BASEOFFSET_SYSTEMID		0x00   
	#define BASEOFFSET_SYSTEMMAC	0x04
	#define BASEOFFSET_SYSTEMKEY	0x08	//8�ֽ�
	#define BASEOFFSET_ANTID   		0x10	//2�ֽ�
	
#define FLASHADDR_CMDBASE				0x0020
	#define CMDBASEOFFSET_DATAVERSION		0x00
	#define CMDBASEOFFSET_REV				0x02
	#define CMDBASEOFFSET_DEVICEADDR		0x04	//�豸��ַ��2B��


bool drv_Flash_check(UInt16 uAddr,void* pBuff,UInt32 nLen);
bool drv_Flash_read(UInt16 uAddr,void* pBuff,UInt32 nLen);
bool drv_Flash_write(UInt16 uAddr,void* pBuff,UInt32 nLen);

void fns_Flash_setSystemData(UInt32 sysId,UInt32 sysMac,UInt64 sysKey);
void fns_Flash_getSystemData(UInt8* sysId,UInt32* sysMac,UInt8* sysKey);
void fns_Flash_setAntId(UInt16 ant);
UInt16 fns_Flash_getAntId(void);
#endif

