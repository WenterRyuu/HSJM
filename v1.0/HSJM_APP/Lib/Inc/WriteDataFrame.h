#ifndef _WRITEDATAFRAME_H_
#define _WRITEDATAFRAME_H_
#include "gd32a50x.h"

#define UDS_SIGNAL_TYPE     0x00
#define UDS_FIRST_TYPE     0x01
#define UDS_CONTINOUS_TYPE     0x02
#define EXT_FRAME_WRITE_DATA_MAX_LENGTH  (0xFF-0x03)
#define MAX_DATA_LENGTH		0xFF

#define CHECKSUMERROR    0x01


#define STD_FRAME_WRITE_DATA_LENGTH 0x09




typedef struct
{
    uint8_t WriteCmd_Id;
    uint8_t WriteData[STD_FRAME_WRITE_DATA_LENGTH];
    uint8_t ExtLength;
    uint8_t checksum;
} Std_WriteDataFrameStr;

typedef struct
{
    uint8_t WriteCmd_Id;
    uint8_t WriteData[MAX_DATA_LENGTH];
    uint8_t ExtLength;
    uint8_t checksum;
} Ext_WriteDataFrameStr;


typedef struct
{
    uint8_t LcdCheckCmd;
}WrCmd_0x81_LcdInformationCheck;

typedef struct
{
    uint8_t TimeoutValue;
    uint8_t TickTimeValue;
}WrCmd_0x82_SetFrameTimeoutValue;

typedef struct
{
    uint8_t BackLightValue;
    uint8_t Reserved;
    uint8_t BackLightSwitch;
}WrCmd_0x83_BackLightModeConfigure;

typedef struct
{
    uint8_t ResetTouchChip;
}WrCmd_0x84_TouchChipResetCmd;

typedef struct
{
    uint8_t TimeStamp1;
    uint8_t TimeStamp2;
    uint8_t TimeStamp3;
    uint8_t TimeStamp4;
}WrCmd_0x85_TimeStamp;

typedef struct
{
    uint8_t FogSwitchStatus;
}WrCmd_0x86_OpenCloseLcd;

typedef struct
{
    uint8_t BackLightSwitchGrade;
}WrCmd_0x87_BackLightSwitchCmd;


typedef struct
{
    uint8_t DiagnosisRequest;
    uint8_t DiagnosisAddress;
    uint8_t FrameFormat1;
    uint8_t FrameFormat2;
    uint8_t data[5];
}WrCmd_0xF0_UpdateCmd;

typedef struct
{
    uint8_t Part_1st_digit;
    uint8_t Part_2nd_digit;
    uint8_t Part_3rd_digit;
    uint8_t Part_4th_digit;    
    uint8_t Part_5th_digit;
}WrCmd_0x90_PartNumberWriteCmd;

typedef struct
{
    uint8_t Part_6th_digit;
    uint8_t Part_7th_digit;
    uint8_t Part_8th_digit;
    uint8_t Part_9th_digit;    
    uint8_t Part_10th_digit;
}WrCmd_0x91_PartNumberWriteCmd;

typedef struct
{
    uint8_t HWversion_before_dot;
    uint8_t HWversion_after_dot;
    uint8_t HWversion_Revision;
    uint8_t HWversion_Year;    
    uint8_t HWversion_Month;
    uint8_t HWversion_Day;
}WrCmd_0x92_HWversionWriteCmd;

typedef struct
{
	uint8_t ExtF1ActiveFlag;
    uint8_t VaildDataArrLenght;
    uint8_t DiagnosisRequest;
    uint8_t DiagnosisAddress;
    uint8_t FrameFormat;
    uint8_t DataArr[EXT_FRAME_WRITE_DATA_MAX_LENGTH-3];
}WrCmd_0xF1_ExtUpdateCmd;

typedef struct
{
	uint8_t ExtF2ActiveFlag;
    uint8_t VaildDataArrLenght;
    uint8_t DiagnosisRequest;
    uint8_t DiagnosisAddress;
    uint8_t FrameFormat;
    uint8_t DataArr[EXT_FRAME_WRITE_DATA_MAX_LENGTH-3];
}WrCmd_0xF2_ExtUpdateCmd;

typedef struct
{
	uint8_t ExtF3ActiveFlag;
    uint8_t VaildDataArrLenght;
    uint8_t DiagnosisRequest;
    uint8_t DiagnosisAddress;
    uint8_t FrameFormat;
    uint8_t DataArr[EXT_FRAME_WRITE_DATA_MAX_LENGTH-3];
}WrCmd_0xF3_ExtUpdateCmd;

typedef struct
{
    uint8_t DiagnosisRequest;
    uint8_t DiagnosisAddress;
    uint8_t UdsFrameType;
    uint8_t VaildUdsDataLength;
    uint8_t SingleFrame_ServiceId;
    uint8_t SingleFrame_SubId;
    uint8_t SingleFrameData[4]; 
}DiagnosisSingleFrameStr;


typedef struct
{
    uint8_t DiagnosisRequest;
    uint8_t DiagnosisAddress;
    uint8_t UdsFrameType;
    uint16_t VaildUdsDataLength;
    uint8_t FirstFrameData[5]; 
}DiagnosisFirstFrameStr;



extern void WriteCmdIdStrInit();
extern uint8_t ChecksumCheck(uint8_t * CheckArr);
extern void StdWriteFrameDataWrite(Std_WriteDataFrameStr *DataFrameStr, uint8_t *ReceiveDataArr);
extern void ExtWriteFrameDataWrite(Ext_WriteDataFrameStr *DataFrameStr, uint8_t *ReceiveDataArr,uint8_t Vaildlength);
extern void LcdInformationCheckFun(Std_WriteDataFrameStr *DataFrameStr);
extern void SetFrameTimeoutValueFun(Std_WriteDataFrameStr *DataFrameStr);
extern void BackLightModeConfigureFun(Std_WriteDataFrameStr *DataFrameStr);
extern void TouchChipResetCmdFun(Std_WriteDataFrameStr *DataFrameStr);
extern void TimeStampFun(Std_WriteDataFrameStr *DataFrameStr);
extern void OpenCloseLcdFun(Std_WriteDataFrameStr *DataFrameStr);
extern void BackLightSwitchCmdFun(Std_WriteDataFrameStr *DataFrameStr);
extern void UpdateCmdFun(Std_WriteDataFrameStr *DataFrameStr);
extern void ExtUpdateCmd1Fun(Ext_WriteDataFrameStr *DataFrameStr);
extern void ExtUpdateCmd2Fun(Ext_WriteDataFrameStr *DataFrameStr);
extern void ExtUpdateCmd3Fun(Ext_WriteDataFrameStr *DataFrameStr);
extern uint8_t WriteFrameTransmit();
extern uint8_t MasterTransmitData[MAX_DATA_LENGTH];

extern ErrStatus Part_Number_Write(void);
extern ErrStatus HW_VERSION_Write(void);

#endif
