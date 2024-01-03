#ifndef _FMC_APP_H_
#define _FMC_APP_H_


#include "Define.h"
#include "gd32a50x_fmc.h"




void get_Bin64_Data_Array(uint8_t arr[], uint64_t Bin64_Data_Array[]);
void fmc_flags_clear(void);
void fmc_erase_pages(uint32_t start_address, uint32_t page_num);//�������ӣ�start_address����ʼ�ģ�page_cnt����ô��ҳ��
void fmc_uint64_program(uint32_t address, uint64_t data);//��дһ��˫�֣�data����д��ָ����ַ��addresss����
void fmc_program(uint32_t address, uint64_t arr[]);//����bin���ݣ�arr[]����д��ָ����ַ��addresss����
ErrStatus fmc_program_check(uint32_t address, uint64_t arr[]);
void TEST(void);


#endif