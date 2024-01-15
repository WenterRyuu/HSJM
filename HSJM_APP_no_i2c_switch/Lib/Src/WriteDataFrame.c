#include "WriteDataFrame.h"
#include "Define.h"
#include "string.h"

extern tI2c_Type tI2cSlave ;
uint8_t MasterTransmitData[MAX_DATA_LENGTH] = {0x00};



// Std_WriteDataFrameStr  WriteDataFrameStr = 
// {
    // 0x00,
    // {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    // 0x00,
    // 0x00
// };


WrCmd_0x81_LcdInformationCheck  LcdInformationCheck;
WrCmd_0x82_SetFrameTimeoutValue SetFrameTimeoutValue;
WrCmd_0x83_BackLightModeConfigure   BackLightModeConfigure;
WrCmd_0x84_TouchChipResetCmd    TouchChipResetCmd;
WrCmd_0x85_TimeStamp    TimeStamp;
WrCmd_0x86_OpenCloseLcd OpenCloseLcd;
WrCmd_0x87_BackLightSwitchCmd   BackLightSwitchCmd;
WrCmd_0xF0_UpdateCmd    UpdateCmd;
WrCmd_0xF1_ExtUpdateCmd ExtUpdateCmd_F1;
WrCmd_0xF2_ExtUpdateCmd ExtUpdateCmd_F2;
WrCmd_0xF3_ExtUpdateCmd ExtUpdateCmd_F3;

DiagnosisSingleFrameStr DiagnosisSingleFrame;
DiagnosisFirstFrameStr  DiagnosisFirstFrame;

void WriteCmdIdStrInit()
{
	uint8_t i = 0;
    /*0x81*/
    LcdInformationCheck.LcdCheckCmd = 0x00;

    /*0x82*/
    SetFrameTimeoutValue.TimeoutValue = 0x0A;
    SetFrameTimeoutValue.TickTimeValue = 0x14;

    /*0x83*/
    BackLightModeConfigure.BackLightValue = 0x00;
    BackLightModeConfigure.Reserved = 0x00;
    BackLightModeConfigure.BackLightSwitch = 0x00;

    /*0x84*/
    TouchChipResetCmd.ResetTouchChip = 0x00;

    /*0x85*/
    TimeStamp.TimeStamp1 = 0x00;
    TimeStamp.TimeStamp2 = 0x00;
    TimeStamp.TimeStamp3 = 0x00;
    TimeStamp.TimeStamp4 = 0x00;

    /*0x86*/
    OpenCloseLcd.FogSwitchStatus = 0x01;

    /*0x87*/
    BackLightSwitchCmd.BackLightSwitchGrade = 0x00;

    /*0xF0*/
    UpdateCmd.DiagnosisRequest = 0x3C;
    UpdateCmd.DiagnosisAddress = 0x00;
    UpdateCmd.FrameFormat1 = 0x00;
    UpdateCmd.FrameFormat2 = 0x00;
    UpdateCmd.data[0] = 0x00;
    UpdateCmd.data[1] = 0x00;
    UpdateCmd.data[2] = 0x00;
    UpdateCmd.data[3] = 0x00;
    UpdateCmd.data[4] = 0x00;

    /*F1*/
    ExtUpdateCmd_F1.DiagnosisRequest = 0x3C;
    ExtUpdateCmd_F1.DiagnosisRequest = 0x28;
    ExtUpdateCmd_F1.FrameFormat = 0x21;
	for(i = 0 ; i < 249; i++)
	{
		ExtUpdateCmd_F1.DataArr[i] = 0x00;
	}
     

    /*F2*/
    ExtUpdateCmd_F2.DiagnosisRequest = 0x3C;
    ExtUpdateCmd_F2.DiagnosisRequest = 0x28;
    ExtUpdateCmd_F2.FrameFormat = 0x22;
    for(i = 0 ; i < 249; i++)
	{
		ExtUpdateCmd_F2.DataArr[i] = 0x00;
	} 
	
	/*F3*/
    ExtUpdateCmd_F3.DiagnosisRequest = 0x3C;
    ExtUpdateCmd_F3.DiagnosisRequest = 0x28;
    ExtUpdateCmd_F3.FrameFormat = 0x22;
    for(i = 0 ; i < 249; i++)
	{
		ExtUpdateCmd_F3.DataArr[i] = 0x00;
	} 
}

