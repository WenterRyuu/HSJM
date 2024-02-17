#include "ReadDataFrame.h"
#include "Define.h"
#include "string.h"

extern tI2c_Type tI2cSlave ;

//每个ID的读取帧定义
Std_ReadDataFrameStr Std_ReadDataFrame_ID01;//暂时没触摸的功能,故不实现
Ext_ReadDataFrameStr Ext_ReadDataFrame_ID02;
Std_ReadDataFrameStr Std_ReadDataFrame_ID10;
Std_ReadDataFrameStr Std_ReadDataFrame_ID11;
Std_ReadDataFrameStr Std_ReadDataFrame_ID12;
Std_ReadDataFrameStr Std_ReadDataFrame_ID13;
Std_ReadDataFrameStr Std_ReadDataFrame_ID14;
Std_ReadDataFrameStr Std_ReadDataFrame_ID15;
Std_ReadDataFrameStr Std_ReadDataFrame_ID16;
Std_ReadDataFrameStr Std_ReadDataFrame_ID17;
Std_ReadDataFrameStr Std_ReadDataFrame_ID18;
Std_ReadDataFrameStr Std_ReadDataFrame_ID21;

//主机读取的数据的结构体定义,这些数据会在函数中轮询赋予对应ID的Std帧
ReCmd_0x01_TP_TouchPoint_StdDataStr TP_TouchPoint_StdDataStr;
ReCmd_0x02_TP_TouchPoint_StdDataStr TouchPoint_StdDataStr;
ReCmd_0x10_LCD_statusStr LCD_statusStr;
ReCmd_0x11_BackLightRequestStr BackLightRequestStr;
ReCmd_0x12_Resp_timeoutCmdStr Resp_timeoutCmdStr;
ReCmd_0x13_RequestLcdStatusCmdStr RequestLcdStatusCmdStr;
ReCmd_0x14_ProductIDStr ProductIDStr;
ReCmd_0x15_VersionStr VersionStr;
ReCmd_0x16_LcdDiagnosisCmdStr LcdDiagnosisCmdStr;
ReCmd_0x17_UpdateCmdStr UpdateCmdStr;
ReCmd_0x18_RequestTimestampCmdStr RequestTimestampCmdStr;
ReCmd_0x1E_KeyStatusCmdStr KeyStatusCmdStr;
ReCmd_0x1F_LightSeneorValueStr LightSeneorValueStr;
ReCmd_0x21_PartNumber PartNumber;



/*初始化主机读取数据帧结构体*/
void ReadDataFrameInit()
{
    uint8_t i = 0;
    Std_ReadDataFrameStr Std_ReadDataFrameInitStr =   
    {
        0x00,
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
        0x00,
        0x00
    };

    /*Std_ReadDataFrame_ID01 Init*/
    //Std_ReadDataFrame_ID01 = Std_ReadDataFrameInitStr;暂时没触摸的功能,故不实现
    //Ext_ReadDataFrame_ID02 = Std_ReadDataFrameInitStr;
    Std_ReadDataFrame_ID10 = Std_ReadDataFrameInitStr;
    Std_ReadDataFrame_ID11 = Std_ReadDataFrameInitStr;
    Std_ReadDataFrame_ID12 = Std_ReadDataFrameInitStr;
    Std_ReadDataFrame_ID13 = Std_ReadDataFrameInitStr;
    Std_ReadDataFrame_ID14 = Std_ReadDataFrameInitStr;
    Std_ReadDataFrame_ID15 = Std_ReadDataFrameInitStr;
    Std_ReadDataFrame_ID16 = Std_ReadDataFrameInitStr;
    Std_ReadDataFrame_ID17 = Std_ReadDataFrameInitStr;
    Std_ReadDataFrame_ID18 = Std_ReadDataFrameInitStr;

    Std_ReadDataFrame_ID01.ReadCmd_Id = 0x01;
    Ext_ReadDataFrame_ID02.ReadCmd_Id = 0x02;
    Std_ReadDataFrame_ID10.ReadCmd_Id = 0x10;
    Std_ReadDataFrame_ID11.ReadCmd_Id = 0x11;
    Std_ReadDataFrame_ID12.ReadCmd_Id = 0x12;
    Std_ReadDataFrame_ID13.ReadCmd_Id = 0x13;
    Std_ReadDataFrame_ID14.ReadCmd_Id = 0x14;
    Std_ReadDataFrame_ID15.ReadCmd_Id = 0x15;
    Std_ReadDataFrame_ID16.ReadCmd_Id = 0x16;
    Std_ReadDataFrame_ID17.ReadCmd_Id = 0x17;
    Std_ReadDataFrame_ID18.ReadCmd_Id = 0x18;
}


