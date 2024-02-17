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
	.Programming_Completed = {0x5A, 0x06, 0x0B, 0x06, 0xB0, 0x02, 0x01, 0x04, 0x00, 0x00, 0x28},//Bootloader ��ǰ���� Programming_Completed ״̬����ʾ�����ɹ�
	.Waiting_for_Reset = {0x5A, 0x06, 0x0B, 0x06, 0xB0, 0x02, 0x01, 0x05, 0x00, 0x00, 0x29},//Bootloader ��ǰ���� Waiting_for_Reset ״̬����ʾ�ȴ���λ
}; 


_Counter_1ms Counter_1ms;
tI2c_Type Update_tI2cSlave;	
uint64_t BootLoader_State = IDLE;
_BootloaderStatus BootloaderStatus;
pFunction Jump_To_Application;
uint32_t JumpAddress = 0;
extern uint64_t *pBL_State;
extern uint64_t *pApp_Once;

tI2c_Type Update_tI2cSlave={
//	.RecBuff = {
//		0x53, 0x03, 0x1D, 0x00, 0x00, 0x51, 0x60, 
//		0x85, 0x10, 0x00, 0x08, 0x08, 0x00, 0x00, 0x20, 
//		0xD8, 0x0E, 0x00, 0x69, 0x00, 
//		0xAC
//	},
	.Send_Buff = {0x11, 0x22, 0x33},
	.SendSize = 20,
};


//-----------------------------------------------------------------------------------------------------------------------------------------------

