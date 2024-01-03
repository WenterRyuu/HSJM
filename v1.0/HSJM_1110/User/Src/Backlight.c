/*******************************************************************************
 * Project	:������ĻMCU����_BOE�ɶ�
 * MCU	  	:GD32A503KCU3
 * Data		:2023/10/23
 * Files	:Backlight.c
 * Library	:V1.1.0, firmware for GD32A50x
 * Function	:��Ļ����Ĺ���Ӧ�ú���
 * Author	:Liu Wei
 * Phone	:13349168457
 ******************************************************************************/
#include "Backlight.h"

_Backlight Backlight;

/*------------------------------------------------------------------------
*Function name		 :Backlight_Brightness_Init
*Function description:�������ȳ�ʼ��
*Ipunt				 :��1������������ṹ�������2��Ҫ���õ�ռ�ձ�
*OutPut				 :none
*-------------------------------------------------------------------------*/
void Backlight_Brightness_Init(BacklightController* blc, uint8_t initialDutyCycle)
{
    Backlight.blc.currDutyCycle = initialDutyCycle;
    Backlight.blc.targetDutyCycle = initialDutyCycle;
    Backlight.blc.stepDutyCycle = 0;
    Backlight.blc.state = STATE_IDLE;
	Modify_PWM_Directly(initialDutyCycle);
}

/*------------------------------------------------------------------------
*Function name		 :SetTargetDutyCycle
*Function description:����Ŀ�����ȣ�������ṹ��������¸�ֵ
*Ipunt				 :��1������������ṹ�������2��Ҫ���õ�Ŀ������ռ�ձ�
*OutPut				 :none
*-------------------------------------------------------------------------*/
void SetTargetDutyCycle(BacklightController* blc, uint8_t target) 
{
	int8_t delta = (target - Backlight.blc.currDutyCycle);//��=35-96=-61
	
    Backlight.blc.targetDutyCycle = target;//35																									//Ŀ������
	if(delta > 0)
	{
		if(delta <= 25)		Backlight.blc.stepDutyCycle = 1;	
		else				Backlight.blc.stepDutyCycle = (delta%25==0) ? (delta/25) : ((delta/25)+1);											//����		
		Backlight.blc.laststepDutyCycle = (delta%(Backlight.blc.stepDutyCycle)==0) ? Backlight.blc.stepDutyCycle : (delta%(Backlight.blc.stepDutyCycle));					//���һ���Ĳ���
		Backlight.blc.state = STATE_ADJUSTING;
	}
	else if(delta < 0)
	{
		//delta = abs(delta);//-61
		if(delta >= -25)	Backlight.blc.stepDutyCycle = -1;
		else				Backlight.blc.stepDutyCycle = (delta%25==0) ? (delta/25) : ((delta/25)-1);											//����		
		Backlight.blc.laststepDutyCycle = (delta%(Backlight.blc.stepDutyCycle)==0) ? Backlight.blc.stepDutyCycle : (delta%(Backlight.blc.stepDutyCycle));					//���һ���Ĳ���
		Backlight.blc.state = STATE_ADJUSTING;		
	}
}

/*------------------------------------------------------------------------
*Function name		 :UpdateBacklightController
*Function description:����Ļ���ȵ���ΪĿ������״̬����ִ���������֮ǰӦ��ִ��SetTargetDutyCycle����
*Ipunt				 :��1������������ṹ�����
*OutPut				 :none
*-------------------------------------------------------------------------*/
uint8_t UpdateBacklightController(BacklightController* blc) 
{
    switch (Backlight.blc.state) {
    case STATE_IDLE:
        // Do nothing
        break;
	
    case STATE_ADJUSTING:
        if (Backlight.blc.currDutyCycle != Backlight.blc.targetDutyCycle) //��ǰ���Ȳ�����Ŀ������
		{			
			if(abs(Backlight.blc.targetDutyCycle - Backlight.blc.currDutyCycle) < abs(Backlight.blc.stepDutyCycle))
			{
				Backlight.blc.currDutyCycle += Backlight.blc.laststepDutyCycle;		//���һ�μӵĲ���Ӧ��С��֮ǰ�Ĳ���
				Backlight.Backlight_PWM = Backlight.blc.currDutyCycle;
				Backlight.blc.state = STATE_ADJUSTED;						//������˵���һ�μ��꣬���Ѿ��ﵽĿ�������PWMֵ�ˣ����������дһ���жϣ�	
				return Backlight.Backlight_PWM;
			}
			else
			{
				Backlight.blc.currDutyCycle += Backlight.blc.stepDutyCycle;	//��һ�����沽��
				Backlight.Backlight_PWM = Backlight.blc.currDutyCycle;			//��Backlight.blc.currDutyCycle�仯ʱ��Ҳ����Backlight_PWM��ֵ
				Backlight.Brightness_Counter = 20;
				Backlight.blc.state = STATE_WAITING_for_20ms;					
				return Backlight.Backlight_PWM;
			}
        } 
		else 
		{
            Backlight.blc.state = STATE_ADJUSTED;
        }
        break;
		
	case STATE_WAITING_for_20ms:
		if(Backlight.Brightness_Counter == 0)						//����20ms�Ժ�
		{
			Backlight.blc.state = STATE_ADJUSTING;				//�����ж�
			break;
		}	
		else
			break;	
		
    case STATE_ADJUSTED:
        Backlight.blc.state = STATE_IDLE;
		return Backlight.Backlight_PWM;
        break;

    }
	return Backlight.Backlight_PWM;
}





/*------------------------------------------------------------------------
*Function name		 :Modify_PWM_Directly
*Function description:ֱ���޸Ķ�ʱ�����ñ�����������ռ�ձ�
*Ipunt				 :��1��Ŀ��ռ�ձ�,
*OutPut				 :none
*-------------------------------------------------------------------------*/
void Modify_PWM_Directly(uint8_t Brightness)
{
    timer_channel_output_pulse_value_config(TIMER19, TIMER_CH_0, (100 - Brightness));//�����(100 - Brightness)��20��ʱ�򣬶�Ӧ80%pwm
}

/*------------------------------------------------------------------------
*Function name		 :Update_Brightness
*Function description:����Ļ���ȵ���ΪĿ������
*Ipunt				 :none
*OutPut				 :none
*-------------------------------------------------------------------------*/
void Update_Brightness(uint8_t target_brightness) 
{    	
    SetTargetDutyCycle(&Backlight.blc, target_brightness);
	do
    {
        Modify_PWM_Directly( UpdateBacklightController(&Backlight.blc) );		
    }	
	while(Backlight.blc.state != STATE_IDLE);
}
