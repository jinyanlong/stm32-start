#include "includes.h"
#include "halCPU.h"
#include "DataFile.h"
#include "cacheFile.h"

//命名规范: 
//filePage:   文件中的页码
//pageRIndex: 物理页面逻辑索引
//pageAIndex: 物理页面绝对索引(仅在调用hal_Flash_*** 函数时使用)
//sectorRIndex: 物理页面逻辑索引
//sectorAIndex: 物理页面绝对索引(仅在调用hal_Flash_*** 函数时使用)
static UInt8 s_nand_pageBuff[NAND_PAGE_SIZE];       //一个临时用的物理页面缓冲区

#define pDCH  ((DATACACHE*)pThis->pCache)

//BNAD 所在页的偏移地址
#define DATAFILE_BAND_OFFSET           FILE_PAGE_SIZE
#define DATAFILE_DATA_OFFSET           0

//从文件FPM 表中提取物理页面相对索引
#define FPM_GETRINDEX(nFilePage)      PM_GETINDEX(pDCH->FPM[nFilePage])
#define FPM_GETAINDEX(nFilePage)      FILE_NANDPAGE_TOAINDEX(PM_GETINDEX(pDCH->FPM[nFilePage]))

UInt8 drv_DataFile_checkBand(const SFILE* pThis,PAGEBAND* pBand);

UInt16 drv_DataFile_getCRC16(void* pBuff,UInt32 nSize){
    return crc16_ccitt_l(0xF5A3,pBuff,nSize);
}

    

//清除物理页面信息表
void drv_DataFile_resetNPMs(const SFILE* pThis){
    memset(pDCH->NPM,0,FILE_PAGE_NANDCOUNT/4); //每2bits代表一个页面
}

//获取物理页面的状态信息,nNandIndex物理页面索引(相对于DATAFILE_NANDSTART)
UInt8 drv_DataFile_getNPM(const SFILE* pThis,short nNandIndex){
    if( nNandIndex < FILE_PAGE_NANDCOUNT){
        return (pDCH->NPM[nNandIndex/4] >> ((nNandIndex%4)*2)) & 0x03;
    }else{
        char desc[64];
        sprintf(desc,"getNPM(%X)",nNandIndex);
        drv_Exception_throwEx(ERROR_TYPE_FILE,0x01,desc);
        return 0;
    }    
}

void drv_DataFile_setNPM(const SFILE* pThis,short nNandIndex,UInt8 nState){
    if( nNandIndex<FILE_PAGE_NANDCOUNT){
        pDCH->NPM[(nNandIndex)/4] &= ~(0x03<<(((nNandIndex)%4)*2));
        pDCH->NPM[(nNandIndex)/4] |= ((nState&0x03)<<(((nNandIndex)%4)*2));
        
        
        if(drv_DataFile_getNPM(pThis,nNandIndex)==NAND_PM_EMPTY){
            PAGEBAND band;
            UInt8 nmp;
            hal_Flash_readPageN(FILE_NANDPAGE_TOAINDEX(nNandIndex),DATAFILE_BAND_OFFSET,(UInt8*)&band,sizeof(PAGEBAND));
            nmp=drv_DataFile_checkBand(pThis,&band);
            if(nmp!=NAND_PM_EMPTY){
                __nop();
            }
        }
 
                        
    }else{
        char desc[64];
        sprintf(desc,"setNPM(%X)",nNandIndex);
        drv_Exception_throwEx(ERROR_TYPE_FILE,0x02,desc);
    }

}


//清除逻辑页面信息表
void drv_DataFile_resetFPMs(const SFILE* pThis){
    memset(pDCH->FPM,0,FILE_PAGE_COUNT*sizeof(DATAFILE_PAGEMAP));
}
//更新逻辑页面的附加信息(更新到另一个物理页面)
void drv_DataFile_createFPM(const SFILE* pThis,short nPage,short nandIndex){
    pDCH->FPM[nPage]=FPM_CREATE(FILE_PM_VALID,nandIndex);
}

////获取逻辑页面的状态信息
//DATAFILE_PAGEMAP* drv_DataFile_getFPM(const SFILE* pThis,short dataPage){
//#ifdef __DEBUG
//    if( dataPage>=FILE_PAGE_COUNT ){
//        drv_DataFile_throwException(ERROR_PARAM_OVER);
//    }
//#endif
//    return &pDCH->FPM[dataPage];
//}