/*初始化主机读取数据*/
void ReadCmdIdStrInit()
{
    /*READ_CMD_ID 0x01*/
    // TP_TouchPoint_StdDataStr.TouchNum = 0x00;
    // TP_TouchPoint_StdDataStr.TP_TouchPoint1.Status = 0x00;
    // TP_TouchPoint_StdDataStr.TP_TouchPoint1.TouchId = 0x00;
    // TP_TouchPoint_StdDataStr.TP_TouchPoint1.XpositionLSB = 0x00;
    // TP_TouchPoint_StdDataStr.TP_TouchPoint1.XpositionMSB = 0x00;
    // TP_TouchPoint_StdDataStr.TP_TouchPoint1.YpositionLSB = 0x00;
    // TP_TouchPoint_StdDataStr.TP_TouchPoint1.YpositionMSB = 0x00;
    // TP_TouchPoint_StdDataStr.TP_TouchPoint2.Status = 0x00;
    // TP_TouchPoint_StdDataStr.TP_TouchPoint2.TouchId = 0x00;
    // TP_TouchPoint_StdDataStr.TP_TouchPoint2.XpositionLSB = 0x00;
    // TP_TouchPoint_StdDataStr.TP_TouchPoint2.XpositionMSB = 0x00;
    // TP_TouchPoint_StdDataStr.TP_TouchPoint2.YpositionLSB = 0x00;
    // TP_TouchPoint_StdDataStr.TP_TouchPoint2.YpositionMSB = 0x00;

    /*READ_CMD_ID 0x02*/

    /*READ_CMD_ID 0x10  显示屏状态指令*/
    LCD_statusStr.LcdStatus = 0x01;//0x00: Reserved  0x01: Normal Mode  0x02: Upgrade Mode  0x03: Self test Mode

    /*READ_CMD_ID 0x11  请求发送背光值*/
    BackLightRequestStr.RequestStatus = 0x01;//0x00:NotCare  0x01:请求发送背光值

    /*READ_CMD_ID 0x12  响应超时指令*/
    Resp_timeoutCmdStr.Ticktime = 0x0A;//【10】这两个参数是默认的，一般不用更改
    Resp_timeoutCmdStr.TimeoutValue = 0x14;//【20】这两个参数是默认的，一般不用更改

    /*READ_CMD_ID 0x13  显示屏信息查询指令*/
    RequestLcdStatusCmdStr.LcdBacklightTemStatus = 0x00;//0x00:温度正常  0x01:温度过高，亮度降低  0x02:温度过高，屏幕关闭
    RequestLcdStatusCmdStr.BacklightTemValue = Backlight.Final_Temperature;//-40~125有符号数表示-40~125℃
    RequestLcdStatusCmdStr.BacklightValue = 0x23;//有效范围0~100表示背光亮度 0-100%
    RequestLcdStatusCmdStr.reserved = 0x00;//默认值0，此为背光亮度扩展位，16位背光值才需使用
    RequestLcdStatusCmdStr.BackLightPowerSwitchstatus = 0x01;//0x00:背光关闭  0x01:背光开启
    RequestLcdStatusCmdStr.FOGPowerSwitchStatus = 0x00;//【硬件没有FOG电路】0x00:FOG电源关闭  0x01:FOG电源开启

    /*READ_CMD_ID 0x14  产品ID*/
    ProductIDStr.SupplierInformation = 0x04;//0x00：保留  0x01：按照供应商名称
    ProductIDStr.LcdSize = 0x04;//0x00：保留  0x01：8 Inch  0x02：9 Inch  0x03：10.1 Inch  0x04：10.25 Inch  0x05：10.4 Inch  0x06： 12.3 Inch  0x07：12.6Inch  0x08:15.6Inch  0x09:16.2Inch  0x0A:27 Inch
    ProductIDStr.LcdDpi = 0x01;//0x00：保留  0x01: 1920*720  0x02：1280*720  0x03：1920*1080  0x04:1920*1200
    ProductIDStr.LVDSFormat = 0x01;//0x00：保留  0x01: VESA  0x02：JEIDA
    ProductIDStr.LcdType = 0x02;//0x00：保留  0x01：中控屏  0x02：仪表屏  0x03：空调屏  0x04：扶手屏  0x05：后枕屏

    /*READ_CMD_ID 0x15  版本号*/
    VersionStr.HWversion = 0x00;//0~99(Dec)硬件正式发布版本号
    VersionStr.HWVersionDot = 0x00;//0~99(Dec)硬件版本号,预留
    VersionStr.SWversion = 0x00;//0~99(Dec)软件正式发布版本号
    VersionStr.SWVersionDot = 0x00;//0~99(Dec)软件版本号,预留
    VersionStr.HWVersion = 0x01;
    VersionStr.HWYear = 0x23;
    VersionStr.HWMonth = 0x12;
    VersionStr.HWDay = 0x20;
    VersionStr.SWVersion = 0x01;
    VersionStr.SWYear = 0x23;
    VersionStr.SWMonth = 0x12;
    VersionStr.SWDay = 0x20;

    /*READ_CMD_ID 0x16  显示屏 诊断信息指令*/
    LcdDiagnosisCmdStr.VedioSignalStatus = (gpio_input_bit_get(GPIOB, GPIO_PIN_13)==SET) ? 0x00 : 0x02;
	//0x00：Lock  0x01：显示屏不支持该诊断  0x02：No Lock
    LcdDiagnosisCmdStr.BacklightValueReceive = 0x02;
	//0x00：有接收到背光亮度  0x01：显示屏不支持该诊断  0x02：没有接收到背光亮度
    LcdDiagnosisCmdStr.TouchHWErrorStatus = 0x01;
	//0x00：正常  【0x01：显示屏不支持该诊断】  0x02：芯片错误检测脚报错  0x03：短路  0x04：开路
    LcdDiagnosisCmdStr.DisplaymodeErrorStatus = (gpio_input_bit_get(GPIOB, GPIO_PIN_13)==RESET) ? 0x02 : 0x00;
	//0x00：正常  0x01：显示屏不支持该诊断  0x02：芯片错误检测脚报错【LCD排针的34号引脚PB13】
    LcdDiagnosisCmdStr.BacklightHWErrorStatus = (gpio_input_bit_get(GPIOA, GPIO_PIN_3)==RESET) ? 0x02 : 0x00;
	//0x00：正常  0x01：显示屏不支持该诊断  0x02：芯片错误检测脚报错【背光电压错误指示PA3】  0x03：短路  0x04：开路
    LcdDiagnosisCmdStr.SerdesChipErrorStatus = 0x00;
	//0x00：正常  0x01：显示屏不支持该诊断  0x02：芯片错误检测脚报错【解串器芯片错误检测脚状态对应PC15 TFT-Fault信号】
    LcdDiagnosisCmdStr.BatterErrorStatus = 0x01;
	//0x00：正常  【0x01：显示屏不支持该诊断】  0x02：电池过压（举例：高于17V低于16V,要有回滞区间）  0x03：电池欠压（举例：低于7V高于9V，要有回滞区间）
    LcdDiagnosisCmdStr.DisplaymodeVoltageErrorStatus = 0x01;
	//0x00：正常  【0x01：显示屏不支持该诊断】  0x02：电压异常
    LcdDiagnosisCmdStr.TouchvoltageErrorStatus = 0x01;
	//0x00：正常  【0x01：显示屏不支持该诊断】  0x02：电压异常

    /*READ_CMD_ID 0x17  升级指令*/
    UpdateCmdStr.DiagnosisResponse = 0x3D;//诊断响应
    UpdateCmdStr.DiagnosisAddress = 0x28;//物理寻址对指定的设备进行诊断访问
    UpdateCmdStr.FrameFormat = 0x00;
    UpdateCmdStr.ResponseParameters1 = 0x00;
    UpdateCmdStr.ResponseParameters2 = 0x00;
    UpdateCmdStr.ResponseParameters3 = 0x00;
    UpdateCmdStr.ResponseParameters4 = 0x00;
    UpdateCmdStr.ResponseParameters5 = 0x00;
    UpdateCmdStr.ResponseParameters6 = 0x00;
    UpdateCmdStr.ResponseParameters7 = 0x00;
    UpdateCmdStr.ResponseParameters8 = 0x00;
    UpdateCmdStr.ResponseParameters9 = 0x00;
    UpdateCmdStr.ResponseParameters10 = 0x00;

    /*READ_CMD_ID 0x18  请求发送时间戳*/
    RequestTimestampCmdStr.RequestStatus = 0x00;//0x00:NotCare

    /*READ_CMD_ID 0x1E  按键状态指令*/
    KeyStatusCmdStr.KeyStatus1 = 0x00;
    KeyStatusCmdStr.KeyStatus2 = 0x00;
    KeyStatusCmdStr.KeyStatus3 = 0x00;
    KeyStatusCmdStr.KeyStatus4 = 0x00;
    KeyStatusCmdStr.KeyStatus5 = 0x00;

    /*READ_CMD_ID 0x1F  光线传感器值*/
    LightSeneorValueStr.LightSeneorValueLSB = 0x00;
    LightSeneorValueStr.LightSeneorValueMSB = 0x00;
    
    /*READ_CMD_ID 0x1F  零件号*/    
//    PartNumber.Part_1st_digit = 0x32;//248102HH0A
//    PartNumber.Part_2nd_digit = 0x34;
//    PartNumber.Part_3rd_digit = 0x38;
//    PartNumber.Part_4th_digit = 0x31;
//    PartNumber.Part_5th_digit = 0x30;
//    PartNumber.Part_6th_digit = 0x32;
//    PartNumber.Part_7th_digit = 0x48;
//    PartNumber.Part_8th_digit = 0x48;
//    PartNumber.Part_9th_digit = 0x30;
//    PartNumber.Part_10th_digit = 0x41;    
    uint8_t *ptr;
    ptr = PART_ID_ADDRESS_5;
    PartNumber.Part_1st_digit = *ptr;ptr++;                                     //248102HH0A    
    PartNumber.Part_2nd_digit = *ptr;ptr++;        
    PartNumber.Part_3rd_digit = *ptr;ptr++;    
    PartNumber.Part_4th_digit = *ptr;ptr++;    
    PartNumber.Part_5th_digit = *ptr;
    ptr = PART_ID_ADDRESS_10;    
    PartNumber.Part_6th_digit = *ptr;ptr++;    
    PartNumber.Part_7th_digit = *ptr;ptr++;    
    PartNumber.Part_8th_digit = *ptr;ptr++;    
    PartNumber.Part_9th_digit = *ptr;ptr++;    
    PartNumber.Part_10th_digit = *ptr;  
}

