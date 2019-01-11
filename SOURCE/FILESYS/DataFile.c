#include "includes.h"
#include "halCPU.h"
#include "DataFile.h"
#include "cacheFile.h"

//�����淶: 
//filePage:   �ļ��е�ҳ��
//pageRIndex: ����ҳ���߼�����
//pageAIndex: ����ҳ���������(���ڵ���hal_Flash_*** ����ʱʹ��)
//sectorRIndex: ����ҳ���߼�����
//sectorAIndex: ����ҳ���������(���ڵ���hal_Flash_*** ����ʱʹ��)
static UInt8 s_nand_pageBuff[NAND_PAGE_SIZE];       //һ����ʱ�õ�����ҳ�滺����

#define pDCH  ((DATACACHE*)pThis->pCache)

//BNAD ����ҳ��ƫ�Ƶ�ַ
#define DATAFILE_BAND_OFFSET           FILE_PAGE_SIZE
#define DATAFILE_DATA_OFFSET           0

//���ļ�FPM ������ȡ����ҳ���������
#define FPM_GETRINDEX(nFilePage)      PM_GETINDEX(pDCH->FPM[nFilePage])
#define FPM_GETAINDEX(nFilePage)      FILE_NANDPAGE_TOAINDEX(PM_GETINDEX(pDCH->FPM[nFilePage]))

UInt8 drv_DataFile_checkBand(const SFILE* pThis,PAGEBAND* pBand);

UInt16 drv_DataFile_getCRC16(void* pBuff,UInt32 nSize){
    return crc16_ccitt_l(0xF5A3,pBuff,nSize);
}

    

//�������ҳ����Ϣ��
void drv_DataFile_resetNPMs(const SFILE* pThis){
    memset(pDCH->NPM,0,FILE_PAGE_NANDCOUNT/4); //ÿ2bits����һ��ҳ��
}

//��ȡ����ҳ���״̬��Ϣ,nNandIndex����ҳ������(�����DATAFILE_NANDSTART)
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


//����߼�ҳ����Ϣ��
void drv_DataFile_resetFPMs(const SFILE* pThis){
    memset(pDCH->FPM,0,FILE_PAGE_COUNT*sizeof(DATAFILE_PAGEMAP));
}
//�����߼�ҳ��ĸ�����Ϣ(���µ���һ������ҳ��)
void drv_DataFile_createFPM(const SFILE* pThis,short nPage,short nandIndex){
    pDCH->FPM[nPage]=FPM_CREATE(FILE_PM_VALID,nandIndex);
}

////��ȡ�߼�ҳ���״̬��Ϣ
//DATAFILE_PAGEMAP* drv_DataFile_getFPM(const SFILE* pThis,short dataPage){
//#ifdef __DEBUG
//    if( dataPage>=FILE_PAGE_COUNT ){
//        drv_DataFile_throwException(ERROR_PARAM_OVER);
//    }
//#endif
//    return &pDCH->FPM[dataPage];
//}


