#include  <includes.h>
#include  "KeyBoard.h"

static HOOK_KEYBOARD_ONPRESS _hook_Keyboard_onPress;
volatile UInt32 _keyValue;  //低8位是键值,高位指示键值状态,如是否连续触发
volatile UInt32 _keyValidDelay;
volatile UInt32 _keyState;  //键盘的当前状态


//发送键盘消息
void drv_Keyboard_init(HOOK_KEYBOARD_ONPRESS pCallback){
    _hook_Keyboard_onPress=pCallback;
    _keyValidDelay =0;
	_keyState =KEYSTATE_NULL; //禁止键盘
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

    //3.发现新的按键,进入防抖状态(BEFOREVALID)
	if( (_keyState==KEYSTATE_READY) || (_keyValue!=keyvalue) ){
		_keyValue=keyvalue;
		_keyState=KEYSTATE_NEW;
		_keyValidDelay=now+KEYDEALY_VALID;
		return;
	}
    
    //4. 防抖处理
    if( now < _keyValidDelay ){
        return;
    }
    
    //5. 修改键盘状态
    if(_keyState==KEYSTATE_NEW){
        _keyState=KEYSTATE_REPEAT;
        _keyValidDelay=now+KEYDEALY_FIRSTREPEAT;
    }else{
        _keyState=KEYSTATE_REPEAT;
        _keyValidDelay=now+KEYDEALY_NORMALREPEAT;
    }
    
    //6. 触发按键消息
    if(_hook_Keyboard_onPress) _hook_Keyboard_onPress(_keyValue,_keyState);
}
