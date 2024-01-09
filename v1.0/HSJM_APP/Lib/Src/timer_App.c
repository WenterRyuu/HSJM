/*******************************************************************************
 * Project	:车载屏幕MCU开发_BOE成都
 * MCU	  	:GD32A503KCU3
 * Data		:2023/10/07
 * Files	:Initialization.c
 * Library	:V1.1.0, firmware for GD32A50x
 * Function	:MCU定时器的功能应用函数
 * Author	:Liu Wei
 * Phone	:13349168457
 ******************************************************************************/
 #include "timer_App.h"



/*------------------------------------------------------------------------
*Function name		 :Timer1_Generate_1ms_Interrupt
*Function description:定时器1的初始化，产生1ms定时中断
*Ipunt				 :None
*OutPut				 :None
*-------------------------------------------------------------------------*/
void Timer1_Generate_1ms_Interrupt(void)
{
    timer_parameter_struct timer_initpara;

    /* enable the peripherals clock */
    rcu_periph_clock_enable(RCU_TIMER1);

    /* deinit a TIMER */
    timer_deinit(TIMER1);
    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_initpara);
    /* TIMER1 configuration */
    timer_initpara.prescaler         = 9999;	//
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 9;//99999;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_init(TIMER1, &timer_initpara);

    /* enable the TIMER interrupt */
    timer_interrupt_enable(TIMER1, TIMER_INT_UP);
    /* enable a TIMER */
    timer_enable(TIMER1);
	
	nvic_irq_enable(TIMER1_IRQn, 0, 0);
}



/*------------------------------------------------------------------------
*Function name		 :Timer5_Generate_2s_Interrupt
*Function description:定时器5的初始化，产生2s定时中断
*Ipunt				 :None
*OutPut				 :None
*-------------------------------------------------------------------------*/
void Timer5_Generate_2s_Interrupt(void)
{
    timer_parameter_struct timer_initpara;

    /* deinit a TIMER */
    timer_deinit(TIMER5);
    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_initpara);
    /* TIMER1 configuration */
    timer_initpara.prescaler         = 9999;	//1MHz
	//系统主时钟100MHz,10000(9999+1)分频是10000（10K）Hz,timer5每（1/10K）秒，即每0.0001秒（0.1ms）开始加一
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 19999;//9999;
	//此处设置20000（19999+1）那么就是经过（0.1ms*20K =2秒）让数清零再加一
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_init(TIMER5, &timer_initpara);

    /* enable the TIMER interrupt */
    timer_interrupt_enable(TIMER5, TIMER_INT_UP);
    /* enable a TIMER */
    timer_enable(TIMER5);
	
	nvic_irq_disable(TIMER5_DAC_IRQn);
}

void Timer6_Generate_2s_Interrupt(void)
{
    timer_parameter_struct timer_initpara;

    /* deinit a TIMER */
    timer_deinit(TIMER6);
    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_initpara);
    /* TIMER1 configuration */
    timer_initpara.prescaler         = 9999;	//1MHz
	//系统主时钟100MHz,10000(9999+1)分频是10000（10K）Hz,timer5每（1/10K）秒，即每0.0001秒（0.1ms）开始加一
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 19999;//9999;
	//此处设置20000（19999+1）那么就是经过（0.1ms*20K =2秒）让数清零再加一
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_init(TIMER6, &timer_initpara);

    /* enable the TIMER interrupt */
    timer_interrupt_enable(TIMER6, TIMER_INT_UP);
    /* enable a TIMER */
    timer_enable(TIMER6);
	
	nvic_irq_disable(TIMER6_IRQn);
}
///////////////////////////////////////////////////////////////////////////////////


/*------------------------------------------------------------------------
*Function name		 :void Timer19_Generate_PWM_config(void) 
*Function description:定时器19的初始化，产生PWM信号
*Ipunt				 :None
*OutPut				 :None
*-------------------------------------------------------------------------*/
void Timer19_Generate_PWM_config(void)
{
    /* -----------------------------------------------------------------------
        TIMER19 configuration to:
        - TIMER19CLK is fixed to systemcoreclock, the TIMER19 prescaler is equal to 5000 
			(timer_initpara.prescaler = 4999;)
			so the TIMER19 counter clock used is 20KHz.（100 000 000 ÷ 5 000 = 20000 = 20KHz）
           the four duty cycles are computed as the following description:
           the channel 0 duty cycle is set to 20%, so multi mode channel 0 is set to 80%.
        - CH0/MCH0 and CH1/MCH1 are configured in PWM mode 0
      ----------------------------------------------------------------------- */
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER19);

    timer_deinit(TIMER19);
    /* TIMER19 configuration */
    timer_struct_para_init(&timer_initpara);
    timer_initpara.prescaler         = 4999;
	//系统主时钟100MHz，5000（4999+1）分频是20KHz，Timer19每(1/20K)秒，即0.05ms开始加1
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 99;
	//设置周期，即定时器循环加一的周期，即每当定时器的数值加到多少的时候，从0重新开始加1，此处设置100（99+1），那么就是
	//加到100的时候加一，那么就是每（0.05ms*100 = 5ms）让数清零再加一
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER19, &timer_initpara);

    /* CH0 configuration in PWM mode */
    timer_channel_output_struct_para_init(&timer_ocintpara);
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_ENABLE;
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

    timer_channel_output_config(TIMER19, TIMER_CH_0, &timer_ocintpara);

    /* CH0/MCH0 configuration in PWM mode 0 */
    //timer_channel_output_pulse_value_config(TIMER19, TIMER_CH_0, (100 - LED_PWM));//当这个数是20的时候，对应80%pwm
    timer_channel_output_mode_config(TIMER19, TIMER_CH_0, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER19, TIMER_CH_0, TIMER_OC_SHADOW_ENABLE);//开启影子寄存器

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER19);
    timer_primary_output_config(TIMER19, ENABLE);
    timer_enable(TIMER19);
}

/*-------------------------------------------------------------------------
*Function name		 :void McuRcuInitialization(void) 
*Function description:RCU的初始化
*Ipunt				 :None
*OutPut				 :None
*-------------------------------------------------------------------------*/
void McuTimerInitialization(void) 
{
	Timer1_Generate_1ms_Interrupt();
	Timer5_Generate_2s_Interrupt();
	delay_1ms(1000);
	Timer6_Generate_2s_Interrupt();//有两个需求，都要间隔2秒触发一次中断，为了不冲突，这两个定时器间隔1秒开启
	Timer19_Generate_PWM_config();
}	

