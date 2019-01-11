#include "includes.h"
#include "mem_alloc.h"

typedef struct _SHAREMEMORY{
    UInt32  nSOF;
    UInt32  nSize;
	UInt8	buff[4];
} SHAREMEMORY;

//�ú������̰߳�ȫ
//pΪSHAREMEMORY���͵�ָ��,Ϊ����c�����в�������,��void*����
void* fns_MessageMemory_alloc(void * p, UInt32 nSize){
    void* ptr;
    
    SHAREMEMORY* pMem= (SHAREMEMORY*)p;
    
    //ȷ��4�ֽڶ���
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
    //Ŀǰ����Ҫ�κδ���
    //taskENTER_CRITICAL();
    //taskEXIT_CRITICAL();
}
