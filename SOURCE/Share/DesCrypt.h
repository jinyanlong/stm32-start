#ifndef DESENCRYPT_H_001
#define DESENCRYPT_H_001

#include "DesBox.h"
void drv_DES_createSubKey(BYTE* pKey,DESKEYSTRUCT* pKeyInfo);
void drv_DES_encrypt(BYTE* rawData,BYTE* encryptData,DESKEYSTRUCT* pSubKey);
void drv_DES_decrypt(BYTE* encryptData,BYTE* rawData,DESKEYSTRUCT* pSubKey);

#endif