//标记为坏页
void drv_DataFile_badNandPage(const SFILE* pThis,short pageRIndex){
    PAGEBAND* pBand=(PAGEBAND*)(s_nand_pageBuff+DATAFILE_BAND_OFFSET);

    memset(pBand,0,sizeof(PAGEBAND));
    PBND_SETFLAG(pBand,NAND_PM_BAD);
    hal_Flash_writePageN(FILE_NANDPAGE_TOAINDEX(pageRIndex),s_nand_pageBuff);
    drv_DataFile_setNPM(pThis,pageRIndex,NAND_PM_BAD);
    
    if( (pageRIndex%NAND_SECTOR_PAGECOUNT)==0 ){ 
        //区域的首页损坏,这个区域其它页面一起作废
        UInt16 i;
        for(i=1;i<NAND_SECTOR_PAGECOUNT;i++){
            PBND_SETFLAG(pBand,NAND_PM_BAD);
            drv_DataFile_setNPM(pThis,pageRIndex+i,NAND_PM_BAD);
        }
    }
}



void drv_DataFile_writePage(const SFILE* pThis,short pageRIndex,UInt8* pageBuff){
    short pageAIndex;
#ifdef __DEBUG
    if(pageRIndex%NAND_SECTOR_PAGECOUNT!=0){ 
        //读一下区块首页看看是不是为空
        PAGEBAND band;
        
        hal_Flash_readPageN(FILE_NANDPAGE_TOAINDEX(pageRIndex/NAND_SECTOR_PAGECOUNT*NAND_SECTOR_PAGECOUNT),DATAFILE_BAND_OFFSET,(UInt8*)&band,sizeof(PAGEBAND));
        if( NAND_PM_EMPTY==drv_DataFile_checkBand(pThis,&band) ){
            char desc[64];
            sprintf(desc,"checkBand(%X)",pageRIndex);
            
            //异常
            drv_Exception_throwEx(ERROR_TYPE_FILE,0x13,desc);
            return;
        }
    }
#endif
    
   
    pageAIndex=FILE_NANDPAGE_TOAINDEX(pageRIndex);
    if( (pageAIndex<pThis->nandStartPage) ||  (pageAIndex>=(pThis->nandStartPage+pThis->nandPageCount)) ){
        char desc[64];
        sprintf(desc,"writePage(%X)",pageAIndex);
        drv_Exception_throwEx(ERROR_TYPE_FILE,0x03,desc);
    }
    if(!hal_Flash_writePageN(pageAIndex,pageBuff)){
        //release 版本总是返回true,所以不会执行到这里
        drv_DataFile_badNandPage(pThis,pageRIndex);
        drv_Exception_throwEx(ERROR_TYPE_FILE,0x04,"writePage");
    }
}

//擦除物理Sector,并同步NPM
bool drv_DataFile_eraseNandSector(const SFILE* pThis,short sectorRIndex){
    int i;
    bool bEmpty=true;
    
    hal_Flash_eraseSector(FILE_NANDSECTOR_TOAINDEX(sectorRIndex));
    for(i=0;i<NAND_SECTOR_PAGECOUNT;i++){
        //设置物理页状态表为空
        drv_DataFile_setNPM(pThis,sectorRIndex*NAND_SECTOR_PAGECOUNT+i,NAND_PM_EMPTY);
    }
    return bEmpty;
}



//擦除物理Sector,并同步NPM ,并且清除文件FPM
void drv_DataFile_eraseNandSector2(const SFILE* pThis,short sectorRIndex){
    short filePage;
    
    drv_DataFile_eraseNandSector(pThis,sectorRIndex);
        
    //检查所有FPM,所有在sectorRIndex 区内的全部清除
    for(filePage=0;filePage<FILE_PAGE_COUNT;filePage++){   
        if(PM_GETFLAG(pDCH->FPM[filePage])==FILE_PM_VALID){
            short pageRIndex;
            pageRIndex=FPM_GETRINDEX(filePage);          
            if( (pageRIndex>=(sectorRIndex*NAND_SECTOR_PAGECOUNT)) && (pageRIndex<((sectorRIndex+1)*NAND_SECTOR_PAGECOUNT)) ){
                pDCH->FPM[filePage]=FILE_PM_NONE;
            }
        }
    }
}

