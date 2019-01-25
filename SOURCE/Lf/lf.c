#include <includes.h>
#include "halLF.h"
#include "halADC.h"
#include "halMCP4018T.h"

//va-pc0-ch10(天线电压)|vb-pc1-ch11|vm-pa6-ch6|vn-pc5-ch15|vcc1-pc2-ch12(电源电压)|vcc2-pb0-ch8|
void drv_LF_config(void){
    hal_LF_config();
    hal_ADC_config();
}

uint32_t get_LFANT_VCC(UInt8 ant){//获取天线
    uint32_t val;
    if(ant==LF_ANTA){
        val=hal_ADC_getAverageVal(ADC_Channel_10,5);
    }else if(ant==LF_ANTB){
        val=hal_ADC_getAverageVal(ADC_Channel_11,5);
    }else if(ant==LF_ANTM){
        val=hal_ADC_getAverageVal(ADC_Channel_6,5);
    }else if(ant==LF_ANTN){
        val=hal_ADC_getAverageVal(ADC_Channel_15,5);
    }
    return val;
}

uint32_t drv_GET_LFPOWVCC(UInt8 pNO){//天线电源电压
    uint32_t vcc;
    if(MCP4018T_1){
        vcc=hal_ADC_getAverageVal(ADC_Channel_12,5);
    }else if(MCP4018T_2){
        vcc=hal_ADC_getAverageVal(ADC_Channel_8,5);
    }
    return vcc;
}
void drv_SET_LFPOWVAL(UInt8 pNO,UInt8 data){
    hal_MCP4018_write(pNO,&data,1);
}


UInt8 drv_adjust_LFCap(UInt8 ant){//电容调整，获取最高电压
    UInt32 vcc;
    UInt8 i,pro=0,tmp=0;
    hal_OpenLF(ant);
    for(i=7;i<16;i++){
        hal_LF_SetCapPin(ant,i);
        delay_us(1000);
        vcc=get_LFANT_VCC(ant);
        if(tmp<vcc){
            pro=i;
            tmp=vcc;
        }
    }
    hal_CloseLF(ant);

    return pro;
}

void drv_LFset_LFQVAL(UInt8 ant,UInt8 val){
    hal_LF_setQVAL(ant,val);
}