//校验正确则返回0
uint8_t ChecksumCheck(uint8_t * CheckArr)
{
    uint8_t i =0;
    uint8_t CheckSumCount = 0;
    for(i = 0; i < 11; i++)
    {
        CheckSumCount += *(CheckArr + i);
    }

    if(CheckSumCount == *(CheckArr + i))
    {
        return 0;
    }
    else
    {
        return 1;
    }
} 


void StdWriteFrameDataWrite(Std_WriteDataFrameStr *DataFrameStr, uint8_t *ReceiveDataArr)
{
    uint8_t i = 0;
    DataFrameStr->WriteCmd_Id =  *ReceiveDataArr;
    for(i = 0; i< STD_FRAME_WRITE_DATA_LENGTH; i++)
    {
        DataFrameStr->WriteData[i] = *(ReceiveDataArr + i + 1);
    }

    DataFrameStr->ExtLength = *(ReceiveDataArr+ i + 1);

    DataFrameStr->checksum = *(ReceiveDataArr+ i + 1 + 1);

}

void ExtWriteFrameDataWrite(Ext_WriteDataFrameStr *DataFrameStr, uint8_t *ReceiveDataArr,uint8_t Vaildlength)
{
    uint8_t i = 0;
    DataFrameStr->WriteCmd_Id =  *ReceiveDataArr;
    for(i = 0; i<Vaildlength;i++)
    {
        DataFrameStr->WriteData[i] = *(ReceiveDataArr + i + 1);
    }

    DataFrameStr->ExtLength = *(ReceiveDataArr+ Vaildlength + 1);

    DataFrameStr->checksum = *(ReceiveDataArr+ Vaildlength + 1 + 1);

}

void LcdInformationCheckFun(Std_WriteDataFrameStr *DataFrameStr)
{
    LcdInformationCheck.LcdCheckCmd =  DataFrameStr->WriteData[0];

    //还需要显示屏返回CMD_ID:0x13,0x14,0x15,0x16
    //return 0x0;
}


void SetFrameTimeoutValueFun(Std_WriteDataFrameStr *DataFrameStr)
{
    SetFrameTimeoutValue.TimeoutValue = DataFrameStr->WriteData[0];
    SetFrameTimeoutValue.TickTimeValue = DataFrameStr->WriteData[1];
}

void BackLightModeConfigureFun(Std_WriteDataFrameStr *DataFrameStr)
{
	
    BackLightModeConfigure.BackLightValue = DataFrameStr->WriteData[0];
	BackLightModeConfigure.Reserved = DataFrameStr->WriteData[1];
    BackLightModeConfigure.BackLightSwitch = DataFrameStr->WriteData[2];	
}

void TouchChipResetCmdFun(Std_WriteDataFrameStr *DataFrameStr)
{
    TouchChipResetCmd.ResetTouchChip = DataFrameStr->WriteData[0];
}

void TimeStampFun(Std_WriteDataFrameStr *DataFrameStr)
{
    TimeStamp.TimeStamp1 = DataFrameStr->WriteData[0];
    TimeStamp.TimeStamp2 = DataFrameStr->WriteData[1];
    TimeStamp.TimeStamp3 = DataFrameStr->WriteData[2];
    TimeStamp.TimeStamp4 = DataFrameStr->WriteData[3];

}

void OpenCloseLcdFun(Std_WriteDataFrameStr *DataFrameStr)
{
    OpenCloseLcd.FogSwitchStatus = DataFrameStr->WriteData[0];
}

void BackLightSwitchCmdFun(Std_WriteDataFrameStr *DataFrameStr)
{
    BackLightSwitchCmd.BackLightSwitchGrade = DataFrameStr->WriteData[0];
}

