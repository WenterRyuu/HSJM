/*!
    \file    main.c
    \brief   USART DMA transmitter receiver

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

uint8_t rxbuffer[10];
uint8_t txbuffer[] = "\n\rUSART DMA receive and transmit example, please input 10 bytes:\n\r";
#define ARRAYNUM(arr_nanme)       (uint32_t)(sizeof(arr_nanme) / sizeof(*(arr_nanme)))
#define USART0_TDATA_ADDRESS      ((uint32_t)&USART_TDATA(USART0))
#define USART0_RDATA_ADDRESS      ((uint32_t)&USART_RDATA(USART0))

void com_usart_init(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    dma_parameter_struct dma_init_struct;
    /* enable DMA clock */
    rcu_periph_clock_enable(RCU_DMA0);
    rcu_periph_clock_enable(RCU_DMAMUX);

    /* initialize the com */
    com_usart_init();

    printf("\n\ra usart dma function test example!\n\r");

    /* initialize DMA0 channel 0 */
    dma_deinit(DMA0, DMA_CH0);
    dma_struct_para_init(&dma_init_struct);
    dma_init_struct.request      = DMA_REQUEST_USART0_TX;
    dma_init_struct.direction    = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_addr  = (uint32_t)txbuffer;
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number       = ARRAYNUM(txbuffer);
    dma_init_struct.periph_addr  = (uint32_t)USART0_TDATA_ADDRESS;
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA0, DMA_CH0, &dma_init_struct);

    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH0);
    dma_memory_to_memory_disable(DMA0, DMA_CH0);
    /* disable the DMAMUX_MUXCH0 synchronization mode */
    dmamux_synchronization_disable(DMAMUX_MULTIPLEXER_CH0);
    /* enable DMA0 channel 0 */
    dma_channel_enable(DMA0, DMA_CH0);

    /* enable USART DMA for transmission */
    usart_dma_transmit_config(USART0, USART_TRANSMIT_DMA_ENABLE);
    /* wait DMA0 channel0 transfer complete */
    while(RESET == dma_flag_get(DMA0, DMA_CH0, DMA_FLAG_FTF)) {
    }
    while(1) {
        /* initialize DMA0 channel1 */
        dma_deinit(DMA0, DMA_CH1);
        usart_flag_clear(USART0, USART_FLAG_RBNE);
        usart_dma_receive_config(USART0, USART_RECEIVE_DMA_ENABLE);
        dma_struct_para_init(&dma_init_struct);
        dma_init_struct.request      = DMA_REQUEST_USART0_RX;
        dma_init_struct.direction    = DMA_PERIPHERAL_TO_MEMORY;
        dma_init_struct.memory_addr  = (uint32_t)rxbuffer;
        dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
        dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
        dma_init_struct.number       = ARRAYNUM(rxbuffer);
        dma_init_struct.periph_addr  = (uint32_t)USART0_RDATA_ADDRESS;
        dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
        dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
        dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;
        dma_init(DMA0, DMA_CH1, &dma_init_struct);

        /* configure DMA mode */
        dma_circulation_disable(DMA0, DMA_CH1);
        dma_memory_to_memory_disable(DMA0, DMA_CH1);
        /* disable the DMAMUX_MUXCH0 synchronization mode */
        dmamux_synchronization_disable(DMAMUX_MULTIPLEXER_CH1);

        /* enable DMA0 channel 1 */
        dma_channel_enable(DMA0, DMA_CH1);

        /* wait DMA0 channel 1 transfer complete */
        while(RESET == dma_flag_get(DMA0, DMA_CH1, DMA_FLAG_FTF));
        usart_dma_receive_config(USART0, USART_RECEIVE_DMA_DISABLE);
        printf("\n\r%s\n\r", rxbuffer);
    }
}

/*!
    \brief      initialize the USART configuration of the com
    \param[in]  none
    \param[out] none
    \retval     none
*/
void com_usart_init(void)
{
    /* enable TX GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    
    /* enable USART clock */
    rcu_periph_clock_enable(RCU_USART0);
    rcu_usart_clock_config(USART0, RCU_USARTSRC_CKSYS);

    /* connect port to USART TX */
    gpio_af_set(GPIOA, GPIO_AF_5, GPIO_PIN_10);
    /* connect port to USART RX */
    gpio_af_set(GPIOA, GPIO_AF_5, GPIO_PIN_11);

    /* configure USART TX as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_10);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_10);

    /* configure USART RX as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_11);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_11);

    /* USART configure */
    usart_deinit(USART0);
    usart_word_length_set(USART0, USART_WL_8BIT);
    usart_stop_bit_set(USART0, USART_STB_1BIT);
    usart_parity_config(USART0, USART_PM_NONE);
    usart_baudrate_set(USART0, 115200U);
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);

    usart_enable(USART0);
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(USART0, (uint8_t)ch);
    while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
    return ch;
}
