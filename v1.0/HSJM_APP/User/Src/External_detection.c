#include "External_detection.h"




_IO_Detection IO_Detection;
uint8_t Detection_Counter = 0;

/*------------------------------------------------------------------------
*Function name		 :PIN_level_Detection
*Function description:Ӧ�������ʼ��
*Ipunt				 :��1���˿ڡ�2�����š�3������Ĳ���������HIGH����LOW�������е�һ��
*OutPut				 :��⵽�ĵ�ƽ��ȷ���û����ú���ʱ������Ĳ������򷵻�SUCCESS�������������ERROR
*-------------------------------------------------------------------------*/
ErrStatus PIN_level_Detection(uint32_t gpio_periph, uint32_t pin, Voltage_level_Detection need_to_be_detected_level)
{
	if((need_to_be_detected_level != LOW) && (need_to_be_detected_level != HIGH))
	{
		return ERROR;
	}
	
	Voltage_level_Detection RM5366_error_state = IDLEE;
	do
	{
		switch(RM5366_error_state)
		{
			case IDLEE:
				if(gpio_input_bit_get(gpio_periph, pin) == (FlagStatus)need_to_be_detected_level)
				{
					RM5366_error_state = need_to_be_detected_level;
				}			
			case LOW:
				IO_Detection.Detection_Counter = 3;
				RM5366_error_state = WAIT;
				break;		
			case HIGH:
				IO_Detection.Detection_Counter = 3;
				RM5366_error_state = WAIT;
				break;			
			case WAIT:
				if(IO_Detection.Detection_Counter == 0)
				{
					RM5366_error_state = CHECK;
					break;
				}
				break;
			case CHECK:
				if(gpio_input_bit_get(gpio_periph, pin) == (FlagStatus)need_to_be_detected_level)
				{
					RM5366_error_state = IDLEE;
					return SUCCESS;
				}
				else
				{
					RM5366_error_state = IDLEE;
					return ERROR;			
				}			
			default:
				return ERROR;			
				break;
		}	
	}
	while(RM5366_error_state != IDLEE);
	return ERROR;
}


/*------------------------------------------------------------------------
*Function name		 :LOCK_Detection
*Function description:LOCK�ź�
*Ipunt				 :none
*OutPut				 :[1]SUCCESS���ɹ���⵽��LOCK�ź�[2]ERROR:û�м�⵽LOCK�ź�
*-------------------------------------------------------------------------*/
ErrStatus LOCK_Detection(void)
{	
	return 	PIN_level_Detection(GPIOA, GPIO_PIN_0, HIGH);
}
/*------------------------------------------------------------------------
*Function name		 :Backlight_level_Error_Detection
*Function description:�����ѹ����ָʾ	
*Ipunt				 :none
*OutPut				 :[1]SUCCESS���ɹ���⵽��[2]ERROR:û�м�⵽
*-------------------------------------------------------------------------*/
ErrStatus Backlight_level_Error_Detection(void)
{
	return PIN_level_Detection(GPIOA, GPIO_PIN_3, LOW);
}
/*------------------------------------------------------------------------
*Function name		 :LCD_level_error_Detection
*Function description:LCD��ѹ����IC�Ĵ���ָʾ	
*Ipunt				 :none
*OutPut				 :[1]SUCCESS���ɹ���⵽��[2]ERROR:û�м�⵽
*-------------------------------------------------------------------------*/
ErrStatus LCD_level_error_Detection(void)
{
	return PIN_level_Detection(GPIOE, GPIO_PIN_13, LOW);
}

/*------------------------------------------------------------------------
*Function name		 :Source_ic_Error_Detection
*Function description:Source Driver ����ָʾ	
*Ipunt				 :none
*OutPut				 :[1]SUCCESS���ɹ���⵽��[2]ERROR:û�м�⵽
*-------------------------------------------------------------------------*/
ErrStatus Source_ic_Error_Detection(void)
{
	return PIN_level_Detection(GPIOB, GPIO_PIN_13, LOW);
}

/*------------------------------------------------------------------------
*Function name		 :Update_level_Detection
*Function description:����MCU�̼���ָʾ
*Ipunt				 :none
*OutPut				 :[1]SUCCESS���ɹ���⵽��[2]ERROR:û�м�⵽
*-------------------------------------------------------------------------*/
ErrStatus Update_level_Detection(void)
{
	return PIN_level_Detection(GPIOA, GPIO_PIN_8, HIGH);
}

/*------------------------------------------------------------------------
*Function name		 :Update_level_Detection
*Function description:����MCU�̼���ָʾ
*Ipunt				 :none
*OutPut				 :[1]SUCCESS���ɹ���⵽��[2]ERROR:û�м�⵽
*-------------------------------------------------------------------------*/
ErrStatus IRQ_HIGH_level_Detection(void)
{
	return PIN_level_Detection(GPIOB, GPIO_PIN_14, HIGH);
}


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
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "string.h"
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
        if(Update_tI2cSlave.RecBuff[0] == 0x85)//!!û��У�飡������������
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

//////////////////////////////////////////////////////////////////////////////////////////////

void cmd_process()
{
	WriteFrameTransmit();
	switch(MasterTransmitData[0])
	{
		case 0x10:
			break;
		case 0x11:
			break;
		case 0x12:
			break;
		case 0x13:
			break;
		case 0x14:
			ReadFrameTransmit(0x14) ;
			WriteFrameTransmit();
			IRQ_LOW_DOWN;
			IRQ_100ms = 100;
			while(tI2cSlave.uFlag.Bits.SendSucess != 1);
			tI2cSlave.uFlag.Bits.SendSucess = 0;
			IRQ_RELEASE;
			break;
		case 0x15:
			break;
		case 0x16:
			break;
		case 0x17:
			break;
	}

}