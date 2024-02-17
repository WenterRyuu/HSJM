#ifndef _UPDATE_H_
#define _UPDATE_H_


#include "Define.h"

typedef void (*pFunction)(void);
extern pFunction Jump_To_Application;
extern uint32_t JumpAddress;

typedef enum{
	IDLE=0, 
	READY, 
	BLANK, 
	PROGRAMMING, 
	PROGRAMMING_CHECK,
	PROGRAMMING_COMPLETED,
	WAITING_FOR_RESET
}_BootLoader_State;//主机查询Bootloader状态枚举
extern uint64_t BootLoader_State;

typedef struct{
	uint8_t RequestBootloaderAccess[7];
	uint8_t InputAccessKey1[9];
	uint8_t InputAccessKey2[9];
	uint8_t QueryBootloaderStatus[11];
	uint8_t EraseAppArea[11];
	uint8_t StartProgramming[11];
	uint8_t BootloaderReset[11];
}_Std_Receive_Arr;//标准接收数组
extern const _Std_Receive_Arr Std_Receive_Arr;

typedef struct{
	uint8_t RequestBootloaderAccess[7];
	uint8_t InputAccessKey1[7];
	uint8_t InputAccessKey2[7];
	uint8_t Check_Ready[11];
	uint8_t Check_Blank[11];
	uint8_t Check_Programming[11];
	uint8_t Programming_Completed[11];
	uint8_t Line_Programming_Succeed[11];
	uint8_t Line_Programming_Failed[11];
	uint8_t Waiting_for_Reset[11];
}_Std_Replay_Arr;//标准回复数组
extern const _Std_Replay_Arr Std_Replay_Arr;

typedef struct{
	uint16_t Timeout_in_Updating;
	uint16_t Waiting_in_Updating;
	uint8_t IRQ_Update;
}_Counter_1ms;
extern _Counter_1ms Counter_1ms;

typedef struct{
	uint8_t Rec_Valid_Array[50];
	uint8_t Bin_Data_Array[32];
	uint64_t Bin64_Data_Array[4];
	uint64_t AllDataSum;
	uint16_t AllDataSum_LSB;
	uint32_t LineSum;
	uint8_t LineSum_LSB;
	uint8_t EndIndex;
	uint16_t AllDataSum_LSB_in_CMD;
}_S19Fire;//文件烧录及校验部分的结构体
extern _S19Fire S19_Fire;

typedef enum {
	Start = 0,
    RequestBootloaderAccess, // 申请进入Bootloader权限
    InputAccessKey1,         // 输入Access Key 1
    InputAccessKey2,         // 输入Access Key 2
	Waiting_2s,
	Waiting_1s,
	Waiting_100ms,
	Waiting_200ms,
    QueryBootloaderStatus,   // 查询Bootloader状态
    EraseAppArea,            // 擦除APP区域
    StartProgramming,        // 开始编程
	ReceiveS19,
	Verify_Flash_Checksum,
	BL_Reset,
	End
}_BootloaderStatus;
extern _BootloaderStatus BootloaderStatus;


void Update_Process(void);
void Updating(void);
bool compareArrays(uint8_t arr1[], uint8_t arr2[], uint8_t size);
static ErrStatus SaveProc(void);
void get_Rec_Valid_Array(uint8_t arr[]) ;
void get_Bin_Data_Array(uint8_t arr[]) ;
void Calculate_Sum_of_Data(uint8_t arr[], uint64_t *sum);
void Calculate_Sum_of_Line(uint8_t arr[], uint32_t *sum, uint8_t *sum_lsb) ;








#endif