//将某个区的数据页移动到另一个空白区(不考虑坏快),这样就可以腾空一个完整的区,然后擦除整个区
//直接读取物理页面的内容，依次存放到另一个物理页面
//返回剩余的第一个空白页面索引
short drv_DataFile_moveSector(const SFILE* pThis,short srcSectorRIndex,short trgSectorRIndex){
    PAGEBAND* pBand;
    UInt32 srcSectorTag;    //读取数据源的区块时间戳
      
    short srcIndex=0;
    short trgIndex=0;
    
    //读取首页srcTagSector
    hal_Flash_readPageN(FILE_NANDPAGE_TOAINDEX(srcSectorRIndex*NAND_SECTOR_PAGECOUNT),DATAFILE_BAND_OFFSET+4,(UInt8* )&srcSectorTag,4);

    while(1){
        UInt8 npm;
        
        //检查是否还有需要拷贝的页面
        while(srcIndex<NAND_SECTOR_PAGECOUNT){
            npm=drv_DataFile_getNPM(pThis,srcSectorRIndex*NAND_SECTOR_PAGECOUNT+srcIndex);
            if(npm==NAND_PM_VALID) break;
            srcIndex++;
        }
        if(srcIndex>=NAND_SECTOR_PAGECOUNT){
            //没有页面需要拷贝了
            break;
        }

        //拷贝数据,这个过程中不能递增TIMETAG
        hal_Flash_readPageN(FILE_NANDPAGE_TOAINDEX(srcSectorRIndex*NAND_SECTOR_PAGECOUNT+srcIndex),0,s_nand_pageBuff,NAND_PAGE_SIZE);
        pBand=(PAGEBAND*)(s_nand_pageBuff+DATAFILE_BAND_OFFSET);
        if(trgIndex==0){
            pBand->sectorTag=srcSectorTag+1; //第0块拷贝过程中需要递增区块号
        }else{
            pBand->sectorTag=0;
        }
        pBand->bandEcc=drv_DataFile_getCRC16(pBand,sizeof(PAGEBAND)-2);
        drv_DataFile_writePage(pThis,trgSectorRIndex*NAND_SECTOR_PAGECOUNT+trgIndex,s_nand_pageBuff);
       
        //drv_DataFile_setNPM(pThis,srcSectorRIndex*NAND_SECTOR_PAGECOUNT+srcIndex,NAND_PM_DUMP);//旧页面无需处理,最后会一次性擦除
        drv_DataFile_setNPM(pThis,trgSectorRIndex*NAND_SECTOR_PAGECOUNT+trgIndex,NAND_PM_VALID);
        PM_SETINDEX(pDCH->FPM[PBND_GETPAGE(pBand)],trgSectorRIndex*NAND_SECTOR_PAGECOUNT+trgIndex);
        
        srcIndex++;
        trgIndex++;
    }
 
    //擦除源区块(FPM 在移动过程中已经被重写,所以这里不再需要处理)
    drv_DataFile_eraseNandSector(pThis,srcSectorRIndex);
    return trgIndex+trgSectorRIndex*NAND_SECTOR_PAGECOUNT;
}

#ifdef __DEBUG
short drv_DataFile_getEmptySectors(const SFILE* pThis){
    short i,sectorCount,emptyCount;

    emptyCount=0;
    sectorCount=pThis->nandPageCount/NAND_SECTOR_PAGECOUNT;
    for(i=0;i<sectorCount;i++){
        if( NAND_PM_EMPTY==drv_DataFile_getNPM(pThis,i*NAND_SECTOR_PAGECOUNT)){            
            //区首页为空就认为整个区域为空
            emptyCount++;
        }
    }
    return emptyCount;
}
#endif

//检查是否有足够的空白页面
short drv_DataFile_checkEmptySectors(const SFILE* pThis,short minEmptyCount){
    short i,sectorCount,sectorStart,sectorRIndex,emptyCount;

    emptyCount=0;
    sectorStart=(short)((UInt16)drv_Time_getTick()% (pThis->nandPageCount/NAND_SECTOR_PAGECOUNT));
    sectorCount=pThis->nandPageCount/NAND_SECTOR_PAGECOUNT;
    for(i=0;i<sectorCount;i++){
        sectorRIndex=(sectorStart+i)%sectorCount;
        if( NAND_PM_EMPTY==drv_DataFile_getNPM(pThis,sectorRIndex*NAND_SECTOR_PAGECOUNT)){            
            //区首页为空就认为整个区域为空
            emptyCount++;
            
            if(emptyCount>minEmptyCount){
                return sectorRIndex;
            }
        }
    }
#ifdef __DEBUG
    {
        if(emptyCount<minEmptyCount){ //移动之后,空白块数量必须充足
            char desc[64];
            sprintf(desc,"checkEmptySectors(%X)",emptyCount);
            drv_Exception_throwEx(ERROR_TYPE_FILE,0x05,desc);
        }
    }
#endif
    
    return SECTOR_INDEX_INVALID;

}


//随机查找一个空白区(所有页面均为空,实际上只要检测首页为空)
short drv_DataFile_findEmptySector(const SFILE* pThis){
    short i,sectorCount,sectorStart,sectorRIndex;

    sectorStart=(short)((UInt16)drv_Time_getTick()% (pThis->nandPageCount/NAND_SECTOR_PAGECOUNT));
    sectorCount=pThis->nandPageCount/NAND_SECTOR_PAGECOUNT;
    for(i=0;i<sectorCount;i++){
        sectorRIndex=(sectorStart+i)%sectorCount;
        if( NAND_PM_EMPTY==drv_DataFile_getNPM(pThis,sectorRIndex*NAND_SECTOR_PAGECOUNT)){
            return sectorRIndex;
        }
    }
    
    return SECTOR_INDEX_INVALID;
}



