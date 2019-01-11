#ifndef BYTESTREAM_H_001
#define BYTESTREAM_H_001
#include <stddef.h>

typedef UInt8 STREAM_ITEMTYPE;

//一个基本假设:sof和eof永远不会溢出UInt32

typedef void (*StreamItemCallback)(STREAM_ITEMTYPE);
typedef struct _STREAMBUFFER{
    UInt32 bufferSize;
    UInt32 sof;
    UInt32 eof;
    STREAM_ITEMTYPE items[1];
} STREAMBUFFER;


//maxSize指的是pMem的尺寸,而不是纯的数据缓冲区大小
__INLINE STREAMBUFFER* STREAM_INIT(void* pMem,UInt32 maxSize){
    STREAMBUFFER* pStream=(STREAMBUFFER*)pMem;
    
    pStream->bufferSize=maxSize-offsetof(STREAMBUFFER, items);
    pStream->sof=pStream->eof=0;
    return pStream;
}

//数据缓冲区能存储的最大数据量
__INLINE UInt32 STREAM_MAXCOUNT(STREAMBUFFER* pStream){
    return pStream->bufferSize;
}

//数据缓冲区存储的数据量
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

//reset操作,同时写sof和eof,一定要加锁,STREAM_CLEAR则不会
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

