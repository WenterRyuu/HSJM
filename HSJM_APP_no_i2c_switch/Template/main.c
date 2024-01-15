
#include "Define.h"

void Updating(void);
extern void i2c1_78_Slave_config(void);
uint16_t LOCK_10S = 10000;
bool finish = 0;
int main(void)
{
//	nvic_vector_table_set(NVIC_VECTTAB_FLASH, 0x5000);
	Initialization();	
	fmc_uint64_program(APP2BOOT_FLAG_ADDRESS,0);	
    
//	Update_Brightness(50);
    
//��I2C��MAX96752--------------------------------------------------------
//	MAX96752_REG_Config();
//	I2C_Slave_Config();	
//	Update_Brightness(50);
	
//��SPI��Source_IC-------------------------------------------------------
//	SPI_Send_Bytes(data, LENGTH_of_(data));
//	Source_IC_writeread();
	
//��I2C��LED Driver------------------------------------------------------	
//	Read_MPQ3367_Register();
	
//TCON-------------------------------------------------------------------	
//	Read_TCON_Register();
	
    
	delay_1ms(5000);                                                            //@20240104,��������׼��ʱ��
	ReadCmdIdStrInit();
	handshake();                                                                //����
    nvic_irq_enable(TIMER6_IRQn, 0, 0);                                         //����2����������ʱ�ж�

    while(1) 
	{
		WriteFrameTransmit();                                                   //��������ʾ��������ʾ��Ӧ��������Ӧ      

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
            ReadCmdIdStrInit();
			ReadFrameWriteData();                                               //��ͣ���
		}
		
		
		//---------------------------
        if(compareArrays(Update_tI2cSlave.RecBuff, Std_Receive_Arr.QueryBootloaderStatus, sizeof(Std_Receive_Arr.QueryBootloaderStatus)) == true)
        {                
            nvic_irq_disable(TIMER6_IRQn);
            nvic_irq_disable(TIMER5_DAC_IRQn);
            BootLoader_State = READY;
            //�������״̬
            switch(BootLoader_State)//��ʲô״̬����ʲô���飬��ת����Ӧ�Ĳ���
            {
                case READY:
                    memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.Check_Ready, sizeof(Std_Replay_Arr.Check_Ready));
                    Update_tI2cSlave.SendSize = 11;
                    IRQ_LOW_DOWN;//�����ж�֪ͨ��������
                    Counter_1ms.IRQ_Update = 100;
                    break;
                default:
                    break;
            }
        }       
        
        if(compareArrays(Update_tI2cSlave.RecBuff, Std_Receive_Arr.RequestBootloaderAccess, sizeof(Std_Receive_Arr.RequestBootloaderAccess)) == true)
		{                                                               //��ѯbootloder״̬
			Updating();
		}

		


    }
}