void Update_Process(void)
{
	switch(BootloaderStatus)
	{
		case Start:
			fmc_uint64_program(UPDATE_FLAG_ADDRESS, BootLoader_State);//���浱ǰ��Bootloader״̬��־��Flash������־�洢����
			BootloaderStatus = RequestBootloaderAccess;
		case RequestBootloaderAccess:
			if(Counter_1ms.Timeout_in_Updating > 0)
			{
				if(compareArrays(Update_tI2cSlave.RecBuff, Std_Receive_Arr.RequestBootloaderAccess, sizeof(Std_Receive_Arr.RequestBootloaderAccess)) == true)
				{
					//׼����Ҫ���͵����ݣ�ÿ��װһ�����ݣ�Ҫ����.SendSize
					memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.RequestBootloaderAccess, sizeof(Std_Replay_Arr.RequestBootloaderAccess));
					Update_tI2cSlave.SendSize = 7;
					IRQ_LOW_DOWN;//�����ж�֪ͨ��������
					Counter_1ms.IRQ_Update = 100;
					BootloaderStatus = InputAccessKey1;
				}
				else
					break;				
			}
			else
				break;
		case InputAccessKey1:
			if(Counter_1ms.Timeout_in_Updating > 0)
			{
				if(compareArrays(Update_tI2cSlave.RecBuff, Std_Receive_Arr.InputAccessKey1, sizeof(Std_Receive_Arr.InputAccessKey1)) == true)
				{
					//׼����Ҫ���͵����ݣ�ÿ��װһ�����ݣ�Ҫ����.SendSize
					memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.InputAccessKey1, sizeof(Std_Replay_Arr.InputAccessKey1));
					Update_tI2cSlave.SendSize = 7;
					IRQ_LOW_DOWN;//�����ж�֪ͨ��������
					Counter_1ms.IRQ_Update = 100;
					BootloaderStatus = InputAccessKey2;
				}
				else
					break;
			}
			else
				break;
		case InputAccessKey2:
			if(Counter_1ms.Timeout_in_Updating > 0)
			{
				if(compareArrays(Update_tI2cSlave.RecBuff, Std_Receive_Arr.InputAccessKey2, sizeof(Std_Receive_Arr.InputAccessKey2)) == true)
				{				
					//׼����Ҫ���͵����ݣ�ÿ��װһ�����ݣ�Ҫ����.SendSize
					memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.InputAccessKey2, sizeof(Std_Replay_Arr.InputAccessKey2));
					Update_tI2cSlave.SendSize = 7;
					IRQ_LOW_DOWN;//�����ж�֪ͨ��������
					Counter_1ms.IRQ_Update = 100;
					//����BL״̬��Flash
					BootLoader_State = READY;
					fmc_uint64_program(UPDATE_FLAG_ADDRESS, BootLoader_State);	
					//Ϊ��һ��״̬��׼��
					Counter_1ms.Waiting_in_Updating = 2000;
					BootloaderStatus = Waiting_2s;
				}
				else
					break;
			}
			else
				break;
		case Waiting_2s:
			if(Counter_1ms.Waiting_in_Updating == 0)
			{
				BootloaderStatus = QueryBootloaderStatus;
				break;
			}
			else
				break;
		case QueryBootloaderStatus:
			if(Counter_1ms.Timeout_in_Updating > 0)
			{
				if(compareArrays(Update_tI2cSlave.RecBuff, Std_Receive_Arr.QueryBootloaderStatus, sizeof(Std_Receive_Arr.QueryBootloaderStatus)) == true)
				{
					//�������״̬
					switch(*pBL_State)//��ʲô״̬����ʲô���飬��ת����Ӧ�Ĳ���
					{
						case READY:
							memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.Check_Ready, sizeof(Std_Replay_Arr.Check_Ready));
							Update_tI2cSlave.SendSize = 11;
							IRQ_LOW_DOWN;//�����ж�֪ͨ��������
							Counter_1ms.IRQ_Update = 100;
							BootloaderStatus = EraseAppArea;
							break;
						case BLANK:
							memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.Check_Blank, sizeof(Std_Replay_Arr.Check_Blank));
							Update_tI2cSlave.SendSize = 11;
							IRQ_LOW_DOWN;//�����ж�֪ͨ��������
							Counter_1ms.IRQ_Update = 100;
							BootloaderStatus = StartProgramming;
							break;
						case PROGRAMMING://���������� PROGRAMMING_CHECK ״̬��������ѯ���Ķ��Ƕ�Ӧͬһ���ظ�
							memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.Check_Programming, sizeof(Std_Replay_Arr.Check_Programming));
							Update_tI2cSlave.SendSize = 11;
							IRQ_LOW_DOWN;//�����ж�֪ͨ��������
							Counter_1ms.IRQ_Update = 100;
							BootloaderStatus = ReceiveS19;
							break;
						case PROGRAMMING_CHECK:
							memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.Check_Programming, sizeof(Std_Replay_Arr.Check_Programming));
							Update_tI2cSlave.SendSize = 11;
							IRQ_LOW_DOWN;//�����ж�֪ͨ��������
							Counter_1ms.IRQ_Update = 100;
							BootloaderStatus = Verify_Flash_Checksum;
							break;
						case PROGRAMMING_COMPLETED:
							memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.Programming_Completed, sizeof(Std_Replay_Arr.Programming_Completed));
							Update_tI2cSlave.SendSize = 11;
							IRQ_LOW_DOWN;//�����ж�֪ͨ��������
							Counter_1ms.IRQ_Update = 100;
							BootloaderStatus = BL_Reset;
							break;
						case WAITING_FOR_RESET:
							//��λBootloader��־��������浽flash
							BootLoader_State = IDLE;
							fmc_uint64_program(UPDATE_FLAG_ADDRESS, BootLoader_State);
							//���ظ�����״̬
							memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.Waiting_for_Reset, sizeof(Std_Replay_Arr.Waiting_for_Reset));
							Update_tI2cSlave.SendSize = 11;
							IRQ_LOW_DOWN;//�����ж�֪ͨ��������
							Counter_1ms.IRQ_Update = 100;
							BootloaderStatus = End;							
							break;						
						default:
							break;
					}
					break;
				}
				else
					break;
			}
			else
				break;		
		case EraseAppArea:
			if(Counter_1ms.Timeout_in_Updating > 0)
			{
				if(compareArrays(Update_tI2cSlave.RecBuff, Std_Receive_Arr.EraseAppArea, sizeof(Std_Receive_Arr.EraseAppArea)) == true)
				{
					//MCU����APP����
					fmc_erase_pages(APP_START_ADDRESS, APP_PAGE_NUM);
					BootLoader_State = BLANK;//�����˱��״̬
					fmc_uint64_program(UPDATE_FLAG_ADDRESS, BootLoader_State);
					Counter_1ms.Waiting_in_Updating = 1000;
					BootloaderStatus = Waiting_1s;
					break;
				}
				else
					break;
			}
			else
				break;	
		case Waiting_1s:
			if(Counter_1ms.Waiting_in_Updating == 0)
			{
				BootloaderStatus = QueryBootloaderStatus;
				break;
			}
			else
				break;
		case StartProgramming:
			if(Counter_1ms.Timeout_in_Updating > 0)
			{
				if(compareArrays(Update_tI2cSlave.RecBuff, Std_Receive_Arr.StartProgramming, sizeof(Std_Receive_Arr.StartProgramming)) == true)
				{
					BootLoader_State = PROGRAMMING;//�л�Ϊ���״̬
					fmc_uint64_program(UPDATE_FLAG_ADDRESS, BootLoader_State);//����״̬
					Counter_1ms.Waiting_in_Updating = 100;
					BootloaderStatus = Waiting_100ms;
				}
				else
					break;				
			}
			else
				break;
		case Waiting_100ms:
			if(Counter_1ms.Waiting_in_Updating == 0)
			{
				BootloaderStatus = QueryBootloaderStatus;
				break;
			}
			else
				break;
		case Waiting_200ms:
			if(Counter_1ms.Waiting_in_Updating == 0)
			{
				BootloaderStatus = QueryBootloaderStatus;
				break;
			}
			else
				break;
		case ReceiveS19:
			if(Counter_1ms.Timeout_in_Updating > 0)
			{
				if(Update_tI2cSlave.uFlag.Bits.RecSuccess)
				{
					Update_tI2cSlave.uFlag.Bits.RecSuccess = 0 ;	
					memset(S19_Fire.Rec_Valid_Array, 0, sizeof(S19_Fire.Rec_Valid_Array));//��IIC�����µ�����֮ǰ��Ҫ������������
					memcpy(S19_Fire.Rec_Valid_Array, &Update_tI2cSlave.RecBuff, Update_tI2cSlave.RecCount) ;
					Update_tI2cSlave.RecCount = 0x00 ;
					if((S19_Fire.Rec_Valid_Array[0] == 0x53)&&(S19_Fire.Rec_Valid_Array[1] == 0x03))//У������ͷ��S3��
					{
						Calculate_Sum_of_Line(S19_Fire.Rec_Valid_Array, &S19_Fire.LineSum, &S19_Fire.LineSum_LSB);
						if(S19_Fire.LineSum_LSB != S19_Fire.Rec_Valid_Array[S19_Fire.EndIndex])//��У��ʧ��
						{
							S19_Fire.LineSum = 0;
							S19_Fire.LineSum_LSB = 0;
							//�˴�Ӧ�����S19_Fire.Rec_Valid_Array
							for(uint8_t i = 0; i <= S19_Fire.EndIndex; i++)
							{
								S19_Fire.Rec_Valid_Array[i] = 0;
							}
							//return 1;//����Ӧ�÷���У��ʧ�ܵ���Ϣ
							break;
						}
						else//У������ͷ�ɹ�
						{
							S19_Fire.LineSum = 0;
							S19_Fire.LineSum_LSB = 0;
							if(SaveProc() == SUCCESS)//�����ݴ浽APP�����棬���Ҽ��û�д���
							{
								//����У�����ݣ�����һ��״̬���õ�
								Calculate_Sum_of_Data(S19_Fire.Rec_Valid_Array, &S19_Fire.AllDataSum);
								//׼����Ҫ���͵����ݣ�ÿ��װһ�����ݣ�Ҫ����.SendSize
								memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.Line_Programming_Succeed, sizeof(Std_Replay_Arr.Line_Programming_Succeed));
								Update_tI2cSlave.SendSize = 11;
								IRQ_LOW_DOWN;//�����ж�֪ͨ��������
								Counter_1ms.IRQ_Update = 100;
								break;
							}
							else
							{								
								memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.Line_Programming_Failed, sizeof(Std_Replay_Arr.Line_Programming_Failed));
								Update_tI2cSlave.SendSize = 11;
								IRQ_LOW_DOWN;//�����ж�֪ͨ��������
								Counter_1ms.IRQ_Update = 100;
								break;
							}							
						}
					}
					else if((S19_Fire.Rec_Valid_Array[0]==0x53) && (S19_Fire.Rec_Valid_Array[1]==0x07))
					{						
						//���һ�����ݣ����ء�������־����
						S19_Fire.LineSum = 0;
						S19_Fire.LineSum_LSB = 0;
						BootLoader_State = PROGRAMMING_CHECK;//Bootloader ������״̬��������ѯ���Ļ��Ǳ���У�PROGRAMMING��״̬��
						BootloaderStatus = Verify_Flash_Checksum;
						break;
					}
					else
					{
						S19_Fire.LineSum = 0;
						S19_Fire.LineSum_LSB = 0;
						//���ء��Ƿ����ݣ���
						break;
					}
				}
				else
					break;
			}
			else
				break;
		case Verify_Flash_Checksum:
			if(Counter_1ms.Timeout_in_Updating > 0)
			{
				if(Update_tI2cSlave.uFlag.Bits.RecSuccess)
				{
					Update_tI2cSlave.uFlag.Bits.RecSuccess = 0 ;
					
					memcpy(S19_Fire.Rec_Valid_Array, &Update_tI2cSlave.RecBuff, Update_tI2cSlave.RecCount) ;
					Update_tI2cSlave.RecCount = 0x00 ;
					S19_Fire.AllDataSum_LSB_in_CMD = ((S19_Fire.Rec_Valid_Array[7]<<8) | S19_Fire.Rec_Valid_Array[8]);
					S19_Fire.AllDataSum_LSB = S19_Fire.AllDataSum & 0XFFFF;
					if(S19_Fire.AllDataSum_LSB == S19_Fire.AllDataSum_LSB_in_CMD)
					{					
						BootLoader_State = PROGRAMMING_COMPLETED;//�л�Ϊ������״̬
						fmc_uint64_program(UPDATE_FLAG_ADDRESS, BootLoader_State);//����״̬
						BootloaderStatus = Waiting_100ms;
						break;
					}
					else
						break;
				}
				else
					break;
			}
		case BL_Reset://���ڼ����ʾ���Ƿ�������ɣ�������λ��������Ӧ��λ�ɹ�
			if(Counter_1ms.Timeout_in_Updating > 0)
			{
				if(compareArrays(Update_tI2cSlave.RecBuff, Std_Receive_Arr.BootloaderReset, sizeof(Std_Receive_Arr.BootloaderReset)) == true)
				{
					//IDLE���Ǹ�λ�ɹ��ˣ����浽flash
					BootLoader_State = IDLE;
					fmc_uint64_program(UPDATE_FLAG_ADDRESS, BootLoader_State);
					fmc_uint64_program(APP2BOOT_FLAG_ADDRESS, 0);
					BootloaderStatus = End;
					
					//������Ϳ���ֱ����ת��APP������
					rcu_periph_clock_disable(RCU_GPIOA);
					rcu_periph_clock_disable(RCU_I2C0);
					rcu_periph_clock_disable(RCU_TIMER1);
					gpio_deinit(GPIOA);
					i2c_deinit(I2C0);
					timer_deinit(TIMER1);
					rcu_deinit();

//					if (((*(__IO uint32_t*)APP_START_ADDRESS) & 0x2FFE0000 ) == 0x20000000)    //ApplicationAddressΪ�³������ʼ��ַ�����ջ����ַ�Ƿ�Ϸ�����ջ����ַ�Ƿ�Ϊ0x2000xxxx������SRAM��
//					{
						JumpAddress = *(__IO uint32_t*)(APP_START_ADDRESS + 4);
						Jump_To_Application = (pFunction)JumpAddress;
						__set_MSP(*(__IO uint32_t*) APP_START_ADDRESS);
						Jump_To_Application();
						break;
//					}
				}
				else
				{
					//û�и�λ�ɹ�����ôÿ200ms����Ҫ��һ��״̬����Ӧ�鵽��Ӧ���ǡ��ȴ���λ���������Ժ󣬾͵���һ��״̬��End�����渴λ
					//BootLoader_State = WAITING_FOR_RESET;
					//BootloaderStatus = Waiting_200ms;
					break;
				}			
			}
			else
				break;	
		case End:
			////������Ϳ���ֱ����ת��APP������
			//������������
