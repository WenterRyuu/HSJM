
#include "Define.h"

void Updating(void);
extern void i2c1_78_Slave_config(void);
uint16_t LOCK_10S = 10000;
bool finish = 0;

void com_usart_init(void)
{
    /* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    
    /* enable USART clock */
    rcu_periph_clock_enable(RCU_USART0);
    rcu_usart_clock_config(USART0, RCU_USARTSRC_CKSYS);

    /* connect port to USART TX */
    gpio_af_set(GPIOA, GPIO_AF_5, GPIO_PIN_3);
    /* connect port to USART RX */
    gpio_af_set(GPIOA, GPIO_AF_5, GPIO_PIN_4);

    /* configure USART TX as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_3);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_3);

    /* configure USART RX as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_4);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_4);

    /* USART configure */
    usart_deinit(USART0);
    usart_word_length_set(USART0, USART_WL_8BIT);
    usart_stop_bit_set(USART0, USART_STB_1BIT);
    usart_parity_config(USART0, USART_PM_NONE);
    usart_baudrate_set(USART0, 115200U);
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
    usart_enable(USART0);
}


int main(void)
{
	nvic_vector_table_set(NVIC_VECTTAB_FLASH, 0x5000);
	Initialization();	
	fmc_uint64_program(APP2BOOT_FLAG_ADDRESS,0);	
    
//	Update_Brightness(50);
    
//【I2C】MAX96752--------------------------------------------------------
	MAX96752_REG_Config();                                                      //这个必须要有，必须要执行完成，否则主机不能传输数据到显示屏
	I2C_Slave_Config();
//	Update_Brightness(50);
	
//【SPI】Source_IC-------------------------------------------------------
//	SPI_Send_Bytes(data, LENGTH_of_(data));
//	Source_IC_writeread();
	
//【I2C】LED Driver------------------------------------------------------	
//	Read_MPQ3367_Register();
	
//TCON-------------------------------------------------------------------	
//	Read_TCON_Register();
	
//    printf("ok");
	delay_1ms(5000);                                                            //@20240104,开机就绪准备时间
	ReadCmdIdStrInit();
	handshake();                                                                //握手
    nvic_irq_enable(TIMER6_IRQn, 0, 0);                                         //开启2秒心跳包定时中断
    
    int i = 1;
    while(1) 
	{
		WriteFrameTransmit();                                                   //主机向显示屏发，显示屏应该做的响应      

        
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


