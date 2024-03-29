/*!
    \file    main.c
    \brief   master transmitter slave receiver through DMA

    \version 2023-06-20, V1.1.0, firmware for GD32A50x
*/

/*
    Copyright (c) 2023, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#include "gd32a50x.h"
#include <stdio.h>
#include "gd32a503v_eval.h"

#define I2C0_SLAVE_ADDRESS7    0x82
#define I2C1_SLAVE_ADDRESS7    0x72
#define ARRAYNUM(arr_nanme)    (uint32_t)(sizeof(arr_nanme) / sizeof(*(arr_nanme)))
#define BUFFER_SIZE            (ARRAYNUM(i2c0_buff_tx)-1)

uint8_t i2c0_buff_tx[] = "I2C DMA test";
uint8_t i2c1_buff_rx[BUFFER_SIZE];
__IO ErrStatus state = ERROR;

void rcu_config(void);
void i2c0_gpio_config(void);
void i2c1_gpio_config(void);
void i2c_config(void);
ErrStatus memory_compare(uint8_t *src, uint8_t *dst, uint16_t length);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    dma_parameter_struct dma_init_struct;

    /* initialize LED1, LED2, as the transfer instruction */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    /* enable the peripheral clock */
    rcu_config();
    /* configure the I2C0 and I2C1 GPIO ports */
    i2c0_gpio_config();
    i2c1_gpio_config();
    /* configure I2C0 and I2C1 */
    i2c_config();

    /* initialize DMA channel0 for I2C1_RX */
    dma_deinit(DMA0, DMA_CH0);

    /* initialize DMA channel 0 */
    dma_deinit(DMA0, DMA_CH0);
    dma_struct_para_init(&dma_init_struct);
    dma_init_struct.request      = DMA_REQUEST_I2C1_RX;
    dma_init_struct.direction    = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.memory_addr  = (uint32_t)i2c1_buff_rx;
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number       = BUFFER_SIZE;
    dma_init_struct.periph_addr  = (uint32_t)&I2C_RDATA(I2C1);
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA0, DMA_CH0, &dma_init_struct);

    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH0);
    dma_memory_to_memory_disable(DMA0, DMA_CH0);
    /* enable DMA channel 0 */
    dma_channel_enable(DMA0, DMA_CH0);

    /* initialize DMA channel1 for I2C0_TX */
    dma_deinit(DMA0, DMA_CH1);
    dma_init_struct.request     = DMA_REQUEST_I2C0_TX;
    dma_init_struct.direction   = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_addr = (uint32_t)i2c0_buff_tx;
    dma_init_struct.periph_addr = (uint32_t)&I2C_TDATA(I2C0);
    dma_init_struct.priority    = DMA_PRIORITY_HIGH;
    dma_init(DMA0, DMA_CH1, &dma_init_struct);

    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH1);
    dma_memory_to_memory_disable(DMA0, DMA_CH1);

    /* enable DMA channel 1 */
    dma_channel_enable(DMA0, DMA_CH1);

    /* enable I2C1 receive DMA */
    i2c_dma_enable(I2C1, I2C_DMA_RECEIVE);
    /* enable I2C0 transmit DMA */
    i2c_dma_enable(I2C0, I2C_DMA_TRANSMIT);
    /* wait until I2C bus is idle */
    while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY));

    /* send a start condition to I2C bus */
    i2c_start_on_bus(I2C0);
    /* wait until ADDSEND bit is set*/
    while(!i2c_flag_get(I2C1, I2C_FLAG_ADDSEND));
    /* clear ADDSEND bit */
    i2c_flag_clear(I2C1, I2C_FLAG_ADDSEND);

    /* DMA channel0 full transfer finish flag */
    while(!dma_flag_get(DMA0, DMA_CH0, DMA_FLAG_FTF));
    /* DMA channel1 full transfer finish flag */
    while(!dma_flag_get(DMA0, DMA_CH1, DMA_FLAG_FTF));
    while(!i2c_flag_get(I2C0, I2C_FLAG_TC));
    /* send a stop condition to I2C bus*/
    i2c_stop_on_bus(I2C0);
    /* wait until stop condition generate */
    while(!i2c_flag_get(I2C1, I2C_FLAG_STPDET));
    /* clear the STPDET bit */
    i2c_flag_clear(I2C1, I2C_FLAG_STPDET);
    /* wait until stop condition generate */
    while(!i2c_flag_get(I2C0, I2C_FLAG_STPDET));
    /* clear the STPDET bit */
    i2c_flag_clear(I2C0, I2C_FLAG_STPDET);
    state = memory_compare(i2c0_buff_tx, i2c1_buff_rx, BUFFER_SIZE);
    if(SUCCESS == state) {
        gd_eval_led_on(LED1);
        gd_eval_led_on(LED2);
    } else {
        gd_eval_led_off(LED1);
        gd_eval_led_off(LED2);
    }
    while(1) {
    }
}

