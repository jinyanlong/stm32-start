#ifndef BYTESTREAM_H_001
#define BYTESTREAM_H_001
#include <stddef.h>

typedef UInt8 STREAM_ITEMTYPE;

//һ����������:sof��eof��Զ�������UInt32

typedef void (*StreamItemCallback)(STREAM_ITEMTYPE);
typedef struct _STREAMBUFFER{
    UInt32 bufferSize;
    UInt32 sof;
    UInt32 eof;
    STREAM_ITEMTYPE items[1];
} STREAMBUFFER;


//maxSizeָ����pMem�ĳߴ�,�����Ǵ������ݻ�������С
__INLINE STREAMBUFFER* STREAM_INIT(void* pMem,UInt32 maxSize){
    STREAMBUFFER* pStream=(STREAMBUFFER*)pMem;
    
    pStream->bufferSize=maxSize-offsetof(STREAMBUFFER, items);
    pStream->sof=pStream->eof=0;
    return pStream;
}

//���ݻ������ܴ洢�����������
__INLINE UInt32 STREAM_MAXCOUNT(STREAMBUFFER* pStream){
    return pStream->bufferSize;
}

//���ݻ������洢��������
__INLINE UInt32 STREAM_COUNT(STREAMBUFFER* pStream){
    return pStream->eof-pStream->sof;
}


__INLINE bool STREAM_ISEMPTY(STREAMBUFFER* pStream){
    return (pStream->sof==pStream->eof);
}

__INLINE bool STREAM_ISFULL(STREAMBUFFER* pStream){
    return (pStream->eof-pStream->sof)==pStream->bufferSize;
}

__INLINE void STREAM_CLEAR(STREAMBUFFER* pStream){
    pStream->sof=pStream->eof;
}

//reset����,ͬʱдsof��eof,һ��Ҫ����,STREAM_CLEAR�򲻻�
__INLINE void STREAM_RESET(STREAMBUFFER* pStream){
    pStream->sof=pStream->eof=0;
}

__INLINE void STREAM_PUSH(STREAMBUFFER* pStream,STREAM_ITEMTYPE item){
    pStream->items[pStream->eof%(pStream->bufferSize)]=item;
    pStream->eof++;
}

__INLINE STREAM_ITEMTYPE STREAM_POP(STREAMBUFFER* pStream){
    STREAM_ITEMTYPE item;
    item=pStream->items[pStream->sof%(pStream->bufferSize)];
    pStream->sof++;
    return item;
}

#endif