/*计算校验和*/
uint8_t ReadFrameChecksumCount(Std_ReadDataFrameStr *ReadDataFrameStr)
{
    uint8_t i = 0;
    uint8_t checksum = 0x00;
    for(i = 0; i < STD_FRAME_READ_DATA_LENGTH; i++)
    {
        checksum += ReadDataFrameStr->ReadData[i];
    }
    checksum = checksum + ReadDataFrameStr->ReadCmd_Id + ReadDataFrameStr->ExtLength;

    return checksum;
}

uint8_t ReadFrameChecksumBuff(uint8_t *Buff, uint8_t Length)
{
    uint8_t i = 0;
    uint8_t checksum = 0x00;
    for(i = 0; i < Length; i++)
    {
        checksum += Buff[i] ;
    }
   
    return checksum;
}

/*读数据帧的数据填充,该函数周期调用,将发送给主机的数据填充到各个ID对应的I2C帧结构体中*/
void ReadFrameWriteData()
{
    /*0x01 datawrite*/
    //Std_ReadDataFrame_ID01.ReadData[0] = TP_TouchPoint_StdDataStr.TouchNum;
    //Std_ReadDataFrame_ID01.ReadData[1] = TP_TouchPoint_StdDataStr.TP_TouchPoint1.TouchId;
    //Std_ReadDataFrame_ID01.ReadData[2] = TP_TouchPoint_StdDataStr.TP_TouchPoint1.Status;
    //Std_ReadDataFrame_ID01.ReadData[3] = TP_TouchPoint_StdDataStr.TP_TouchPoint1.XpositionMSB;
    //Std_ReadDataFrame_ID01.ReadData[4] = TP_TouchPoint_StdDataStr.TP_TouchPoint1.XpositionLSB;
    //Std_ReadDataFrame_ID01.ReadData[5] = TP_TouchPoint_StdDataStr.TP_TouchPoint1.YpositionMSB;
    //Std_ReadDataFrame_ID01.ReadData[6] = TP_TouchPoint_StdDataStr.TP_TouchPoint1.YpositionLSB;
    //Std_ReadDataFrame_ID01.ReadData[7] = TP_TouchPoint_StdDataStr.TP_TouchPoint2.TouchId;
    //Std_ReadDataFrame_ID01.ReadData[8] = TP_TouchPoint_StdDataStr.TP_TouchPoint2.Status;
    //Std_ReadDataFrame_ID01.ReadData[9] = TP_TouchPoint_StdDataStr.TP_TouchPoint2.XpositionMSB;
    //Std_ReadDataFrame_ID01.ReadData[10] = TP_TouchPoint_StdDataStr.TP_TouchPoint2.XpositionLSB;
    //Std_ReadDataFrame_ID01.ReadData[11] = TP_TouchPoint_StdDataStr.TP_TouchPoint2.YpositionMSB;
    //Std_ReadDataFrame_ID01.ReadData[12] = TP_TouchPoint_StdDataStr.TP_TouchPoint2.YpositionLSB;
    //Std_ReadDataFrame_ID01.ExtLength = 0x00;
    //Std_ReadDataFrame_ID01.checksum = ReadFrameChecksumCount(&Std_ReadDataFrame_ID01);

    /*0x10 datawrite*/
    Std_ReadDataFrame_ID10.ReadData[0] = LCD_statusStr.LcdStatus;
    Std_ReadDataFrame_ID10.ReadData[1] = 0x00;
    Std_ReadDataFrame_ID10.ReadData[2] = 0x00;
    Std_ReadDataFrame_ID10.ReadData[3] = 0x00;
    Std_ReadDataFrame_ID10.ReadData[4] = 0x00;
    Std_ReadDataFrame_ID10.ReadData[5] = 0x00;
    Std_ReadDataFrame_ID10.ReadData[6] = 0x00;
    Std_ReadDataFrame_ID10.ReadData[7] = 0x00;
    Std_ReadDataFrame_ID10.ReadData[8] = 0x00;
    Std_ReadDataFrame_ID10.ReadData[9] = 0x00;
    Std_ReadDataFrame_ID10.ReadData[10] = 0x00;
    Std_ReadDataFrame_ID10.ReadData[11] = 0x00;
    Std_ReadDataFrame_ID10.ReadData[12] = 0x00;
    Std_ReadDataFrame_ID10.ExtLength = 0x00;
    Std_ReadDataFrame_ID10.checksum = ReadFrameChecksumCount(&Std_ReadDataFrame_ID10);

    /*0x11 datawrite*/
    Std_ReadDataFrame_ID11.ReadData[0] = BackLightRequestStr.RequestStatus;
    Std_ReadDataFrame_ID11.ReadData[1] = 0x00;
    Std_ReadDataFrame_ID11.ReadData[2] = 0x00;
    Std_ReadDataFrame_ID11.ReadData[3] = 0x00;
    Std_ReadDataFrame_ID11.ReadData[4] = 0x00;
    Std_ReadDataFrame_ID11.ReadData[5] = 0x00;
    Std_ReadDataFrame_ID11.ReadData[6] = 0x00;
    Std_ReadDataFrame_ID11.ReadData[7] = 0x00;
    Std_ReadDataFrame_ID11.ReadData[8] = 0x00;
    Std_ReadDataFrame_ID11.ReadData[9] = 0x00;
    Std_ReadDataFrame_ID11.ReadData[10] = 0x00;
    Std_ReadDataFrame_ID11.ReadData[11] = 0x00;
    Std_ReadDataFrame_ID11.ReadData[12] = 0x00;
    Std_ReadDataFrame_ID11.ExtLength = 0x00;
    Std_ReadDataFrame_ID11.checksum = ReadFrameChecksumCount(&Std_ReadDataFrame_ID11);

    /*0x12 datawrite*/
    Std_ReadDataFrame_ID12.ReadData[0] = Resp_timeoutCmdStr.TimeoutValue;
    Std_ReadDataFrame_ID12.ReadData[1] = Resp_timeoutCmdStr.Ticktime;
    Std_ReadDataFrame_ID12.ReadData[2] = 0x00;
    Std_ReadDataFrame_ID12.ReadData[3] = 0x00;
    Std_ReadDataFrame_ID12.ReadData[4] = 0x00;
    Std_ReadDataFrame_ID12.ReadData[5] = 0x00;
    Std_ReadDataFrame_ID12.ReadData[6] = 0x00;
    Std_ReadDataFrame_ID12.ReadData[7] = 0x00;
    Std_ReadDataFrame_ID12.ReadData[8] = 0x00;
    Std_ReadDataFrame_ID12.ReadData[9] = 0x00;
    Std_ReadDataFrame_ID12.ReadData[10] = 0x00;
    Std_ReadDataFrame_ID12.ReadData[11] = 0x00;
    Std_ReadDataFrame_ID12.ReadData[12] = 0x00;
    Std_ReadDataFrame_ID12.ExtLength = 0x00;
    Std_ReadDataFrame_ID12.checksum = ReadFrameChecksumCount(&Std_ReadDataFrame_ID12);

    /*0x13 datawrite*/
    Std_ReadDataFrame_ID13.ReadData[0] = RequestLcdStatusCmdStr.LcdBacklightTemStatus;
    Std_ReadDataFrame_ID13.ReadData[1] = RequestLcdStatusCmdStr.BacklightTemValue;
    Std_ReadDataFrame_ID13.ReadData[2] = RequestLcdStatusCmdStr.BacklightValue;
    Std_ReadDataFrame_ID13.ReadData[3] = 0x00;
    Std_ReadDataFrame_ID13.ReadData[4] = RequestLcdStatusCmdStr.FOGPowerSwitchStatus;
    Std_ReadDataFrame_ID13.ReadData[5] = RequestLcdStatusCmdStr.FOGPowerSwitchStatus;
    Std_ReadDataFrame_ID13.ReadData[6] = 0x00;
    Std_ReadDataFrame_ID13.ReadData[7] = 0x00;
    Std_ReadDataFrame_ID13.ReadData[8] = 0x00;
    Std_ReadDataFrame_ID13.ReadData[9] = 0x00;
    Std_ReadDataFrame_ID13.ReadData[10] = 0x00;
    Std_ReadDataFrame_ID13.ReadData[11] = 0x00;
    Std_ReadDataFrame_ID13.ReadData[12] = 0x00;
    Std_ReadDataFrame_ID13.ExtLength = 0x00;
    Std_ReadDataFrame_ID13.checksum = ReadFrameChecksumCount(&Std_ReadDataFrame_ID13);

    /*0x14 datawrite*/
    Std_ReadDataFrame_ID14.ReadData[0] = ProductIDStr.SupplierInformation;
    Std_ReadDataFrame_ID14.ReadData[1] = ProductIDStr.LcdSize;
    Std_ReadDataFrame_ID14.ReadData[2] = ProductIDStr.LcdDpi;
    Std_ReadDataFrame_ID14.ReadData[3] = ProductIDStr.LVDSFormat;
    Std_ReadDataFrame_ID14.ReadData[4] = ProductIDStr.LcdType;
    Std_ReadDataFrame_ID14.ReadData[5] = 0x00;
    Std_ReadDataFrame_ID14.ReadData[6] = 0x00;
    Std_ReadDataFrame_ID14.ReadData[7] = 0x00;
    Std_ReadDataFrame_ID14.ReadData[8] = 0x00;
    Std_ReadDataFrame_ID14.ReadData[9] = 0x00;
    Std_ReadDataFrame_ID14.ReadData[10] = 0x00;
    Std_ReadDataFrame_ID14.ReadData[11] = 0x00;
    Std_ReadDataFrame_ID14.ReadData[12] = 0x00;
    Std_ReadDataFrame_ID14.ExtLength = 0x00;
    Std_ReadDataFrame_ID14.checksum = ReadFrameChecksumCount(&Std_ReadDataFrame_ID14);

    /*0x15 datawrite*/
    Std_ReadDataFrame_ID15.ReadData[0] = VersionStr.HWversion;
    Std_ReadDataFrame_ID15.ReadData[1] = VersionStr.HWVersionDot;
    Std_ReadDataFrame_ID15.ReadData[2] = VersionStr.SWversion;
    Std_ReadDataFrame_ID15.ReadData[3] = VersionStr.SWVersionDot;
    Std_ReadDataFrame_ID15.ReadData[4] = VersionStr.HWVersion;
    Std_ReadDataFrame_ID15.ReadData[5] = VersionStr.HWYear;
    Std_ReadDataFrame_ID15.ReadData[6] = VersionStr.HWMonth;
    Std_ReadDataFrame_ID15.ReadData[7] = VersionStr.HWDay;
    Std_ReadDataFrame_ID15.ReadData[8] = VersionStr.SWVersion;
    Std_ReadDataFrame_ID15.ReadData[9] = VersionStr.SWYear;
    Std_ReadDataFrame_ID15.ReadData[10] = VersionStr.SWMonth;
    Std_ReadDataFrame_ID15.ReadData[11] = VersionStr.SWDay;
    Std_ReadDataFrame_ID15.ReadData[12] = 0x00;
    Std_ReadDataFrame_ID15.ExtLength = 0x00;
    Std_ReadDataFrame_ID15.checksum = ReadFrameChecksumCount(&Std_ReadDataFrame_ID15);

    /*0x16 datawrite*/
    Std_ReadDataFrame_ID16.ReadData[0] = LcdDiagnosisCmdStr.VedioSignalStatus;
    Std_ReadDataFrame_ID16.ReadData[1] = LcdDiagnosisCmdStr.BacklightValueReceive;
    Std_ReadDataFrame_ID16.ReadData[2] = LcdDiagnosisCmdStr.TouchHWErrorStatus;
    Std_ReadDataFrame_ID16.ReadData[3] = LcdDiagnosisCmdStr.DisplaymodeErrorStatus;
    Std_ReadDataFrame_ID16.ReadData[4] = LcdDiagnosisCmdStr.BacklightHWErrorStatus;
    Std_ReadDataFrame_ID16.ReadData[5] = LcdDiagnosisCmdStr.SerdesChipErrorStatus;
    Std_ReadDataFrame_ID16.ReadData[6] = LcdDiagnosisCmdStr.BatterErrorStatus;
    Std_ReadDataFrame_ID16.ReadData[7] = LcdDiagnosisCmdStr.DisplaymodeVoltageErrorStatus;
    Std_ReadDataFrame_ID16.ReadData[8] = LcdDiagnosisCmdStr.TouchvoltageErrorStatus;
    Std_ReadDataFrame_ID16.ReadData[9] = 0x00;
    Std_ReadDataFrame_ID16.ReadData[10] = 0x00;
    Std_ReadDataFrame_ID16.ReadData[11] = 0x00;
    Std_ReadDataFrame_ID16.ReadData[12] = 0x00;
    Std_ReadDataFrame_ID16.ExtLength = 0x00;
    Std_ReadDataFrame_ID16.checksum = ReadFrameChecksumCount(&Std_ReadDataFrame_ID16);

    /*0x17 datawrite*/
    Std_ReadDataFrame_ID17.ReadData[0] = UpdateCmdStr.DiagnosisResponse;
    Std_ReadDataFrame_ID17.ReadData[1] = UpdateCmdStr.DiagnosisAddress;
    Std_ReadDataFrame_ID17.ReadData[2] = UpdateCmdStr.FrameFormat;
    Std_ReadDataFrame_ID17.ReadData[3] = UpdateCmdStr.ResponseParameters1;
    Std_ReadDataFrame_ID17.ReadData[4] = UpdateCmdStr.ResponseParameters2;
    Std_ReadDataFrame_ID17.ReadData[5] = UpdateCmdStr.ResponseParameters3;
    Std_ReadDataFrame_ID17.ReadData[6] = UpdateCmdStr.ResponseParameters4;
    Std_ReadDataFrame_ID17.ReadData[7] = UpdateCmdStr.ResponseParameters5;
    Std_ReadDataFrame_ID17.ReadData[8] = UpdateCmdStr.ResponseParameters6;
    Std_ReadDataFrame_ID17.ReadData[9] = UpdateCmdStr.ResponseParameters7;
    Std_ReadDataFrame_ID17.ReadData[10] = UpdateCmdStr.ResponseParameters8;
    Std_ReadDataFrame_ID17.ReadData[11] = UpdateCmdStr.ResponseParameters9;
    Std_ReadDataFrame_ID17.ReadData[12] = UpdateCmdStr.ResponseParameters10;
    Std_ReadDataFrame_ID17.ExtLength = 0x00;
    Std_ReadDataFrame_ID17.checksum = ReadFrameChecksumCount(&Std_ReadDataFrame_ID17);

    /*0x18 datawrite*/
    Std_ReadDataFrame_ID18.ReadData[0] = RequestTimestampCmdStr.RequestStatus;
    Std_ReadDataFrame_ID18.ReadData[1] = 0x00;
    Std_ReadDataFrame_ID18.ReadData[2] = 0x00;
    Std_ReadDataFrame_ID18.ReadData[3] = 0x00;
    Std_ReadDataFrame_ID18.ReadData[4] = 0x00;
    Std_ReadDataFrame_ID18.ReadData[5] = 0x00;
    Std_ReadDataFrame_ID18.ReadData[6] = 0x00;
    Std_ReadDataFrame_ID18.ReadData[7] = 0x00;
    Std_ReadDataFrame_ID18.ReadData[8] = 0x00;
    Std_ReadDataFrame_ID18.ReadData[9] = 0x00;
    Std_ReadDataFrame_ID18.ReadData[10] = 0x00;
    Std_ReadDataFrame_ID18.ReadData[11] = 0x00;
    Std_ReadDataFrame_ID18.ReadData[12] = 0x00;
    Std_ReadDataFrame_ID18.ExtLength = 0x00;
    Std_ReadDataFrame_ID18.checksum = ReadFrameChecksumCount(&Std_ReadDataFrame_ID18);

    /*0x21 datawrite*/
    uint8_t *ptr;
    ptr = PART_ID_ADDRESS_5;
    PartNumber.Part_1st_digit = *ptr;ptr++;                                     //248102HH0A    
    PartNumber.Part_2nd_digit = *ptr;ptr++;        
    PartNumber.Part_3rd_digit = *ptr;ptr++;    
    PartNumber.Part_4th_digit = *ptr;ptr++;    
    PartNumber.Part_5th_digit = *ptr;
    ptr = PART_ID_ADDRESS_10;    
    PartNumber.Part_6th_digit = *ptr;ptr++;    
    PartNumber.Part_7th_digit = *ptr;ptr++;    
    PartNumber.Part_8th_digit = *ptr;ptr++;    
    PartNumber.Part_9th_digit = *ptr;ptr++;    
    PartNumber.Part_10th_digit = *ptr;    
    Std_ReadDataFrame_ID21.ReadData[0] = PartNumber.Part_1st_digit;
    Std_ReadDataFrame_ID21.ReadData[1] = PartNumber.Part_2nd_digit;
    Std_ReadDataFrame_ID21.ReadData[2] = PartNumber.Part_3rd_digit;
    Std_ReadDataFrame_ID21.ReadData[3] = PartNumber.Part_4th_digit;
    Std_ReadDataFrame_ID21.ReadData[4] = PartNumber.Part_5th_digit;
    Std_ReadDataFrame_ID21.ReadData[5] = PartNumber.Part_6th_digit;
    Std_ReadDataFrame_ID21.ReadData[6] = PartNumber.Part_7th_digit;
    Std_ReadDataFrame_ID21.ReadData[7] = PartNumber.Part_8th_digit;
    Std_ReadDataFrame_ID21.ReadData[8] = PartNumber.Part_9th_digit;
    Std_ReadDataFrame_ID21.ReadData[9] = PartNumber.Part_10th_digit;
    Std_ReadDataFrame_ID21.ReadData[10] = 0x00;
    Std_ReadDataFrame_ID21.ReadData[11] = 0x00;
    Std_ReadDataFrame_ID21.ReadData[12] = 0x00;
    Std_ReadDataFrame_ID21.ExtLength = 0x00;
    Std_ReadDataFrame_ID21.checksum = ReadFrameChecksumCount(&Std_ReadDataFrame_ID21);
    
}

