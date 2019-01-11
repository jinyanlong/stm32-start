#include <includes.h>
#include "halLF.h"

u16 _antId=0x8011;
#define SETTAGSTATE  0x8200

bool lfSendData(uint16_t data,uint16_t count){
    readyLfSendData(data);
    return lfSendStart(count);
}

bool lfAntCalib(void){//天线校准
    return lfSendData(_antId,6);
}

bool setTagState(uint8_t second){//强制标签进入/退出2.4G接收状态，参数为0，标签退出接受状态
    uint16_t data;
    data=(SETTAGSTATE|second);
    return lfSendData(data,1);
}

