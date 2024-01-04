#ifndef MAX96752_H
#define MAX96752_H
#include "Define.h"
#include "gd32a50x_it.h"

#define MAX96752_ADDRESS8    	0x90		//0x48<<1
#define BUFFER_SIZE              256
#define MAX_RELOAD_SIZE          255


//iic发送/接收时的各种状态枚举///////////////////////////////////////////////////
typedef enum {
    I2C_START = 0,
    I2C_SEND_ADDRESS,
	I2C_SEND_ADDRESS_LOW,		//发送16位地址时，分成高8位和低8位，这是发低8位的状态
    I2C_SEND_LENGTH,
	I2C_STOOP,
	I2C_RESTART,
    I2C_TRANSMIT_DATA,
    I2C_RELOAD,
	I2C_CRC,
    I2C_STOP,
    I2C_END
} i2c_process_enum;

#define I2C_TIME_OUT   (uint32_t)(20000)
#define EEP_FIRST_PAGE 0x00
#define I2C_OK         0
#define I2C_FAIL       1


ErrStatus I2c_MasterWriteMulBytes(uint8_t I2c_Addr,uint16_t Reg_Addr,uint8_t *Buff,uint8_t Length);
void MAX96752_REG_Config(void);



#endif
