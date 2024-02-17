#ifndef _READDATAFRAME_H_
#define _READDATAFRAME_H_
#include "gd32a50x.h"


#define READ_CMD 0xFE   //读数据指令
#define STD_FRAME_READ_DATA_LENGTH 0x0D // 标准帧数据长度固定为14字节
#define MAX_DATA_LENGTH 0xFF    // 读取数据最大长度
#define STD_DATA_LENGTH 0x09	//标准帧data长度
#define LCD_SLAVE_ADRESS 0x34 //Lcd I2C通讯地址




/*标准帧(读数据)*/
typedef struct
{
    uint8_t ReadCmd_Id; //读取指令子Id
    uint8_t ReadData[STD_FRAME_READ_DATA_LENGTH];//Lcd发送的数据
    uint8_t ExtLength;//下一帧拓展帧长度
    uint8_t checksum;//checksum = ReadCmd_Id + ReadData[] + ExtLength
} Std_ReadDataFrameStr;//标准帧(读)结构体,用于发送给主机需要的数据

/*拓展帧(读数据)*/
typedef struct
{
    uint8_t ReadCmd_Id;
    uint8_t ReadData[MAX_DATA_LENGTH];//数据的长度取决于上一个拓展帧的ExtLength-3
    uint8_t ExtLength;//固定为0
    uint8_t checksum;
    uint8_t StopBit;
} Ext_ReadDataFrameStr;

/*触摸点信息结构体*/
typedef struct
{
    uint8_t TouchId;
    uint8_t Status;
    uint8_t XpositionMSB;
    uint8_t XpositionLSB;
    uint8_t YpositionMSB;
    uint8_t YpositionLSB;
} TP_TouchPointStr;


typedef struct
{
    uint8_t TouchNum;//触摸点数量
    TP_TouchPointStr TP_TouchPoint1;//触摸点1的信息
    TP_TouchPointStr TP_TouchPoint2;
} ReCmd_0x01_TP_TouchPoint_StdDataStr;//TP触摸点信息指令

typedef struct
{
    TP_TouchPointStr TP_TouchPoint3;
    TP_TouchPointStr TP_TouchPoint4;
    TP_TouchPointStr TP_TouchPoint5;
    TP_TouchPointStr TP_TouchPoint6;
    TP_TouchPointStr TP_TouchPoint7;
    TP_TouchPointStr TP_TouchPoint8;
    TP_TouchPointStr TP_TouchPoint9;
    TP_TouchPointStr TP_TouchPoint10;
} ReCmd_0x02_TP_TouchPoint_StdDataStr;//TP触摸点信息指令(拓展帧)

typedef struct
{
    uint8_t LcdStatus;//显示屏运行状态
} ReCmd_0x10_LCD_statusStr;//显示屏状态指令

typedef struct
{
    uint8_t RequestStatus;//请求发送背光值
} ReCmd_0x11_BackLightRequestStr;//请求发送背光值指令

typedef struct
{
    uint8_t TimeoutValue;//超时时间
    uint8_t Ticktime;//心跳包时间
} ReCmd_0x12_Resp_timeoutCmdStr;//响应超时指令

typedef struct
{
    uint8_t LcdBacklightTemStatus;//显示屏背光温度状态
    int16_t BacklightTemValue;//背光实时温度值
    uint8_t BacklightValue;//背光亮度
    uint8_t reserved;
    uint8_t BackLightPowerSwitchstatus;//背光电源开关状态
    uint8_t FOGPowerSwitchStatus;//FOG电源开关状态
} ReCmd_0x13_RequestLcdStatusCmdStr;//显示屏信息查询

typedef struct
{
    uint8_t SupplierInformation;//供应商信息
    uint8_t LcdSize;//显示屏尺寸
    uint8_t LcdDpi;//分辨率
    uint8_t LVDSFormat;//LVDS信号格式标准
    uint8_t LcdType;//显示屏类型
} ReCmd_0x14_ProductIDStr;//产品ID

typedef struct
{
    uint8_t HWversion;//硬件版本号整数位
    uint8_t HWVersionDot;//硬件版本号小数位
    uint8_t SWversion;//软件版本号整数位
    uint8_t SWVersionDot;//软件版本号小数位
    uint8_t TPversion;//TP版本号整数位
    uint8_t TPVersionDot;//TP版本号小数位1
    uint8_t TPVersionDot2;//TP版本号小数位2
    uint8_t HWVersion;
    uint8_t HWYear;
    uint8_t HWMonth;
    uint8_t HWDay;
    uint8_t SWVersion;
    uint8_t SWYear;
    uint8_t SWMonth;
    uint8_t SWDay;
} ReCmd_0x15_VersionStr;//版本号

