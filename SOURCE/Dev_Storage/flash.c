#include <includes.h>
#include "Flash.h"
#include "halAT24.h"
#include "lf.h"

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
		drv_Flash_read(FLASHADDR_CMDBASE+BASEOFFSET_SYSTEMID,sysId,4);
	}
	if(sysMac){
		drv_Flash_read(FLASHADDR_CMDBASE+BASEOFFSET_SYSTEMMAC,sysMac,4);
	}
	if(sysKey){
		drv_Flash_read(FLASHADDR_CMDBASE+BASEOFFSET_SYSTEMKEY,sysKey,8);
	}
}

void fns_Flash_setAntId(UInt8 ant,UInt16 value){
	if(ant==LF_ANTA){
		drv_Flash_write(FLASHADDR_BASEINFO+BASEOFFSET_ANTIDA,&value,2);
	}else if(ant==LF_ANTB){
		drv_Flash_write(FLASHADDR_BASEINFO+BASEOFFSET_ANTIDB,&value,2);
	}else if(ant==LF_ANTM){
		drv_Flash_write(FLASHADDR_BASEINFO+BASEOFFSET_ANTIDM,&value,2);
	}else if(ant==LF_ANTN){
		drv_Flash_write(FLASHADDR_BASEINFO+BASEOFFSET_ANTIDN,&value,2);
	}
}

UInt16 fns_Flash_getAntId(UInt8 ant){
	UInt16 value;
	if(ant==LF_ANTA){
		drv_Flash_read(FLASHADDR_BASEINFO+BASEOFFSET_ANTIDA,&value,2);
	}else if(ant==LF_ANTB){
		drv_Flash_read(FLASHADDR_BASEINFO+BASEOFFSET_ANTIDB,&value,2);
	}else if(ant==LF_ANTM){
		drv_Flash_read(FLASHADDR_BASEINFO+BASEOFFSET_ANTIDM,&value,2);
	}else if(ant==LF_ANTN){
		drv_Flash_read(FLASHADDR_BASEINFO+BASEOFFSET_ANTIDN,&value,2);
	}
	return value;
}

void fns_Flash_setQVAL(UInt8 ant,UInt8 value){
	if(ant==LF_ANTA){
		drv_Flash_write(FLASHADDR_CMDBASE+CMDBASEOFFSET_QVALA,&value,1);
	}else if(ant==LF_ANTB){
		drv_Flash_write(FLASHADDR_CMDBASE+CMDBASEOFFSET_QVALB,&value,1);
	}else if(ant==LF_ANTM){
		drv_Flash_write(FLASHADDR_CMDBASE+CMDBASEOFFSET_QVALM,&value,1);
	}else if(ant==LF_ANTN){
		drv_Flash_write(FLASHADDR_CMDBASE+CMDBASEOFFSET_QVALN,&value,1);
	}
}
UInt8 fns_Flash_getQVAL(UInt8 ant){
	UInt8 value;
	if(ant==LF_ANTA){
		drv_Flash_read(FLASHADDR_CMDBASE+CMDBASEOFFSET_QVALA,&value,1);
	}else if(ant==LF_ANTB){
		drv_Flash_read(FLASHADDR_CMDBASE+CMDBASEOFFSET_QVALB,&value,1);
	}else if(ant==LF_ANTM){
		drv_Flash_read(FLASHADDR_CMDBASE+CMDBASEOFFSET_QVALM,&value,1);
	}else if(ant==LF_ANTN){
		drv_Flash_read(FLASHADDR_CMDBASE+CMDBASEOFFSET_QVALN,&value,1);
	}
	return value;
}
void fns_Flash_setLFC(UInt8 ant,UInt8 value){
	if(ant==LF_ANTA){
		drv_Flash_write(FLASHADDR_CMDBASE+CMDBASEOFFSET_LFCA,&value,1);
	}else if(ant==LF_ANTB){
		drv_Flash_write(FLASHADDR_CMDBASE+CMDBASEOFFSET_LFCB,&value,1);
	}else if(ant==LF_ANTM){
		drv_Flash_write(FLASHADDR_CMDBASE+CMDBASEOFFSET_LFCM,&value,1);
	}else if(ant==LF_ANTN){
		drv_Flash_write(FLASHADDR_CMDBASE+CMDBASEOFFSET_LFCN,&value,1);
	}
}
UInt8 fns_Flash_getLFC(UInt8 ant){
	UInt8 value;
	if(ant==LF_ANTA){
		drv_Flash_read(FLASHADDR_CMDBASE+CMDBASEOFFSET_LFCA,&value,1);
	}else if(ant==LF_ANTB){
		drv_Flash_read(FLASHADDR_CMDBASE+CMDBASEOFFSET_LFCB,&value,1);
	}else if(ant==LF_ANTM){
		drv_Flash_read(FLASHADDR_CMDBASE+CMDBASEOFFSET_LFCM,&value,1);
	}else if(ant==LF_ANTN){
		drv_Flash_read(FLASHADDR_CMDBASE+CMDBASEOFFSET_LFCN,&value,1);
	}
	return value;	
}