//计算区内某种类型页面的数量
short drv_DataFile_querySectorPages(const SFILE* pThis,short sectorRIndex,UInt8 npmFlag){
    short i,count;
    
    UInt8 npm;
    count=0;
    for(i=0;i<NAND_SECTOR_PAGECOUNT;i++){
        npm=drv_DataFile_getNPM(pThis,sectorRIndex*NAND_SECTOR_PAGECOUNT+i);
        if(npm==npmFlag){
            count++;
        }
    }
    return count;
}

//随机查找一个可以整理的区,这个区的有效数据页必须小于maxPageCount,并且不是空白区/坏区
short drv_DataFile_findConfuseSector(const SFILE* pThis,short maxPageCount){
    short i,j,sectorStart,sectorRIndex,sectorCount,validPageCount;
    UInt8 npm0;

    sectorCount=pThis->nandPageCount/NAND_SECTOR_PAGECOUNT;
    sectorStart=((UInt16)(drv_Time_getTick()*37)%sectorCount);    
    for(i=0;i<sectorCount;i++){
        sectorRIndex=(sectorStart+i)%sectorCount;
        //检查首页是否为空白区/坏区,是则跳过
        npm0=drv_DataFile_getNPM(pThis,sectorRIndex*NAND_SECTOR_PAGECOUNT);
        if( (npm0==NAND_PM_BAD) || (npm0==NAND_PM_EMPTY) ) continue;
        
        validPageCount=(npm0==NAND_PM_VALID)?1:0;
        for(j=1;j<NAND_SECTOR_PAGECOUNT;j++){
            if(drv_DataFile_getNPM(pThis,sectorRIndex*NAND_SECTOR_PAGECOUNT+j)==NAND_PM_VALID){
                validPageCount++;
            }
        }
        if( validPageCount<maxPageCount ){
            if(validPageCount==0){
                __nop();
            }
            return sectorRIndex;
        }
    }
    
    return SECTOR_INDEX_INVALID;
}


//执行一次页面回收操作,返回空白页面索引
short drv_DataFile_adjustPages(const SFILE* pThis){
    short trgSectorRIndex,srcSectorRIndex,emptyPages;
    
    //随机查找一个空白区,作为移动的目标
    trgSectorRIndex=drv_DataFile_findEmptySector(pThis); 
    
    //可能有坏页，所以先汇总一下空白页的数量，正常情况下就是NAND_SECTOR_PAGECOUNT
    emptyPages=drv_DataFile_querySectorPages(pThis,trgSectorRIndex,NAND_PM_EMPTY); 

    //找到一个回收的数据区
    srcSectorRIndex=drv_DataFile_findConfuseSector(pThis,emptyPages);
    
    if(srcSectorRIndex==SECTOR_INDEX_INVALID){ //文件系统异常,不可能发生
        drv_DataFile_findConfuseSector(pThis,emptyPages);
        drv_Exception_throwEx(ERROR_TYPE_FILE,0x13,"adjustPages");
        return PAGE_INDEX_INVALID;
    }
     
    //移动整理后,返回目标区块第一个空白页面的索引位置    
    return drv_DataFile_moveSector(pThis,srcSectorRIndex,trgSectorRIndex); 
}

//推进pDCH->nandSOF, 如果不是区块的最后一页，则递增
void drv_DataFile_adjustSOF(const SFILE* pThis){
#ifdef __DEBUG    
    if( pDCH->nandSOF==PAGE_INDEX_INVALID){
        drv_Exception_throwEx(ERROR_TYPE_FILE,0x12,"adjustSOF");
    }
#endif
    if(((pDCH->nandSOF+1)%NAND_SECTOR_PAGECOUNT)==0){ //本区块页面耗尽
        pDCH->nandSOF=PAGE_INDEX_INVALID;
    }else{
        pDCH->nandSOF++;
        if( drv_DataFile_getNPM(pThis,pDCH->nandSOF)!=NAND_PM_EMPTY ){ 
            //发现理应空白的页面出现了数据
            char desc[64];
            sprintf(desc,"adjustSOF(%X)",pDCH->nandSOF);
            drv_Exception_throwEx(ERROR_TYPE_FILE,0x12,desc);
        }
    }
}