//			if (((*(__IO uint32_t*)APP_START_ADDRESS) & 0x2FFE0000 ) == 0x20000000)    //ApplicationAddressΪ�³������ʼ��ַ�����ջ����ַ�Ƿ�Ϸ�����ջ����ַ�Ƿ�Ϊ0x2000xxxx������SRAM��
//			{
				JumpAddress = *(__IO uint32_t*)(APP_START_ADDRESS + 4);
				Jump_To_Application = (pFunction)JumpAddress;
				__set_MSP(*(__IO uint32_t*) APP_START_ADDRESS);
				Jump_To_Application();
//			}
			break;
		default:
			break;
	}
	
}

void cx(void);
void Updating(void)
{
	do
	{
		Update_Process();
		cx();//���������ж����Բ�ѯ
		//��⵽�µĿ�ʼ
		if(compareArrays(Update_tI2cSlave.RecBuff, Std_Receive_Arr.RequestBootloaderAccess, sizeof(Std_Receive_Arr.RequestBootloaderAccess)) == true)
		{
			//������־
			fmc_uint64_program(UPDATE_FLAG_ADDRESS,0);
			fmc_uint64_program(APP2BOOT_FLAG_ADDRESS,0);
			//׼����Ҫ���͵����ݣ�ÿ��װһ�����ݣ�Ҫ����.SendSize
			memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.RequestBootloaderAccess, sizeof(Std_Replay_Arr.RequestBootloaderAccess));
			Update_tI2cSlave.SendSize = 7;
			IRQ_LOW_DOWN;//�����ж�֪ͨ��������
			Counter_1ms.IRQ_Update = 100;
			BootloaderStatus = InputAccessKey1;
			//ֱ���˳�����Ϊ����ֱ�ӷ�����������
			break;
		}
	}
	while(BootloaderStatus != End);
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
����ʾ����SaveProc(tUpdata.PackSize) ;
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
	if(is_check_ok)
	{
		start_address += 0x20;
		return SUCCESS;
	}
	else
		return ERROR;	
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

