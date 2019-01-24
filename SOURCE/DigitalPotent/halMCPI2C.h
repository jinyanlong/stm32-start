#ifndef __HALMCPI2C_001
#define __HALMCPI2C_001

#define MCP4018T_1  0x01
#define MCP4018T_2  0x02

void hal_MCPI2C_config(void);
void hal_MCPI2C_init(UInt8 val);
void hal_MCPI2C_start(UInt8 val);
void hal_MCPI2C_stop(UInt8 val);
void hal_MCPI2C_ack(UInt8 val);
void hal_MCPI2C_noack(UInt8 val);
bool hal_MCPI2C_send(UInt8 val,unsigned char bytData);
unsigned char hal_MCPI2C_receive(UInt8 val);

#endif

