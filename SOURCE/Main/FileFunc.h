#ifndef _FILEFUNC_H
#define _FILEFUNC_H

#include "RaceData.h"

void fns_Data_getSiteName(UStringBuffer pSiteName);
void fns_Data_setURI(UInt8 nMode,AString pURI);
void fns_Data_getURI(UInt8 nMode,AStringBuffer pURI);
void fns_Data_getTag(UInt16 tagPos,TAGRECORD* tagItem);

bool fns_Record_get(UInt16 tagPos,TAGRECORD* tagItem);
#endif
