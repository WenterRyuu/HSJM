/*******************************************************************************
 * Project	:车载屏幕MCU开发_BOE成都
 * MCU	  	:GD32A503KCU3
 * Data		:2023/10/24
 * Files	:spi_App.c
 * Library	:V1.1.0, firmware for GD32A50x
 * Function	:单片机SPI的功能应用函数
 * Author	:Liu Wei
 * Phone	:13349168457
 ******************************************************************************/
 #include "spi_App.h"


/*------------------------------------------------------------------------
*Function name		 :void SPI_Master_Init(void)
*Function description:SPI主机初始化函数
*Ipunt				 :none
*OutPut				 :none
-------------------------------------------------------------------------*/
void SPI_Master_Init(void)
{    
	
    spi_parameter_struct spi_init_struct = {0};		
	spi_i2s_deinit(SPI0);
    spi_struct_para_init(&spi_init_struct);
    
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;  
    spi_init_struct.device_mode          = SPI_MASTER;  // 主机模式
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;  // 8位数据
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;  // 空闲电平为，第个边沿采样-----------------
    spi_init_struct.nss                  = SPI_NSS_SOFT;  // 软件片选
    spi_init_struct.prescale             = SPI_PSC_256;  // 时钟256分频，100MHz / 256 = 390KHz
    spi_init_struct.endian               = SPI_ENDIAN_MSB;  // 大端模式
    spi_init(SPI0, &spi_init_struct);
    
    spi_nss_output_enable(SPI0);
    spi_enable(SPI0);
}

/*------------------------------------------------------------------------
*Function name		 :SPI_Delay(unsigned char xrate)
*Function description:用于SPI的延时
*Ipunt				 :要延时的时间长度
*OutPut				 :none
-------------------------------------------------------------------------*/
void SPI_Delay(unsigned char xrate)
{
	while(xrate--);
}

/*------------------------------------------------------------------------
*Function name		 :SPI_WriteCMD(unsigned char command)
*Function description:SPI发送一个字节的数据
*Ipunt				 :要发送的一个字节的指令
*OutPut				 :none
-------------------------------------------------------------------------*/
void SPI_Send_Byte(uint8_t value)
{
	CSB_LOW;
	uint16_t timeout = 1000;	//设置超时时间
	while((timeout > 0) && (RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE)))
	{
		timeout --;
	}	
	spi_i2s_data_transmit(SPI0, value);
	timeout = 1000;				//重置超时时间
	while((timeout > 0) && (RESET != spi_i2s_flag_get(SPI0,I2S_FLAG_TRANS)))
	{
		timeout --;
	}
	CSB_HIGH;
}

/*------------------------------------------------------------------------
*Function name		 :SPI_WriteCMD(unsigned char command)
*Function description:SPI写一个字节的数据到从设备的寄存器里面
*Ipunt				 :【1】从设备寄存器地址【2】要写入的数据
*OutPut				 :none
-------------------------------------------------------------------------*/
void SPI_Write_Data_to_Register(uint8_t command, uint8_t data)
{
	SPI_Delay(1);
	CSB_LOW;		
	SPI_Send_Byte(command);//0|00|command(5)|data(8)//command|0x00
	SPI_Send_Byte(data);
	CSB_HIGH;
}

