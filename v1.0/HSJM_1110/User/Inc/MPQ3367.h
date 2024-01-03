#ifndef _MPQ3367_H_
#define _MPQ3367_H_



#define MPQ3367_ADDRESS8    	0x70		//(0x38<<1)
#define BUFFER_SIZE              256
#define MAX_RELOAD_SIZE          255

#include "Define.h" 

void I2C_Standard_Reading_Process(uint8_t I2c_Addr, uint8_t Reg_Addr,uint8_t *Buff,uint8_t Length);
uint8_t Read_MPQ3367_Register(void);


#endif
