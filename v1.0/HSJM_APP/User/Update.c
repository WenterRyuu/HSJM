#include "Update.h"



const _Std_Receive_Arr Std_Receive_Arr = {
	.RequestBootloaderAccess = {0x5A, 0x05, 0x07, 0x02, 0x31, 0x01, 0x9A},
	.InputAccessKey1 = {0x5A, 0x05, 0x07, 0x04, 0x31, 0x03, 0xA5, 0x5A, 0x9D},
	.InputAccessKey2 = {0x5A, 0x05, 0x07, 0x04, 0x31, 0x04, 0xC3, 0x3C, 0x9E},
	.QueryBootloaderStatus = {0x5A, 0x05, 0x0B, 0x06, 0xB0, 0x01, 0x01, 0x00, 0x00, 0x00, 0x22},
	.EraseAppArea = {0x5A, 0x05, 0x0B, 0x06, 0xB0, 0x01, 0x02, 0x00, 0x00, 0x00, 0x23},
	.StartProgramming = {0x5A, 0x05, 0x0B, 0x06, 0xB0, 0x01, 0x03, 0x00, 0x00, 0x00, 0x24},
	.BootloaderReset = {0x5A, 0x05, 0x0B, 0x06, 0xB0, 0x01, 0x05, 0x00, 0x00, 0x00, 0x26},
};



const _Std_Replay_Arr Std_Replay_Arr = {
	.RequestBootloaderAccess = {0x5A, 0x06, 0x07, 0x02, 0x31, 0x01, 0x9B},
	.InputAccessKey1 = {0x5A, 0x06, 0x07, 0x02, 0x31, 0x03, 0x9D},
	.InputAccessKey2 = {0x5A, 0x06, 0x07, 0x02, 0x31, 0x04, 0x9E},
	.Check_Ready = {0x5A, 0x06, 0x0B, 0x06, 0xB0, 0x02, 0x01, 0x01, 0x00, 0x00, 0x25}, //Bootloader ��ǰ���� Ready ״̬����ʾ Bootloader ����ɹ�
	.Check_Blank = {0x5A, 0x06, 0x0B, 0x06, 0xB0, 0x02, 0x01, 0x02, 0x00, 0x00, 0x26},//Bootloader ��ǰ���� Blank ״̬����ʾ�����ɹ�
	.Check_Programming = {0x5A, 0x06, 0x0B, 0x06, 0xB0, 0x02, 0x01, 0x03, 0x00, 0x00, 0x27}, //Bootloader ��ǰ���� Programming ״̬����ʾ������ģʽ�ɹ�
	.Line_Programming_Succeed = {0x5A, 0x06, 0x0B, 0x06, 0xB0, 0x03, 0x01, 0x00, 0x00, 0x00, 0x25},//һ��д��ɹ�
	.Line_Programming_Failed = {0x5A, 0x06, 0x0B, 0x06, 0xB0, 0x03, 0xFF, 0x00, 0x00, 0x00, 0x23},//һ��д��ʧ��
	.Programming_Completed = {0x5A, 0x06, 0x0B, 0x06, 0xB0, 0x02, 0x01, 0x04, 0x00, 0x00, 0x28},
	.Waiting_for_Reset = {0x5A, 0x06, 0x0B, 0x06, 0xB0, 0x02, 0x01, 0x05, 0x00, 0x00, 0x29},
}; 


tI2c_Type Update_tI2cSlave;	
uint64_t BootLoader_State = IDLE;
_BootloaderStatus BootloaderStatus = RequestBootloaderAccess;
pFunction Jump_To_Bootloader;
uint32_t JumpAddress = 0;




//-----------------------------------------------------------------------------------------------------------------------------------------------
//void Notice_master_to_read(uint8_t *arr)
//{
//	memcpy(Update_tI2cSlave.Send_Buff, arr, sizeof(&arr));
//	Update_tI2cSlave.SendSize = 7;
//	IRQ_LOW_DOWN;//�����ж�֪ͨ��������
//	Counter_1ms.IRQ_Update = 100;
//}