//将某个逻辑页中的数据替换成新的数据(总是整页一次性替换),pBuffer数据是完整的页数据,包括band区域
//调用前,必须确保pDCH->nandSOF指向一个可用的空白页面
void drv_DataFile_saveFilePageN(const SFILE* pThis,short filePage,void* pBuffer){
    short oldNandIndex,newPageRIndex;
    //获取band信息
    PAGEBAND* pBand=(PAGEBAND*)((UInt8*)pBuffer+DATAFILE_BAND_OFFSET);
    
#ifdef __DEBUG    
    if( pDCH->nandSOF==PAGE_INDEX_INVALID){
        char desc[64];
        sprintf(desc,"saveFilePageN(%X)",filePage);
        drv_Exception_throwEx(ERROR_TYPE_FILE,0x06,desc);
    }
#endif
    
    //保存原物理页面索引
    if(PM_GETFLAG(pDCH->FPM[filePage])!=FILE_PM_VALID){
        oldNandIndex=PAGE_INDEX_INVALID;    //filePage 还没有实际的物理存储页面
    }else{
        oldNandIndex=FPM_GETRINDEX(filePage);
    }

    //获取新页面索引,并推进nandSOF
    newPageRIndex=pDCH->nandSOF;
    drv_DataFile_adjustSOF(pThis);
    
    //修改FPM表
    drv_DataFile_createFPM(pThis,filePage,newPageRIndex); //逻辑页指向新物理页
    
    //修改BAND
    memset(pBand,0,sizeof(PAGEBAND));//单纯的保存数据,sectorTag不起作用,清0
    PBND_SETFLAG(pBand,NAND_PM_VALID);      //物理页面有效标志
    PBND_SETPAGE(pBand,filePage);         //设置逻辑页码
    pDCH->maxTimeTag++;
    PBND_SETTIMETAG(pBand,pDCH->maxTimeTag);//设置BNAD中的TimeTag
    pBand->bandEcc=drv_DataFile_getCRC16(pBand,sizeof(PAGEBAND)-2);  
    //编程到新物理页
    drv_DataFile_writePage(pThis,newPageRIndex,pBuffer);
    //更新NPM,修改旧物理页NMP
    if(oldNandIndex!=PAGE_INDEX_INVALID){
        //作废旧物理页面(内存中作废即可)
        drv_DataFile_setNPM(pThis,oldNandIndex,NAND_PM_DUMP);
    }
    //修改新物理页面NMP
    drv_DataFile_setNPM(pThis,newPageRIndex,NAND_PM_VALID);
    
    
#ifdef __DEBUG
    {
        short emptyCount=drv_DataFile_getEmptySectors(pThis);
        if(emptyCount<EMPTY_SECTORCOUNT_MIN){ //移动之后,空白块数量必须充足
            char desc[64];
            sprintf(desc,"saveFilePageN(emptyCount=%X)",emptyCount);
            drv_Exception_throwEx(ERROR_TYPE_FILE,0x07,desc);
        }
    }
#endif
}


//////////////////////////////////////////////////////////////////////////////////////////////////
void drv_DataFile_reset(const SFILE* pThis){
    pDCH->nandSOF=PAGE_INDEX_INVALID;
    drv_DataFile_resetNPMs(pThis);
    drv_DataFile_resetFPMs(pThis);
}

bool bytesEqual(UInt8* buff,int count,UInt8 value){
    int i;
    for(i=0;i<count;i++){
        if( buff[i]!=value) return false; 
    }
    return true;
}
UInt8 drv_DataFile_checkBand(const SFILE* pThis,PAGEBAND* pBand){
    //注意: BAND信息中有CRC区域,空白页的CRC=0,并不满足校验条件
    if( bytesEqual((UInt8*)pBand,sizeof(PAGEBAND),PAGEVALUE_EMPTY) ){
        return NAND_PM_EMPTY;
    }
    //核对BAND CRC
    if( pBand->bandEcc!=drv_DataFile_getCRC16(pBand,sizeof(PAGEBAND)-2) ){
        return NAND_PM_BAD;
    }
    if(  PBND_GETPAGE(pBand) >= FILE_PAGE_COUNT ){ //数据异常
        return NAND_PM_BAD;         
    }
    return PBND_GETFLAG(pBand);
}