//���Ϊ��ҳ
void drv_DataFile_badNandPage(const SFILE* pThis,short pageRIndex){
    PAGEBAND* pBand=(PAGEBAND*)(s_nand_pageBuff+DATAFILE_BAND_OFFSET);

    memset(pBand,0,sizeof(PAGEBAND));
    PBND_SETFLAG(pBand,NAND_PM_BAD);
    hal_Flash_writePageN(FILE_NANDPAGE_TOAINDEX(pageRIndex),s_nand_pageBuff);
    drv_DataFile_setNPM(pThis,pageRIndex,NAND_PM_BAD);
    
    if( (pageRIndex%NAND_SECTOR_PAGECOUNT)==0 ){ 
        //�������ҳ��,�����������ҳ��һ������
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
        //��һ��������ҳ�����ǲ���Ϊ��
        PAGEBAND band;
        
        hal_Flash_readPageN(FILE_NANDPAGE_TOAINDEX(pageRIndex/NAND_SECTOR_PAGECOUNT*NAND_SECTOR_PAGECOUNT),DATAFILE_BAND_OFFSET,(UInt8*)&band,sizeof(PAGEBAND));
        if( NAND_PM_EMPTY==drv_DataFile_checkBand(pThis,&band) ){
            char desc[64];
            sprintf(desc,"checkBand(%X)",pageRIndex);
            
            //�쳣
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
        //release �汾���Ƿ���true,���Բ���ִ�е�����
        drv_DataFile_badNandPage(pThis,pageRIndex);
        drv_Exception_throwEx(ERROR_TYPE_FILE,0x04,"writePage");
    }
}

//��������Sector,��ͬ��NPM
bool drv_DataFile_eraseNandSector(const SFILE* pThis,short sectorRIndex){
    int i;
    bool bEmpty=true;
    
    hal_Flash_eraseSector(FILE_NANDSECTOR_TOAINDEX(sectorRIndex));
    for(i=0;i<NAND_SECTOR_PAGECOUNT;i++){
        //��������ҳ״̬��Ϊ��
        drv_DataFile_setNPM(pThis,sectorRIndex*NAND_SECTOR_PAGECOUNT+i,NAND_PM_EMPTY);
    }
    return bEmpty;
}



//��������Sector,��ͬ��NPM ,��������ļ�FPM
void drv_DataFile_eraseNandSector2(const SFILE* pThis,short sectorRIndex){
    short filePage;
    
    drv_DataFile_eraseNandSector(pThis,sectorRIndex);
        
    //�������FPM,������sectorRIndex ���ڵ�ȫ�����
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

//��ĳ����������ҳ�ƶ�����һ���հ���(�����ǻ���),�����Ϳ����ڿ�һ����������,Ȼ�����������
//ֱ�Ӷ�ȡ����ҳ������ݣ����δ�ŵ���һ������ҳ��
//����ʣ��ĵ�һ���հ�ҳ������
short drv_DataFile_moveSector(const SFILE* pThis,short srcSectorRIndex,short trgSectorRIndex){
    PAGEBAND* pBand;
    UInt32 srcSectorTag;    //��ȡ����Դ������ʱ���
      
    short srcIndex=0;
    short trgIndex=0;
    
    //��ȡ��ҳsrcTagSector
    hal_Flash_readPageN(FILE_NANDPAGE_TOAINDEX(srcSectorRIndex*NAND_SECTOR_PAGECOUNT),DATAFILE_BAND_OFFSET+4,(UInt8* )&srcSectorTag,4);

    while(1){
        UInt8 npm;
        
        //����Ƿ�����Ҫ������ҳ��
        while(srcIndex<NAND_SECTOR_PAGECOUNT){
            npm=drv_DataFile_getNPM(pThis,srcSectorRIndex*NAND_SECTOR_PAGECOUNT+srcIndex);
            if(npm==NAND_PM_VALID) break;
            srcIndex++;
        }
        if(srcIndex>=NAND_SECTOR_PAGECOUNT){
            //û��ҳ����Ҫ������
            break;
        }

        //��������,��������в��ܵ���TIMETAG
        hal_Flash_readPageN(FILE_NANDPAGE_TOAINDEX(srcSectorRIndex*NAND_SECTOR_PAGECOUNT+srcIndex),0,s_nand_pageBuff,NAND_PAGE_SIZE);
        pBand=(PAGEBAND*)(s_nand_pageBuff+DATAFILE_BAND_OFFSET);
        if(trgIndex==0){
            pBand->sectorTag=srcSectorTag+1; //��0�鿽����������Ҫ���������
        }else{
            pBand->sectorTag=0;
        }
        pBand->bandEcc=drv_DataFile_getCRC16(pBand,sizeof(PAGEBAND)-2);
        drv_DataFile_writePage(pThis,trgSectorRIndex*NAND_SECTOR_PAGECOUNT+trgIndex,s_nand_pageBuff);
       
        //drv_DataFile_setNPM(pThis,srcSectorRIndex*NAND_SECTOR_PAGECOUNT+srcIndex,NAND_PM_DUMP);//��ҳ�����账��,����һ���Բ���
        drv_DataFile_setNPM(pThis,trgSectorRIndex*NAND_SECTOR_PAGECOUNT+trgIndex,NAND_PM_VALID);
        PM_SETINDEX(pDCH->FPM[PBND_GETPAGE(pBand)],trgSectorRIndex*NAND_SECTOR_PAGECOUNT+trgIndex);
        
        srcIndex++;
        trgIndex++;
    }
 
    //����Դ����(FPM ���ƶ��������Ѿ�����д,�������ﲻ����Ҫ����)
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
            //����ҳΪ�վ���Ϊ��������Ϊ��
            emptyCount++;
        }
    }
    return emptyCount;
}
#endif

