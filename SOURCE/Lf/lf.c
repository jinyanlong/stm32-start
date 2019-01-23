#include <includes.h>
#include "halLF.h"
#include "halADC.h"

//va-pc0-ch10(天线电压)|vb-pc1-ch11|vm-pa6-ch6|vn-pc5-ch15|vcc1-pc2-ch12(电源电压)|vcc2-pb0-ch8|
void drv_LF_config(void){
    hal_LF_config();
    hal_ADC_config();
}

uint32_t get_LFANT_VCC(uint8_t ant){
    uint32_t val;
    hal_OpenLF(ant);
    if(ant==LF_ANTA){
        val=hal_ADC_getAverageVal(ADC_Channel_10,5);
    }else if(ant==LF_ANTB){
        val=hal_ADC_getAverageVal(ADC_Channel_11,5);
    }else if(ant==LF_ANTM){
        val=hal_ADC_getAverageVal(ADC_Channel_6,5);
    }else if(ant==LF_ANTN){
        val=hal_ADC_getAverageVal(ADC_Channel_15,5);
    }
    hal_CloseLF(ant);
    return val;
}

uint32_t get_LFPOW_VCC1(){//电源电压
    return hal_ADC_getAverageVal(ADC_Channel_12,5);
}
uint32_t get_LFVPOW_VCC2(){
    return hal_ADC_getAverageVal(ADC_Channel_8,5);
}

uint8_t adjust_LFCap(uint8_t ant){//电容调整，获取最高电压
    uint8_t i,pro=0,tmp=0;
    for(i=0;i<16;i++){
        hal_LF_SetCapPin(ant,i);
        delay_ms(1);
        if(tmp<get_LFANT_VCC(ant)){
            pro=i;
        }
    }
    return pro;
}



