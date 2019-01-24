#include <includes.h>
#include "Flash.h"
#include "halAT24.h"

bool drv_Flash_check(UInt16 uAddr,void* pBuff,UInt32 nLen){//���豸���õ���AT24C256(32k)
    return hal_AT24C64_check(0xA0,uAddr, pBuff,nLen);
}
bool drv_Flash_read(UInt16 uAddr,void* pBuff,UInt32 nLen){
    return hal_AT24C64_read(0xA0,uAddr,pBuff,nLen);
}
bool drv_Flash_write(UInt16 uAddr,void* pBuff,UInt32 nLen){
    return hal_AT24C64_write(0xA0,uAddr,pBuff,nLen);
}

//������
void fns_Flash_setBaud(UInt8 baud){
	drv_Flash_write(FLASHADDR_BASEINFO+BASEOFFSET_BAUD,&baud,1);
}
UInt8 fns_Flash_getBaud(void){
	UInt8 baud;
	drv_Flash_read(FLASHADDR_BASEINFO+BASEOFFSET_BAUD,&baud,1);
	return baud;
}

//�豸��ַ
void fns_Flash_setDeviceAddr(UInt8 addr){
	drv_Flash_write(FLASHADDR_CMDBASE+CMDBASEOFFSET_DEVICEADDR,&addr,1);
}
UInt8 fns_Flash_getDeviceAddr(void){
	UInt8 addr;
	drv_Flash_read(FLASHADDR_CMDBASE+CMDBASEOFFSET_DEVICEADDR,&addr,1);
	return addr;
}

//�����ʽ
void fns_Flash_setOutputFormat(UInt8 fmt){
	drv_Flash_write(FLASHADDR_CMDBASE+CMDBASEOFFSET_OUTPUTFORMAT,&fmt,1);
}
UInt8 fns_Flash_getOutputFormat(void){
	UInt8 fmt;
	drv_Flash_read(FLASHADDR_CMDBASE+CMDBASEOFFSET_OUTPUTFORMAT,&fmt,1);
	return fmt;
}

//���ģʽ
void fns_Flash_setOutputMode(UInt8 mode){
	drv_Flash_write(FLASHADDR_CMDBASE+CMDBASEOFFSET_OUTPUTMODE,&mode,1);
}
UInt8 fns_Flash_getOutputMode(void){
	UInt8 mode;
	drv_Flash_read(FLASHADDR_CMDBASE+CMDBASEOFFSET_OUTPUTMODE,&mode,1);
	return mode;
}

//������
void fns_Flash_setBuzzer(UInt8 buz){
	drv_Flash_write(FLASHADDR_CMDBASE+CMDBASEOFFSET_BUZZER,&buz,1);
}
UInt8 fns_Flash_getBuzzer(void){
	UInt8 buz;
	drv_Flash_read(FLASHADDR_CMDBASE+CMDBASEOFFSET_BUZZER,&buz,1);
	return buz;
}

//��������
void fns_Flash_setTagType(UInt8 tagtype){
	drv_Flash_write(FLASHADDR_CMDBASE+CMDBASEOFFSET_TAGTYPE,&tagtype,1);
}
UInt8 fns_Flash_getTagType(void){
	UInt8 tagtype;
	drv_Flash_read(FLASHADDR_CMDBASE+CMDBASEOFFSET_TAGTYPE,&tagtype,1);
	return tagtype;
}

//�޿���ʱʱ��
void fns_Flash_setNoTagTimeout(UInt16 time){
	drv_Flash_write(FLASHADDR_CMDBASE+CMDBASEOFFSET_TIMEOVER,&time,2);
}
UInt16 fns_Flash_getNoTagTimeout(void){
	UInt16 time;
	drv_Flash_read(FLASHADDR_CMDBASE+CMDBASEOFFSET_TIMEOVER,&time,2);
	return time;
}

//TTF���������
void fns_Flash_setTTFBaud(UInt8 baud){
	drv_Flash_write(FLASHADDR_CMDBASE+CMDBASEOFFSET_TTFBAUD,&baud,1);
}
UInt8 fns_Flash_getTTFBaud(void){
	UInt8 baud;
	drv_Flash_read(FLASHADDR_CMDBASE+CMDBASEOFFSET_TTFBAUD,&baud,1);
	return baud;
}

//TTFģʽ
void fns_Flash_setTTFMode(UInt8 mode){
	drv_Flash_write(FLASHADDR_CMDBASE+CMDBASEOFFSET_TTFMODE,&mode,1);
}
UInt8 fns_Flash_getTTFMode(void){
	UInt8 mode;
	drv_Flash_read(FLASHADDR_CMDBASE+CMDBASEOFFSET_TTFMODE,&mode,1);
	return mode;
}

//���Ƶ��
void fns_Flash_setOutFreq(UInt16 freq){
	drv_Flash_write(FLASHADDR_CMDBASE+CMDBASEOFFSET_OUTFREQ,&freq,2);
}
UInt16 fns_Flash_getOutFreq(void){
	UInt16 freq;
	drv_Flash_read(FLASHADDR_CMDBASE+CMDBASEOFFSET_OUTFREQ,&freq,2);
	return freq;
}
////�Ƿ��ظ����
//void fns_Flash_setRepeatFlag(UInt8 rep){
//	drv_Flash_write(FLASHADDR_CMDBASE+CMDBASEOFFSET_REPEADOUTPUT,&rep,1);
//}
//UInt8 fns_Flash_getRepeatFlag(void){
//	UInt8 rep;
//	drv_Flash_read(FLASHADDR_CMDBASE+CMDBASEOFFSET_REPEADOUTPUT,&rep,1);
//	return rep;
//}

////�Ƿ����NOTAG��־
//void fns_Flash_setNoTagFlag(UInt8 flag){
//	drv_Flash_write(FLASHADDR_CMDBASE+CMDBASEOFFSET_NOTAGFLAG,&flag,1);
//}
//UInt8 fns_Flash_getNoTagFlag(void){
//	UInt8 flag;
//	drv_Flash_read(FLASHADDR_CMDBASE+CMDBASEOFFSET_NOTAGFLAG,&flag,1);
//	return flag;
//}



