#include "includes.h"

#include "DesBox.h"
#include "DesCrypt.h"

void drv_DES_createSubKey(BYTE* pKey,DESKEYSTRUCT* pKeyInfo){
    BYTE i;
	BitToByte(pKey);
	Des_KP1();//��λѡ��1
	for( i=0;i<16;i++){
		Des_KM();//����1λ
		if(i>1 && i!=8 && i!=15)
			Des_KM();//��0��1��8��15������1λ
		Des_KP2(pKeyInfo->KeyItem[i]);//��λѡ��2,�������i������Կ
	}
}
void drv_DES_encrypt(BYTE* encryptData,BYTE* rawData,DESKEYSTRUCT* pKeyInfo){
	Des_Crypt(encryptData,rawData,pKeyInfo->KeyItem,true);
}
void drv_DES_decrypt(BYTE* rawData,BYTE* encryptData,DESKEYSTRUCT* pKeyInfo){
	Des_Crypt(rawData,encryptData,pKeyInfo->KeyItem,false);
}
