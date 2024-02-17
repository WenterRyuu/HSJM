/*******************************************************************************
 * Project	:车载屏幕MCU开发_BOE成都
 * MCU	  	:GD32A503KCU3
 * Data		:2023/10/07
 * Files	:Initialization.c
 * Library	:V1.1.0, firmware for GD32A50x
 * Function	:应用代码和外设的初始化
 * Author	:Liu Wei
 * Phone	:13349168457
 ******************************************************************************/
#include "Initialization.h"



/*------------------------------------------------------------------------
*Function name		 :void McuInitialization(void) 
*Function description:MCU外设初始化
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
*Function description:应用外设初始化
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
*Function description:主初始化
*Ipunt				 :none
*OutPut				 :none
*-------------------------------------------------------------------------*/
void Initialization(void)
{
	McuInitialization();
	delay_1ms(5);
	AppInitialization();
}