/*!
    \brief      memory compare function
    \param[in]  src : source data
    \param[in]  dst : destination data
    \param[in]  length : the compare data length
    \param[out] none
    \retval     ErrStatus : ERROR or SUCCESS
*/
ErrStatus memory_compare(uint8_t *src, uint8_t *dst, uint16_t length)
{
    while(length--) {
        if(*src++ != *dst++) {
            return ERROR;
        }
    }
    return SUCCESS;
}

/*!
    \brief      enable the peripheral clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* enable GPIOA clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* enable GPIOE clock */
    rcu_periph_clock_enable(RCU_GPIOE);
    /* enable I2C0 clock */
    rcu_periph_clock_enable(RCU_I2C0);
    /* enable I2C1 clock */
    rcu_periph_clock_enable(RCU_I2C1);
    /* enable DMA clock */
    rcu_periph_clock_enable(RCU_DMA0);
    /* enable DMAMUX clock */
    rcu_periph_clock_enable(RCU_DMAMUX);
}

/*!
    \brief      configure the I2C0 GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
*/
void i2c0_gpio_config(void)
{
    /* connect PA14 to I2C0_SCL */
    gpio_af_set(GPIOA, GPIO_AF_3, GPIO_PIN_14);
    /* connect PA13 to I2C0_SDA */
    gpio_af_set(GPIOA, GPIO_AF_3, GPIO_PIN_13);
    /* configure GPIO pins of I2C0 */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_14);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_14);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_13);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_13);
}

/*!
    \brief      configure the I2C1 GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
*/
void i2c1_gpio_config(void)
{
    /* connect PE10 to I2C1_SCL */
    gpio_af_set(GPIOE, GPIO_AF_5, GPIO_PIN_10);
    /* connect PE11 to I2C1_SDA */
    gpio_af_set(GPIOE, GPIO_AF_5, GPIO_PIN_11);
    /* configure GPIO pins of I2C1 */
    gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_10);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
    gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_11);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
}

/*!
    \brief      configure the I2C0 and I2C1 interfaces
    \param[in]  none
    \param[out] none
    \retval     none
*/
void i2c_config(void)
{
    /* configure I2C timing */
    i2c_timing_config(I2C0, 0x0, 0x5, 0);
    i2c_master_clock_config(I2C0, 0x1D, 0x59);
    /* configure slave address */
    i2c_master_addressing(I2C0, I2C1_SLAVE_ADDRESS7, I2C_MASTER_TRANSMIT);
    /* configure I2C address */
    i2c_address_config(I2C0, I2C0_SLAVE_ADDRESS7, I2C_ADDFORMAT_7BITS);
    /* configure number of bytes to be transferred */
    i2c_transfer_byte_number_config(I2C0, BUFFER_SIZE);
    /* enable I2C0 */
    i2c_enable(I2C0);

    /* configure I2C1 timing */
    i2c_timing_config(I2C1, 0x0, 0x5, 0);
    /* configure I2C address */
    i2c_address_config(I2C1, I2C1_SLAVE_ADDRESS7, I2C_ADDFORMAT_7BITS);
    /* enable I2C1 */
    i2c_enable(I2C1);
}