/*------------------------------------------------------------------------
*Function name		 :SPI_WriteDATA(unsigned char data)
*Function description:SPI从从设备寄存器那里读取一个字节的数据
*Ipunt				 :从设备寄存器地址
*OutPut				 :从设备寄存器地址里面的值
-------------------------------------------------------------------------*/
uint8_t SPI_Read_Data_from_Register(uint8_t command)
{
	uint8_t received_data = 0;
	SPI_Delay(1);
	CSB_LOW;                   
	//-------------------------------------------------------------
	uint16_t timeout = 1000;	//设置超时时间
	while((timeout > 0) && (RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE)))
	{
		timeout --;
	}	
	spi_i2s_data_transmit(SPI0, command|0x80);
	timeout = 1000;				//重置超时时间
	while((timeout > 0) && (RESET != spi_i2s_flag_get(SPI0,I2S_FLAG_TRANS)))
	{
		timeout --;
	}	
	//------------------------------------1028-----------------------
	spi_i2s_data_transmit(SPI0, 0xff);
	timeout = 1000;
	while((timeout > 0) && (RESET != spi_i2s_flag_get(SPI0,I2S_FLAG_TRANS)))
	{
		timeout --;
	}		
	//-----------------------------------------------------------
    if(RESET == spi_i2s_interrupt_flag_get(SPI0, SPI_I2S_INT_FLAG_RBNE)) 
	{
	received_data = spi_i2s_data_receive(SPI0);
	}	
	CSB_HIGH;
	//////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//SPI_Send_Byte(command|0x80);//1|00|xxxxx|...
    /* received data *///、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、

        
		
	CSB_HIGH;
	
	return received_data;
}

/*------------------------------------------------------------------------
*Function name		 :void McuRcuInitialization(void) 
*Function description:SPI的初始化
*Ipunt				 :None
*OutPut				 :None
*-------------------------------------------------------------------------*/
void McuSpiInitialize(void)
{
	SPI_Master_Init();
}

/*------------------------------------------------------------------------
*Function name		 :SPI_WriteDATA(unsigned char data)
*Function description:SPI从source ic读数据
*Ipunt				 :none
*OutPut				 :none
-------------------------------------------------------------------------*/
void Source_IC_writeread(void)
{
	uint8_t reg_value[2] = {0x00, 0x48};
	SPI_Send_Bytes(reg_value, LENGTH_of_(reg_value));
	reg_value[0] = SPI_Read_Data_from_Register(0x01);
	reg_value[1] = SPI_Read_Data_from_Register(0x02);
}


	
/*------------------------------------------------------------------------
*Function name		 :SPI_WriteDATA(unsigned char data)
*Function description:SPI发送多个字节
*Ipunt				 :【1】要发送的数据指针【2】要发送的数据的字节个数
*OutPut				 :none
-------------------------------------------------------------------------*/
void SPI_Send_Bytes(uint8_t *data_to_send, uint8_t num_of_data)
{
	CSB_LOW;	
	uint8_t i = 0;
	uint16_t timeout = 1000;	//设置超时时间
	for(i=0; i<num_of_data; i++)
	{
		while((timeout > 0) && (RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE)))
		{
			timeout --;
		}	
		spi_i2s_data_transmit(SPI0, data_to_send[i]);
		timeout = 1000;				//重置超时时间
		while((timeout > 0) && (RESET != spi_i2s_flag_get(SPI0,I2S_FLAG_TRANS)))
		{
			timeout --;
		}		
	}
	CSB_HIGH;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void SPI_MTU_Send_Byte(uint8_t value)
//{
//	CSB_LOW;
//	uint16_t timeout = 1000;	//设置超时时间
//	while((timeout > 0) && (RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE)))
//	{
//		timeout --;
//	}	
//	spi_i2s_data_transmit(SPI0, value);
//	timeout = 1000;				//重置超时时间
//	while((timeout > 0) && (RESET != spi_i2s_flag_get(SPI0,I2S_FLAG_TRANS)))
//	{
//		timeout --;
//	}
//	CSB_HIGH;
//}
//uint8_t SPI_MTU_Receive_Byte(uint8_t value)
//{
//	uint8_t received_data = 0;
//	SPI_Delay(1);
//	CSB_LOW;                   
//    /* received data *///、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、
//    if(RESET != spi_i2s_interrupt_flag_get(SPI0, SPI_I2S_INT_FLAG_RBNE)) 
//	{
//        received_data = spi_i2s_data_receive(SPI0);
//    }	
//	CSB_HIGH;
//	
//	return received_data;
//}










