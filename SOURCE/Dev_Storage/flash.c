#include <includes.h>
#include "Flash.h"
#include "halAT24.h"

bool drv_Flash_check(UInt16 uAddr,void* pBuff,UInt32 nLen){//本设备采用的是AT24C256(32k)
    return hal_AT24C64_check(0xA0,uAddr, pBuff,nLen);
}
bool drv_Flash_read(UInt16 uAddr,void* pBuff,UInt32 nLen){
    return hal_AT24C64_read(0xA0,uAddr,pBuff,nLen);
}
bool drv_Flash_write(UInt16 uAddr,void* pBuff,UInt32 nLen){
    return hal_AT24C64_write(0xA0,uAddr,pBuff,nLen);
}


void fns_Flash_setSystemData(UInt32 sysId,UInt32 sysMac,UInt64 sysKey){
	if(sysId){
		drv_Flash_write(FLASHADDR_BASEINFO+BASEOFFSET_SYSTEMID,&sysId,4);
	}
	if(sysMac){
		drv_Flash_write(FLASHADDR_BASEINFO+BASEOFFSET_SYSTEMMAC,&sysMac,4);
	}
	if(sysKey){
		drv_Flash_write(FLASHADDR_BASEINFO+BASEOFFSET_SYSTEMKEY,&sysKey,8);
	}
}

void fns_Flash_getSystemData(UInt8* sysId,UInt32* sysMac,UInt8* sysKey){
	if(sysId){
		drv_Flash_read(FLASHADDR_CMDBASE+CMDBASEOFFSET_DEVICEADDR,sysId,4);
	}
	if(sysMac){
		drv_Flash_read(FLASHADDR_CMDBASE+BASEOFFSET_SYSTEMMAC,sysMac,4);
	}
	if(sysKey){
		drv_Flash_read(FLASHADDR_CMDBASE+BASEOFFSET_SYSTEMKEY,sysKey,8);
	}
}

void fns_Flash_setAntId(UInt16 ant){
	drv_Flash_write(FLASHADDR_BASEINFO+BASEOFFSET_ANTID,&ant,2);
}

UInt16 fns_Flash_getAntId(void){
	UInt16 ant;
	drv_Flash_write(FLASHADDR_BASEINFO+BASEOFFSET_ANTID,&ant,2);
	return ant;
}



