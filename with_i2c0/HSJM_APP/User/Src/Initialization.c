/*******************************************************************************
 * Project	:������ĻMCU����_BOE�ɶ�
 * MCU	  	:GD32A503KCU3
 * Data		:2023/10/07
 * Files	:Initialization.c
 * Library	:V1.1.0, firmware for GD32A50x
 * Function	:Ӧ�ô��������ĳ�ʼ��
 * Author	:Liu Wei
 * Phone	:13349168457
 ******************************************************************************/
#include "Initialization.h"



/*------------------------------------------------------------------------
*Function name		 :void McuInitialization(void) 
*Function description:MCU�����ʼ��
*Ipunt				 :none
*OutPut				 :none
*-------------------------------------------------------------------------*/
void McuInitialization(void)
{
	systick_config();
	McuRcuInitialization();
    McuGpioInitialization();
	McuTimerInitialization();
	
	McuI2cInitialization();
	
	McuAdcInitialization();
	McuSpiInitialize();
	
}


/*------------------------------------------------------------------------
*Function name		 :void McuInitialization(void) 
*Function description:Ӧ�������ʼ��
*Ipunt				 :none
*OutPut				 :none
*-------------------------------------------------------------------------*/
void AppInitialization(void)
{
//	PowerOn_Sequence();//for gpio
    Counter_1ms.NTC = Temperature_Measurement_Cycle;
    Backlight.Final_Temperature = 0;
	Backlight_Brightness_Init(&Backlight.blc, 0); // Initial duty cycle is 0%
}

/*------------------------------------------------------------------------
*Function name		 :void McuInitialization(void) 
*Function description:����ʼ��
*Ipunt				 :none
*OutPut				 :none
*-------------------------------------------------------------------------*/
void Initialization(void)
{
	McuInitialization();
	delay_1ms(5);
	AppInitialization();
}


