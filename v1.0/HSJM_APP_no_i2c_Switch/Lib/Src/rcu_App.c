/*******************************************************************************
 * Project	:车载屏幕MCU开发_BOE成都
 * MCU	  	:GD32A503KCU3
 * Data		:2023/10/07
 * Files	:Initialization.c
 * Library	:V1.1.0, firmware for GD32A50x
 * Function	:RCU的功能应用函数
 * Author	:Liu Wei
 * Phone	:13349168457
 ******************************************************************************/
#include "rcu_App.h"




/*------------------------------------------------------------------------
*Function name		 :void McuRcuInitialization(void) 
*Function description:RCU的初始化，开启外设时钟
*Ipunt				 :None
*OutPut				 :None
*-------------------------------------------------------------------------*/
void McuRcuInitialization(void) 
{
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_GPIOC);
	rcu_periph_clock_enable(RCU_GPIOD);	
	rcu_periph_clock_enable(RCU_GPIOE);	
	
	rcu_periph_clock_enable(RCU_TIMER1);
    rcu_periph_clock_enable(RCU_TIMER5);
	rcu_periph_clock_enable(RCU_TIMER6);
	rcu_periph_clock_enable(RCU_TIMER19);
	
	rcu_periph_clock_enable(RCU_I2C0);
	rcu_periph_clock_enable(RCU_I2C1);
	
	rcu_periph_clock_enable(RCU_ADC0);				//for ADC
    rcu_adc_clock_config(RCU_CKADC_CKAHB_DIV10);
	
	rcu_periph_clock_enable(RCU_DMA0);				//SPI
    rcu_periph_clock_enable(RCU_DMAMUX);
    rcu_periph_clock_enable(RCU_SPI0);
    rcu_periph_clock_enable(RCU_SPI1);
	
}	



void McuRcuDeinit(void)
{
	rcu_periph_clock_disable(RCU_GPIOA);
	rcu_periph_clock_disable(RCU_GPIOB);
	rcu_periph_clock_disable(RCU_GPIOC);
	rcu_periph_clock_disable(RCU_GPIOD);	
	rcu_periph_clock_disable(RCU_GPIOE);	
	
	rcu_periph_clock_disable(RCU_TIMER1);
    rcu_periph_clock_disable(RCU_TIMER5);
	rcu_periph_clock_disable(RCU_TIMER6);
	rcu_periph_clock_disable(RCU_TIMER19);
	
	rcu_periph_clock_disable(RCU_I2C0);
	rcu_periph_clock_disable(RCU_I2C1);
	
	rcu_periph_clock_disable(RCU_ADC0);				//for ADC
    rcu_adc_clock_config(RCU_CKADC_CKAHB_DIV10);
	
	rcu_periph_clock_disable(RCU_DMA0);				//SPI
    rcu_periph_clock_disable(RCU_DMAMUX);
    rcu_periph_clock_disable(RCU_SPI0);
    rcu_periph_clock_disable(RCU_SPI1);
}


