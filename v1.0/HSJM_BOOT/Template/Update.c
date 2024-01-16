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
	.Check_Ready = {0x5A, 0x06, 0x0B, 0x06, 0xB0, 0x02, 0x01, 0x01, 0x00, 0x00, 0x25}, //Bootloader 当前处于 Ready 状态，表示 Bootloader 进入成功
	.Check_Blank = {0x5A, 0x06, 0x0B, 0x06, 0xB0, 0x02, 0x01, 0x02, 0x00, 0x00, 0x26},//Bootloader 当前处于 Blank 状态，表示擦除成功
	.Check_Programming = {0x5A, 0x06, 0x0B, 0x06, 0xB0, 0x02, 0x01, 0x03, 0x00, 0x00, 0x27}, //Bootloader 当前处于 Programming 状态，表示进入编程模式成功
	.Line_Programming_Succeed = {0x5A, 0x06, 0x0B, 0x06, 0xB0, 0x03, 0x01, 0x00, 0x00, 0x00, 0x25},//一行写入成功
	.Line_Programming_Failed = {0x5A, 0x06, 0x0B, 0x06, 0xB0, 0x03, 0xFF, 0x00, 0x00, 0x00, 0x23},//一行写入失败
	.Programming_Completed = {0x5A, 0x06, 0x0B, 0x06, 0xB0, 0x02, 0x01, 0x04, 0x00, 0x00, 0x28},//Bootloader 当前处于 Programming_Completed 状态，表示擦除成功
	.Waiting_for_Reset = {0x5A, 0x06, 0x0B, 0x06, 0xB0, 0x02, 0x01, 0x05, 0x00, 0x00, 0x29},//Bootloader 当前处于 Waiting_for_Reset 状态，表示等待复位
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
			fmc_uint64_program(UPDATE_FLAG_ADDRESS, BootLoader_State);//保存当前的Bootloader状态标志到Flash升级标志存储区中
			BootloaderStatus = RequestBootloaderAccess;
		case RequestBootloaderAccess:
			if(Counter_1ms.Timeout_in_Updating > 0)
			{
				if(compareArrays(Update_tI2cSlave.RecBuff, Std_Receive_Arr.RequestBootloaderAccess, sizeof(Std_Receive_Arr.RequestBootloaderAccess)) == true)
				{
					//准备好要发送的数据，每重装一次数据，要重置.SendSize
					memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.RequestBootloaderAccess, sizeof(Std_Replay_Arr.RequestBootloaderAccess));
					Update_tI2cSlave.SendSize = 7;
					IRQ_LOW_DOWN;//拉低中断通知主机来读
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
					//准备好要发送的数据，每重装一次数据，要重置.SendSize
					memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.InputAccessKey1, sizeof(Std_Replay_Arr.InputAccessKey1));
					Update_tI2cSlave.SendSize = 7;
					IRQ_LOW_DOWN;//拉低中断通知主机来读
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
					//准备好要发送的数据，每重装一次数据，要重置.SendSize
					memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.InputAccessKey2, sizeof(Std_Replay_Arr.InputAccessKey2));
					Update_tI2cSlave.SendSize = 7;
					IRQ_LOW_DOWN;//拉低中断通知主机来读
					Counter_1ms.IRQ_Update = 100;
					//保存BL状态到Flash
					BootLoader_State = READY;
					fmc_uint64_program(UPDATE_FLAG_ADDRESS, BootLoader_State);	
					//为下一个状态错准备
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
					//主机检查状态
					switch(*pBL_State)//是什么状态返回什么数组，跳转到对应的步骤
					{
						case READY:
							memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.Check_Ready, sizeof(Std_Replay_Arr.Check_Ready));
							Update_tI2cSlave.SendSize = 11;
							IRQ_LOW_DOWN;//拉低中断通知主机来读
							Counter_1ms.IRQ_Update = 100;
							BootloaderStatus = EraseAppArea;
							break;
						case BLANK:
							memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.Check_Blank, sizeof(Std_Replay_Arr.Check_Blank));
							Update_tI2cSlave.SendSize = 11;
							IRQ_LOW_DOWN;//拉低中断通知主机来读
							Counter_1ms.IRQ_Update = 100;
							BootloaderStatus = StartProgramming;
							break;
						case PROGRAMMING://这个和下面的 PROGRAMMING_CHECK 状态，主机查询到的都是对应同一个回复
							memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.Check_Programming, sizeof(Std_Replay_Arr.Check_Programming));
							Update_tI2cSlave.SendSize = 11;
							IRQ_LOW_DOWN;//拉低中断通知主机来读
							Counter_1ms.IRQ_Update = 100;
							BootloaderStatus = ReceiveS19;
							break;
						case PROGRAMMING_CHECK:
							memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.Check_Programming, sizeof(Std_Replay_Arr.Check_Programming));
							Update_tI2cSlave.SendSize = 11;
							IRQ_LOW_DOWN;//拉低中断通知主机来读
							Counter_1ms.IRQ_Update = 100;
							BootloaderStatus = Verify_Flash_Checksum;
							break;
						case PROGRAMMING_COMPLETED:
							memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.Programming_Completed, sizeof(Std_Replay_Arr.Programming_Completed));
							Update_tI2cSlave.SendSize = 11;
							IRQ_LOW_DOWN;//拉低中断通知主机来读
							Counter_1ms.IRQ_Update = 100;
							BootloaderStatus = BL_Reset;
							break;
						case WAITING_FOR_RESET:
							//复位Bootloader标志清除，保存到flash
							BootLoader_State = IDLE;
							fmc_uint64_program(UPDATE_FLAG_ADDRESS, BootLoader_State);
							//返回给主机状态
							memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.Waiting_for_Reset, sizeof(Std_Replay_Arr.Waiting_for_Reset));
							Update_tI2cSlave.SendSize = 11;
							IRQ_LOW_DOWN;//拉低中断通知主机来读
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
					//MCU擦除APP分区
					fmc_erase_pages(APP_START_ADDRESS, APP_PAGE_NUM);
					BootLoader_State = BLANK;//擦完了变更状态
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
					BootLoader_State = PROGRAMMING;//切换为编程状态
					fmc_uint64_program(UPDATE_FLAG_ADDRESS, BootLoader_State);//保存状态
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
					memset(S19_Fire.Rec_Valid_Array, 0, sizeof(S19_Fire.Rec_Valid_Array));//从IIC接收新的数据之前需要清空自身的数组
					memcpy(S19_Fire.Rec_Valid_Array, &Update_tI2cSlave.RecBuff, Update_tI2cSlave.RecCount) ;
					Update_tI2cSlave.RecCount = 0x00 ;
					if((S19_Fire.Rec_Valid_Array[0] == 0x53)&&(S19_Fire.Rec_Valid_Array[1] == 0x03))//校验数据头“S3”
					{
						Calculate_Sum_of_Line(S19_Fire.Rec_Valid_Array, &S19_Fire.LineSum, &S19_Fire.LineSum_LSB);
						if(S19_Fire.LineSum_LSB != S19_Fire.Rec_Valid_Array[S19_Fire.EndIndex])//行校验失败
						{
							S19_Fire.LineSum = 0;
							S19_Fire.LineSum_LSB = 0;
							//此处应该清空S19_Fire.Rec_Valid_Array
							for(uint8_t i = 0; i <= S19_Fire.EndIndex; i++)
							{
								S19_Fire.Rec_Valid_Array[i] = 0;
							}
							//return 1;//这里应该返回校验失败的信息
							break;
						}
						else//校验数据头成功
						{
							S19_Fire.LineSum = 0;
							S19_Fire.LineSum_LSB = 0;
							if(SaveProc() == SUCCESS)//把数据存到APP区里面，并且检查没有错误
							{
								//计算校验数据，在下一个状态会用到
								Calculate_Sum_of_Data(S19_Fire.Rec_Valid_Array, &S19_Fire.AllDataSum);
								//准备好要发送的数据，每重装一次数据，要重置.SendSize
								memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.Line_Programming_Succeed, sizeof(Std_Replay_Arr.Line_Programming_Succeed));
								Update_tI2cSlave.SendSize = 11;
								IRQ_LOW_DOWN;//拉低中断通知主机来读
								Counter_1ms.IRQ_Update = 100;
								break;
							}
							else
							{								
								memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.Line_Programming_Failed, sizeof(Std_Replay_Arr.Line_Programming_Failed));
								Update_tI2cSlave.SendSize = 11;
								IRQ_LOW_DOWN;//拉低中断通知主机来读
								Counter_1ms.IRQ_Update = 100;
								break;
							}							
						}
					}
					else if((S19_Fire.Rec_Valid_Array[0]==0x53) && (S19_Fire.Rec_Valid_Array[1]==0x07))
					{						
						//最后一包数据，返回“结束标志？”
						S19_Fire.LineSum = 0;
						S19_Fire.LineSum_LSB = 0;
						BootLoader_State = PROGRAMMING_CHECK;//Bootloader 进入检查状态（主机查询到的还是编程中（PROGRAMMING）状态）
						BootloaderStatus = Verify_Flash_Checksum;
						break;
					}
					else
					{
						S19_Fire.LineSum = 0;
						S19_Fire.LineSum_LSB = 0;
						//返回“非法数据？”
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
						BootLoader_State = PROGRAMMING_COMPLETED;//切换为编程完成状态
						fmc_uint64_program(UPDATE_FLAG_ADDRESS, BootLoader_State);//保存状态
						BootloaderStatus = Waiting_100ms;
						break;
					}
					else
						break;
				}
				else
					break;
			}
		case BL_Reset://用于检测显示屏是否升级完成，正常复位，如无响应则复位成功
			if(Counter_1ms.Timeout_in_Updating > 0)
			{
				if(compareArrays(Update_tI2cSlave.RecBuff, Std_Receive_Arr.BootloaderReset, sizeof(Std_Receive_Arr.BootloaderReset)) == true)
				{
					//IDLE就是复位成功了，保存到flash
					BootLoader_State = IDLE;
					fmc_uint64_program(UPDATE_FLAG_ADDRESS, BootLoader_State);
					fmc_uint64_program(APP2BOOT_FLAG_ADDRESS, 0);
					BootloaderStatus = End;
					
					//在这里就可以直接跳转到APP程序了
					rcu_periph_clock_disable(RCU_GPIOA);
					rcu_periph_clock_disable(RCU_I2C0);
					rcu_periph_clock_disable(RCU_TIMER1);
					gpio_deinit(GPIOA);
					i2c_deinit(I2C0);
					timer_deinit(TIMER1);
					rcu_deinit();

//					if (((*(__IO uint32_t*)APP_START_ADDRESS) & 0x2FFE0000 ) == 0x20000000)    //ApplicationAddress为新程序的起始地址，检查栈顶地址是否合法，即栈顶地址是否为0x2000xxxx（内置SRAM）
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
					//没有复位成功，那么每200ms主机要查一次状态，对应查到的应该是“等待复位”，查完以后，就到下一个状态（End）里面复位
					//BootLoader_State = WAITING_FOR_RESET;
					//BootloaderStatus = Waiting_200ms;
					break;
				}			
			}
			else
				break;	
		case End:
			////在这里就可以直接跳转到APP程序了
			//。。。。。。