void UpdateCmdFun(Std_WriteDataFrameStr *DataFrameStr)
{
    UpdateCmd.DiagnosisRequest = DataFrameStr->WriteData[0];
    UpdateCmd.DiagnosisAddress = DataFrameStr->WriteData[1];
    UpdateCmd.FrameFormat1 = DataFrameStr->WriteData[2];
    UpdateCmd.FrameFormat2 = DataFrameStr->WriteData[3];
    UpdateCmd.data[0] = DataFrameStr->WriteData[4];
    UpdateCmd.data[1] = DataFrameStr->WriteData[5];
    UpdateCmd.data[2] = DataFrameStr->WriteData[6];
    UpdateCmd.data[3] = DataFrameStr->WriteData[7];
    UpdateCmd.data[4] = DataFrameStr->WriteData[8];

    if(DataFrameStr->ExtLength >= 0x03)
    {
        ExtUpdateCmd_F1.VaildDataArrLenght = (DataFrameStr->ExtLength - 3);
        ExtUpdateCmd_F1.ExtF1ActiveFlag = 0x01;
    }
    else
    {
        ExtUpdateCmd_F1.VaildDataArrLenght = 0;
        ExtUpdateCmd_F1.ExtF1ActiveFlag = 0x00;
    }
    

    // if(UpdateCmd.DiagnosisRequest == 0x3C)
    // {
        // if((UpdateCmd.DiagnosisAddress == 0x28) || (UpdateCmd.DiagnosisAddress == 0x7E))
        // {
            // if((UpdateCmd.FrameFormat1 >> 4) == UDS_SIGNAL_TYPE)
            // {
                // DiagnosisFirstFrame.DiagnosisRequest = UpdateCmd.DiagnosisRequest;
                // DiagnosisSingleFrame.DiagnosisAddress = UpdateCmd.DiagnosisAddress;
                // DiagnosisFirstFrame.UdsFrameType = (UpdateCmd.FrameFormat1 >> 4);
                // DiagnosisSingleFrame.VaildUdsDataLength = (UpdateCmd.FrameFormat1 & 0x0F);
                // DiagnosisSingleFrame.SingleFrame_ServiceId = UpdateCmd.FrameFormat2;
                // DiagnosisSingleFrame.SingleFrame_SubId = UpdateCmd.data[0];
                // DiagnosisSingleFrame.SingleFrameData[0] = UpdateCmd.data[1];
                // DiagnosisSingleFrame.SingleFrameData[1] = UpdateCmd.data[2];
                // DiagnosisSingleFrame.SingleFrameData[2] = UpdateCmd.data[3];
                // DiagnosisSingleFrame.SingleFrameData[3] = UpdateCmd.data[4];
            // }
            // else if((UpdateCmd.FrameFormat1 >> 4) == UDS_FIRST_TYPE)
            // {
                // DiagnosisFirstFrame.DiagnosisRequest = UpdateCmd.DiagnosisRequest;
                // DiagnosisFirstFrame.DiagnosisRequest = UpdateCmd.DiagnosisAddress;
                // DiagnosisFirstFrame.UdsFrameType = (UpdateCmd.FrameFormat1 >> 4);
                // DiagnosisFirstFrame.VaildUdsDataLength = ((uint16_t)(UpdateCmd.FrameFormat1 & 0x0F)<<8);
                // DiagnosisFirstFrame.VaildUdsDataLength |= (uint16_t)(UpdateCmd.FrameFormat2);
                // DiagnosisFirstFrame.FirstFrameData[0] = UpdateCmd.data[0];
                // DiagnosisFirstFrame.FirstFrameData[1] = UpdateCmd.data[1];
                // DiagnosisFirstFrame.FirstFrameData[2] = UpdateCmd.data[2];
                // DiagnosisFirstFrame.FirstFrameData[3] = UpdateCmd.data[3];
                // DiagnosisFirstFrame.FirstFrameData[4] = UpdateCmd.data[4];
            // }
            // else if((UpdateCmd.FrameFormat1 >> 4) == UDS_CONTINOUS_TYPE)
        // }
    // }

 
}

