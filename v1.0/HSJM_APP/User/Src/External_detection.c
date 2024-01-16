/*******************************************************************************
 * Project	:车载屏幕MCU开发_BOE成都
 * MCU	  	:GD32A503KCU3
 * Data		:2023/10/23
 * Files	:Backlight.c
 * Library	:V1.1.0, firmware for GD32A50x
 * Function	:外设状态检测
 * Author	:Liu Wei
 * Phone	:13349168457
 ******************************************************************************/
 #include "External_detection.h"

//【Output】[1]SUCCESS：成功检测到了LOCK信号[2]ERROR:没有检测到LOCK信号
//【Demo】if(LOCK_Detection == SUCCESS)
#define LOCK_Detection                  (return PIN_level_Detection(GPIOA, GPIO_PIN_0, HIGH))
#define Backlight_level_Error_Detection (return PIN_level_Detection(GPIOA, GPIO_PIN_3, LOW))
#define LCD_level_error_Detection       (return PIN_level_Detection(GPIOE, GPIO_PIN_13, LOW))
#define Source_ic_Error_Detection       (return PIN_level_Detection(GPIOB, GPIO_PIN_13, LOW))
#define Update_level_Detection          (return PIN_level_Detection(GPIOA, GPIO_PIN_8, HIGH))
#define IRQ_HIGH_level_Detection        (return PIN_level_Detection(GPIOB, GPIO_PIN_14, HIGH))



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