UInt8 drv_DataFile_scanPage(const SFILE* pThis,short pageRIndex){   
    UInt8 nmp;
    PAGEBAND band;
    hal_Flash_readPageN( FILE_NANDPAGE_TOAINDEX(pageRIndex),DATAFILE_BAND_OFFSET,(UInt8*)&band,sizeof(PAGEBAND));
    nmp=drv_DataFile_checkBand(pThis,&band);
    if(nmp ==NAND_PM_DUMP){
        drv_DataFile_setNPM(pThis,pageRIndex,NAND_PM_DUMP); //非空无效页
    }else if(nmp ==NAND_PM_EMPTY){
        drv_DataFile_setNPM(pThis,pageRIndex,NAND_PM_EMPTY); //空白页
    }else if(nmp ==NAND_PM_VALID){
        if( pDCH->maxTimeTag < BND_GETTIMETAG(band) ){
            pDCH->maxTimeTag=BND_GETTIMETAG(band);
        }
        
        //检查重复页面,数据页信息是否已经存在,如果是回收过期页面
        if( PM_GETFLAG(pDCH->FPM[BND_GETPAGE(band)])== FILE_PM_VALID ){ //逻辑页已存在,需要判断那个页面最新
            //重读一次之前的页面BAND
            PAGEBAND oldband; 
            hal_Flash_readPageN(FILE_NANDPAGE_TOAINDEX(PM_GETINDEX(pDCH->FPM[BND_GETPAGE(band)])),DATAFILE_BAND_OFFSET,(UInt8*)&oldband,sizeof(PAGEBAND));
            if(BND_GETTIMETAG(band)>BND_GETTIMETAG(oldband)){
                //当前物理页面是最新的
                drv_DataFile_setNPM(pThis,pageRIndex,NAND_PM_VALID);
                //修改之前物理页面的NPM信息
                drv_DataFile_setNPM(pThis,PM_GETINDEX(pDCH->FPM[BND_GETPAGE(oldband)]),NAND_PM_DUMP);
            }else if(BND_GETTIMETAG(band)==BND_GETTIMETAG(oldband)){     
                //出现相同TIMETAG的页面,一定是移动数据区的过程中出现了问题,需要撤销移动
                short oldPageRIndex=PM_GETINDEX(pDCH->FPM[BND_GETPAGE(oldband)]); //提取之前的BAND对应的物理页面
                if(oldband.sectorTag > band.sectorTag){                  //擦除sectorTag较大的那个页
                    //旧区块无效
                    if((oldPageRIndex%NAND_SECTOR_PAGECOUNT)==0){
                        //新页面有效
                        drv_DataFile_setNPM(pThis,pageRIndex,NAND_PM_VALID);
                        //擦除所有oldPageRIndex同区页面
                        drv_DataFile_eraseNandSector2(pThis,oldPageRIndex/NAND_SECTOR_PAGECOUNT);
                        
                        //重要: pDCH->nandSOF一定处在oldSector中,所以这里需要重新置为无效
                        pDCH->nandSOF=PAGE_INDEX_INVALID;
                    }else{
                        //异常处理
                        drv_DataFile_setNPM(pThis,pageRIndex,NAND_PM_BAD); //非空，无效页
                        return NAND_PM_BAD;
                    }
                }else{
                    //新区块无效
                    if((pageRIndex%NAND_SECTOR_PAGECOUNT)==0){
                        //直接擦除当前区块(不需要清理逻辑页表)
                        drv_DataFile_eraseNandSector(pThis,pageRIndex/NAND_SECTOR_PAGECOUNT);
                        
                        //擦除后立刻返回,以免后面页表被错误设置
                        return NAND_PM_EMPTY; 
                    }else{
                        //异常处理
                        drv_DataFile_setNPM(pThis,pageRIndex,NAND_PM_BAD); //非空，无效页
                        return NAND_PM_BAD;
                    }
                }
            }else{         
                //当前物理页面是过期的
                drv_DataFile_setNPM(pThis,pageRIndex,NAND_PM_DUMP); //非空,无效页
                return NAND_PM_DUMP;
            }
        }else{ //发现新的逻辑页面
            drv_DataFile_setNPM(pThis,pageRIndex,NAND_PM_VALID);   //标记物理页有效
        }
        //登记FPM
        PM_SETFLAG(pDCH->FPM[BND_GETPAGE(band)],FILE_PM_VALID);//逻辑页面有效
        PM_SETINDEX(pDCH->FPM[BND_GETPAGE(band)],pageRIndex);
    }else{
        char desc[64];
        sprintf(desc,"scanPage(bad page=%X)",pageRIndex);
        drv_Exception_throwEx(ERROR_TYPE_FILE,0x07,desc);            
//        drv_DataFile_setNPM(pThis,pageRIndex,NAND_PM_BAD); //异常页面
//        return NAND_PM_BAD;
    }
    
    return nmp;
}

#define SECTOR_STATE_EMPTY  0x00
#define SECTOR_STATE_VALID  0x01  //存在有效页面
#define SECTOR_STATE_DUMP   0x02  //可以擦除
#define SECTOR_STATE_EXCEPTION   0x03  //异常