void ExtUpdateCmd1Fun(Ext_WriteDataFrameStr *DataFrameStr)
{
	uint8_t i = 0;
	
	ExtUpdateCmd_F1.VaildDataArrLenght = 0X00;
    ExtUpdateCmd_F1.ExtF1ActiveFlag = 0x00;
	
    ExtUpdateCmd_F1.DiagnosisRequest =   DataFrameStr->WriteData[0];
    ExtUpdateCmd_F1.DiagnosisAddress =  DataFrameStr->WriteData[1];
    ExtUpdateCmd_F1.FrameFormat = DataFrameStr->WriteData[2];
    for(i = 0 ; i < (ExtUpdateCmd_F1.VaildDataArrLenght - 3) ; i++)
    {
        ExtUpdateCmd_F1.DataArr[i] = DataFrameStr->WriteData[i+3];
    }
    
    if(DataFrameStr->ExtLength >= 0x03)
    {
        ExtUpdateCmd_F2.VaildDataArrLenght = (DataFrameStr->ExtLength - 3);
        ExtUpdateCmd_F2.ExtF2ActiveFlag = 0x01;
    }
    else
    {
        ExtUpdateCmd_F2.VaildDataArrLenght = 0;
        ExtUpdateCmd_F2.ExtF2ActiveFlag = 0x00;
    }
}

void ExtUpdateCmd2Fun(Ext_WriteDataFrameStr *DataFrameStr)
{
	uint8_t i = 0;
	
	ExtUpdateCmd_F2.VaildDataArrLenght = 0X00;
    ExtUpdateCmd_F2.ExtF2ActiveFlag = 0x00;
	
    ExtUpdateCmd_F2.DiagnosisRequest =   DataFrameStr->WriteData[0];
    ExtUpdateCmd_F2.DiagnosisAddress =  DataFrameStr->WriteData[1];
    ExtUpdateCmd_F2.FrameFormat = DataFrameStr->WriteData[2];
    for(i = 0 ; i < (ExtUpdateCmd_F2.VaildDataArrLenght - 3) ; i++)
    {
        ExtUpdateCmd_F2.DataArr[i] = DataFrameStr->WriteData[i+3];
    }
    if(DataFrameStr->ExtLength >= 0x03)
    {
        ExtUpdateCmd_F3.VaildDataArrLenght = (DataFrameStr->ExtLength - 3);
        ExtUpdateCmd_F3.ExtF3ActiveFlag = 0x01;
    }
    else
    {
        ExtUpdateCmd_F3.VaildDataArrLenght = 0;
        ExtUpdateCmd_F3.ExtF3ActiveFlag = 0x00;
    }
}

void ExtUpdateCmd3Fun(Ext_WriteDataFrameStr *DataFrameStr)
{
	uint8_t i = 0;
	
	ExtUpdateCmd_F3.VaildDataArrLenght = 0X00;
    ExtUpdateCmd_F3.ExtF3ActiveFlag = 0x00;
	
    ExtUpdateCmd_F3.DiagnosisRequest =   DataFrameStr->WriteData[0];
    ExtUpdateCmd_F3.DiagnosisAddress =  DataFrameStr->WriteData[1];
    ExtUpdateCmd_F3.FrameFormat = DataFrameStr->WriteData[2];
    for(i = 0 ; i < (ExtUpdateCmd_F3.VaildDataArrLenght - 3) ; i++)
    {
        ExtUpdateCmd_F3.DataArr[i] = DataFrameStr->WriteData[i+3];
    }
}

