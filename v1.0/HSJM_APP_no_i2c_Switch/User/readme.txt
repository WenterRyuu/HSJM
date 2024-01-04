//�ƻ���PA1011ֻ��IIC�ӻ�������Ϊ�˸�������linux��ͨѶ
//�ƻ���PB78�ϵ��ʱ��������������96752�������������Ժ���λ��ͨGPIO�˿ڣ�ʹ�õ��Թ���


�������������/////////////////////////////////////////////////////////////////////////////////////////////////			
���ű��	��������	��;							�ź���Դ						��Ч�ź�˵��
--------------------------------------------------------------------------------------------------------------
9			PA0			LOCK�ź�						�⴮��PIN13						�ߵ�ƽ��Ч
12			PA3			�����ѹ����ָʾ				LED Driver						�͵�ƽ��ʾerror���ߵ�ƽ��ʾnormal
15			PE13		LCD��ѹ����IC�Ĵ���ָʾ			��ԴоƬEPA9900��12������		�͵�ƽ��ʾerror���ߵ�ƽ��ʾnormal��������Ǹ���©���������PCB�н���������
17			PB13		Source Driver ����ָʾ			LCD�����34������				�͵�ƽ��ʾerror���ߵ�ƽ��ʾnormal
22			PA8			����MCU�̼���ָʾ				�⴮����41������				��Ҫ����ʱ���������Ҫ��Ϊ�ߵ�ƽ
25			PC10		�ⲿNTC�¶ȼ������				10pin�����7�����ţ�NTC+��		ģ��������
�ܼ�6��


�������������/////////////////////////////////////////////////////////////////////////////////////////////////		
���ű��	��������	��;							�ź�ָ��								��Ч�ź�˵��
-----------------------------------------------------------------------------------------------------------------
1			PC15		��ʾ���ܳ���ϴ���ָʾ�ź�		�⴮��
13			PA4			STBYB(�ϵ�ʱ��)					��Ļ��26������
14			PA7			����������(�ߵ�ƽ)				������
16			PE14		LCD_RESET						LCD�����25������
18			PB14		��SOC���ж��ź�IRQ				�⴮����29������
19			PD9			�⴮��powerdownʹ��				�⴮��									�ߵ�ƽ��Ч
26			PC11		�������PWM�ź�(��ʱ������)		LED Driver��MPQ3367GR����1������
27			PB3			����ʹ�ܺ�ʧ��					LED Driver��MPQ3367GR����4������
�ܼ�7��


IO���õ�����/////////////////////////////////////////////////////////////////////////////////////////////////	
���ű��	��������	��;							�ź�ָ��											��Ч�ź�˵��
----------------------------------------------------------------------------------------------------------------
7			PF5			SPI�������_SDAO(�����ֲ�MISO)	LCD�����89������
8			PC0			SPIʱ��_SCL						LCD�����87������
10			PA1			SPIƬѡ_CSB						LCD�����86������
11			PA2			SPI��������_SDAI(�����ֲ�MOSI)	LCD�����88������
23			PA10		I2C0_SCL��MCU���ӣ��⴮��������	�⴮����42�����š�TCON��45�����š�leddriver			�������źŵ�TCON��TCONͨ���⴮����MCUͨѶ
24			PA11		I2C0_SDA��MCU���ӣ��⴮��������	�⴮����40�����š�TCON��43�����š�leddriver			�������źŵ�TCON��TCONͨ���⴮����MCUͨѶ������TCON��MCUֻ��Ҫȥ��
29			PB7			I2C1_SCL��MCU�������⴮�����ӣ�	�⴮����9������
30			PB8			I2C1_SDA��MCU�������⴮�����ӣ�	�⴮����8������								
�ܼ�8��


��������
30			SWCLK(PB8)
31			NRST
32			SWDIO
2			BOOT0
�ܼ�4��



