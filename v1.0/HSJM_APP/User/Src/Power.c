/*******************************************************************************
 * Project	:车载屏幕MCU开发_BOE成都
 * MCU	  	:GD32A503KCU3
 * Data		:2023/10/07
 * Files	:Power.c
 * Library	:V1.1.0, firmware for GD32A50x
 * Function	:供电掉电相关的功能应用函数
 * Author	:Liu Wei
 * Phone	:13349168457
 ******************************************************************************/
 #include "Power.h"


Power_State PowerOff_IO_State = START;
Power_State PowerOn_IO_State = START;

uint16_t Power_Counter = 0;


_Power Power;
void PowerOn_Order(void)
{	
	switch(PowerOn_IO_State)
	{
		case START:
			RESET_SET_LOW;
			STBYB_SET_LOW;
			LED_DRIVER_DISABLE;
			PowerOn_IO_State = RESET_ACTION;
		
		case RESET_ACTION:
			RESET_SET_HIGH;
			Power.PowerOn_Counter = 20;
			PowerOn_IO_State = Waiting_for_STBYB_ACTION;
		
		case Waiting_for_STBYB_ACTION:
			if(gpio_input_bit_get(RESET_PORT, RESET_PIN) == SET)
			{
				if(Power.PowerOn_Counter == 0)
				{
					PowerOn_IO_State = STBYB_ACTION;
				}	
				else
					break;	
			}
			else
				break;
			
		case STBYB_ACTION:
			STBYB_SET_HIGH;
			Power.PowerOn_Counter = 184;
			PowerOn_IO_State = Waiting_for_LedDriverEn_ACTION;
			break;
		
		case Waiting_for_LedDriverEn_ACTION:
			if(gpio_input_bit_get(STBYB_PORT, STBYB_PIN) == SET)
			{
				if(Power.PowerOn_Counter == 0)
				{
					PowerOn_IO_State = LedDriverEn_ACTION;
				}
				else
					break;
			}
			else
				break;		
			
		case LedDriverEn_ACTION:
			LED_DRIVER_ENABLE;
			PowerOn_IO_State = END;
			break;
			
		case END:			
			break;
		
		default:
			
			break;
	}
	
}


uint16_t PowerOff_Counter = 0;
void PowerOff_Order(void)
{
	switch(PowerOff_IO_State)
	{
		case START:
			if(SET == gpio_input_bit_get(RESET_PORT, RESET_PIN) &&
				SET == gpio_input_bit_get(STBYB_PORT, STBYB_PIN) &&
				SET == gpio_input_bit_get(LED_DRIVER_EN_PORT, LED_DRIVER_EN_PIN))//检查这三根线是不是都是高电平
			{
				PowerOff_IO_State = LedDriverEn_ACTION;
			}
			else
				break;			
		
		case LedDriverEn_ACTION:
			LED_DRIVER_DISABLE;
			Power.PowerOff_Counter = 1;
			PowerOff_IO_State = Waiting_for_STBYB_ACTION;
		
		case Waiting_for_STBYB_ACTION:
			if(RESET == gpio_input_bit_get(LED_DRIVER_EN_PORT, LED_DRIVER_EN_PIN))
			{
				if(Power.PowerOff_Counter == 0)
				{
					PowerOff_IO_State = STBYB_ACTION;
				}				
			}
			else
				break;
			
		case STBYB_ACTION:
			STBYB_SET_LOW;
			Power.PowerOff_Counter = 120;
			PowerOff_IO_State = Waiting_for_RESET_ACTION;
		
		case Waiting_for_RESET_ACTION:
			if(RESET == gpio_input_bit_get(STBYB_PORT, STBYB_PIN))
			{
				if(Power.PowerOff_Counter == 0)
				{
					PowerOff_IO_State = RESET_ACTION;
				}
			}
			else
				break;		
			
		case RESET_ACTION:
			RESET_SET_LOW;
			PowerOff_IO_State = END;
			
		case END:
			break;
		
		default:
			
			break;
	}
	
}



void PowerOn_Sequence(void)
{
	do
	{
		PowerOn_Order();
	}
	while(PowerOn_IO_State != END);
}

void PowerOff_Sequence(void)
{
	do
	{
		PowerOff_Order();
	}
	while(PowerOff_IO_State != END);
}