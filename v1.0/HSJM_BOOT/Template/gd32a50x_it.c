
#include "gd32a50x_it.h"
#include "systick.h"
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
    while(1){
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
    \brief      this function handles PendSV exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void PendSV_Handler(void)
{
}

/*!
    \brief      this function handles SysTick exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SysTick_Handler(void)
{
    delay_decrement();
}

void TIMER1_IRQHandler(void)
{
	if(SET == timer_interrupt_flag_get(TIMER1, TIMER_INT_UP)) 
	{
        timer_interrupt_flag_clear(TIMER1, TIMER_INT_UP);
			
		if(Counter_1ms.Timeout_in_Updating > 0)			Counter_1ms.Timeout_in_Updating --;
		else											Counter_1ms.Timeout_in_Updating = 10000;
		
		if(Counter_1ms.Waiting_in_Updating > 0)			Counter_1ms.Waiting_in_Updating --;	
		
		
		//【每2秒向主机请求发送背光值】来自【TIMER5_DAC_IRQHandler】-----------------
		if(Counter_1ms.IRQ_Update > 0)	
		{
			Counter_1ms.IRQ_Update --;
		}
		else
			IRQ_RELEASE;
	}
}