UInt8 drv_DataFile_scanSector(const SFILE* pThis,short sectorRIndex){
    short i;
    UInt8 nmp0,nmp;
    int validPages=0;
    
    nmp0=drv_DataFile_scanPage(pThis,sectorRIndex*NAND_SECTOR_PAGECOUNT);    
    if(nmp0==NAND_PM_BAD) return SECTOR_STATE_EXCEPTION;     //首页损坏,整个区块被认为是无效的,需要擦除
    if(nmp0==NAND_PM_VALID){
        validPages++;
    }
    for(i=1;i<NAND_SECTOR_PAGECOUNT;i++){
        short pageRIndex=sectorRIndex*NAND_SECTOR_PAGECOUNT+i;
        if(nmp0==NAND_PM_EMPTY){  //检查是否有非空页面，如果要求，返回SECTOR_STATE_DUMP, 要求擦除
            PAGEBAND band;
            hal_Flash_readPageN( FILE_NANDPAGE_TOAINDEX(pageRIndex),DATAFILE_BAND_OFFSET,(UInt8*)&band,sizeof(PAGEBAND));
            
            nmp=drv_DataFile_checkBand(pThis,&band);         
            //首页为空,其余页也必须为空,否则就被认为是坏块
            if(nmp!=NAND_PM_EMPTY){
                //异常处理,强制将首页设置成无效页,作废该区块,正常情况下不应该发生
                //nmp0=NAND_PM_DUMP;
                //validPages=0;
                //drv_DataFile_setNPM(pThis,sectorRIndex*NAND_SECTOR_PAGECOUNT,NAND_PM_DUMP); 
                return SECTOR_STATE_EXCEPTION;
            }
            drv_DataFile_setNPM(pThis,pageRIndex,NAND_PM_EMPTY);
        }else{
            nmp=drv_DataFile_scanPage(pThis,pageRIndex);
            if(nmp==NAND_PM_VALID){
                validPages++;
            }
            //将nandSOF定位于第一个空白页
            if( (pDCH->nandSOF==PAGE_INDEX_INVALID) && (nmp==NAND_PM_EMPTY) ){
                pDCH->nandSOF=pageRIndex;
            }
        }
    }
    
    if(nmp0==NAND_PM_EMPTY) return SECTOR_STATE_EMPTY;     //首页为空,区块就是空
    
    if(validPages==0){   
        return SECTOR_STATE_DUMP;
    }else{
        return SECTOR_STATE_VALID;
    }
}


//fix@2018-12-14 ,检查内存物理页表,看是否有区块可以擦除
bool drv_DataFile_checkDumpSector_(const SFILE* pThis,short sectorRIndex){
    short i;
    UInt8 nmp0,nmp;
    short validPages=0;
    nmp0=drv_DataFile_getNPM(pThis,sectorRIndex*NAND_SECTOR_PAGECOUNT);
    if(nmp0==NAND_PM_EMPTY) return false; //空白页面,不需要擦除
    if(nmp0==NAND_PM_VALID) validPages++;
    for(i=1;i<NAND_SECTOR_PAGECOUNT;i++){
        short pageRIndex=sectorRIndex*NAND_SECTOR_PAGECOUNT+i;
        nmp=drv_DataFile_getNPM(pThis,pageRIndex);
        if(nmp==NAND_PM_VALID) validPages++;
    }
    return validPages==0;
}

void drv_DataFile_scanAll(const SFILE* pThis){
    short sectorRIndex;

    drv_DataFile_reset(pThis);
    //扫描所有物理页面的BAND区,一次扫描一个区(同时擦除移动过程中被中断的目标数据区(即撤销移动))
    for(sectorRIndex=0;sectorRIndex<(FILE_PAGE_NANDCOUNT/NAND_SECTOR_PAGECOUNT);sectorRIndex++){
        UInt8 state=drv_DataFile_scanSector(pThis,sectorRIndex);
        if(state==SECTOR_STATE_EXCEPTION){ //fix@ 先执行擦除一次异常的页面
            drv_DataFile_eraseNandSector(pThis,sectorRIndex);
        }
    }

    
    //扫描完成后，再全部检查一次，看是否有可擦除的区块
    for(sectorRIndex=0;sectorRIndex<(FILE_PAGE_NANDCOUNT/NAND_SECTOR_PAGECOUNT);sectorRIndex++){
        if(drv_DataFile_checkDumpSector_(pThis,sectorRIndex)){
            drv_DataFile_eraseNandSector(pThis,sectorRIndex);
        }
    }
    

    
    //检查空白区是否正常
#ifdef __DEBUG
    drv_DataFile_checkEmptySectors(pThis,EMPTY_SECTORCOUNT_MIN);
#endif
}

/////////////////////DataFile的功能函数////////////////////////////////////////////////////////////
void drv_DataFile_init(const SFILE* pThis){
    pDCH->nandSOF=PAGE_INDEX_INVALID;
    pDCH->maxTimeTag=0;
}

void drv_DataFile_openN(const SFILE* pThis){
    drv_DataFile_scanAll(pThis);
}

