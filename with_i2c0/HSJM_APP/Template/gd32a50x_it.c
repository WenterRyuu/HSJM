

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



//��GD��///////////////////////////////////////////////////////////////////////////////////////////////////
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
*Function description:1ms����һ��
*Ipunt				 :None
*OutPut				 :None
*-------------------------------------------------------------------------*/
void TIMER1_IRQHandler(void)
{
    if(SET == timer_interrupt_flag_get(TIMER1, TIMER_INT_UP)) {
        /* clear update interrupt bit */
        timer_interrupt_flag_clear(TIMER1, TIMER_INT_UP);

		
		//��״̬����/////////////////////////////////////////////////////////////
		//[�ϵ�ʱ��]-------------------------------------------------------------
		if(Power.PowerOn_Counter > 0)				
			Power.PowerOn_Counter --;
		
		//[����]-----------------------------------------------------------------
		if(Backlight.Brightness_Counter > 0)		
			Backlight.Brightness_Counter --;
		
		//[�����豸״̬���]------------------------------------------------------
		if(IO_Detection.Detection_Counter > 0)		
			IO_Detection.Detection_Counter --;
	
		
		//����Ҫ������ִ�еĹ��ܡ�////////////////////////////////////////////////
		//[����¶ȵļ��ʱ��]----------------------------------------------------
        if(Counter_1ms.NTC > 0)	
		{
			Counter_1ms.NTC --;
		}

		//[�������ݵ�����֡����������]----------------------------------------------
		if(Counter_1ms.RData_Loading_Cycle > 0)
			Counter_1ms.RData_Loading_Cycle --;					
		
		//��ÿ2�������������ͱ���ֵ�����ԡ�TIMER5_DAC_IRQHandler��-----------------
		if(Counter_1ms.IRQ_0X11 > 0)	
		{
			Counter_1ms.IRQ_0X11 --;
		}
		else
			IRQ_RELEASE;		
		//��������0X13����ʾ����Ϣ��ѯָ����ԡ�TIMER6_IRQHandler��----------------
		if(Counter_1ms.IRQ_0X13 > 0)	
		{
			Counter_1ms.IRQ_0X13 --;
		}
		else
			IRQ_RELEASE;
		
		//��֪ͨ���������ĳ����ж�100ms��/////////////////////////////////////////
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
*Function description:2�����һ��
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
			ReadFrameTransmit(0x11);//�����ͱ���ֵ
			IRQ_LOW_DOWN;
			Counter_1ms.IRQ_0X11 = 100;
		}
	}
}


/*------------------------------------------------------------------------
*Function name		 :TIMER6_IRQHandler
*Function description:2�����һ��
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
            nvic_irq_enable(TIMER5_DAC_IRQn, 0, 0);                             //��������жϣ���˵������1�룬���ÿ���Timer5
            i5 = 1;
        }
        
        i6 ++;
        if(i6 == 2)
        {
            i6 = 0;
            ReadFrameTransmit(0x13);//������������ʾ����Ϣ��ѯָ��
            IRQ_LOW_DOWN;
            Counter_1ms.IRQ_0X13 = 100;
        }
	}
}
