#ifndef __STM32F10x_CONF_H
#define __STM32F10x_CONF_H

//#define DEBUG
#undef assert


//#ifdef  __DEBUG
//    #define assert_param(expr)          ((void)0)
//    #define debug_signal_set(offset)    ATOMIC_OR(v_debug_flag, ((UInt64)0x01<<offset))
//    #define debug_signal_reset()        ATOMIC_SET(v_debug_flag,0)
//    
//    #ifdef __DEBUG_SIGNAL
//        volatile unsigned long long  v_debug_flag;
//    #else
//        extern volatile unsigned long long  v_debug_flag;
//    #endif
//#else
    #define assert_param(expr)          ((void)0)
    #define debug_signal_set(offset)    ((void)0)
    #define debug_signal_reset()        ((void)0)
//#endif

#endif