ָ���//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
0x10 ��ʾ��״ָ̬��  			����ʾ����⵽״̬�仯ʱ������������IRQ������
0x11 �����ͱ���ֵ	  		�������յ���ָ���Ӧ����100ms�ڸ���ʾ������CMD_ID:0x83
0x12 ��Ӧ��ʱָ��  			��ʾ����Ӧ�������õĳ�ʱʱ�䣬Ĭ��Ϊ100ms
0x13 ��ʾ����Ϣ��ѯָ��	  	��������Ҫ����ָ��
0x14 ��ƷID  				��������Ҫ����ָ��
0x15 �汾��  				��������Ҫ����ָ��
0x16 ��ʾ�� �����Ϣָ��  		����ʾ����⵽״̬�仯ʱ������IRQ�����ͣ��������Σ����1S��		10s������⵽LOCK�ź��øߺ�����������0x00����û�м�⵽����0x02��lock��ӦMCU PA0
0x17 ����ָ��
0x18 ������ʱ���  			�������յ���ָ���Ӧ����100ms�ڸ���ʾ������CMD_ID:0x85

0x81 ��ѯ��ʾ����Ϣ			��ʾ������ CMD_ID:0x13,0x14,0x15,0x16
0x82 ����ָ��֡��ʱʱ��  		��ʾ�����յ���ָ���Ӧ��Ӧ0x12ָ���ʱʱ������������Ĭ��100ms
0x83 ����ģʽָ��
0x85 ʱ���  				�������յ���ʾ����0x18ָ���Ӧ����100ms����Ӧ��ʾ��

