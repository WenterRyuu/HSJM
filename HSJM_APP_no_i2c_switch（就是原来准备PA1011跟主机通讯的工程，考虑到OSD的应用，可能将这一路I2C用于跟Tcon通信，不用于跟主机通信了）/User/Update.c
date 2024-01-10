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
//	IRQ_LOW_DOWN;//拉低中断通知主机来读
//	Counter_1ms.IRQ_Update = 100;
//}

void Notice_master_to_read(uint8_t *source) 
{
    memcpy(Update_tI2cSlave.Send_Buff, source, sizeof(Std_Receive_Arr.RequestBootloaderAccess));
	Update_tI2cSlave.SendSize = 7;
	IRQ_LOW_DOWN;//拉低中断通知主机来读
	Counter_1ms.IRQ_Update = 100;	
	
}
void Update_Process(void)
{

	switch(BootloaderStatus)
	{
		case RequestBootloaderAccess:
			if(compareArrays(Update_tI2cSlave.RecBuff, Std_Receive_Arr.RequestBootloaderAccess, sizeof(Std_Receive_Arr.RequestBootloaderAccess)) == true)
			{
				rcu_periph_clock_disable(RCU_TIMER5);//升级过程中不允许读心跳包和查询背光值，防止心跳包和背光值的数据覆盖了升级指令的内容，一旦进入一次，主机就不需要读心跳包
				rcu_periph_clock_disable(RCU_TIMER6);
				Notice_master_to_read(Std_Replay_Arr.RequestBootloaderAccess);
				BootloaderStatus = InputAccessKey1;
			}
			break;
		case InputAccessKey1:                                                   //主机若是一直发错，或者不发，显示屏有卡死的风险
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
				rcu_periph_clock_enable(RCU_TIMER5);//READY以后允许读心跳包和查询背光值
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
	rcu_periph_clock_disable(RCU_TIMER5);//升级过程中不允许读心跳包和查询背光值，防止心跳包和背光值的数据覆盖了升级指令的内容，一旦进入一次，主机就不需要读心跳包
	rcu_periph_clock_disable(RCU_TIMER6);
	Notice_master_to_read(Std_Replay_Arr.RequestBootloaderAccess);
	BootloaderStatus = InputAccessKey1;
	do
	{
		Update_Process();
	}
	while(*pBL_State != (uint64_t)READY);
	
//不用了，	delay_1ms(200);
	if(*pBL_State == (uint64_t)READY)
	{
		//跳转
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
		//主机检查状态
		switch(BootLoader_State)//是什么状态返回什么数组，跳转到对应的步骤
		{
			case READY:
				memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.Check_Ready, sizeof(Std_Replay_Arr.Check_Ready));
				Update_tI2cSlave.SendSize = 11;
				IRQ_LOW_DOWN;//拉低中断通知主机来读
				Counter_1ms.IRQ_Update = 100;
				//BootloaderStatus = EraseAppArea;
				break;
/*APP部分的代码，不存在以下几种状态
//			case BLANK:
//				memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.Check_Blank, sizeof(Std_Replay_Arr.Check_Blank));
//				Update_tI2cSlave.SendSize = 11;
//				IRQ_LOW_DOWN;//拉低中断通知主机来读
//				Counter_1ms.IRQ_Update = 100;
//				BootloaderStatus = StartProgramming;
//				break;
//			case PROGRAMMING://这个和下面的 PROGRAMMING_CHECK 状态，主机查询到的都是对应同一个回复
//				memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.Check_Programming, sizeof(Std_Replay_Arr.Check_Programming));
//				Update_tI2cSlave.SendSize = 11;
//				IRQ_LOW_DOWN;//拉低中断通知主机来读
//				Counter_1ms.IRQ_Update = 100;
//				BootloaderStatus = ReceiveS19;
//				break;
//			case PROGRAMMING_CHECK:
//				memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.Check_Programming, sizeof(Std_Replay_Arr.Check_Programming));
//				Update_tI2cSlave.SendSize = 11;
//				IRQ_LOW_DOWN;//拉低中断通知主机来读
//				Counter_1ms.IRQ_Update = 100;
//				BootloaderStatus = Verify_Flash_Checksum;
//				break;
//			case PROGRAMMING_COMPLETED:
//				memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.Programming_Completed, sizeof(Std_Replay_Arr.Programming_Completed));
//				Update_tI2cSlave.SendSize = 11;
//				IRQ_LOW_DOWN;//拉低中断通知主机来读
//				Counter_1ms.IRQ_Update = 100;
//				BootloaderStatus = BL_Reset;
//				break;
//			case WAITING_FOR_RESET:
//				//复位Bootloader标志清除，保存到flash
//				BootLoader_State = IDLE;
//				fmc_uint64_program(UPDATE_FLAG_ADDRESS, BootLoader_State);
//				//返回给主机状态
//				memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.Waiting_for_Reset, sizeof(Std_Replay_Arr.Waiting_for_Reset));
//				Update_tI2cSlave.SendSize = 11;
//				IRQ_LOW_DOWN;//拉低中断通知主机来读
//				Counter_1ms.IRQ_Update = 100;
//				BootloaderStatus = End;							
//				break;	
*/
			default:
				break;
		}
	}
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