//读数据
void drv_DataFile_readN(const SFILE* pThis,UInt16 nPage,UInt16 nAddr,void* pData,UInt16 nLen ){
    short cashIndex;
    cashIndex=hal_Cache_find(pThis->pCache,nPage);
    if(cashIndex<0){
        if(PM_GETFLAG(pDCH->FPM[nPage])!=FILE_PM_VALID){
            memset(pData,PAGEVALUE_EMPTY,nLen);
            return;
        }else{
            //内存无缓存,直接读取物理页面
            hal_Flash_readPageN(FPM_GETAINDEX(nPage),DATAFILE_DATA_OFFSET+nAddr,pData,nLen);
        }
    }else{
        hal_Cache_read(pThis->pCache,DATAFILE_DATA_OFFSET+nAddr,pData,nLen);
    }
}
//把完整物理页数据加载到缓冲区，如果缓冲区满则执行刷新操作并释放一个缓冲区
void drv_DataFile_loadN(const SFILE* pThis,UInt16 nPage){
    //nPage尚未缓冲,请求一个可用的缓冲索引

    if(hal_Cache_find(pThis->pCache,CACHEPAGE_NONE)<0){ 
        //没有空闲区,则先将当前cache中的数据保存起来
        drv_DataFile_flushN(pThis);
    }
        
    //将物理数据读入缓冲
    if(PM_GETFLAG(pDCH->FPM[nPage])!=FILE_PM_VALID){
        //逻辑页面不存在时,用0填充
        memset(pThis->pCache->buffer,0,NAND_PAGE_SIZE);
    }else{
        hal_Flash_readPageN(FPM_GETAINDEX(nPage),0,pThis->pCache->buffer,NAND_PAGE_SIZE); 
    }
 
    pThis->pCache->page=nPage;
    pThis->pCache->dirty=false;

}
//写数据,数据总是先写入缓冲区,如果缓冲区满则执行刷新动作.
void drv_DataFile_writeN(const SFILE* pThis,UInt16 nPage,UInt16 nAddr,const void* pData,UInt16 nLen ){
    if( hal_Cache_find(pThis->pCache,nPage) <0 ){
        drv_DataFile_loadN(pThis,nPage);
    }
    hal_Cache_write(pThis->pCache,DATAFILE_DATA_OFFSET+nAddr,pData,nLen);
}

//将更改过的逻辑页面刷新到物理页
void drv_DataFile_flushN(const SFILE* pThis){
    if(pThis->pCache->dirty){
        
        if( (pThis->pCache->page<0) ){
            char desc[64];
            sprintf(desc,"flushN1(%X)",pThis->pCache->page);
            drv_Exception_throwEx(ERROR_TYPE_FILE,0x10,desc);
        }
        
        if( pDCH->nandSOF==PAGE_INDEX_INVALID){ //当前区的空白页面耗尽
            short emptySectorRIndex=drv_DataFile_checkEmptySectors(pThis,EMPTY_SECTORCOUNT_MIN);
            //检查是否有足够多的空白区
            if( emptySectorRIndex==SECTOR_INDEX_INVALID ){
                //空白区不足,执行一次页面回收操作
                pDCH->nandSOF=drv_DataFile_adjustPages(pThis);
            }else{
                pDCH->nandSOF=emptySectorRIndex*NAND_SECTOR_PAGECOUNT;
            }
        }

        if(pDCH->nandSOF==PAGE_INDEX_INVALID){
            drv_Exception_throwEx(ERROR_TYPE_FILE,0x11,"flushN2");
        }else{
            //保存页面内容
            drv_DataFile_saveFilePageN(pThis,pThis->pCache->page,pThis->pCache->buffer);
            pThis->pCache->dirty=false;
        }
    }
    
}

void drv_DataFile_close(const SFILE* pThis){
    pThis->flush(pThis);
}


//文件格式化,为加快速度,用的是块擦除
void drv_DataFile_format(const SFILE* pThis,FILEFORMAT_CALLBACK pFunc){
    short i;
    
    //开始擦除
    if(pFunc) pFunc(pThis,FILE_FORMATFLAG_BEFORE,-1);
    for(i=0; i< FILE_SECTOR_NANDCOUNT;i++){
        hal_IDOG_flush(); //格式化时间较长,需要刷狗
        if(pFunc) pFunc(pThis,FILE_FORMATFLAG_BEFORE,i);
        drv_DataFile_eraseNandSector(pThis,i); //DataFile中允许eraseBlock过程中出现坏页
        if(pFunc) pFunc(pThis,FILE_FORMATFLAG_AFTER,i);
    }
    if(pFunc) pFunc(pThis,FILE_FORMATFLAG_AFTER,-1);
    
    pThis->open(pThis);
}
