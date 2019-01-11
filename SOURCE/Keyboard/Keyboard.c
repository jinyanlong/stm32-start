#include  <includes.h>
#include  "KeyBoard.h"

static HOOK_KEYBOARD_ONPRESS _hook_Keyboard_onPress;
volatile UInt32 _keyValue;  //��8λ�Ǽ�ֵ,��λָʾ��ֵ״̬,���Ƿ���������
volatile UInt32 _keyValidDelay;
volatile UInt32 _keyState;  //���̵ĵ�ǰ״̬


//���ͼ�����Ϣ
void drv_Keyboard_init(HOOK_KEYBOARD_ONPRESS pCallback){
    _hook_Keyboard_onPress=pCallback;
    _keyValidDelay =0;
	_keyState =KEYSTATE_NULL; //��ֹ����
}

void fns_Keyboard_enable(void){
    ATOMIC_SET(_keyState,KEYSTATE_READY);
}

void fns_Keyboard_disable(void){
    ATOMIC_SET(_keyState,KEYSTATE_NULL);
}

void drv_Keyboard_tick(UInt32 now){
	UInt32 keyvalue;
    
    if(	_keyState==KEYSTATE_NULL )
        return;
        
	keyvalue=hal_Keyboard_get();

	if(keyvalue==KEYVALUE_NULL){
		_keyState = KEYSTATE_READY;
		return;
	}

    //3.�����µİ���,�������״̬(BEFOREVALID)
	if( (_keyState==KEYSTATE_READY) || (_keyValue!=keyvalue) ){
		_keyValue=keyvalue;
		_keyState=KEYSTATE_NEW;
		_keyValidDelay=now+KEYDEALY_VALID;
		return;
	}
    
    //4. ��������
    if( now < _keyValidDelay ){
        return;
    }
    
    //5. �޸ļ���״̬
    if(_keyState==KEYSTATE_NEW){
        _keyState=KEYSTATE_REPEAT;
        _keyValidDelay=now+KEYDEALY_FIRSTREPEAT;
    }else{
        _keyState=KEYSTATE_REPEAT;
        _keyValidDelay=now+KEYDEALY_NORMALREPEAT;
    }
    
    //6. ����������Ϣ
    if(_hook_Keyboard_onPress) _hook_Keyboard_onPress(_keyValue,_keyState);
}
