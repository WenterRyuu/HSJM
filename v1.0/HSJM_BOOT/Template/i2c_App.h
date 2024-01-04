#ifndef _I2C_APP_H_
#define _I2C_APP_H_

#include "Update.h"
#include "Define.h"

//用于调试（只能改这里的宏定义）-------------------------------------
#define I2C_INDEX				I2C1
#define I2C_PORT				GPIOB
#define I2C_GPIO_AF_INDEX		GPIO_AF_5
#define I2C_SCL_PIN				GPIO_PIN_7
#define I2C_SDA_PIN				GPIO_PIN_8

//正式使用（最后再用，这里的宏定义不允许更改）-------------------------------------
#define I2C0_INDEX				I2C0
#define I2C0_PORT				GPIOA
#define I2C0_GPIO_AF_INDEX		GPIO_AF_3
#define I2C0_SCL_PIN			GPIO_PIN_10
#define I2C0_SDA_PIN			GPIO_PIN_11
#define I2C1_INDEX				I2C1
#define I2C1_PORT				GPIOB
#define I2C1_GPIO_AF_INDEX		GPIO_AF_5
#define I2C1_SCL_PIN			GPIO_PIN_7
#define I2C1_SDA_PIN			GPIO_PIN_8



void McuI2cInitialization(void);
void I2C_Gpio_Config(void);
void I2C_Config(void);
void I2C_Gpio_Release(void);
void I2C_Master_Config(void);
void I2C_Slave_Config(void);
void I2C_BUS_Reset(void);
void I2C_GPIO_Reset(void);


////////////////////////////////////////////以上是I2C相关设置函数/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////以下是I2C中断函数/////////////////////////////////////////////////////////////////////////////

void I2C0_EV_IRQHandler(void);
void I2C0_ER_IRQHandler(void);
void I2C1_EV_IRQHandler(void);
void I2C1_ER_IRQHandler(void);








#endif
