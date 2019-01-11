#include "includes.h"

#include "tagList.h"
#include "SFile.h"
#include "RaceTask.h"
#include "FileFunc.h"

void fns_Data_getSiteName(UStringBuffer pSiteName){
    RACETASK_LOCK();
    drv_Data_getSiteName(pSiteName);
    RACETASK_UNLOCK();
}

void fns_Data_setURI(UInt8 nMode,AString pURI){
    RACETASK_LOCK();
    drv_Data_setAString(nMode,pURI);    
    drv_File_flush(&s_dataFile);
    RACETASK_UNLOCK();
}

void fns_Data_getURI(UInt8 nMode,AStringBuffer pURI){
    RACETASK_LOCK();
    drv_Data_getAString(nMode,pURI);
    RACETASK_UNLOCK();
}

extern volatile MONITOR s_monitor;
bool fns_Record_get(UInt16 tagPos,TAGRECORD* tagItem){
    bool ret=false;
    RACETASK_LOCK();
    if(tagPos>=s_monitor.totalSOF && tagPos<s_monitor.totalEOF){
        drv_Record_get(tagPos,tagItem);
        ret=true;
    }
    RACETASK_UNLOCK();
    return ret;
}

