/*******************************************************************************
 * Project	:车载屏幕MCU开发_BOE成都
 * MCU	  	:GD32A503KCU3
 * Data		:2023/10/23
 * Files	:Backlight.c
 * Library	:V1.1.0, firmware for GD32A50x
 * Function	:屏幕背光的功能应用函数
 * Author	:Liu Wei
 * Phone	:13349168457
 ******************************************************************************/
#include "Backlight.h"

_Backlight Backlight;

/*------------------------------------------------------------------------
*Function name		 :Backlight_Brightness_Init
*Function description:背光亮度初始化
*Ipunt				 :【1】背光控制器结构体变量【2】要设置的占空比
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
*Function description:设置目标亮度，给背光结构体参数重新赋值
*Ipunt				 :【1】背光控制器结构体变量【2】要设置的目标亮度占空比
*OutPut				 :none
*-------------------------------------------------------------------------*/
void SetTargetDutyCycle(BacklightController* blc, uint8_t target) 
{
	int8_t delta = (target - Backlight.blc.currDutyCycle);//Δ=35-96=-61
	
    Backlight.blc.targetDutyCycle = target;//35																									//目标亮度
	if(delta > 0)
	{
		if(delta <= 25)		Backlight.blc.stepDutyCycle = 1;	
		else				Backlight.blc.stepDutyCycle = (delta%25==0) ? (delta/25) : ((delta/25)+1);											//步长		
		Backlight.blc.laststepDutyCycle = (delta%(Backlight.blc.stepDutyCycle)==0) ? Backlight.blc.stepDutyCycle : (delta%(Backlight.blc.stepDutyCycle));					//最后一步的步长
		Backlight.blc.state = STATE_ADJUSTING;
	}
	else if(delta < 0)
	{
		//delta = abs(delta);//-61
		if(delta >= -25)	Backlight.blc.stepDutyCycle = -1;
		else				Backlight.blc.stepDutyCycle = (delta%25==0) ? (delta/25) : ((delta/25)-1);											//步长		
		Backlight.blc.laststepDutyCycle = (delta%(Backlight.blc.stepDutyCycle)==0) ? Backlight.blc.stepDutyCycle : (delta%(Backlight.blc.stepDutyCycle));					//最后一步的步长
		Backlight.blc.state = STATE_ADJUSTING;		
	}
}

/*------------------------------------------------------------------------
*Function name		 :UpdateBacklightController
*Function description:将屏幕亮度调整为目标亮度状态机，执行这个函数之前应该执行SetTargetDutyCycle函数
*Ipunt				 :【1】背光控制器结构体变量
*OutPut				 :none
*-------------------------------------------------------------------------*/
uint8_t UpdateBacklightController(BacklightController* blc) 
{
    switch (Backlight.blc.state) {
    case STATE_IDLE:
        // Do nothing
        break;
	
    case STATE_ADJUSTING:
        if (Backlight.blc.currDutyCycle != Backlight.blc.targetDutyCycle) //当前亮度不等于目标亮度
		{			
			if(abs(Backlight.blc.targetDutyCycle - Backlight.blc.currDutyCycle) < abs(Backlight.blc.stepDutyCycle))
			{
				Backlight.blc.currDutyCycle += Backlight.blc.laststepDutyCycle;		//最后一次加的步长应该小于之前的步长
				Backlight.Backlight_PWM = Backlight.blc.currDutyCycle;
				Backlight.blc.state = STATE_ADJUSTED;						//正常来说最后一次加完，就已经达到目标的亮度PWM值了（上面可以再写一个判断）	
				return Backlight.Backlight_PWM;
			}
			else
			{
				Backlight.blc.currDutyCycle += Backlight.blc.stepDutyCycle;	//加一个常规步长
				Backlight.Backlight_PWM = Backlight.blc.currDutyCycle;			//当Backlight.blc.currDutyCycle变化时，也跟新Backlight_PWM的值
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
		if(Backlight.Brightness_Counter == 0)						//经过20ms以后
		{
			Backlight.blc.state = STATE_ADJUSTING;				//继续判断
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
*Function description:直接修改定时器配置变量，来调节占空比
*Ipunt				 :【1】目标占空比,
*OutPut				 :none
*-------------------------------------------------------------------------*/
void Modify_PWM_Directly(uint8_t Brightness)
{
    timer_channel_output_pulse_value_config(TIMER19, TIMER_CH_0, (100 - Brightness));//当这个(100 - Brightness)是20的时候，对应80%pwm
}

/*------------------------------------------------------------------------
*Function name		 :Update_Brightness
*Function description:将屏幕亮度调整为目标亮度
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
