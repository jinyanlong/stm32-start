#ifndef DESBOX_H_001
#define DESBOX_H_001


typedef BYTE DESKEYITEM[48];

typedef struct _DESKEYSTRUCT{
	DESKEYITEM KeyItem[16];
} DESKEYSTRUCT;

//����pInBit(bit)�е����ݵ�m_T(byte)
void BitToByte(BYTE *pInBit);
//��ȡm_T(byte)�е����ݵ�pOutBit(bit)
void ByteToBit(BYTE *pOutBit);
//��λѡ��1
//��ڣ�m_T[64]
//���ڣ�m_B[56]
void Des_KP1(void);

//ѭ����λ
//��ڣ�m_B[0-27],m_B[28-55]
//���ڣ�m_B[0-27],m_B[28-55]
void Des_KM(void);

//��λѡ��2
//��ڣ�m_B[56]
//���ڣ�pKey[48]
void Des_KP2(BYTE* pKey);

	//��ڣ�m_T��ų�ʼ����T
	//���ڣ�m_B[0-63]�д��IP(T)
void Des_IP(void);

	//��ڣ�m_B[0-63]�д��IP(T),��L(i),R(i)
	//���ڣ�m_OldL[0-31]�д��L(i),m_B[0-31]�д��R(i)
	//      m_J[0-47]�д��E(R(i))
void Des_E(void);

//��ڣ�m_J[0-47]�д��E(R(i))
//���ڣ�m_J[0-47]�д��E(R(i))^K(i+1)
void Des_XR(BYTE* pKeyBit);

	//��ڣ�m_J[0-47]�д��J
	//���ڣ�m_T[0-63]�д��S(J)
void Des_S(void);

//��ڣ�m_T���S(J)
//���ڣ�m_J[0-31]�д��P(S(J))
void Des_P(void);

//��ڣ�m_B[0-31],m_B[32-63]�д��R(i),m_J[0-31]�д��f(R,K)
//���ڣ�
//Order!=15: m_B[0-31]�д��R(i),m_B[32-63]�д��R(i+1)
//Order==15: m_B[0-31]�д��R(i+1),m_B[32-63]�д��R(i)
void Des_XT(BYTE Order);

//��ڣ�m_B
//���ڣ�m_T�д������
void Des_UNIP(void);

//bDirect=TRUE Ϊ���� ��bDirect=FALSE Ϊ����
void Des_Crypt(BYTE *pIn, BYTE *pOut,DESKEYITEM* pKeyGroup,bool bDirect);

#endif