//主机向显示屏发送数据
extern bool handshake_is_ok;
uint8_t WriteFrameTransmit(void)
{
    Std_WriteDataFrameStr   Std_WriteDataFrame;
    Ext_WriteDataFrameStr   Ext_WriteDataFrame;
    uint8_t i = 0;
    if(tI2cSlave.uFlag.Bits.RecSuccess)
    {
        tI2cSlave.uFlag.Bits.RecSuccess = 0 ;
     
 
        memcpy(MasterTransmitData,&tI2cSlave.RecBuff,tI2cSlave.RecCount) ;
        tI2cSlave.RecCount = 0x00 ;
        if(((MasterTransmitData[0] == 0x81)||
        (MasterTransmitData[0] == 0x82)||
        (MasterTransmitData[0] == 0x83)||
        (MasterTransmitData[0] == 0x84)||
        (MasterTransmitData[0] == 0x85)||
        (MasterTransmitData[0] == 0x86)||
        (MasterTransmitData[0] == 0x87)||
        (MasterTransmitData[0] == 0xF0)) 
        && (ExtUpdateCmd_F1.ExtF1ActiveFlag == 0x00)
        && (ExtUpdateCmd_F2.ExtF2ActiveFlag == 0x00)
        && (ExtUpdateCmd_F3.ExtF3ActiveFlag == 0x00))
        {
            if(ChecksumCheck(&MasterTransmitData) == 1)
            {
                //此处应该清空MasterTransmitData
                for(i = 0; i < MAX_DATA_LENGTH; i++)
                {
                    MasterTransmitData[i] = 0;
                }
                return CHECKSUMERROR;
            }
            else
            {
                StdWriteFrameDataWrite(&Std_WriteDataFrame,&MasterTransmitData);
            }
        }
        else if((MasterTransmitData[0] == 0xF1) && (ExtUpdateCmd_F1.ExtF1ActiveFlag == 0x01))
        {

            ExtWriteFrameDataWrite(&Ext_WriteDataFrame,&MasterTransmitData,ExtUpdateCmd_F1.VaildDataArrLenght);
        }
        else if((MasterTransmitData[0] == 0xF2) && (ExtUpdateCmd_F2.ExtF2ActiveFlag == 0x01))
        {
            ExtWriteFrameDataWrite(&Ext_WriteDataFrame,&MasterTransmitData,ExtUpdateCmd_F2.VaildDataArrLenght);
        }
        else if((MasterTransmitData[0] == 0xF3) && (ExtUpdateCmd_F3.ExtF3ActiveFlag == 0x01))
        {
            ExtWriteFrameDataWrite(&Ext_WriteDataFrame,&MasterTransmitData,ExtUpdateCmd_F3.VaildDataArrLenght);
        }
        else
        {
            //此处应该清空MasterTransmitData
            for(i = 0; i < MAX_DATA_LENGTH; i++)
            {
                MasterTransmitData[i] = 0;
            }
        }

        switch(MasterTransmitData[0])
        {
            case 0x81:
                LcdInformationCheckFun(&Std_WriteDataFrame);
				process_0x81();
                break;
            case 0x82:
                SetFrameTimeoutValueFun(&Std_WriteDataFrame);
                break;
            case 0x83:
                BackLightModeConfigureFun(&Std_WriteDataFrame);
				RequestLcdStatusCmdStr.BacklightValue = Std_WriteDataFrame.WriteData[0];
				Update_Brightness(RequestLcdStatusCmdStr.BacklightValue);
				LcdDiagnosisCmdStr.BacklightValueReceive = 0x00;
                nvic_irq_disable(TIMER5_DAC_IRQn);
                break;
            case 0x84:
                TouchChipResetCmdFun(&Std_WriteDataFrame);
                break;
            case 0x85:
                TimeStampFun(&Std_WriteDataFrame);
                if(!handshake_is_ok)
                {
                    ReadFrameTransmit(0x15) ;
                    IRQ_LOW_DOWN;
                    IRQ_100ms = 100;         
                    handshake_is_ok = 1;
                }
                break;
            case 0x86:
                OpenCloseLcdFun(&Std_WriteDataFrame);
                break;
            case 0x87:
                BackLightSwitchCmdFun(&Std_WriteDataFrame);
                break;
            case 0xF0:
                UpdateCmdFun(&Std_WriteDataFrame);
                break;
            case 0xF1:
                ExtUpdateCmd1Fun(&Ext_WriteDataFrame);
                break;
            case 0xF2:
                ExtUpdateCmd2Fun(&Ext_WriteDataFrame);
                break;
            case 0xF3:
                ExtUpdateCmd3Fun(&Ext_WriteDataFrame);
                break;
            default:
                break;
        }
    }
}