void Notice_master_to_read(uint8_t *source) 
{
    memcpy(Update_tI2cSlave.Send_Buff, source, sizeof(Std_Receive_Arr.RequestBootloaderAccess));
	Update_tI2cSlave.SendSize = 7;
	IRQ_LOW_DOWN;//�����ж�֪ͨ��������
	Counter_1ms.IRQ_Update = 100;	
	
}
void Update_Process(void)
{

	switch(BootloaderStatus)
	{
		case RequestBootloaderAccess:
			if(compareArrays(Update_tI2cSlave.RecBuff, Std_Receive_Arr.RequestBootloaderAccess, sizeof(Std_Receive_Arr.RequestBootloaderAccess)) == true)
			{
				rcu_periph_clock_disable(RCU_TIMER5);//���������в�������������Ͳ�ѯ����ֵ����ֹ�������ͱ���ֵ�����ݸ���������ָ������ݣ�һ������һ�Σ������Ͳ���Ҫ��������
				rcu_periph_clock_disable(RCU_TIMER6);
				Notice_master_to_read(Std_Replay_Arr.RequestBootloaderAccess);
				BootloaderStatus = InputAccessKey1;
			}
			break;
		case InputAccessKey1:                                                   //��������һֱ�������߲�������ʾ���п����ķ���
			if(compareArrays(Update_tI2cSlave.RecBuff, Std_Receive_Arr.InputAccessKey1, sizeof(Std_Receive_Arr.InputAccessKey2)) == true)
			{
				Notice_master_to_read(Std_Replay_Arr.InputAccessKey1);
				BootloaderStatus = InputAccessKey2;
			}
			break;
		case InputAccessKey2:
			if(compareArrays(Update_tI2cSlave.RecBuff, Std_Receive_Arr.InputAccessKey2, sizeof(Std_Receive_Arr.InputAccessKey2)) == true)
			{
				Notice_master_to_read(Std_Replay_Arr.InputAccessKey2);
				BootloaderStatus = QueryBootloaderStatus;
				BootLoader_State = READY;
				fmc_uint64_program(UPDATE_FLAG_ADDRESS, BootLoader_State);
				fmc_uint64_program(APP2BOOT_FLAG_ADDRESS, 1);
				rcu_periph_clock_enable(RCU_TIMER5);//READY�Ժ�������������Ͳ�ѯ����ֵ
				rcu_periph_clock_enable(RCU_TIMER6);
				break;
			}
			break;
		default:
			break;
		
	}
}

uint64_t *pBL_State = (uint64_t *)UPDATE_FLAG_ADDRESS;
uint64_t *pApp_Once = (uint64_t *)APP2BOOT_FLAG_ADDRESS;
void Updating(void)
{	
	rcu_periph_clock_disable(RCU_TIMER5);//���������в�������������Ͳ�ѯ����ֵ����ֹ�������ͱ���ֵ�����ݸ���������ָ������ݣ�һ������һ�Σ������Ͳ���Ҫ��������
	rcu_periph_clock_disable(RCU_TIMER6);
	Notice_master_to_read(Std_Replay_Arr.RequestBootloaderAccess);
	BootloaderStatus = InputAccessKey1;
	do
	{
		Update_Process();
	}
	while(*pBL_State != (uint64_t)READY);
	
//�����ˣ�	delay_1ms(200);
	if(*pBL_State == (uint64_t)READY)
	{
		//��ת
		rcu_deinit();                                               
		gpio_deinit(GPIOA);
		gpio_deinit(GPIOB);
		gpio_deinit(GPIOC);
		gpio_deinit(GPIOD);
		gpio_deinit(GPIOE);
		timer_deinit(TIMER1);
		timer_deinit(TIMER5);
		timer_deinit(TIMER6);
		timer_deinit(TIMER19);
		i2c_deinit(I2C0);
		i2c_deinit(I2C1);
		adc_deinit(ADC0);
		spi_i2s_deinit(SPI0);
		McuRcuDeinit();
		//Systick?
        
        
		JumpAddress = *(__IO uint32_t*)(BOOT_START_ADDRESS + 4);
		Jump_To_Bootloader = (pFunction)JumpAddress;
		__set_MSP(*(__IO uint32_t*) BOOT_START_ADDRESS);
		Jump_To_Bootloader();		
	}
}


