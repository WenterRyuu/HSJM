/*******************************************************************************
 * Project	:������ĻMCU����_BOE�ɶ�
 * MCU	  	:GD32A503KCU3
 * Data		:2023/10/23
 * Files	:Backlight.c
 * Library	:V1.1.0, firmware for GD32A50x
 * Function	:����״̬���
 * Author	:Liu Wei
 * Phone	:13349168457
 ******************************************************************************/
 #include "External_detection.h"

//��Output��[1]SUCCESS���ɹ���⵽��LOCK�ź�[2]ERROR:û�м�⵽LOCK�ź�
//��Demo��if(LOCK_Detection == SUCCESS)
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


