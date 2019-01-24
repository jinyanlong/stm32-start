#ifndef __HALI2C_001
#define __HALI2C_001

void hal_I2C_config(void);
void hal_I2C_init(void);
void hal_I2C_start(void);
void hal_I2C_stop(void);
void hal_I2C_ack(void);
void hal_I2C_noack(void);
bool hal_I2C_send(unsigned char bytData);
unsigned char hal_I2C_receive(void);

#endif

