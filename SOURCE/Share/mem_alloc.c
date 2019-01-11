#include "includes.h"
#include "mem_alloc.h"

typedef struct _SHAREMEMORY{
    UInt32  nSOF;
    UInt32  nSize;
	UInt8	buff[4];
} SHAREMEMORY;

//该函数对线程安全
//p为SHAREMEMORY类型的指针,为了在c语言中操作便利,用void*代替
void* fns_MessageMemory_alloc(void * p, UInt32 nSize){
    void* ptr;
    
    SHAREMEMORY* pMem= (SHAREMEMORY*)p;
    
    //确保4字节对齐
    if( nSize%4 !=0 ){
        nSize=nSize+(4-nSize%4);
    }
    
    taskENTER_CRITICAL();
    if( (pMem->nSOF+nSize) > pMem->nSize ){
        ptr=&pMem->buff[0];
        pMem->nSOF=nSize;
    }else{
        ptr=&pMem->buff[pMem->nSOF];
        pMem->nSOF+=nSize;
    }
    taskEXIT_CRITICAL();
    return ptr;
}
void fns_MessageMemory_free(void* p){
    //目前不需要任何处理
    //taskENTER_CRITICAL();
    //taskEXIT_CRITICAL();
}
