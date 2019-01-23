#ifndef __HALMCPI2C_001
#define __HALMCPI2C_001

#define MCP4018T_1  0x01
#define MCP4018T_2  0x02

void hal_MCPI2C_config(void);
void hal_MCPI2C_init(uint8_t val);
void hal_MCPI2C_start(uint8_t val);
void hal_MCPI2C_stop(uint8_t val);
void hal_MCPI2C_ack(uint8_t val);
void hal_MCPI2C_noack(uint8_t val);
bool hal_MCPI2C_send(uint8_t val,unsigned char bytData);
unsigned char hal_MCPI2C_receive(uint8_t val);

#endif

