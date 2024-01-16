/*******************************************************************************
 * Project	:������ĻMCU����_BOE�ɶ�
 * MCU	  	:GD32A503KCU3
 * Data		:2023/10/23
 * Files	:Backlight.c
 * Library	:V1.1.0, firmware for GD32A50x
 * Function	:Flash������д��صĺ���
 * Author	:Liu Wei
 * Phone	:13349168457
 ******************************************************************************/
 #include "fmc_App.h"


/*------------------------------------------------------------------------------
*Function name		 :get_Bin64_Data_Array
*Function description:uint8_t����ϲ���uint64_t�������棬С�˶���
*Ipunt				 :��1��ԭ���顾2��Ŀ������
*OutPut				 :none
*-----------------------------------------------------------------------------*/
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
                //С��ģʽ��, the least significant byte is placed in the lowest address
                combined |= (uint64_t)arr[index] << (j * 8);
            }
        }
        Bin64_Data_Array[i] = combined;
    }
}


/*------------------------------------------------------------------------------
*Function name		 :fmc_flags_clear
*Function description:���Flash������״̬
*Ipunt				 :��1��ԭ���顾2��Ŀ������
*OutPut				 :none
*-----------------------------------------------------------------------------*/
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


/*------------------------------------------------------------------------------
*Function name		 :fmc_flags_clear
*Function description:�����ӣ�start_address����ʼ�ģ�page_num����ô��ҳ
*Ipunt				 :��1��start_address��2��page_num
*OutPut				 :none
*-----------------------------------------------------------------------------*/
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

/*------------------------------------------------------------------------------
*Function name		 :fmc_uint64_program
*Function description:дһ��˫�֣�data����д���Ȳ���д����ָ����ַ��addresss��
*Ipunt				 :��1��address��2��data
*OutPut				 :none
*-----------------------------------------------------------------------------*/
void fmc_uint64_program(uint32_t address, uint64_t data)
{
	fmc_erase_pages(address, 1);
    fmc_unlock();
	fmc_doubleword_program(address, data);
	fmc_flags_clear();    
    fmc_lock();
}

/*------------------------------------------------------------------------------
*Function name		 :fmc_program
*Function description:��bin���ݣ�arr[]����д��ָ����ַ��addresss��
*Ipunt				 :��1��address��2��arr[]
*OutPut				 :none
*-----------------------------------------------------------------------------*/
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

/*------------------------------------------------------------------------------
*Function name		 :fmc_program_check
*Function description:���uint64_t�����ݣ�arr[]���Ƿ�ɹ�д���Ӧ��ַ��address��
*Ipunt				 :��1��address��2��data
*OutPut				 :none
*-----------------------------------------------------------------------------*/
ErrStatus fmc_program_check(uint32_t address, uint64_t arr[])
{
	uint64_t * ptrd;
	ptrd = (uint64_t *)address;
	for(uint8_t i = 0; i < 4; i++) //һ�����ֻд��4��˫�֣�����ֻѭ��4��
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

//NO USED
void TEST(void)
{
//    fmc_erase_pages(0x0803C000, 1);
//	  get_Bin64_Data_Array(Bin_Data_Array, Bin64_Data_Array);
//    fmc_program(0x0803C000, Bin64_Data_Array);
}
