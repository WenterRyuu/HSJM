#ifndef _FMC_APP_H_
#define _FMC_APP_H_


#include "Define.h"
#include "gd32a50x_fmc.h"




void get_Bin64_Data_Array(uint8_t arr[], uint64_t Bin64_Data_Array[]);
void fmc_flags_clear(void);
void fmc_erase_pages(uint32_t start_address, uint32_t page_num);//【擦除从（start_address）开始的（page_cnt）这么多页】
void fmc_uint64_program(uint32_t address, uint64_t data);//【写一个双字（data）烧写到指定地址（addresss）】
void fmc_program(uint32_t address, uint64_t arr[]);//【把bin数据（arr[]）烧写到指定地址（addresss）】
ErrStatus fmc_program_check(uint32_t address, uint64_t arr[]);
void TEST(void);


#endif