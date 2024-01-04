#ifndef _GPIO_APP_H_
#define _GPIO_APP_H_


#include "Define.h"
#include "gd32a50x_gpio.h"



#define IRQ_LOW_DOWN 	gpio_bit_reset(GPIOB, GPIO_PIN_14)
#define IRQ_RELEASE 	gpio_bit_set(GPIOB, GPIO_PIN_14)


//-----------------------------------------------------------------------------------
//º¯ÊýÉùÃ÷
static void Gpio_Config(void);
static void Afio_Config(void);
void McuGpioInitialization(void);


#endif



