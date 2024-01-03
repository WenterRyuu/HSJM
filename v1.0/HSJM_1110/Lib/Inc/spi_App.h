#ifndef _SPI_APP_H_
#define _SPI_APP_H_


#include "Define.h"
#include <gd32a50x_spi.h>


#define CSB_LOW		gpio_bit_reset(GPIOA, GPIO_PIN_1)
#define CSB_HIGH	gpio_bit_set(GPIOA, GPIO_PIN_1)
#define CLK_LOW		gpio_bit_reset(GPIOC, GPIO_PIN_0)
#define CLK_HIGH	gpio_bit_set(GPIOC, GPIO_PIN_0)
#define MOSI_LOW	gpio_bit_reset(GPIOF, GPIO_PIN_5)
#define MOSI_HIGH	gpio_bit_set(GPIOF, GPIO_PIN_5)
#define MISO_LOW	gpio_bit_reset(GPIOA, GPIO_PIN_2)
#define MISO_HIGH	gpio_bit_set(GPIOA, GPIO_PIN_2)


void McuSpiInitialize(void);
void SPI_Master_Init(void);
void SPI_Write_Data_to_Register(uint8_t command, uint8_t data);
uint8_t SPI_Read_Data_from_Register(uint8_t command);
void Source_IC_writeread(void);
void SPI_Send_Byte(uint8_t value);
void SPI_Send_Bytes(uint8_t *data_to_send, uint8_t num_of_data);


#endif
