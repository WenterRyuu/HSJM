/*******************************************************************************
 * Project	:������ĻMCU����_BOE�ɶ�
 * MCU	  	:GD32A503KCU3
 * Data		:2023/10/07
 * Files	:Initialization.c
 * Library	:V1.1.0, firmware for GD32A50x
 * Function	:GPIO�Ĺ���Ӧ�ú���
 * Author	:Liu Wei
 * Phone	:13349168457
 ******************************************************************************/


#include "gpio_App.h"


/*------------------------------------------------------------------------
*Function name		 :void McuGpioInitialization(void) 
*Function description:GPIO�ڵĳ�ʼ��
*Ipunt				 :None
*OutPut				 :None
*-------------------------------------------------------------------------*/
void Gpio_Config(void)
{
	//Output Pin////////////////////////////////////////////////////////////////////////////////////////////////
	/* configure GPIO pin */
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_3);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);  		//PB3,LED Driver����ʹ�ܺ�ʧ��
    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_4);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4);		//PA4,STBYB(�ϵ�ʱ��)
    gpio_mode_set(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_14);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_14);		//PE14,LCD_RESET
	//@1025
    gpio_mode_set(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_9);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);		//PD9,�⴮��powerdownʹ��
    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_7);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);		//PA7,����������(�ߵ�ƽ)	
    gpio_mode_set(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_15);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);		//PC15,��ʾ���ܳ���ϴ���ָʾ�ź�	
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_14);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_14);		//PB14,������SOC���ж��ź�
	/* reset LED GPIO pin */
    gpio_bit_reset(GPIOB, GPIO_PIN_3);
    gpio_bit_reset(GPIOA, GPIO_PIN_4);
    gpio_bit_reset(GPIOE, GPIO_PIN_14);	
	gpio_bit_set(GPIOD, GPIO_PIN_9);	//�⴮��ʹ�ܸߵ�ƽ��Ч
    gpio_bit_reset(GPIOA, GPIO_PIN_7);
    gpio_bit_reset(GPIOC, GPIO_PIN_15);	
    gpio_bit_set(GPIOB, GPIO_PIN_14);	//IRQ����ʱΪ�ߵ�ƽ
	
	//Input Pin//////////////////////////////////////////////////////////////////////////////////////////////////
    gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_0);		//PA0,LOCK�ź�
    gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_0);		//PA3,�����ѹ����ָʾ
    gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_0);		//PE13,LCD��ѹ����IC�Ĵ���ָʾ
    gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_0);		//PB17,Source Driver ����ָʾ
    gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_0);		//PA8,����MCU�̼���ָʾ	
	
	gpio_mode_set(GPIOC, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_10);	//PC10,�ⲿNTC�¶ȼ��ģ������
	
}




/*------------------------------------------------------------------------
*Function name		 :void McuGpioInitialization(void) 
*Function description:���Ÿ��ù��ܣ�AFIO���ĳ�ʼ��
*Ipunt				 :None
*OutPut				 :None
*-------------------------------------------------------------------------*/
static void Afio_Config(void)
{
    /* ��MCU������IIC��PB7 to I2C1_SCL , PB8 to I2C1_SDA*/
//    gpio_af_set(GPIOB, GPIO_AF_5, GPIO_PIN_7 | GPIO_PIN_8);
//    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP,GPIO_PIN_7 | GPIO_PIN_8);
//    gpio_output_options_set(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ,GPIO_PIN_7 | GPIO_PIN_8);
	
	
	/* ��MCU���Ÿ���Ϊ��ʱ�����PWM�źš�configure PC11(TIMER19 CH0) as alternate function*/
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_11);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
    gpio_af_set(GPIOC, GPIO_AF_1, GPIO_PIN_11);
	
	/* ��MCU���ӻ�IIC��PA10 to I2C1_SCL , PA11 to I2C1_SDA */
    gpio_af_set(GPIOA, GPIO_AF_3, GPIO_PIN_10|GPIO_PIN_11);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_10|GPIO_PIN_11);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_10|GPIO_PIN_11);		
	/*��MCU������IIC��PB7 to I2C1_SCL , PB8 to I2C1_SDA */
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
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);		//PA1,NSS(CS)����ͨ��io,��spi�����ŷ�һ��
	gpio_bit_set(GPIOA,GPIO_PIN_1);
	
	
	
}

/*------------------------------------------------------------------------
*Function name		 :void McuGpioInitialization(void) 
*Function description:MCU��GPIOģ���ʼ��
*Ipunt				 :None
*OutPut				 :None
*-------------------------------------------------------------------------*/
void McuGpioInitialization(void)
{
	Gpio_Config();
	Afio_Config();
}



