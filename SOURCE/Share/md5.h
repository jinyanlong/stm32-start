#ifndef  MD5_H_001
#define  MD5_H_001

#include "SFTypes.H"
#include "SFLib.H"


typedef struct {
	UInt32 state[4];
	UInt32 count[2];
	UInt8 buffer[64];
}MD5_CTX;

void MD5Init(MD5_CTX* pContext);
void MD5Update(MD5_CTX* pContext,const UInt8* input,UInt32 inputLen);
void MD5Final(MD5_CTX* pContext,UInt8* digest);

#endif
