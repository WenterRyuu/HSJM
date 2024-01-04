#include "External_detection.h"




_IO_Detection IO_Detection;
uint8_t Detection_Counter = 0;

/*------------------------------------------------------------------------
*Function name		 :PIN_level_Detection
*Function description:应用外设初始化
*Ipunt				 :【1】端口【2】引脚【3】传入的参数必须是HIGH或者LOW，两个中的一个
*OutPut				 :检测到的电平的确是用户调用函数时所传入的参数，则返回SUCCESS，其他情况返回ERROR
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
*Function description:LOCK信号
*Ipunt				 :none
*OutPut				 :[1]SUCCESS：成功检测到了LOCK信号[2]ERROR:没有检测到LOCK信号
*-------------------------------------------------------------------------*/
ErrStatus LOCK_Detection(void)
{	
	return 	PIN_level_Detection(GPIOA, GPIO_PIN_0, HIGH);
}
/*------------------------------------------------------------------------
*Function name		 :Backlight_level_Error_Detection
*Function description:背光电压错误指示	
*Ipunt				 :none
*OutPut				 :[1]SUCCESS：成功检测到了[2]ERROR:没有检测到
*-------------------------------------------------------------------------*/
ErrStatus Backlight_level_Error_Detection(void)
{
	return PIN_level_Detection(GPIOA, GPIO_PIN_3, LOW);
}
/*------------------------------------------------------------------------
*Function name		 :LCD_level_error_Detection
*Function description:LCD电压驱动IC的错误指示	
*Ipunt				 :none
*OutPut				 :[1]SUCCESS：成功检测到了[2]ERROR:没有检测到
*-------------------------------------------------------------------------*/
ErrStatus LCD_level_error_Detection(void)
{
	return PIN_level_Detection(GPIOE, GPIO_PIN_13, LOW);
}

/*------------------------------------------------------------------------
*Function name		 :Source_ic_Error_Detection
*Function description:Source Driver 错误指示	
*Ipunt				 :none
*OutPut				 :[1]SUCCESS：成功检测到了[2]ERROR:没有检测到
*-------------------------------------------------------------------------*/
ErrStatus Source_ic_Error_Detection(void)
{
	return PIN_level_Detection(GPIOB, GPIO_PIN_13, LOW);
}

/*------------------------------------------------------------------------
*Function name		 :Update_level_Detection
*Function description:升级MCU固件的指示
*Ipunt				 :none
*OutPut				 :[1]SUCCESS：成功检测到了[2]ERROR:没有检测到
*-------------------------------------------------------------------------*/
ErrStatus Update_level_Detection(void)
{
	return PIN_level_Detection(GPIOA, GPIO_PIN_8, HIGH);
}

/*------------------------------------------------------------------------
*Function name		 :Update_level_Detection
*Function description:升级MCU固件的指示
*Ipunt				 :none
*OutPut				 :[1]SUCCESS：成功检测到了[2]ERROR:没有检测到
*-------------------------------------------------------------------------*/
ErrStatus IRQ_HIGH_level_Detection(void)
{
	return PIN_level_Detection(GPIOB, GPIO_PIN_14, HIGH);
}


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
        if(Update_tI2cSlave.RecBuff[0] == 0x85)//!!没有校验！！！！！！！
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