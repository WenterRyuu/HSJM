/*******************************************************************************
 * Project	:������ĻMCU����_BOE�ɶ�
 * MCU	  	:GD32A503KCU3
 * Data		:2023/10/23
 * Files	:Backlight.c
 * Library	:V1.1.0, firmware for GD32A50x
 * Function	:Ӧ�ÿ���
 * Author	:Liu Wei
 * Phone	:13349168457
 ******************************************************************************/
 #include "Control.h"





_ScreenLitAfterStartup ScreenLitAfterStartup;

void IRQ_LOOW_DOWN(void)
{
    if(gpio_input_bit_get(GPIOB, GPIO_PIN_14) == SET)                   //����Ǹߵ�ƽ������������
    {
        IRQ_LOW_DOWN;
        IRQ_100ms = 100;
    }
    else
    {
        while(gpio_input_bit_get(GPIOB, GPIO_PIN_14) == RESET);         //����ǵ͵�ƽ����һֱ�ȴ���ֱ����ɸߵ�ƽ
        for(uint8_t i=0; i<50; i++)                                     //ά�ָߵ�ƽһС��ʱ��
        {
            __NOP();
        }
        IRQ_LOW_DOWN;                                                   //Ȼ��������
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
            if(gpio_input_bit_get(GPIOB, GPIO_PIN_14) == SET)                   //����Ǹߵ�ƽ������������
            {
                IRQ_LOW_DOWN;
                IRQ_100ms = 100;
            }
			else
            {
                while(gpio_input_bit_get(GPIOB, GPIO_PIN_14) == RESET);         //����ǵ͵�ƽ����һֱ�ȴ���ֱ����ɸߵ�ƽ
                for(uint8_t i=0; i<50; i++)                                     //ά�ָߵ�ƽһС��ʱ��
                {
                    __NOP();
                }
                IRQ_LOW_DOWN;                                                   //Ȼ��������
                IRQ_100ms = 100;                                                
            }
            delay_1ms(110);
            index ++;
          
		case 1:
#if defined ok            
/*            
//            timeout = 1000000;                                                  //�ӽ�10ms
//            do
//            {
//                WriteFrameTransmit();//��������0x85��ȥ����0x85��
//                timeout --;
//            }
//            while((handshake_is_ok == 0)&&(timeout > 0));
//            if(handshake_is_ok)                                                 //�˳����ѭ��ֻ�����������һ��������������0x15��handshake_is_ok�����о��ǳ�ʱ
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
