

#include "gd32a50x_it.h"
#include "Define.h"

/*!
    \brief      this function handles NMI exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void NMI_Handler(void)
{
}

/*!
    \brief      this function handles HardFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void HardFault_Handler(void)
{
    /* if Hard Fault exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles MemManage exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void MemManage_Handler(void)
{
    /* if Memory Manage exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles BusFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void BusFault_Handler(void)
{
    /* if Bus Fault exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles UsageFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void UsageFault_Handler(void)
{
    /* if Usage Fault exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles SVC exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SVC_Handler(void)
{
}

/*!
    \brief      this function handles DebugMon exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DebugMon_Handler(void)
{
}

/*!
    \brief      this function handles PendSV exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void PendSV_Handler(void)
{
}



//【GD】///////////////////////////////////////////////////////////////////////////////////////////////////
uint16_t count = 0;
uint8_t Task100msflg = 0;
void SysTick_Handler(void)
{
	delay_decrement();
	
	count++;
	if(count == 100 )
	{
		Task100msflg = 1;
		count = 0;
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
uint16_t IRQ_100ms = 0;
extern uint16_t LOCK_10S;
_Counter_1ms Counter_1ms;

/*------------------------------------------------------------------------
*Function name		 :TIMER5_DAC_IRQHandler
*Function description:1ms进入一次
*Ipunt				 :None
*OutPut				 :None
*-------------------------------------------------------------------------*/
void TIMER1_IRQHandler(void)
{
    if(SET == timer_interrupt_flag_get(TIMER1, TIMER_INT_UP)) {
        /* clear update interrupt bit */
        timer_interrupt_flag_clear(TIMER1, TIMER_INT_UP);

		
		//【状态机】/////////////////////////////////////////////////////////////
		//[上电时序]-------------------------------------------------------------
		if(Power.PowerOn_Counter > 0)				
			Power.PowerOn_Counter --;
		
		//[调光]-----------------------------------------------------------------
		if(Backlight.Brightness_Counter > 0)		
			Backlight.Brightness_Counter --;
		
		//[板上设备状态检测]------------------------------------------------------
		if(IO_Detection.Detection_Counter > 0)		
			IO_Detection.Detection_Counter --;
	
		
		//【需要周期判执行的功能】////////////////////////////////////////////////
		//[监测温度的间隔时间]----------------------------------------------------
        if(Counter_1ms.NTC > 0)	
		{
			Counter_1ms.NTC --;
		}

		//[往读数据的数据帧里填入数据]----------------------------------------------
		if(Counter_1ms.RData_Loading_Cycle > 0)
			Counter_1ms.RData_Loading_Cycle --;					
		
		//【每2秒向主机请求发送背光值】来自【TIMER5_DAC_IRQHandler】-----------------
		if(Counter_1ms.IRQ_0X11 > 0)	
		{
			Counter_1ms.IRQ_0X11 --;
		}
		else
			IRQ_RELEASE;		
		//【心跳包0X13】显示屏信息查询指令，来自【TIMER6_IRQHandler】----------------
		if(Counter_1ms.IRQ_0X13 > 0)	
		{
			Counter_1ms.IRQ_0X13 --;
		}
		else
			IRQ_RELEASE;
		
		//【通知主机来读的常规中断100ms】/////////////////////////////////////////
		if(IRQ_100ms > 0)
		{
			IRQ_100ms --;
		}
		else
			IRQ_RELEASE;
    }
}



/*------------------------------------------------------------------------
*Function name		 :TIMER5_DAC_IRQHandler
*Function description:2秒进入一次
*Ipunt				 :None
*OutPut				 :None
*-------------------------------------------------------------------------*/
void TIMER5_DAC_IRQHandler(void)
{
    if(SET == timer_interrupt_flag_get(TIMER5, TIMER_INT_UP)) 
	{
        /* clear update interrupt bit */
        timer_interrupt_flag_clear(TIMER5, TIMER_INT_UP);
		
		if(!ScreenLitAfterStartup.finish)
		{
			ReadFrameTransmit(0x11);//请求发送背光值
			IRQ_LOW_DOWN;
			Counter_1ms.IRQ_0X11 = 100;
		}
	}
}


/*------------------------------------------------------------------------
*Function name		 :TIMER6_IRQHandler
*Function description:2秒进入一次
*Ipunt				 :None
*OutPut				 :None
*-------------------------------------------------------------------------*/
uint8_t i5 = 0;
uint8_t i6 = 0;
void TIMER6_IRQHandler(void)
{
    if(SET == timer_interrupt_flag_get(TIMER6, TIMER_INT_UP)) 
	{
        /* clear update interrupt bit */
        timer_interrupt_flag_clear(TIMER6, TIMER_INT_UP);	

        if(i5 == 0)
        {
            nvic_irq_enable(TIMER5_DAC_IRQn, 0, 0);                             //进了这个中断，就说明过了1秒，正好开启Timer5
            i5 = 1;
        }
        
        i6 ++;
        if(i6 == 2)
        {
            i6 = 0;
            ReadFrameTransmit(0x13);//【心跳包】显示屏信息查询指令
            IRQ_LOW_DOWN;
            Counter_1ms.IRQ_0X13 = 100;
        }
	}
}