�����ݲ���////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /*READ_CMD_ID 0x10  ��ʾ��״ָ̬��*/
    LCD_statusStr.LcdStatus = 0x00;//0x00: Reserved  0x01: Normal Mode  0x02: Upgrade Mode  0x03: Self test Mode

    /*READ_CMD_ID 0x11  �����ͱ���ֵ*/
    BackLightRequestStr.RequestStatus = 0x00;//0x00:NotCare  0x01:�����ͱ���ֵ

    /*READ_CMD_ID 0x12  ��Ӧ��ʱָ��*/
    Resp_timeoutCmdStr.Ticktime = 0x0A;//������������Ĭ�ϵģ�һ�㲻�ø���
    Resp_timeoutCmdStr.TimeoutValue = 0x14;//������������Ĭ�ϵģ�һ�㲻�ø���

    /*READ_CMD_ID 0x13  ��ʾ����Ϣ��ѯָ��*/
    RequestLcdStatusCmdStr.LcdBacklightTemStatus = 0x00;//0x00:�¶�����  0x01:�¶ȹ��ߣ����Ƚ���  0x02:�¶ȹ��ߣ���Ļ�ر�
    RequestLcdStatusCmdStr.BacklightTemValue = 0x00;//-40~125�з�������ʾ-40~125��
    RequestLcdStatusCmdStr.BacklightValue = 0x23;//��Ч��Χ0~100��ʾ�������� 0-100%
    RequestLcdStatusCmdStr.reserved = 0x00;//Ĭ��ֵ0����Ϊ����������չλ��16λ����ֵ����ʹ��
    RequestLcdStatusCmdStr.BackLightPowerSwitchstatus = 0x00;//0x00:����ر�  0x01:���⿪��
    RequestLcdStatusCmdStr.FOGPowerSwitchStatus = 0x00;//��Ӳ��û��FOG��·��0x00:FOG��Դ�ر�  0x01:FOG��Դ����

    /*READ_CMD_ID 0x14  ��ƷID*/
    ProductIDStr.SupplierInformation = 0x00;//0x00������  0x01�����չ�Ӧ������
    ProductIDStr.LcdSize = 0x00;//0x00������  0x01��8 Inch  0x02��9 Inch  0x03��10.1 Inch  0x04��10.25 Inch  0x05��10.4 Inch  0x06�� 12.3 Inch  0x07��12.6Inch  0x08:15.6Inch  0x09:16.2Inch  0x0A:27 Inch
    ProductIDStr.LcdDpi = 0x00;//0x00������  0x01: 1920*720  0x02��1280*720  0x03��1920*1080  0x04:1920*1200
    ProductIDStr.LVDSFormat = 0x00;//0x00������  0x01: VESA  0x02��JEIDA
    ProductIDStr.LcdType = 0x00;//0x00������  0x01���п���  0x02���Ǳ���  0x03���յ���  0x04��������  0x05��������

    /*READ_CMD_ID 0x15  �汾��*/
    VersionStr.HWversion = 0x00;//0~99(Dec)Ӳ����ʽ�����汾��
    VersionStr.HWVersionDot = 0x00;//0~99(Dec)Ӳ���汾��,Ԥ��
    VersionStr.SWversion = 0x00;//0~99(Dec)�����ʽ�����汾��
    VersionStr.SWVersionDot = 0x00;//0~99(Dec)����汾��,Ԥ��
    VersionStr.TPversion = 0x00;//0~99(Dec),TP Version number
    VersionStr.TPVersionDot = 0x00;//0~99(Dec),TP Version number
    VersionStr.TPVersionDot2 = 0x00;//0~99(Dec),TP Version number

    /*READ_CMD_ID 0x16  ��ʾ�� �����Ϣָ��*/
    LcdDiagnosisCmdStr.VedioSignalStatus = 0x00;//0x00��Lock  0x01����ʾ����֧�ָ����  0x02��No Lock
    LcdDiagnosisCmdStr.BacklightValueReceive = 0x00;//0x00���н��յ���������  0x01����ʾ����֧�ָ����  0x02��û�н��յ���������
    LcdDiagnosisCmdStr.TouchHWErrorStatus = 0x00;//0x00������  0x01����ʾ����֧�ָ����  0x02��оƬ������ű���  0x03����·  0x04����·
    LcdDiagnosisCmdStr.DisplaymodeErrorStatus = 0x00;//0x00������  0x01����ʾ����֧�ָ����  0x02��оƬ������ű���
    LcdDiagnosisCmdStr.BacklightHWErrorStatus = 0x00;//0x00������  0x01����ʾ����֧�ָ����  0x02��оƬ������ű���  0x03����·  0x04����·
    LcdDiagnosisCmdStr.SerdesChipErrorStatus = 0x00;//0x00������  0x01����ʾ����֧�ָ����  0x02��оƬ������ű���
    LcdDiagnosisCmdStr.BatterErrorStatus = 0x00;//0x00������  0x01����ʾ����֧�ָ����  0x02����ع�ѹ������������17V����16V,Ҫ�л������䣩  0x03�����Ƿѹ������������7V����9V��Ҫ�л������䣩
    LcdDiagnosisCmdStr.DisplaymodeVoltageErrorStatus = 0x00;//0x00������  0x01����ʾ����֧�ָ����  0x02����ѹ�쳣
    LcdDiagnosisCmdStr.TouchvoltageErrorStatus = 0x00;//0x00������  0x01����ʾ����֧�ָ����  0x02����ѹ�쳣

    /*READ_CMD_ID 0x17  ����ָ��*/
    UpdateCmdStr.DiagnosisResponse = 0x3D;//�����Ӧ
    UpdateCmdStr.DiagnosisAddress = 0x28;//����Ѱַ��ָ�����豸������Ϸ���
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

    /*READ_CMD_ID 0x18  ������ʱ���*/
    RequestTimestampCmdStr.RequestStatus = 0x00;//0x00:NotCare

    /*READ_CMD_ID 0x1E  ����״ָ̬��*/
    KeyStatusCmdStr.KeyStatus1 = 0x00;
    KeyStatusCmdStr.KeyStatus2 = 0x00;
    KeyStatusCmdStr.KeyStatus3 = 0x00;
    KeyStatusCmdStr.KeyStatus4 = 0x00;
    KeyStatusCmdStr.KeyStatus5 = 0x00;

    /*READ_CMD_ID 0x1F  ���ߴ�����ֵ*/
    LightSeneorValueStr.LightSeneorValueLSB = 0x00;
    LightSeneorValueStr.LightSeneorValueMSB = 0x00;