void cx(void)
{
	if(compareArrays(Update_tI2cSlave.RecBuff, Std_Receive_Arr.QueryBootloaderStatus, sizeof(Std_Receive_Arr.QueryBootloaderStatus)) == true)
	{
		//�������״̬
		switch(BootLoader_State)//��ʲô״̬����ʲô���飬��ת����Ӧ�Ĳ���
		{
			case READY:
				memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.Check_Ready, sizeof(Std_Replay_Arr.Check_Ready));
				Update_tI2cSlave.SendSize = 11;
				IRQ_LOW_DOWN;//�����ж�֪ͨ��������
				Counter_1ms.IRQ_Update = 100;
				//BootloaderStatus = EraseAppArea;
				break;
/*APP���ֵĴ��룬���������¼���״̬
//			case BLANK:
//				memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.Check_Blank, sizeof(Std_Replay_Arr.Check_Blank));
//				Update_tI2cSlave.SendSize = 11;
//				IRQ_LOW_DOWN;//�����ж�֪ͨ��������
//				Counter_1ms.IRQ_Update = 100;
//				BootloaderStatus = StartProgramming;
//				break;
//			case PROGRAMMING://���������� PROGRAMMING_CHECK ״̬��������ѯ���Ķ��Ƕ�Ӧͬһ���ظ�
//				memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.Check_Programming, sizeof(Std_Replay_Arr.Check_Programming));
//				Update_tI2cSlave.SendSize = 11;
//				IRQ_LOW_DOWN;//�����ж�֪ͨ��������
//				Counter_1ms.IRQ_Update = 100;
//				BootloaderStatus = ReceiveS19;
//				break;
//			case PROGRAMMING_CHECK:
//				memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.Check_Programming, sizeof(Std_Replay_Arr.Check_Programming));
//				Update_tI2cSlave.SendSize = 11;
//				IRQ_LOW_DOWN;//�����ж�֪ͨ��������
//				Counter_1ms.IRQ_Update = 100;
//				BootloaderStatus = Verify_Flash_Checksum;
//				break;
//			case PROGRAMMING_COMPLETED:
//				memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.Programming_Completed, sizeof(Std_Replay_Arr.Programming_Completed));
//				Update_tI2cSlave.SendSize = 11;
//				IRQ_LOW_DOWN;//�����ж�֪ͨ��������
//				Counter_1ms.IRQ_Update = 100;
//				BootloaderStatus = BL_Reset;
//				break;
//			case WAITING_FOR_RESET:
//				//��λBootloader��־��������浽flash
//				BootLoader_State = IDLE;
//				fmc_uint64_program(UPDATE_FLAG_ADDRESS, BootLoader_State);
//				//���ظ�����״̬
//				memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.Waiting_for_Reset, sizeof(Std_Replay_Arr.Waiting_for_Reset));
//				Update_tI2cSlave.SendSize = 11;
//				IRQ_LOW_DOWN;//�����ж�֪ͨ��������
//				Counter_1ms.IRQ_Update = 100;
//				BootloaderStatus = End;							
//				break;	
*/
			default:
				break;
		}
	}
}

//�Ƚ����������Ƿ���ȫ��ͬ
bool compareArrays(uint8_t arr1[], uint8_t arr2[], uint8_t size) 
{
    for (int i = 0; i < size; i++) 
	{
        if (arr1[i] != arr2[i]) 
		{
            return false; // ������κ�λ���ҵ���ͬ��Ԫ�أ�����false
        }
    }
    return true; // ����Ԫ�ض���ͬ������true
}

/*
��Flashд����
*/
static ErrStatus SaveProc(void)
{
	//BIN�ļ���ʼ��ȫΪ0XFF
	memset(S19_Fire.Bin_Data_Array, 0xFF, sizeof(S19_Fire.Bin_Data_Array));
	//��ȡ��Data������Ļ���0xFF
	get_Bin_Data_Array(S19_Fire.Rec_Valid_Array);
	get_Bin64_Data_Array(S19_Fire.Bin_Data_Array, S19_Fire.Bin64_Data_Array);
	//���°�BIN�ļ���ʼ��ȫΪ0XFF
	memset(S19_Fire.Bin_Data_Array, 0xFF, sizeof(S19_Fire.Bin_Data_Array));
	
	static uint32_t start_address = APP_START_ADDRESS;
	fmc_program(start_address, S19_Fire.Bin64_Data_Array);
	
	bool is_check_ok = fmc_program_check(start_address, S19_Fire.Bin64_Data_Array);
	
	start_address += 0x20;
	return is_check_ok;
}

//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------

_S19Fire S19_Fire;

