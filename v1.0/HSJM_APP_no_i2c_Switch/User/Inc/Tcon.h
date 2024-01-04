#ifndef _TCON_H_
#define _TCON_H_

#include "Define.h"


#define TCON_ADDRESS8    		0xC0		//(0x60<<1)
#define BUFFER_SIZE              256
#define MAX_RELOAD_SIZE          255

//给位域操作的每个位重新定义别名
#define R041D_1 bit0
#define R041D_2 bit1
#define R041D_6 bit2
#define R041D_7 bit3
#define R041E_3 bit4
#define R041F_7 bit5
#define R0420_1 bit6
#define R0420_6 bit7
#define R0420_7 bit8
#define R0421_0 bit9
#define R0422_4 bit10
#define RFC00_0 bit11
#define RFC00_1 bit12
#define RFC00_2 bit13
#define RFC00_3 bit14

//为数组的下标重定义别名
enum{
	REG_041D = 0,
	REG_041E,
	REG_041F,
	REG_0420,
	REG_0421,
	REG_0422,
	REG_FC00
};

struct BitFields {
    uint16_t R041D_1 : 1;
    uint16_t R041D_2 : 1;
    uint16_t R041D_6 : 1;
    uint16_t R041D_7 : 1;
    uint16_t R041E_3 : 1;
    uint16_t R041F_7 : 1;
    uint16_t R0420_1 : 1;
    uint16_t R0420_6 : 1;
    uint16_t R0420_7 : 1;
    uint16_t R0421_0 : 1;
    uint16_t R0422_4 : 1;
    uint16_t RFC00_0 : 1;
    uint16_t RFC00_1 : 1;
    uint16_t RFC00_2 : 1;
    uint16_t RFC00_3 : 1;
    uint16_t bit15 : 1;
};

void I2C0_Read_TCON_Process(uint8_t I2c_Addr, uint16_t Reg_Addr, uint8_t *Buff, uint8_t Length);
void Read_TCON_Register(void);



#endif
