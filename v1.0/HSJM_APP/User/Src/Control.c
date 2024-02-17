/*******************************************************************************
 * Project	:车载屏幕MCU开发_BOE成都
 * MCU	  	:GD32A503KCU3
 * Data		:2023/10/23
 * Files	:Backlight.c
 * Library	:V1.1.0, firmware for GD32A50x
 * Function	:应用控制
 * Author	:Liu Wei
 * Phone	:13349168457
 ******************************************************************************/
 #include "Control.h"





_ScreenLitAfterStartup ScreenLitAfterStartup;

void IRQ_LOOW_DOWN(void)
{
    if(gpio_input_bit_get(GPIOB, GPIO_PIN_14) == SET)                   //如果是高电平，就马上拉低
    {
        IRQ_LOW_DOWN;
        IRQ_100ms = 100;
    }
    else
    {
        while(gpio_input_bit_get(GPIOB, GPIO_PIN_14) == RESET);         //如果是低电平，就一直等待，直到变成高电平
        for(uint8_t i=0; i<50; i++)                                     //维持高电平一小段时间
        {
            __NOP();
        }
        IRQ_LOW_DOWN;                                                   //然后再拉低
        IRQ_100ms = 100;                                                
    }
}



uint8_t flag = 0;
void process_0x81(void)
{
	uint8_t index = 0;
	uint8_t id[5] = {0x13,0x14,0x15,0x16,0x21}; 
    
	for(index=0; index<5; index++)
	{
        while(gpio_input_bit_get(GPIOB, GPIO_PIN_14) == RESET);
        ReadFrameTransmit(id[index]);
        IRQ_RELEASE;
        delay_1ms(10);		
        IRQ_LOW_DOWN;
		IRQ_100ms = 100;
	}
    
    if(flag == 0)
    {
        nvic_irq_enable(TIMER6_IRQn, 0, 0);
        flag = 1;
    }   

}


uint8_t index=0;
bool handshake_is_ok = 0;
void handshake_process(void)
{	 
    Std_WriteDataFrameStr   Std_WriteDataFrame;
    uint32_t timeout = 10000000;
	switch(index)
	{
		case 0:
			ReadFrameTransmit(0x10) ; 
            if(gpio_input_bit_get(GPIOB, GPIO_PIN_14) == SET)                   //如果是高电平，就马上拉低
            {
                IRQ_LOW_DOWN;
                IRQ_100ms = 100;
            }
			else
            {
                while(gpio_input_bit_get(GPIOB, GPIO_PIN_14) == RESET);         //如果是低电平，就一直等待，直到变成高电平
                for(uint8_t i=0; i<50; i++)                                     //维持高电平一小段时间
                {
                    __NOP();
                }
                IRQ_LOW_DOWN;                                                   //然后再拉低
                IRQ_100ms = 100;                                                
            }
            delay_1ms(110);
            index ++;
          
		case 1:
#if defined ok            
/*            
//            timeout = 1000000;                                                  //接近10ms
//            do
//            {
//                WriteFrameTransmit();//主机发来0x85，去处理0x85的
//                timeout --;
//            }
//            while((handshake_is_ok == 0)&&(timeout > 0));
//            if(handshake_is_ok)                                                 //退出这个循环只有两种情况，一个是向主机发了0x15，handshake_is_ok，还有就是超时
//            {
//                delay_1ms(100);                                                 //
//                index ++;                                                      
//            }
//            else
//            {
//                index --;
//            }                
//            break;
*/
#endif 
            if(Update_tI2cSlave.RecBuff[0] == 0x85)
            {
                ReadFrameTransmit(0x15);            
                IRQ_RELEASE;
                delay_1ms(10);
                IRQ_LOW_DOWN;
                IRQ_100ms = 100;
                index ++;
            }
            else
            {
                index --;
                break;
            }
          
        case 2:            
			break;

		default:
			break;
	}
	
}

ErrStatus handshake(void)
{
	do
	{
		handshake_process();
	}
	while(index != 2);
	return SUCCESS;
}
