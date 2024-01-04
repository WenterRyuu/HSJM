
#include "Define.h"

void Updating(void);
extern void i2c1_78_Slave_config(void);
uint16_t LOCK_10S = 10000;
bool finish = 0;
int main(void)
{
	nvic_vector_table_set(NVIC_VECTTAB_FLASH, 0x5000);
	Initialization();	
	fmc_uint64_program(APP2BOOT_FLAG_ADDRESS,0);	
    
//	Update_Brightness(50);
    
//【I2C】MAX96752--------------------------------------------------------
	MAX96752_REG_Config();
	I2C_Slave_Config();	
//	Update_Brightness(50);
	
//【SPI】Source_IC-------------------------------------------------------
//	SPI_Send_Bytes(data, LENGTH_of_(data));
//	Source_IC_writeread();
	
//【I2C】LED Driver------------------------------------------------------	
//	Read_MPQ3367_Register();
	
//TCON-------------------------------------------------------------------	
//	Read_TCON_Register();
	
    
	delay_1ms(5000);//@20240104,开机就绪准备时间
	ReadCmdIdStrInit();
	handshake();//握手
    
    delay_1ms(500);
    Timer5_Generate_2s_Interrupt();
	delay_1ms(1000);
	Timer6_Generate_2s_Interrupt();//有两个需求，都要间隔2秒触发一次中断，为了不冲突，这两个定时器间隔1秒开启
    

    while(1) 
	{
		WriteFrameTransmit();//主机向显示屏发，显示屏应该做的响应
        

        if(Counter_1ms.NTC == 0)
		{
			Counter_1ms.NTC = Temperature_Measurement_Cycle;
			readTemper((uint16_t)adc_mean_filter(),&RequestLcdStatusCmdStr.BacklightTemValue);
			if(Backlight.Final_Temperature >= NTC_MAX)	
				Update_Brightness(Backlight.Backlight_PWM -= 5);
		}
        
		if(Counter_1ms.RData_Loading_Cycle == 0)
		{
			Counter_1ms.RData_Loading_Cycle = READ_DATA_LOADING_CYCLE;          
			ReadFrameWriteData();                                               //不停检查
		}
		
		
		//---------------------------
		if(compareArrays(Update_tI2cSlave.RecBuff, Std_Receive_Arr.RequestBootloaderAccess, sizeof(Std_Receive_Arr.RequestBootloaderAccess)) == true)
		{
			cx();                                                               //查询bootloder状态
			Updating();

		}

		


    }
}





