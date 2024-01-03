/*******************************************************************************
 * Project	:车载屏幕MCU开发_BOE成都
 * MCU	  	:GD32A503KCU3
 * Data		:2023/10/07
 * Files	:Initialization.c
 * Library	:V1.1.0, firmware for GD32A50x
 * Function	:GPIO的功能应用函数
 * Author	:Liu Wei
 * Phone	:13349168457
 ******************************************************************************/


#include "gpio_App.h"


/*------------------------------------------------------------------------
*Function name		 :void McuGpioInitialization(void) 
*Function description:GPIO口的初始化
*Ipunt				 :None
*OutPut				 :None
*-------------------------------------------------------------------------*/
void Gpio_Config(void)
{
	//Output Pin////////////////////////////////////////////////////////////////////////////////////////////////
	/* configure GPIO pin */
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_3);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);  		//PB3,LED Driver背光使能和失能
    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_4);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4);		//PA4,STBYB(上电时序)
    gpio_mode_set(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_14);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_14);		//PE14,LCD_RESET
	//@1025
    gpio_mode_set(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_9);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);		//PD9,解串器powerdown使能
    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_7);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);		//PA7,驱动蜂鸣器(高电平)	
    gpio_mode_set(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_15);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);		//PC15,显示屏总成诊断错误指示信号	
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_14);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_14);		//PB14,给主机SOC的中断信号
	/* reset LED GPIO pin */
    gpio_bit_reset(GPIOB, GPIO_PIN_3);
    gpio_bit_reset(GPIOA, GPIO_PIN_4);
    gpio_bit_reset(GPIOE, GPIO_PIN_14);	
	gpio_bit_set(GPIOD, GPIO_PIN_9);	//解串器使能高电平有效
    gpio_bit_reset(GPIOA, GPIO_PIN_7);
    gpio_bit_reset(GPIOC, GPIO_PIN_15);	
    gpio_bit_set(GPIOB, GPIO_PIN_14);	//IRQ空闲时为高电平
	
	//Input Pin//////////////////////////////////////////////////////////////////////////////////////////////////
    gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_0);		//PA0,LOCK信号
    gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_0);		//PA3,背光电压错误指示
    gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_0);		//PE13,LCD电压驱动IC的错误指示
    gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_0);		//PB17,Source Driver 错误指示
    gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_0);		//PA8,升级MCU固件的指示	
	
	gpio_mode_set(GPIOC, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_10);	//PC10,外部NTC温度检测模拟输入
	
}




/*------------------------------------------------------------------------
*Function name		 :void McuGpioInitialization(void) 
*Function description:引脚复用功能（AFIO）的初始化
*Ipunt				 :None
*OutPut				 :None
*-------------------------------------------------------------------------*/
static void Afio_Config(void)
{
    /* 【MCU作主机IIC】PB7 to I2C1_SCL , PB8 to I2C1_SDA*/
//    gpio_af_set(GPIOB, GPIO_AF_5, GPIO_PIN_7 | GPIO_PIN_8);
//    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP,GPIO_PIN_7 | GPIO_PIN_8);
//    gpio_output_options_set(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ,GPIO_PIN_7 | GPIO_PIN_8);
	
	
	/* 【MCU引脚复用为定时器输出PWM信号】configure PC11(TIMER19 CH0) as alternate function*/
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_11);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
    gpio_af_set(GPIOC, GPIO_AF_1, GPIO_PIN_11);
	
	/* 【MCU作从机IIC】PA10 to I2C1_SCL , PA11 to I2C1_SDA */
    gpio_af_set(GPIOA, GPIO_AF_3, GPIO_PIN_10|GPIO_PIN_11);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_10|GPIO_PIN_11);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_10|GPIO_PIN_11);		
	/*【MCU作主机IIC】PB7 to I2C1_SCL , PB8 to I2C1_SDA */
//    gpio_af_set(GPIOB, GPIO_AF_5, GPIO_PIN_7 | GPIO_PIN_8);
//    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_7 | GPIO_PIN_8);
//    gpio_output_options_set(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_7 | GPIO_PIN_8);	
	
	//@1025
	/* configure SPI0 GPIO: SCK/PB6, MISO/PB5, MOSI/PB13 */
    gpio_af_set(GPIOC, GPIO_AF_4, GPIO_PIN_0);
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_0);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);		//PC0,CLK
    gpio_af_set(GPIOF, GPIO_AF_4, GPIO_PIN_5);
    gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5);		//PF5,MISO
    gpio_af_set(GPIOA, GPIO_AF_4, GPIO_PIN_2);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_2);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);		//PA2,MOSI	
	
    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_1);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);		//PA1,NSS(CS)是普通的io,跟spi的引脚放一起
	gpio_bit_set(GPIOA,GPIO_PIN_1);
	
	
	
}

/*------------------------------------------------------------------------
*Function name		 :void McuGpioInitialization(void) 
*Function description:MCU的GPIO模块初始化
*Ipunt				 :None
*OutPut				 :None
*-------------------------------------------------------------------------*/
void McuGpioInitialization(void)
{
	Gpio_Config();
	Afio_Config();
}



