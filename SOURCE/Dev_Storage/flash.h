#ifndef __FLASH_001
#define __FLASH_001

#define FLASHADDR_BASEINFO      		0x0000	//������Ϣ
	#define BASEOFFSET_DATAVERSION		0x00    //���ݰ汾
	#define BASEOFFSET_BAUD				0x09

	
#define FLASHADDR_CMDBASE				0x0020
	#define CMDBASEOFFSET_DATAVERSION		0x00
	#define CMDBASEOFFSET_REV				0x02
	#define CMDBASEOFFSET_DEVICEADDR		0x04	//�豸��ַ��2B��
	#define CMDBASEOFFSET_OUTPUTFORMAT		0x06	//�����ʽ��ʮ���ƻ�ʮ�����ƣ�2B��
	#define CMDBASEOFFSET_OUTPUTMODE		0x08	//���ģʽ����������ͱ��������2B��
	#define CMDBASEOFFSET_BUZZER			0x0A	//���������ƣ�2B��
	#define CMDBASEOFFSET_TAGTYPE			0x0C	//��������(1B)
	#define CMDBASEOFFSET_TIMEOVER			0x0E	//�޿���ʱʱ��(4B)
	#define CMDBASEOFFSET_TTFBAUD			0x12	//TTF���������(1B)
	#define CMDBASEOFFSET_TTFMODE			0x14	//TTFģʽ(1B)
	#define CMDBASEOFFSET_OUTFREQ			0x16	//���Ƶ��(4B)
	#define CMDBASEOFFSET_REPEADOUTPUT		0x1B	//�Ƿ��ظ����(1B)
	#define CMDBASEOFFSET_NOTAGFLAG			0x1D	//�Ƿ��Ƿ����NOTAG��־(1B)

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