/* 
�������ܣ���RecBuff������Ѱ�����һ������0��Ԫ�ص�λ�ã�Ȼ�󽫴�RecBuff[0]����λ�õ�����Ԫ�ظ��Ƶ�Rec_Valid_Array������ 
����ʾ����get_Rec_Valid_Array(Update_tI2cSlave.RecBuff);
*/
void get_Rec_Valid_Array(uint8_t arr[]) 
{
    // ��ʼ��EndIndex
    S19_Fire.EndIndex = 49;
    // �����һ��Ԫ����ǰ�ң��ҵ���һ������Ԫ��
    while (S19_Fire.EndIndex >= 0 && arr[S19_Fire.EndIndex] == 0) 
	{
        --S19_Fire.EndIndex;
    }
    // ����ҵ��˷���Ԫ�أ���RecBuff�а������㼰֮ǰ������Ԫ�ظ��Ƶ�Rec_Valid_Array
    if (S19_Fire.EndIndex >= 0) 
	{
		uint8_t i = 0;
        for (i = 0; i <= S19_Fire.EndIndex; ++i) 
		{
            S19_Fire.Rec_Valid_Array[i] = arr[i];
        }
    }
}

/* 
�������ܣ���S19_Fire.Rec_Valid_Array������Ѱ�����һ������0��Ԫ�ص�λ�ã�Ȼ�󽫴ӵ�8����������2��֮�������Ԫ�ظ��Ƶ�S19_Fire.Bin_Data_Array������ 
����ʾ����get_Rec_Valid_Array(S19_Fire.Rec_Valid_Array);
*/
void get_Bin_Data_Array(uint8_t arr[]) 
{
    // ��ʼ��EndIndex
    S19_Fire.EndIndex = 49;
    // �����һ��Ԫ����ǰ�ң��ҵ���һ������Ԫ��
    while (S19_Fire.EndIndex >= 0 && arr[S19_Fire.EndIndex] == 0) 
	{
        --S19_Fire.EndIndex;
    }
	//�ѽ��յ���S3��¼�����8�����������ڶ�����֮�������ֵ��Bin_Data_Array������Ҫд�뵽APP��������������
	for(uint8_t i = 0; i <= (S19_Fire.EndIndex-8); ++i) 
	{
		S19_Fire.Bin_Data_Array[i] = arr[i+7];
	}
}
/* 
�������ܣ�����Rec_Valid_Array������ָ����Χ��Ԫ�ص��ܺͣ��ӵ�8��Ԫ�ص�������2��Ԫ�أ������µ�AllDataSum���� 
����ʾ����Calculate_Sum_of_Data(Rec_Valid_Array, &AllDataSum);
*/
void Calculate_Sum_of_Data(uint8_t arr[], uint64_t *sum) 
{
    //*sum = 0; // ��ʼ���ܺ�
    S19_Fire.EndIndex = 49;
    // �ҵ�Rec_Valid_Array�е����һ������Ԫ��
    while (S19_Fire.EndIndex >= 0 && arr[S19_Fire.EndIndex] == 0) 
	{
        --S19_Fire.EndIndex;
    }
    // �ӵ�8��Ԫ�ؿ�ʼ����������2��Ԫ�ؽ����������ܺ�
    uint8_t i = 0;
    for (i = 7; i <= S19_Fire.EndIndex-1; ++i) 
	{
        *sum += arr[i];
    }
	//*sum_lsb = *sum & 0xFFFF;//����Data(��������¼�ļ�)��У��
}


/* 
�������ܣ��� Rec_Valid_Array �������棬�ӵ�3��Ԫ�ص�������2��Ԫ��֮�������Ԫ�أ�������β����ӣ��ѽ���洢�� LineSum ���档
����ʾ����Calculate_Sum_of_Line(Rec_Valid_Array, &LineSum);
*/
void Calculate_Sum_of_Line(uint8_t arr[], uint32_t *sum, uint8_t *sum_lsb) 
{
    S19_Fire.EndIndex = 49;
    // �ҵ�Rec_Valid_Array�е����һ������Ԫ��
    while (S19_Fire.EndIndex >= 0 && arr[S19_Fire.EndIndex] == 0) 
	{
        --S19_Fire.EndIndex;
    }
    // �ӵ�3��Ԫ�ؿ�ʼ����������2��Ԫ�ؽ����������ܺ�
    uint8_t i = 0;
    for (i = 2; i <= S19_Fire.EndIndex-1; ++i) 
	{
        *sum += arr[i];
    }
    *sum_lsb = ~(*sum & 0xFF);//��У�������
}


//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------
