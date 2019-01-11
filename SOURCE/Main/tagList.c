#include "includes.h"
#include "SFLib.h"
#include "tagList.h"

//���ļ�ʵ��һ����ǩ�������,�����д��ڼ���״̬�ı�ǩȫ�����浽�ڴ�
//���Ի������㹻������������δ���???
TAGCACHE s_record_list[CACHE_CAPACITY];
UInt32 s_record_sof;
UInt32 s_record_eof;

void drv_RecordCache_init(void){
    s_record_sof=s_record_eof=0;
    memset(s_record_list,0,sizeof(s_record_list));
}

bool drv_RecordCache_isFull(void){
    return (s_record_eof -s_record_sof) >= (CACHE_CAPACITY-1);
}

UInt32 drv_RecordCache_sof(void){return s_record_sof;}
UInt32 drv_RecordCache_eof(void){return s_record_eof;}
UInt32 drv_RecordCache_count(void){return s_record_eof-s_record_sof;}
TAGCACHE* drv_RecordCache_get(UInt32 pos){
    return &s_record_list[pos%CACHE_CAPACITY];
}

//ͳ���ж�������,����Ч�����⣬�ݲ�ʵ��
//int drv_RecordCache_getCoinCount(void){

//}
//index ��0��ʼ����������
bool fns_RecordCache_getByIndex(int index,TAGCACHE* tagCache){
    bool ret=false;
    
    OS_LOCK();
    if( index < drv_RecordCache_count() ){
        memcpy(tagCache,&s_record_list[(index+s_record_sof)%CACHE_CAPACITY],sizeof(TAGCACHE));
        ret=true;
    }
    OS_UNLOCK();
    return ret;
}


TAGCACHE* drv_RecordCache_find(UInt16 coinId,UInt32 tagId){
    UInt32 i;
    TAGCACHE* pTagCache;
    for(i=s_record_sof;i<s_record_eof;i++){
        pTagCache=drv_RecordCache_get(i);
        if( (tagId==pTagCache->tagId) && (coinId==pTagCache->coinId) ){
            return pTagCache;
        }
    }
    return NULL;
}

TAGCACHE* drv_RecordCache_pop(void){
    TAGCACHE* p;
    p=&s_record_list[s_record_sof%CACHE_CAPACITY];
    s_record_sof++;
    return p;
}

int drv_RecordCache_push(TAGCACHE* tagCache){
    if(drv_RecordCache_isFull()) return -1;
    memcpy(&s_record_list[s_record_eof%CACHE_CAPACITY],tagCache,sizeof(TAGCACHE));
    s_record_eof++;
    return s_record_eof-1;
}