//			if (((*(__IO uint32_t*)APP_START_ADDRESS) & 0x2FFE0000 ) == 0x20000000)    //ApplicationAddress为新程序的起始地址，检查栈顶地址是否合法，即栈顶地址是否为0x2000xxxx（内置SRAM）
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
		cx();//升级过程中都可以查询
		//检测到新的开始
		if(compareArrays(Update_tI2cSlave.RecBuff, Std_Receive_Arr.RequestBootloaderAccess, sizeof(Std_Receive_Arr.RequestBootloaderAccess)) == true)
		{
			//擦除标志
			fmc_uint64_program(UPDATE_FLAG_ADDRESS,0);
			fmc_uint64_program(APP2BOOT_FLAG_ADDRESS,0);
			//准备好要发送的数据，每重装一次数据，要重置.SendSize
			memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.RequestBootloaderAccess, sizeof(Std_Replay_Arr.RequestBootloaderAccess));
			Update_tI2cSlave.SendSize = 7;
			IRQ_LOW_DOWN;//拉低中断通知主机来读
			Counter_1ms.IRQ_Update = 100;
			BootloaderStatus = InputAccessKey1;
			//直接退出，视为主机直接放弃本次升级
			break;
		}
	}
	while(BootloaderStatus != End);
}



//比较两个数组是否完全相同
bool compareArrays(uint8_t arr1[], uint8_t arr2[], uint8_t size) 
{
    for (int i = 0; i < size; i++) 
	{
        if (arr1[i] != arr2[i]) 
		{
            return false; // 如果在任何位置找到不同的元素，返回false
        }
    }
    return true; // 所有元素都相同，返回true
}