/*主机根据Id读取LCD的数据，该函数用于发送主机要读取的数据，把数据准备好放到发送缓冲区*/
void ReadFrameTransmit(uint8_t cmd_id)
{
    switch (cmd_id)
    {
        case 0x01://触摸相关
            //Std_ReadDataFrame_ID01

        case 0x02://触摸相关
            //Ext_ReadDataFrame_ID02
            break;

        case 0x10:
            //Std_ReadDataFrame_ID10
            tI2cSlave.Send_Buff[0] = cmd_id ;
            tI2cSlave.Send_Buff[1] = LCD_statusStr.LcdStatus ;
            memset(&tI2cSlave.Send_Buff[2],0x00,12) ;
            tI2cSlave.Send_Buff[14] = 0 ; //无扩展帧
            tI2cSlave.Send_Buff[15] = ReadFrameChecksumBuff(&tI2cSlave.Send_Buff[0],15) ;
            tI2cSlave.SendSize = 16 ;
            break;
		
        case 0x11:
            tI2cSlave.Send_Buff[0] = cmd_id ;
            tI2cSlave.Send_Buff[1] = BackLightRequestStr.RequestStatus ;
            memset(&tI2cSlave.Send_Buff[2],0x00,12) ;
            tI2cSlave.Send_Buff[14] = 0 ; //无扩展帧
            tI2cSlave.Send_Buff[15] = ReadFrameChecksumBuff(&tI2cSlave.Send_Buff[0],15) ;
            tI2cSlave.SendSize = 16 ;	
            //Std_ReadDataFrame_ID11	
            break;

        case 0x12:
            tI2cSlave.Send_Buff[0] = cmd_id ;
            tI2cSlave.Send_Buff[1] = Resp_timeoutCmdStr.TimeoutValue ;
            tI2cSlave.Send_Buff[2] = Resp_timeoutCmdStr.Ticktime;
            memset(&tI2cSlave.Send_Buff[3],0x00,11) ;
            tI2cSlave.Send_Buff[14] = 0 ; //无扩展帧
            tI2cSlave.Send_Buff[15] = ReadFrameChecksumBuff(&tI2cSlave.Send_Buff[0],15) ;
            tI2cSlave.SendSize = 16 ;	
	
            break;

        case 0x13:
            //Std_ReadDataFrame_ID13	
		
            tI2cSlave.Send_Buff[0] = cmd_id ;
            tI2cSlave.Send_Buff[1] = RequestLcdStatusCmdStr.LcdBacklightTemStatus ;
            tI2cSlave.Send_Buff[2] = RequestLcdStatusCmdStr.BacklightTemValue ;
            tI2cSlave.Send_Buff[3] = RequestLcdStatusCmdStr.BacklightValue ;
            tI2cSlave.Send_Buff[4] = RequestLcdStatusCmdStr.reserved;
            tI2cSlave.Send_Buff[5] = RequestLcdStatusCmdStr.BackLightPowerSwitchstatus ;
            tI2cSlave.Send_Buff[6] = RequestLcdStatusCmdStr.FOGPowerSwitchStatus ;
            memset(&tI2cSlave.Send_Buff[7],0x00,7) ;
            tI2cSlave.Send_Buff[14] = 0 ; //无扩展帧
            tI2cSlave.Send_Buff[15] = ReadFrameChecksumBuff(&tI2cSlave.Send_Buff[0],15) ;
            tI2cSlave.SendSize = 16 ;
            break;

        case 0x14:
            //Std_ReadDataFrame_ID14
            tI2cSlave.Send_Buff[0] = cmd_id ;
            tI2cSlave.Send_Buff[1] = ProductIDStr.SupplierInformation;
            tI2cSlave.Send_Buff[2] = ProductIDStr.LcdSize;
            tI2cSlave.Send_Buff[3] = ProductIDStr.LcdDpi;
            tI2cSlave.Send_Buff[4] = ProductIDStr.LVDSFormat;
            tI2cSlave.Send_Buff[5] = ProductIDStr.LcdType;
            memset(&tI2cSlave.Send_Buff[6],0x00,8) ;
            tI2cSlave.Send_Buff[14] = 0 ; //无扩展帧
            tI2cSlave.Send_Buff[15] = ReadFrameChecksumBuff(&tI2cSlave.Send_Buff[0],15) ;
            tI2cSlave.SendSize = 16 ;		
            break;

        case 0x15:		
            //Std_ReadDataFrame_ID15
            tI2cSlave.Send_Buff[0] = cmd_id ;
            tI2cSlave.Send_Buff[1] = VersionStr.HWversion ;
            tI2cSlave.Send_Buff[2] = VersionStr.HWVersionDot ;	
            tI2cSlave.Send_Buff[3] = VersionStr.SWversion ;	
            tI2cSlave.Send_Buff[4] = VersionStr.SWVersionDot ;         
            tI2cSlave.Send_Buff[5] = VersionStr.HWVersion ;
            tI2cSlave.Send_Buff[6] = VersionStr.HWYear ;
            tI2cSlave.Send_Buff[7] = VersionStr.HWMonth ;
            tI2cSlave.Send_Buff[8] = VersionStr.HWDay ;
            tI2cSlave.Send_Buff[9] = VersionStr.SWVersion ;
            tI2cSlave.Send_Buff[10] = VersionStr.SWYear ;
            tI2cSlave.Send_Buff[11] = VersionStr.SWMonth ;
            tI2cSlave.Send_Buff[12] = VersionStr.SWDay ;
            tI2cSlave.Send_Buff[13] = 0x00 ;
            tI2cSlave.Send_Buff[14] = 0 ; //无扩展帧
            tI2cSlave.Send_Buff[15] = ReadFrameChecksumBuff(&tI2cSlave.Send_Buff[0],15) ;
            tI2cSlave.SendSize = 16 ;
            break;

        case 0x16:
            //Std_ReadDataFrame_ID16
            tI2cSlave.Send_Buff[0] = cmd_id ;
            tI2cSlave.Send_Buff[1] = LcdDiagnosisCmdStr.VedioSignalStatus;
            tI2cSlave.Send_Buff[2] = LcdDiagnosisCmdStr.BacklightValueReceive;
            tI2cSlave.Send_Buff[3] = LcdDiagnosisCmdStr.TouchHWErrorStatus;
            tI2cSlave.Send_Buff[4] = LcdDiagnosisCmdStr.DisplaymodeErrorStatus;
            tI2cSlave.Send_Buff[5] = LcdDiagnosisCmdStr.BacklightHWErrorStatus;
            tI2cSlave.Send_Buff[6] = LcdDiagnosisCmdStr.SerdesChipErrorStatus;
            tI2cSlave.Send_Buff[7] = LcdDiagnosisCmdStr.BatterErrorStatus;
            tI2cSlave.Send_Buff[8] = LcdDiagnosisCmdStr.DisplaymodeVoltageErrorStatus;
            tI2cSlave.Send_Buff[9] = LcdDiagnosisCmdStr.TouchvoltageErrorStatus;		
            memset(&tI2cSlave.Send_Buff[10],0x00,4) ;
            tI2cSlave.Send_Buff[14] = 0 ; //无扩展帧
            tI2cSlave.Send_Buff[15] = ReadFrameChecksumBuff(&tI2cSlave.Send_Buff[0],15) ;
            tI2cSlave.SendSize = 16 ;		
            break;

        case 0x17:
            //Std_ReadDataFrame_ID17
            tI2cSlave.Send_Buff[0] = cmd_id ;
            tI2cSlave.Send_Buff[1] = UpdateCmdStr.DiagnosisResponse;	
            tI2cSlave.Send_Buff[2] = UpdateCmdStr.DiagnosisAddress;		
            tI2cSlave.Send_Buff[3] = UpdateCmdStr.FrameFormat;			
            tI2cSlave.Send_Buff[4] = UpdateCmdStr.ResponseParameters1;	
            tI2cSlave.Send_Buff[5] = UpdateCmdStr.ResponseParameters2;	
            tI2cSlave.Send_Buff[6] = UpdateCmdStr.ResponseParameters3;	
            tI2cSlave.Send_Buff[7] = UpdateCmdStr.ResponseParameters4;	
            tI2cSlave.Send_Buff[8] = UpdateCmdStr.ResponseParameters5;	
            tI2cSlave.Send_Buff[9] = UpdateCmdStr.ResponseParameters6;	
            tI2cSlave.Send_Buff[10] = UpdateCmdStr.ResponseParameters7;	
            tI2cSlave.Send_Buff[11] = UpdateCmdStr.ResponseParameters8;	
            tI2cSlave.Send_Buff[12] = UpdateCmdStr.ResponseParameters9;	
            tI2cSlave.Send_Buff[13] = UpdateCmdStr.ResponseParameters10;	
            //memset(&tI2cSlave.Send_Buff[10],0x00,4) ;
            tI2cSlave.Send_Buff[14] = 0 ; //无扩展帧
            tI2cSlave.Send_Buff[15] = ReadFrameChecksumBuff(&tI2cSlave.Send_Buff[0],15) ;
            tI2cSlave.SendSize = 16 ;		
            break;

        case 0x18:
            //Std_ReadDataFrame_ID18
			tI2cSlave.Send_Buff[0] = cmd_id;
			tI2cSlave.Send_Buff[1] = RequestTimestampCmdStr.RequestStatus;
            memset(&tI2cSlave.Send_Buff[2],0x00,12) ;
            tI2cSlave.Send_Buff[14] = 0 ; //无扩展帧
            tI2cSlave.Send_Buff[15] = ReadFrameChecksumBuff(&tI2cSlave.Send_Buff[0],15) ;
            tI2cSlave.SendSize = 16 ;		
            break;
        
        case 0x21:
            tI2cSlave.Send_Buff[0] = cmd_id ;
            tI2cSlave.Send_Buff[1] = PartNumber.Part_1st_digit;	
            tI2cSlave.Send_Buff[2] = PartNumber.Part_2nd_digit;
            tI2cSlave.Send_Buff[3] = PartNumber.Part_3rd_digit;
            tI2cSlave.Send_Buff[4] = PartNumber.Part_4th_digit;
            tI2cSlave.Send_Buff[5] = PartNumber.Part_5th_digit;		
            tI2cSlave.Send_Buff[6] = PartNumber.Part_6th_digit;
            tI2cSlave.Send_Buff[7] = PartNumber.Part_7th_digit;
            tI2cSlave.Send_Buff[8] = PartNumber.Part_8th_digit;
            tI2cSlave.Send_Buff[9] = PartNumber.Part_9th_digit;
            tI2cSlave.Send_Buff[10] = PartNumber.Part_10th_digit;
            memset(&tI2cSlave.Send_Buff[11],0x00,3) ;
            tI2cSlave.Send_Buff[14] = 0 ; //无扩展帧
            tI2cSlave.Send_Buff[15] = ReadFrameChecksumBuff(&tI2cSlave.Send_Buff[0],15) ;
            tI2cSlave.SendSize = 16 ;		
        default:
            break;
    }
}



