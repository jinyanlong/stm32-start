#include <includes.h>
#include "halLF.h"

u16 _antId=0x8011;
#define SETTAGSTATE  0x8200

bool lfSendData(uint16_t data,uint16_t count){
    readyLfSendData(data);
    return lfSendStart(count);
}

bool lfAntCalib(void){//����У׼
    return lfSendData(_antId,6);
}

bool setTagState(uint8_t second){//ǿ�Ʊ�ǩ����/�˳�2.4G����״̬������Ϊ0����ǩ�˳�����״̬
    uint16_t data;
    data=(SETTAGSTATE|second);
    return lfSendData(data,1);
}