//����Ƿ����㹻�Ŀհ�ҳ��
short drv_DataFile_checkEmptySectors(const SFILE* pThis,short minEmptyCount){
    short i,sectorCount,sectorStart,sectorRIndex,emptyCount;

    emptyCount=0;
    sectorStart=(short)((UInt16)drv_Time_getTick()% (pThis->nandPageCount/NAND_SECTOR_PAGECOUNT));
    sectorCount=pThis->nandPageCount/NAND_SECTOR_PAGECOUNT;
    for(i=0;i<sectorCount;i++){
        sectorRIndex=(sectorStart+i)%sectorCount;
        if( NAND_PM_EMPTY==drv_DataFile_getNPM(pThis,sectorRIndex*NAND_SECTOR_PAGECOUNT)){            
            //����ҳΪ�վ���Ϊ��������Ϊ��
            emptyCount++;
            
            if(emptyCount>minEmptyCount){
                return sectorRIndex;
            }
        }
    }
#ifdef __DEBUG
    {
        if(emptyCount<minEmptyCount){ //�ƶ�֮��,�հ׿������������
            char desc[64];
            sprintf(desc,"checkEmptySectors(%X)",emptyCount);
            drv_Exception_throwEx(ERROR_TYPE_FILE,0x05,desc);
        }
    }
#endif
    
    return SECTOR_INDEX_INVALID;

}


//�������һ���հ���(����ҳ���Ϊ��,ʵ����ֻҪ�����ҳΪ��)
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



//��������ĳ������ҳ�������
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