void cx(void)
{
	if(compareArrays(Update_tI2cSlave.RecBuff, Std_Receive_Arr.QueryBootloaderStatus, sizeof(Std_Receive_Arr.QueryBootloaderStatus)) == true)
	{
		//�������״̬
		switch(*pBL_State)
		//switch(BootLoader_State)//��ʲô״̬����ʲô���飬��ת����Ӧ�Ĳ���
		{
			case READY:
				memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.Check_Ready, sizeof(Std_Replay_Arr.Check_Ready));
				Update_tI2cSlave.SendSize = 11;
				IRQ_LOW_DOWN;//�����ж�֪ͨ��������
				Counter_1ms.IRQ_Update = 100;
				BootloaderStatus = EraseAppArea;
				break;
			case BLANK:
				memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.Check_Blank, sizeof(Std_Replay_Arr.Check_Blank));
				Update_tI2cSlave.SendSize = 11;
				IRQ_LOW_DOWN;//�����ж�֪ͨ��������
				Counter_1ms.IRQ_Update = 100;
				BootloaderStatus = StartProgramming;
				break;
			case PROGRAMMING://���������� PROGRAMMING_CHECK ״̬��������ѯ���Ķ��Ƕ�Ӧͬһ���ظ�
				memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.Check_Programming, sizeof(Std_Replay_Arr.Check_Programming));
				Update_tI2cSlave.SendSize = 11;
				IRQ_LOW_DOWN;//�����ж�֪ͨ��������
				Counter_1ms.IRQ_Update = 100;
				BootloaderStatus = ReceiveS19;
				break;
			case PROGRAMMING_CHECK:
				memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.Check_Programming, sizeof(Std_Replay_Arr.Check_Programming));
				Update_tI2cSlave.SendSize = 11;
				IRQ_LOW_DOWN;//�����ж�֪ͨ��������
				Counter_1ms.IRQ_Update = 100;
				BootloaderStatus = Verify_Flash_Checksum;
				break;
			case PROGRAMMING_COMPLETED:
				memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.Programming_Completed, sizeof(Std_Replay_Arr.Programming_Completed));
				Update_tI2cSlave.SendSize = 11;
				IRQ_LOW_DOWN;//�����ж�֪ͨ��������
				Counter_1ms.IRQ_Update = 100;
				BootloaderStatus = BL_Reset;
				break;
			case WAITING_FOR_RESET:
				//��λBootloader��־��������浽flash
				BootLoader_State = IDLE;
				fmc_uint64_program(UPDATE_FLAG_ADDRESS, BootLoader_State);
				//���ظ�����״̬
				memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.Waiting_for_Reset, sizeof(Std_Replay_Arr.Waiting_for_Reset));
				Update_tI2cSlave.SendSize = 11;
				IRQ_LOW_DOWN;//�����ж�֪ͨ��������
				Counter_1ms.IRQ_Update = 100;
				BootloaderStatus = End;							
				break;						
			default:
				break;
		}
	}
}