/*
向Flash写数据
调用示例：SaveProc(tUpdata.PackSize) ;
*/
static ErrStatus SaveProc(void)
{
	//BIN文件初始化全为0XFF
	memset(S19_Fire.Bin_Data_Array, 0xFF, sizeof(S19_Fire.Bin_Data_Array));
	//提取出Data，不足的还是0xFF
	get_Bin_Data_Array(S19_Fire.Rec_Valid_Array);
	get_Bin64_Data_Array(S19_Fire.Bin_Data_Array, S19_Fire.Bin64_Data_Array);
	//重新把BIN文件初始化全为0XFF
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
函数功能：从RecBuff数组中寻找最后一个不是0的元素的位置，然后将从RecBuff[0]到该位置的所有元素复制到Rec_Valid_Array数组中 
调用示例：get_Rec_Valid_Array(Update_tI2cSlave.RecBuff);
*/
void get_Rec_Valid_Array(uint8_t arr[]) 
{
    // 初始化EndIndex
    S19_Fire.EndIndex = 49;
    // 从最后一个元素往前找，找到第一个非零元素
    while (S19_Fire.EndIndex >= 0 && arr[S19_Fire.EndIndex] == 0) 
	{
        --S19_Fire.EndIndex;
    }
    // 如果找到了非零元素，将RecBuff中包含非零及之前的所有元素复制到Rec_Valid_Array
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
函数功能：从S19_Fire.Rec_Valid_Array数组中寻找最后一个不是0的元素的位置，然后将从第8个到倒数第2个之间的所有元素复制到S19_Fire.Bin_Data_Array数组中 
调用示例：get_Rec_Valid_Array(S19_Fire.Rec_Valid_Array);
*/
void get_Bin_Data_Array(uint8_t arr[]) 
{
    // 初始化EndIndex
    S19_Fire.EndIndex = 49;
    // 从最后一个元素往前找，找到第一个非零元素
    while (S19_Fire.EndIndex >= 0 && arr[S19_Fire.EndIndex] == 0) 
	{
        --S19_Fire.EndIndex;
    }
	//把接收到的S3记录里面第8个数到倒数第二给数之间的数赋值到Bin_Data_Array，真正要写入到APP区里面的这个数组
	for(uint8_t i = 0; i <= (S19_Fire.EndIndex-8); ++i) 
	{
		S19_Fire.Bin_Data_Array[i] = arr[i+7];
	}
}
/* 
函数功能：计算Rec_Valid_Array数组中指定范围内元素的总和（从第8个元素到倒数第2个元素）并更新到AllDataSum变量 
调用示例：Calculate_Sum_of_Data(Rec_Valid_Array, &AllDataSum);
*/
void Calculate_Sum_of_Data(uint8_t arr[], uint64_t *sum) 
{
    //*sum = 0; // 初始化总和
    S19_Fire.EndIndex = 49;
    // 找到Rec_Valid_Array中的最后一个非零元素
    while (S19_Fire.EndIndex >= 0 && arr[S19_Fire.EndIndex] == 0) 
	{
        --S19_Fire.EndIndex;
    }
    // 从第8个元素开始，到倒数第2个元素结束，计算总和
    uint8_t i = 0;
    for (i = 7; i <= S19_Fire.EndIndex-1; ++i) 
	{
        *sum += arr[i];
    }
	//*sum_lsb = *sum & 0xFFFF;//所有Data(真正的烧录文件)的校验
}


/* 
函数功能：把 Rec_Valid_Array 数组里面，从第3个元素到倒数第2个元素之间的所有元素（包括首尾）相加，把结果存储到 LineSum 里面。
调用示例：Calculate_Sum_of_Line(Rec_Valid_Array, &LineSum);
*/
void Calculate_Sum_of_Line(uint8_t arr[], uint32_t *sum, uint8_t *sum_lsb) 
{
    S19_Fire.EndIndex = 49;
    // 找到Rec_Valid_Array中的最后一个非零元素
    while (S19_Fire.EndIndex >= 0 && arr[S19_Fire.EndIndex] == 0) 
	{
        --S19_Fire.EndIndex;
    }
    // 从第3个元素开始，到倒数第2个元素结束，计算总和
    uint8_t i = 0;
    for (i = 2; i <= S19_Fire.EndIndex-1; ++i) 
	{
        *sum += arr[i];
    }
    *sum_lsb = ~(*sum & 0xFF);//行校验的数据
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
		//主机检查状态
		switch(*pBL_State)
		//switch(BootLoader_State)//是什么状态返回什么数组，跳转到对应的步骤
		{
			case READY:
				memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.Check_Ready, sizeof(Std_Replay_Arr.Check_Ready));
				Update_tI2cSlave.SendSize = 11;
				IRQ_LOW_DOWN;//拉低中断通知主机来读
				Counter_1ms.IRQ_Update = 100;
				BootloaderStatus = EraseAppArea;
				break;
			case BLANK:
				memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.Check_Blank, sizeof(Std_Replay_Arr.Check_Blank));
				Update_tI2cSlave.SendSize = 11;
				IRQ_LOW_DOWN;//拉低中断通知主机来读
				Counter_1ms.IRQ_Update = 100;
				BootloaderStatus = StartProgramming;
				break;
			case PROGRAMMING://这个和下面的 PROGRAMMING_CHECK 状态，主机查询到的都是对应同一个回复
				memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.Check_Programming, sizeof(Std_Replay_Arr.Check_Programming));
				Update_tI2cSlave.SendSize = 11;
				IRQ_LOW_DOWN;//拉低中断通知主机来读
				Counter_1ms.IRQ_Update = 100;
				BootloaderStatus = ReceiveS19;
				break;
			case PROGRAMMING_CHECK:
				memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.Check_Programming, sizeof(Std_Replay_Arr.Check_Programming));
				Update_tI2cSlave.SendSize = 11;
				IRQ_LOW_DOWN;//拉低中断通知主机来读
				Counter_1ms.IRQ_Update = 100;
				BootloaderStatus = Verify_Flash_Checksum;
				break;
			case PROGRAMMING_COMPLETED:
				memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.Programming_Completed, sizeof(Std_Replay_Arr.Programming_Completed));
				Update_tI2cSlave.SendSize = 11;
				IRQ_LOW_DOWN;//拉低中断通知主机来读
				Counter_1ms.IRQ_Update = 100;
				BootloaderStatus = BL_Reset;
				break;
			case WAITING_FOR_RESET:
				//复位Bootloader标志清除，保存到flash
				BootLoader_State = IDLE;
				fmc_uint64_program(UPDATE_FLAG_ADDRESS, BootLoader_State);
				//返回给主机状态
				memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.Waiting_for_Reset, sizeof(Std_Replay_Arr.Waiting_for_Reset));
				Update_tI2cSlave.SendSize = 11;
				IRQ_LOW_DOWN;//拉低中断通知主机来读
				Counter_1ms.IRQ_Update = 100;
				BootloaderStatus = End;							
				break;						
			default:
				break;
		}
	}
}