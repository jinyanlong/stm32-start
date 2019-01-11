#ifndef DESBOX_H_001
#define DESBOX_H_001


typedef BYTE DESKEYITEM[48];

typedef struct _DESKEYSTRUCT{
	DESKEYITEM KeyItem[16];
} DESKEYSTRUCT;

//保存pInBit(bit)中的数据到m_T(byte)
void BitToByte(BYTE *pInBit);
//提取m_T(byte)中的数据到pOutBit(bit)
void ByteToBit(BYTE *pOutBit);
//换位选择1
//入口：m_T[64]
//出口：m_B[56]
void Des_KP1(void);

//循环移位
//入口：m_B[0-27],m_B[28-55]
//出口：m_B[0-27],m_B[28-55]
void Des_KM(void);

//换位选择2
//入口：m_B[56]
//出口：pKey[48]
void Des_KP2(BYTE* pKey);

	//入口：m_T存放初始明文T
	//出口：m_B[0-63]中存放IP(T)
void Des_IP(void);

	//入口：m_B[0-63]中存放IP(T),即L(i),R(i)
	//出口：m_OldL[0-31]中存放L(i),m_B[0-31]中存放R(i)
	//      m_J[0-47]中存放E(R(i))
void Des_E(void);

//入口：m_J[0-47]中存放E(R(i))
//出口：m_J[0-47]中存放E(R(i))^K(i+1)
void Des_XR(BYTE* pKeyBit);

	//入口：m_J[0-47]中存放J
	//出口：m_T[0-63]中存放S(J)
void Des_S(void);

//入口：m_T存放S(J)
//出口：m_J[0-31]中存放P(S(J))
void Des_P(void);

//入口：m_B[0-31],m_B[32-63]中存放R(i),m_J[0-31]中存放f(R,K)
//出口：
//Order!=15: m_B[0-31]中存放R(i),m_B[32-63]中存放R(i+1)
//Order==15: m_B[0-31]中存放R(i+1),m_B[32-63]中存放R(i)
void Des_XT(BYTE Order);

//入口：m_B
//出口：m_T中存放密文
void Des_UNIP(void);

//bDirect=TRUE 为加密 ，bDirect=FALSE 为解密
void Des_Crypt(BYTE *pIn, BYTE *pOut,DESKEYITEM* pKeyGroup,bool bDirect);

#endif

