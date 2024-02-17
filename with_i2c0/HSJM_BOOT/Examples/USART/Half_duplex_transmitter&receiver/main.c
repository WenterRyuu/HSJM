/*!
    \file    main.c
    \brief   USART HalfDuplex transmitter and receiver

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
#include "gd32a503v_eval.h"

#define ARRAYNUM(arr_nanme)      (uint32_t)(sizeof(arr_nanme) / sizeof(*(arr_nanme)))
#define TRANSMIT_SIZE0           (ARRAYNUM(transmitter_buffer0) - 1)
#define TRANSMIT_SIZE1           (ARRAYNUM(transmitter_buffer1) - 1)

uint8_t transmitter_buffer0[] = "\n\ra usart half-duplex test example!\n\r";
uint8_t transmitter_buffer1[] = "\n\ra usart half-duplex test example!\n\r";
uint8_t receiver_buffer0[TRANSMIT_SIZE1];
uint8_t receiver_buffer1[TRANSMIT_SIZE0];
uint8_t transfersize0 = TRANSMIT_SIZE0;
uint8_t transfersize1 = TRANSMIT_SIZE1;
__IO uint8_t txcount0 = 0;
__IO uint16_t rxcount0 = 0;
__IO uint8_t txcount1 = 0;
__IO uint16_t rxcount1 = 0;
ErrStatus state1 = ERROR;
ErrStatus state2 = ERROR;

ErrStatus memory_compare(uint8_t *src, uint8_t *dst, uint16_t length);
void com_usart_init(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    com_usart_init();
    
    /* USART0 transmit and USART1 receive*/
    usart_data_receive(USART1);
    while(transfersize0--) {
        /* wait until end of transmit */
        while(RESET == usart_flag_get(USART0, USART_FLAG_TBE)) {
        }
        usart_data_transmit(USART0, transmitter_buffer0[txcount0++]);

        while(RESET == usart_flag_get(USART1, USART_FLAG_RBNE)) {
        }
        /* store the received byte in the receiver_buffer1 */
        receiver_buffer1[rxcount0++] = usart_data_receive(USART1);
    }

    usart_data_receive(USART0);
    /* USART1 transmit and USART0 receive*/
    while(transfersize1--) {
        /* wait until end of transmit */
        while(RESET == usart_flag_get(USART1, USART_FLAG_TBE)) {
        }
        usart_data_transmit(USART1, transmitter_buffer1[txcount1++]);

        while(RESET == usart_flag_get(USART0, USART_FLAG_RBNE)) {
        }
        /* store the received byte in the receiver_buffer0 */
        receiver_buffer0[rxcount1++] = usart_data_receive(USART0);
    }
    
    /* compare the received data with the send ones */
    state1 = memory_compare(transmitter_buffer0, receiver_buffer1, TRANSMIT_SIZE0);
    state2 = memory_compare(transmitter_buffer1, receiver_buffer0, TRANSMIT_SIZE1);
    if(SUCCESS == state1) {
        /* if the data transmitted from USART0 and received by USART1 are the same */
        gd_eval_led_on(LED1);
    } else {
        /* if the data transmitted from USART0 and received by USART1 are not the same */
        gd_eval_led_off(LED1);
    }
    if(SUCCESS == state2) {
        /* if the data transmitted from USART1 and received by USART0 are the same */
        gd_eval_led_on(LED2);
    } else {
        /* if the data transmitted from USART1 and received by USART0 are not the same */
        gd_eval_led_off(LED2);
    }
    while(1) {
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
    rcu_periph_clock_enable(RCU_GPIOC);
    /* enable RX GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    
    /* enable USART clock */
    rcu_periph_clock_enable(RCU_USART0);
    rcu_periph_clock_enable(RCU_USART1);
    rcu_usart_clock_config(USART0, RCU_USARTSRC_CKSYS);
    rcu_usart_clock_config(USART1, RCU_USARTSRC_CKSYS);
    
    /* connect port to USART1 TX */
    gpio_af_set(GPIOC, GPIO_AF_5, GPIO_PIN_12);
    /* connect port to USART0 TX */
    gpio_af_set(GPIOA, GPIO_AF_5, GPIO_PIN_10);

    /* configure USART0 TX as alternate function push-pull */
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_12);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_12);

    /* configure USART1 TX as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_10);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_10);

    /* USART0 configure */
    usart_deinit(USART0);
    usart_word_length_set(USART0, USART_WL_8BIT);
    usart_stop_bit_set(USART0, USART_STB_1BIT);
    usart_parity_config(USART0, USART_PM_NONE);
    usart_baudrate_set(USART0, 115200U);
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
    /* enable USART0 half duplex Mode */
    usart_halfduplex_enable(USART0);
    usart_enable(USART0);

    /* configure USART1 */
    usart_deinit(USART1);
    usart_word_length_set(USART1, USART_WL_8BIT);
    usart_stop_bit_set(USART1, USART_STB_1BIT);
    usart_parity_config(USART1, USART_PM_NONE);
    usart_baudrate_set(USART1, 115200U);
    usart_receive_config(USART1, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);
    /* enable USART1 half duplex Mode */
    usart_halfduplex_enable(USART1);
    usart_enable(USART1);
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