typedef struct
{
    uint8_t VedioSignalStatus;//视频信号状态
    uint8_t BacklightValueReceive;//没有接受到背光亮度值
    uint8_t TouchHWErrorStatus;//触摸硬件异常
    uint8_t DisplaymodeErrorStatus;//显示模组错误检测脚状态
    uint8_t BacklightHWErrorStatus;//背光硬件异常
    uint8_t SerdesChipErrorStatus;//解串器芯片错误检测脚状态
    uint8_t BatterErrorStatus;//电池异常
    uint8_t DisplaymodeVoltageErrorStatus;//显示模组检测电压异常
    uint8_t TouchvoltageErrorStatus;//触摸电压检测状态
} ReCmd_0x16_LcdDiagnosisCmdStr;//显示屏诊断信息指令

typedef struct
{
    uint8_t DiagnosisResponse;//诊断响应 固定为0x3D
    uint8_t DiagnosisAddress;//物理寻址地址 固定0x28
    uint8_t FrameFormat;//有效报文长度
    uint8_t ResponseParameters1;//诊断响应参数
    uint8_t ResponseParameters2;
    uint8_t ResponseParameters3;
    uint8_t ResponseParameters4;
    uint8_t ResponseParameters5;
    uint8_t ResponseParameters6;
    uint8_t ResponseParameters7;
    uint8_t ResponseParameters8;
    uint8_t ResponseParameters9;
    uint8_t ResponseParameters10;
} ReCmd_0x17_UpdateCmdStr;//升级指令

typedef struct
{
    uint8_t RequestStatus;//请求状态,什么值都无所谓
} ReCmd_0x18_RequestTimestampCmdStr;//请求发送时间戳

typedef struct
{
    uint8_t KeyStatus1;//Key1 是否按下
    uint8_t KeyStatus2;
    uint8_t KeyStatus3;
    uint8_t KeyStatus4;
    uint8_t KeyStatus5;
} ReCmd_0x1E_KeyStatusCmdStr;//按键状态指令

typedef struct
{
    uint8_t LightSeneorValueLSB;//光线传感器值(低八位)
    uint8_t LightSeneorValueMSB;
} ReCmd_0x1F_LightSeneorValueStr;//光线传感器值

typedef struct
{
    uint8_t Part_1st_digit;//'0'~'9',零件号第1位
    uint8_t Part_2nd_digit;
    uint8_t Part_3rd_digit;    
    uint8_t Part_4th_digit; 
    uint8_t Part_5th_digit; 
    uint8_t Part_6th_digit; 
    uint8_t Part_7th_digit; 
    uint8_t Part_8th_digit; 
    uint8_t Part_9th_digit; 
    uint8_t Part_10th_digit;
} ReCmd_0x21_PartNumber;//光线传感器值

//每个ID的读取帧定义
extern Std_ReadDataFrameStr Std_ReadDataFrame_ID01;//暂时没触摸的功能,故不实现
extern Ext_ReadDataFrameStr Ext_ReadDataFrame_ID02;
extern Std_ReadDataFrameStr Std_ReadDataFrame_ID10;
extern Std_ReadDataFrameStr Std_ReadDataFrame_ID11;
extern Std_ReadDataFrameStr Std_ReadDataFrame_ID12;
extern Std_ReadDataFrameStr Std_ReadDataFrame_ID13;
extern Std_ReadDataFrameStr Std_ReadDataFrame_ID14;
extern Std_ReadDataFrameStr Std_ReadDataFrame_ID15;
extern Std_ReadDataFrameStr Std_ReadDataFrame_ID16;
extern Std_ReadDataFrameStr Std_ReadDataFrame_ID17;

//主机读取的数据的结构体定义,这些数据会在函数中轮询赋予对应ID的Std帧
extern ReCmd_0x01_TP_TouchPoint_StdDataStr TP_TouchPoint_StdDataStr;
extern ReCmd_0x02_TP_TouchPoint_StdDataStr TouchPoint_StdDataStr;
extern ReCmd_0x10_LCD_statusStr LCD_statusStr;
extern ReCmd_0x11_BackLightRequestStr BackLightRequestStr;
extern ReCmd_0x12_Resp_timeoutCmdStr Resp_timeoutCmdStr;
extern ReCmd_0x13_RequestLcdStatusCmdStr RequestLcdStatusCmdStr;
extern ReCmd_0x14_ProductIDStr ProductIDStr;
extern ReCmd_0x15_VersionStr VersionStr;
extern ReCmd_0x16_LcdDiagnosisCmdStr LcdDiagnosisCmdStr;
extern ReCmd_0x17_UpdateCmdStr UpdateCmdStr;
extern ReCmd_0x18_RequestTimestampCmdStr RequestTimestampCmdStr;
extern ReCmd_0x1E_KeyStatusCmdStr KeyStatusCmdStr;
extern ReCmd_0x1F_LightSeneorValueStr LightSeneorValueStr;
extern ReCmd_0x21_PartNumber PartNumber;

extern void ReadDataFrameInit();//初始化主机读取数据
extern void ReadCmdIdStrInit();//初始化各个Id结构体变量
extern uint8_t ReadFrameChecksumCount(Std_ReadDataFrameStr *ReadDataFrameStr);//计算checksum
extern void ReadFrameWriteData();//周期调用，给cmd_id结构体赋值
extern void ReadFrameTransmit(uint8_t cmd_id);//主机读取数据时调用，发送对应cmd_id的数据


#endif