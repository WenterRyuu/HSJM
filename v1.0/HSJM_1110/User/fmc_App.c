#include "fmc_App.h"


void get_Bin64_Data_Array(uint8_t arr[], uint64_t Bin64_Data_Array[]) 
{
    for (int i = 0; i < 4; ++i) 
	{
        uint64_t combined = 0;
        for (int j = 0; j < 8; ++j) 
		{
            int index = i * 8 + j;
            if (index < 32) 
			{
                // In little-endian, the least significant byte is placed in the lowest address
                combined |= (uint64_t)arr[index] << (j * 8);
            }
        }
        Bin64_Data_Array[i] = combined;
    }
}



void fmc_flags_clear(void)
{
    fmc_flag_clear(FMC_BANK0_FLAG_END);
    fmc_flag_clear(FMC_BANK0_FLAG_WPERR);
    fmc_flag_clear(FMC_BANK0_FLAG_PGAERR);
    fmc_flag_clear(FMC_BANK0_FLAG_PGERR);
    fmc_flag_clear(FMC_BANK0_FLAG_PGSERR);
    fmc_flag_clear(FMC_BANK0_FLAG_CBCMDERR);
    fmc_flag_clear(FMC_BANK0_FLAG_RSTERR);
}

//【擦除从（start_address）开始的（page_cnt）这么多页】
void fmc_erase_pages(uint32_t start_address, uint32_t page_num)
{
    fmc_unlock();
    fmc_flags_clear();

    for(uint8_t page_cnt = 0; page_cnt < page_num; page_cnt++) 
	{
        fmc_page_erase(start_address + (FMC_PAGE_SIZE * page_cnt));
        fmc_flags_clear();
    }

    /* lock the main FMC after the erase operation */
    fmc_lock();
}

//【写一个双字（data）烧写（先擦后写）到指定地址（addresss）】
void fmc_uint64_program(uint32_t address, uint64_t data)
{
	fmc_erase_pages(address, 1);
    fmc_unlock();
	fmc_doubleword_program(address, data);
	fmc_flags_clear();    
    fmc_lock();
}

//【把bin数据（arr[]）烧写到指定地址（addresss）】
void fmc_program(uint32_t address, uint64_t arr[])
{
    fmc_unlock();

	for(uint8_t i = 0; i<4; i++)
	{
		fmc_doubleword_program(address, arr[i]);
		address += sizeof(uint64_t);
	}
	fmc_flags_clear();
    
    fmc_lock();
}

ErrStatus fmc_program_check(uint32_t address, uint64_t arr[])
{
	uint64_t * ptrd;
	ptrd = (uint64_t *)address;
	for(uint8_t i = 0; i < 4; i++) 
	{
		if((*ptrd) != arr[i]) 
		{
			return ERROR;
		} 
		else 
		{
			ptrd++;
		}
	}
	return SUCCESS;
}

void TEST(void)
{
//    fmc_erase_pages(0x0803C000, 1);
//	  get_Bin64_Data_Array(Bin_Data_Array, Bin64_Data_Array);
//    fmc_program(0x0803C000, Bin64_Data_Array);
}