//�������һ�������������,���������Ч����ҳ����С��maxPageCount,���Ҳ��ǿհ���/����
short drv_DataFile_findConfuseSector(const SFILE* pThis,short maxPageCount){
    short i,j,sectorStart,sectorRIndex,sectorCount,validPageCount;
    UInt8 npm0;

    sectorCount=pThis->nandPageCount/NAND_SECTOR_PAGECOUNT;
    sectorStart=((UInt16)(drv_Time_getTick()*37)%sectorCount);    
    for(i=0;i<sectorCount;i++){
        sectorRIndex=(sectorStart+i)%sectorCount;
        //�����ҳ�Ƿ�Ϊ�հ���/����,��������
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


//ִ��һ��ҳ����ղ���,���ؿհ�ҳ������
short drv_DataFile_adjustPages(const SFILE* pThis){
    short trgSectorRIndex,srcSectorRIndex,emptyPages;
    
    //�������һ���հ���,��Ϊ�ƶ���Ŀ��
    trgSectorRIndex=drv_DataFile_findEmptySector(pThis); 
    
    //�����л�ҳ�������Ȼ���һ�¿հ�ҳ����������������¾���NAND_SECTOR_PAGECOUNT
    emptyPages=drv_DataFile_querySectorPages(pThis,trgSectorRIndex,NAND_PM_EMPTY); 

    //�ҵ�һ�����յ�������
    srcSectorRIndex=drv_DataFile_findConfuseSector(pThis,emptyPages);
    
    if(srcSectorRIndex==SECTOR_INDEX_INVALID){ //�ļ�ϵͳ�쳣,�����ܷ���
        drv_DataFile_findConfuseSector(pThis,emptyPages);
        drv_Exception_throwEx(ERROR_TYPE_FILE,0x13,"adjustPages");
        return PAGE_INDEX_INVALID;
    }
     
    //�ƶ������,����Ŀ�������һ���հ�ҳ�������λ��    
    return drv_DataFile_moveSector(pThis,srcSectorRIndex,trgSectorRIndex); 
}

//�ƽ�pDCH->nandSOF, ���������������һҳ�������
void drv_DataFile_adjustSOF(const SFILE* pThis){
#ifdef __DEBUG    
    if( pDCH->nandSOF==PAGE_INDEX_INVALID){
        drv_Exception_throwEx(ERROR_TYPE_FILE,0x12,"adjustSOF");
    }
#endif
    if(((pDCH->nandSOF+1)%NAND_SECTOR_PAGECOUNT)==0){ //������ҳ��ľ�
        pDCH->nandSOF=PAGE_INDEX_INVALID;
    }else{
        pDCH->nandSOF++;
        if( drv_DataFile_getNPM(pThis,pDCH->nandSOF)!=NAND_PM_EMPTY ){ 
            //������Ӧ�հ׵�ҳ�����������
            char desc[64];
            sprintf(desc,"adjustSOF(%X)",pDCH->nandSOF);
            drv_Exception_throwEx(ERROR_TYPE_FILE,0x12,desc);
        }
    }
}

//��ĳ���߼�ҳ�е������滻���µ�����(������ҳһ�����滻),pBuffer������������ҳ����,����band����
//����ǰ,����ȷ��pDCH->nandSOFָ��һ�����õĿհ�ҳ��
void drv_DataFile_saveFilePageN(const SFILE* pThis,short filePage,void* pBuffer){
    short oldNandIndex,newPageRIndex;
    //��ȡband��Ϣ
    PAGEBAND* pBand=(PAGEBAND*)((UInt8*)pBuffer+DATAFILE_BAND_OFFSET);
    
#ifdef __DEBUG    
    if( pDCH->nandSOF==PAGE_INDEX_INVALID){
        char desc[64];
        sprintf(desc,"saveFilePageN(%X)",filePage);
        drv_Exception_throwEx(ERROR_TYPE_FILE,0x06,desc);
    }
#endif
    
    //����ԭ����ҳ������
    if(PM_GETFLAG(pDCH->FPM[filePage])!=FILE_PM_VALID){
        oldNandIndex=PAGE_INDEX_INVALID;    //filePage ��û��ʵ�ʵ�����洢ҳ��
    }else{
        oldNandIndex=FPM_GETRINDEX(filePage);
    }

    //��ȡ��ҳ������,���ƽ�nandSOF
    newPageRIndex=pDCH->nandSOF;
    drv_DataFile_adjustSOF(pThis);
    
    //�޸�FPM��
    drv_DataFile_createFPM(pThis,filePage,newPageRIndex); //�߼�ҳָ��������ҳ
    
    //�޸�BAND
    memset(pBand,0,sizeof(PAGEBAND));//�����ı�������,sectorTag��������,��0
    PBND_SETFLAG(pBand,NAND_PM_VALID);      //����ҳ����Ч��־
    PBND_SETPAGE(pBand,filePage);         //�����߼�ҳ��
    pDCH->maxTimeTag++;
    PBND_SETTIMETAG(pBand,pDCH->maxTimeTag);//����BNAD�е�TimeTag
    pBand->bandEcc=drv_DataFile_getCRC16(pBand,sizeof(PAGEBAND)-2);  
    //��̵�������ҳ
    drv_DataFile_writePage(pThis,newPageRIndex,pBuffer);
    //����NPM,�޸ľ�����ҳNMP
    if(oldNandIndex!=PAGE_INDEX_INVALID){
        //���Ͼ�����ҳ��(�ڴ������ϼ���)
        drv_DataFile_setNPM(pThis,oldNandIndex,NAND_PM_DUMP);
    }
    //�޸�������ҳ��NMP
    drv_DataFile_setNPM(pThis,newPageRIndex,NAND_PM_VALID);
    
    
#ifdef __DEBUG
    {
        short emptyCount=drv_DataFile_getEmptySectors(pThis);
        if(emptyCount<EMPTY_SECTORCOUNT_MIN){ //�ƶ�֮��,�հ׿������������
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
    //ע��: BAND��Ϣ����CRC����,�հ�ҳ��CRC=0,��������У������
    if( bytesEqual((UInt8*)pBand,sizeof(PAGEBAND),PAGEVALUE_EMPTY) ){
        return NAND_PM_EMPTY;
    }
    //�˶�BAND CRC
    if( pBand->bandEcc!=drv_DataFile_getCRC16(pBand,sizeof(PAGEBAND)-2) ){
        return NAND_PM_BAD;
    }
    if(  PBND_GETPAGE(pBand) >= FILE_PAGE_COUNT ){ //�����쳣
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
        drv_DataFile_setNPM(pThis,pageRIndex,NAND_PM_DUMP); //�ǿ���Чҳ
    }else if(nmp ==NAND_PM_EMPTY){
        drv_DataFile_setNPM(pThis,pageRIndex,NAND_PM_EMPTY); //�հ�ҳ
    }else if(nmp ==NAND_PM_VALID){
        if( pDCH->maxTimeTag < BND_GETTIMETAG(band) ){
            pDCH->maxTimeTag=BND_GETTIMETAG(band);
        }
        
        //����ظ�ҳ��,����ҳ��Ϣ�Ƿ��Ѿ�����,����ǻ��չ���ҳ��
        if( PM_GETFLAG(pDCH->FPM[BND_GETPAGE(band)])== FILE_PM_VALID ){ //�߼�ҳ�Ѵ���,��Ҫ�ж��Ǹ�ҳ������
            //�ض�һ��֮ǰ��ҳ��BAND
            PAGEBAND oldband; 
            hal_Flash_readPageN(FILE_NANDPAGE_TOAINDEX(PM_GETINDEX(pDCH->FPM[BND_GETPAGE(band)])),DATAFILE_BAND_OFFSET,(UInt8*)&oldband,sizeof(PAGEBAND));
            if(BND_GETTIMETAG(band)>BND_GETTIMETAG(oldband)){
                //��ǰ����ҳ�������µ�
                drv_DataFile_setNPM(pThis,pageRIndex,NAND_PM_VALID);
                //�޸�֮ǰ����ҳ���NPM��Ϣ
                drv_DataFile_setNPM(pThis,PM_GETINDEX(pDCH->FPM[BND_GETPAGE(oldband)]),NAND_PM_DUMP);
            }else if(BND_GETTIMETAG(band)==BND_GETTIMETAG(oldband)){     
                //������ͬTIMETAG��ҳ��,һ�����ƶ��������Ĺ����г���������,��Ҫ�����ƶ�
                short oldPageRIndex=PM_GETINDEX(pDCH->FPM[BND_GETPAGE(oldband)]); //��ȡ֮ǰ��BAND��Ӧ������ҳ��
                if(oldband.sectorTag > band.sectorTag){                  //����sectorTag�ϴ���Ǹ�ҳ
                    //��������Ч
                    if((oldPageRIndex%NAND_SECTOR_PAGECOUNT)==0){
                        //��ҳ����Ч
                        drv_DataFile_setNPM(pThis,pageRIndex,NAND_PM_VALID);
                        //��������oldPageRIndexͬ��ҳ��
                        drv_DataFile_eraseNandSector2(pThis,oldPageRIndex/NAND_SECTOR_PAGECOUNT);
                        
                        //��Ҫ: pDCH->nandSOFһ������oldSector��,����������Ҫ������Ϊ��Ч
                        pDCH->nandSOF=PAGE_INDEX_INVALID;
                    }else{
                        //�쳣����
                        drv_DataFile_setNPM(pThis,pageRIndex,NAND_PM_BAD); //�ǿգ���Чҳ
                        return NAND_PM_BAD;
                    }
                }else{
                    //��������Ч
                    if((pageRIndex%NAND_SECTOR_PAGECOUNT)==0){
                        //ֱ�Ӳ�����ǰ����(����Ҫ�����߼�ҳ��)
                        drv_DataFile_eraseNandSector(pThis,pageRIndex/NAND_SECTOR_PAGECOUNT);
                        
                        //���������̷���,�������ҳ����������
                        return NAND_PM_EMPTY; 
                    }else{
                        //�쳣����
                        drv_DataFile_setNPM(pThis,pageRIndex,NAND_PM_BAD); //�ǿգ���Чҳ
                        return NAND_PM_BAD;
                    }
                }
            }else{         
                //��ǰ����ҳ���ǹ��ڵ�
                drv_DataFile_setNPM(pThis,pageRIndex,NAND_PM_DUMP); //�ǿ�,��Чҳ
                return NAND_PM_DUMP;
            }
        }else{ //�����µ��߼�ҳ��
            drv_DataFile_setNPM(pThis,pageRIndex,NAND_PM_VALID);   //�������ҳ��Ч
        }
        //�Ǽ�FPM
        PM_SETFLAG(pDCH->FPM[BND_GETPAGE(band)],FILE_PM_VALID);//�߼�ҳ����Ч
        PM_SETINDEX(pDCH->FPM[BND_GETPAGE(band)],pageRIndex);
    }else{
        char desc[64];
        sprintf(desc,"scanPage(bad page=%X)",pageRIndex);
        drv_Exception_throwEx(ERROR_TYPE_FILE,0x07,desc);            
//        drv_DataFile_setNPM(pThis,pageRIndex,NAND_PM_BAD); //�쳣ҳ��
//        return NAND_PM_BAD;
    }
    
    return nmp;
}

#define SECTOR_STATE_EMPTY  0x00
#define SECTOR_STATE_VALID  0x01  //������Чҳ��
#define SECTOR_STATE_DUMP   0x02  //���Բ���
#define SECTOR_STATE_EXCEPTION   0x03  //�쳣

UInt8 drv_DataFile_scanSector(const SFILE* pThis,short sectorRIndex){
    short i;
    UInt8 nmp0,nmp;
    int validPages=0;
    
    nmp0=drv_DataFile_scanPage(pThis,sectorRIndex*NAND_SECTOR_PAGECOUNT);    
    if(nmp0==NAND_PM_BAD) return SECTOR_STATE_EXCEPTION;     //��ҳ��,�������鱻��Ϊ����Ч��,��Ҫ����
    if(nmp0==NAND_PM_VALID){
        validPages++;
    }
    for(i=1;i<NAND_SECTOR_PAGECOUNT;i++){
        short pageRIndex=sectorRIndex*NAND_SECTOR_PAGECOUNT+i;
        if(nmp0==NAND_PM_EMPTY){  //����Ƿ��зǿ�ҳ�棬���Ҫ�󣬷���SECTOR_STATE_DUMP, Ҫ�����
            PAGEBAND band;
            hal_Flash_readPageN( FILE_NANDPAGE_TOAINDEX(pageRIndex),DATAFILE_BAND_OFFSET,(UInt8*)&band,sizeof(PAGEBAND));
            
            nmp=drv_DataFile_checkBand(pThis,&band);         
            //��ҳΪ��,����ҳҲ����Ϊ��,����ͱ���Ϊ�ǻ���
            if(nmp!=NAND_PM_EMPTY){
                //�쳣����,ǿ�ƽ���ҳ���ó���Чҳ,���ϸ�����,��������²�Ӧ�÷���
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
            //��nandSOF��λ�ڵ�һ���հ�ҳ
            if( (pDCH->nandSOF==PAGE_INDEX_INVALID) && (nmp==NAND_PM_EMPTY) ){
                pDCH->nandSOF=pageRIndex;
            }
        }
    }
    
    if(nmp0==NAND_PM_EMPTY) return SECTOR_STATE_EMPTY;     //��ҳΪ��,������ǿ�
    
    if(validPages==0){   
        return SECTOR_STATE_DUMP;
    }else{
        return SECTOR_STATE_VALID;
    }
}


//fix@2018-12-14 ,����ڴ�����ҳ��,���Ƿ���������Բ���
bool drv_DataFile_checkDumpSector_(const SFILE* pThis,short sectorRIndex){
    short i;
    UInt8 nmp0,nmp;
    short validPages=0;
    nmp0=drv_DataFile_getNPM(pThis,sectorRIndex*NAND_SECTOR_PAGECOUNT);
    if(nmp0==NAND_PM_EMPTY) return false; //�հ�ҳ��,����Ҫ����
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
    //ɨ����������ҳ���BAND��,һ��ɨ��һ����(ͬʱ�����ƶ������б��жϵ�Ŀ��������(�������ƶ�))
    for(sectorRIndex=0;sectorRIndex<(FILE_PAGE_NANDCOUNT/NAND_SECTOR_PAGECOUNT);sectorRIndex++){
        UInt8 state=drv_DataFile_scanSector(pThis,sectorRIndex);
        if(state==SECTOR_STATE_EXCEPTION){ //fix@ ��ִ�в���һ���쳣��ҳ��
            drv_DataFile_eraseNandSector(pThis,sectorRIndex);
        }
    }

    
    //ɨ����ɺ���ȫ�����һ�Σ����Ƿ��пɲ���������
    for(sectorRIndex=0;sectorRIndex<(FILE_PAGE_NANDCOUNT/NAND_SECTOR_PAGECOUNT);sectorRIndex++){
        if(drv_DataFile_checkDumpSector_(pThis,sectorRIndex)){
            drv_DataFile_eraseNandSector(pThis,sectorRIndex);
        }
    }
    

    
    //���հ����Ƿ�����
#ifdef __DEBUG
    drv_DataFile_checkEmptySectors(pThis,EMPTY_SECTORCOUNT_MIN);
#endif
}

/////////////////////DataFile�Ĺ��ܺ���////////////////////////////////////////////////////////////
void drv_DataFile_init(const SFILE* pThis){
    pDCH->nandSOF=PAGE_INDEX_INVALID;
    pDCH->maxTimeTag=0;
}

void drv_DataFile_openN(const SFILE* pThis){
    drv_DataFile_scanAll(pThis);
}

//������
void drv_DataFile_readN(const SFILE* pThis,UInt16 nPage,UInt16 nAddr,void* pData,UInt16 nLen ){
    short cashIndex;
    cashIndex=hal_Cache_find(pThis->pCache,nPage);
    if(cashIndex<0){
        if(PM_GETFLAG(pDCH->FPM[nPage])!=FILE_PM_VALID){
            memset(pData,PAGEVALUE_EMPTY,nLen);
            return;
        }else{
            //�ڴ��޻���,ֱ�Ӷ�ȡ����ҳ��
            hal_Flash_readPageN(FPM_GETAINDEX(nPage),DATAFILE_DATA_OFFSET+nAddr,pData,nLen);
        }
    }else{
        hal_Cache_read(pThis->pCache,DATAFILE_DATA_OFFSET+nAddr,pData,nLen);
    }
}
//����������ҳ���ݼ��ص����������������������ִ��ˢ�²������ͷ�һ��������
void drv_DataFile_loadN(const SFILE* pThis,UInt16 nPage){
    //nPage��δ����,����һ�����õĻ�������

    if(hal_Cache_find(pThis->pCache,CACHEPAGE_NONE)<0){ 
        //û�п�����,���Ƚ���ǰcache�е����ݱ�������
        drv_DataFile_flushN(pThis);
    }
        
    //���������ݶ��뻺��
    if(PM_GETFLAG(pDCH->FPM[nPage])!=FILE_PM_VALID){
        //�߼�ҳ�治����ʱ,��0���
        memset(pThis->pCache->buffer,0,NAND_PAGE_SIZE);
    }else{
        hal_Flash_readPageN(FPM_GETAINDEX(nPage),0,pThis->pCache->buffer,NAND_PAGE_SIZE); 
    }
 
    pThis->pCache->page=nPage;
    pThis->pCache->dirty=false;

}
//д����,����������д�뻺����,�������������ִ��ˢ�¶���.
void drv_DataFile_writeN(const SFILE* pThis,UInt16 nPage,UInt16 nAddr,const void* pData,UInt16 nLen ){
    if( hal_Cache_find(pThis->pCache,nPage) <0 ){
        drv_DataFile_loadN(pThis,nPage);
    }
    hal_Cache_write(pThis->pCache,DATAFILE_DATA_OFFSET+nAddr,pData,nLen);
}

//�����Ĺ����߼�ҳ��ˢ�µ�����ҳ
void drv_DataFile_flushN(const SFILE* pThis){
    if(pThis->pCache->dirty){
        
        if( (pThis->pCache->page<0) ){
            char desc[64];
            sprintf(desc,"flushN1(%X)",pThis->pCache->page);
            drv_Exception_throwEx(ERROR_TYPE_FILE,0x10,desc);
        }
        
        if( pDCH->nandSOF==PAGE_INDEX_INVALID){ //��ǰ���Ŀհ�ҳ��ľ�
            short emptySectorRIndex=drv_DataFile_checkEmptySectors(pThis,EMPTY_SECTORCOUNT_MIN);
            //����Ƿ����㹻��Ŀհ���
            if( emptySectorRIndex==SECTOR_INDEX_INVALID ){
                //�հ�������,ִ��һ��ҳ����ղ���
                pDCH->nandSOF=drv_DataFile_adjustPages(pThis);
            }else{
                pDCH->nandSOF=emptySectorRIndex*NAND_SECTOR_PAGECOUNT;
            }
        }

        if(pDCH->nandSOF==PAGE_INDEX_INVALID){
            drv_Exception_throwEx(ERROR_TYPE_FILE,0x11,"flushN2");
        }else{
            //����ҳ������
            drv_DataFile_saveFilePageN(pThis,pThis->pCache->page,pThis->pCache->buffer);
            pThis->pCache->dirty=false;
        }
    }
    
}

void drv_DataFile_close(const SFILE* pThis){
    pThis->flush(pThis);
}


//�ļ���ʽ��,Ϊ�ӿ��ٶ�,�õ��ǿ����
void drv_DataFile_format(const SFILE* pThis,FILEFORMAT_CALLBACK pFunc){
    short i;
    
    //��ʼ����
    if(pFunc) pFunc(pThis,FILE_FORMATFLAG_BEFORE,-1);
    for(i=0; i< FILE_SECTOR_NANDCOUNT;i++){
        hal_IDOG_flush(); //��ʽ��ʱ��ϳ�,��Ҫˢ��
        if(pFunc) pFunc(pThis,FILE_FORMATFLAG_BEFORE,i);
        drv_DataFile_eraseNandSector(pThis,i); //DataFile������eraseBlock�����г��ֻ�ҳ
        if(pFunc) pFunc(pThis,FILE_FORMATFLAG_AFTER,i);
    }
    if(pFunc) pFunc(pThis,FILE_FORMATFLAG_AFTER,-1);
    
    pThis->open(pThis);
}
