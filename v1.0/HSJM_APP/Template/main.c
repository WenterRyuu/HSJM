
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
        
    //上电默认的硬件版本号，写到Flash
    VersionStr.HWversion = 0x00;//0~99(Dec)硬件正式发布版本号
    VersionStr.HWVersionDot = 0x00;//0~99(Dec)硬件版本号,预留
    VersionStr.HWVersion = 0x01;
    VersionStr.HWYear = 0x23;
    VersionStr.HWMonth = 0x12;
    VersionStr.HWDay = 0x20;
    HW_VERSION_Write();
    
    uint8_t *ptr;
    ptr = PART_ID_ADDRESS_5;
    if(*ptr == 0xFF)
    {
        //上电默认的零件号，写到flash
        PartNumber.Part_1st_digit = 0x32;//248102HH0A
        PartNumber.Part_2nd_digit = 0x34;
        PartNumber.Part_3rd_digit = 0x38;
        PartNumber.Part_4th_digit = 0x31;
        PartNumber.Part_5th_digit = 0x30;
        PartNumber.Part_6th_digit = 0x32;
        PartNumber.Part_7th_digit = 0x48;
        PartNumber.Part_8th_digit = 0x48;
        PartNumber.Part_9th_digit = 0x30;
        PartNumber.Part_10th_digit = 0x41;
        Part_Number_Write();        
    }
    else
    {
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
	
    
	delay_1ms(5000);                                                            //@20240104,开机就绪准备时间
	ReadCmdIdStrInit();
	handshake();                                                                //握手
    nvic_irq_enable(TIMER6_IRQn, 0, 0);                                         //开启2秒心跳包定时中断
    
    int i = 1;
    while(1) 
	{
		WriteFrameTransmit();                                                   //主机向显示屏发，显示屏应该做的响应      
        
        printf("ok\n");
        if(i == 1)
        {
            ReadFrameTransmit(0x15);            
            IRQ_RELEASE;
            delay_1ms(10);
            IRQ_LOW_DOWN;
            IRQ_100ms = 100;
            i = 0;
        }
        
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
//            ReadCmdIdStrInit();
			ReadFrameWriteData();                                               //不停检查
		}
		
		
		//---------------------------
        cx();        
        if(compareArrays(Update_tI2cSlave.RecBuff, Std_Receive_Arr.RequestBootloaderAccess, sizeof(Std_Receive_Arr.RequestBootloaderAccess)) == true)
		{
			cx();                                                               //查询bootloder状态
			Updating();
		}

    }
}




/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(USART0, (uint8_t)ch);
    while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
    return ch;
}



