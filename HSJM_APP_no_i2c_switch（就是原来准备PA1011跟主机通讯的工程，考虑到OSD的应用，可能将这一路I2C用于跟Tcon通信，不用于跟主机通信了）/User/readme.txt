//计划，PA1011只做IIC从机，就是为了跟主机（linux）通讯
//计划，PB78上电的时候作主机（配置96752），配置完了以后作位普通GPIO端口，使用调试功能


用作输入的引脚/////////////////////////////////////////////////////////////////////////////////////////////////			
引脚编号	引脚名称	用途							信号来源						有效信号说明
--------------------------------------------------------------------------------------------------------------
9			PA0			LOCK信号						解串器PIN13						高电平有效
12			PA3			背光电压错误指示				LED Driver						低电平表示error，高电平表示normal
15			PE13		LCD电压驱动IC的错误指示			电源芯片EPA9900的12号引脚		低电平表示error，高电平表示normal，这个脚是个开漏输出，我们PCB有接上拉电阻
17			PB13		Source Driver 错误指示			LCD排针的34号引脚				低电平表示error，高电平表示normal
22			PA8			升级MCU固件的指示				解串器的41号引脚				需要升级时，这个叫需要拉为高电平
25			PC10		外部NTC温度检测输入				10pin排针的7号引脚（NTC+）		模拟量输入
总计6个


用作输出的引脚/////////////////////////////////////////////////////////////////////////////////////////////////		
引脚编号	引脚名称	用途							信号指向								有效信号说明
-----------------------------------------------------------------------------------------------------------------
1			PC15		显示屏总成诊断错误指示信号		解串器
13			PA4			STBYB(上电时序)					屏幕的26号引脚
14			PA7			驱动蜂鸣器(高电平)				蜂鸣器
16			PE14		LCD_RESET						LCD排针的25号引脚
18			PB14		给SOC的中断信号IRQ				解串器的29号引脚
19			PD9			解串器powerdown使能				解串器									高电平有效
26			PC11		背光调光PWM信号(定时器复用)		LED Driver（MPQ3367GR）的1号引脚
27			PB3			背光使能和失能					LED Driver（MPQ3367GR）的4号引脚
总计7个


IO复用的引脚/////////////////////////////////////////////////////////////////////////////////////////////////	
引脚编号	引脚名称	用途							信号指向											有效信号说明
----------------------------------------------------------------------------------------------------------------
7			PF5			SPI数据输出_SDAO(数据手册MISO)	LCD排针的89号引脚
8			PC0			SPI时钟_SCL						LCD排针的87号引脚
10			PA1			SPI片选_CSB						LCD排针的86号引脚
11			PA2			SPI数据输入_SDAI(数据手册MOSI)	LCD排针的88号引脚
23			PA10		I2C0_SCL（MCU作从，解串器作主）	解串器的42号引脚、TCON的45号引脚、leddriver			主机发信号到TCON，TCON通过解串器和MCU通讯
24			PA11		I2C0_SDA（MCU作从，解串器作主）	解串器的40号引脚、TCON的43号引脚、leddriver			主机发信号到TCON，TCON通过解串器和MCU通讯，对于TCON，MCU只需要去读
29			PB7			I2C1_SCL（MCU作主，解串器作从）	解串器的9号引脚
30			PB8			I2C1_SDA（MCU作主，解串器作从）	解串器的8号引脚								
总计8个


调试引脚
30			SWCLK(PB8)
31			NRST
32			SWDIO
2			BOOT0
总计4个



指令表//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
0x10 显示屏状态指令  			当显示屏检测到状态变化时，会立即拉低IRQ请求发送
0x11 请求发送背光值	  		当主机收到该指令后，应当在100ms内给显示屏发送CMD_ID:0x83
0x12 响应超时指令  			显示屏响应主机设置的超时时间，默认为100ms
0x13 显示屏信息查询指令	  	主机不需要返回指令
0x14 产品ID  				主机不需要返回指令
0x15 版本号  				主机不需要返回指令
0x16 显示屏 诊断信息指令  		当显示屏检测到状态变化时，拉低IRQ请求发送（发送两次，间隔1S）		10s内若检测到LOCK信号置高后，向主机发送0x00，若没有检测到发送0x02，lock对应MCU PA0
0x17 升级指令
0x18 请求发送时间戳  			当主机收到该指令后，应当在100ms内给显示屏发送CMD_ID:0x85

0x81 查询显示屏信息			显示屏返回 CMD_ID:0x13,0x14,0x15,0x16
0x82 设置指令帧超时时间  		显示屏接收到该指令后应响应0x12指令，超时时间主机不设置默认100ms
0x83 背光模式指令
0x85 时间戳  				当主机收到显示屏的0x18指令后，应当在100ms内响应显示屏

读数据参数////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /*READ_CMD_ID 0x10  显示屏状态指令*/
    LCD_statusStr.LcdStatus = 0x00;//0x00: Reserved  0x01: Normal Mode  0x02: Upgrade Mode  0x03: Self test Mode

    /*READ_CMD_ID 0x11  请求发送背光值*/
    BackLightRequestStr.RequestStatus = 0x00;//0x00:NotCare  0x01:请求发送背光值

    /*READ_CMD_ID 0x12  响应超时指令*/
    Resp_timeoutCmdStr.Ticktime = 0x0A;//这两个参数是默认的，一般不用更改
    Resp_timeoutCmdStr.TimeoutValue = 0x14;//这两个参数是默认的，一般不用更改

    /*READ_CMD_ID 0x13  显示屏信息查询指令*/
    RequestLcdStatusCmdStr.LcdBacklightTemStatus = 0x00;//0x00:温度正常  0x01:温度过高，亮度降低  0x02:温度过高，屏幕关闭
    RequestLcdStatusCmdStr.BacklightTemValue = 0x00;//-40~125有符号数表示-40~125℃
    RequestLcdStatusCmdStr.BacklightValue = 0x23;//有效范围0~100表示背光亮度 0-100%
    RequestLcdStatusCmdStr.reserved = 0x00;//默认值0，此为背光亮度扩展位，16位背光值才需使用
    RequestLcdStatusCmdStr.BackLightPowerSwitchstatus = 0x00;//0x00:背光关闭  0x01:背光开启
    RequestLcdStatusCmdStr.FOGPowerSwitchStatus = 0x00;//【硬件没有FOG电路】0x00:FOG电源关闭  0x01:FOG电源开启

    /*READ_CMD_ID 0x14  产品ID*/
    ProductIDStr.SupplierInformation = 0x00;//0x00：保留  0x01：按照供应商名称
    ProductIDStr.LcdSize = 0x00;//0x00：保留  0x01：8 Inch  0x02：9 Inch  0x03：10.1 Inch  0x04：10.25 Inch  0x05：10.4 Inch  0x06： 12.3 Inch  0x07：12.6Inch  0x08:15.6Inch  0x09:16.2Inch  0x0A:27 Inch
    ProductIDStr.LcdDpi = 0x00;//0x00：保留  0x01: 1920*720  0x02：1280*720  0x03：1920*1080  0x04:1920*1200
    ProductIDStr.LVDSFormat = 0x00;//0x00：保留  0x01: VESA  0x02：JEIDA
    ProductIDStr.LcdType = 0x00;//0x00：保留  0x01：中控屏  0x02：仪表屏  0x03：空调屏  0x04：扶手屏  0x05：后枕屏

    /*READ_CMD_ID 0x15  版本号*/
    VersionStr.HWversion = 0x00;//0~99(Dec)硬件正式发布版本号
    VersionStr.HWVersionDot = 0x00;//0~99(Dec)硬件版本号,预留
    VersionStr.SWversion = 0x00;//0~99(Dec)软件正式发布版本号
    VersionStr.SWVersionDot = 0x00;//0~99(Dec)软件版本号,预留
    VersionStr.TPversion = 0x00;//0~99(Dec),TP Version number
    VersionStr.TPVersionDot = 0x00;//0~99(Dec),TP Version number
    VersionStr.TPVersionDot2 = 0x00;//0~99(Dec),TP Version number

    /*READ_CMD_ID 0x16  显示屏 诊断信息指令*/
    LcdDiagnosisCmdStr.VedioSignalStatus = 0x00;//0x00：Lock  0x01：显示屏不支持该诊断  0x02：No Lock
    LcdDiagnosisCmdStr.BacklightValueReceive = 0x00;//0x00：有接收到背光亮度  0x01：显示屏不支持该诊断  0x02：没有接收到背光亮度
    LcdDiagnosisCmdStr.TouchHWErrorStatus = 0x00;//0x00：正常  0x01：显示屏不支持该诊断  0x02：芯片错误检测脚报错  0x03：短路  0x04：开路
    LcdDiagnosisCmdStr.DisplaymodeErrorStatus = 0x00;//0x00：正常  0x01：显示屏不支持该诊断  0x02：芯片错误检测脚报错
    LcdDiagnosisCmdStr.BacklightHWErrorStatus = 0x00;//0x00：正常  0x01：显示屏不支持该诊断  0x02：芯片错误检测脚报错  0x03：短路  0x04：开路
    LcdDiagnosisCmdStr.SerdesChipErrorStatus = 0x00;//0x00：正常  0x01：显示屏不支持该诊断  0x02：芯片错误检测脚报错
    LcdDiagnosisCmdStr.BatterErrorStatus = 0x00;//0x00：正常  0x01：显示屏不支持该诊断  0x02：电池过压（举例：高于17V低于16V,要有回滞区间）  0x03：电池欠压（举例：低于7V高于9V，要有回滞区间）
    LcdDiagnosisCmdStr.DisplaymodeVoltageErrorStatus = 0x00;//0x00：正常  0x01：显示屏不支持该诊断  0x02：电压异常
    LcdDiagnosisCmdStr.TouchvoltageErrorStatus = 0x00;//0x00：正常  0x01：显示屏不支持该诊断  0x02：电压异常

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






