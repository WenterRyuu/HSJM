/*!
    \file    main.c
    \brief   receive data using MFCOM emulated I2S master by interrupt mode

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

#define ARRAYSIZE         8
/* recieve buffer */
uint16_t MRxBUF[8] = {0};
/* transmit buffer*/
uint16_t STxBUF[8] = {0xA809,0xB798,0xC687,0xD576,0xE465,0xF354,0x0243,0x1132};
/* transmit buffer counter */
__IO uint8_t stx = 0;
/* recieve buffer counter */
__IO uint8_t mrx = 0;

void mfcom_i2s_master_rx_init(void);
ErrStatus memory_compare(uint16_t *src, uint16_t *dst, uint8_t length);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/    
int main(void)
{
    /* configure LED */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);

    /* enable the GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOE);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);

    /* configure the D4 GPIO port */  
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_11);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
    gpio_af_set(GPIOA, GPIO_AF_6, GPIO_PIN_11);

    /* configure the D3-D2 GPIO port */
    gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_2 | GPIO_PIN_3);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2 | GPIO_PIN_3);
    gpio_af_set(GPIOE, GPIO_AF_6, GPIO_PIN_2 | GPIO_PIN_3);

    /* configure the I2S GPIO port: WS_PB5 SCK_PC6 SD_PB6 */
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5 | GPIO_PIN_6);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_6);
    gpio_af_set(GPIOB, GPIO_AF_5, GPIO_PIN_5 | GPIO_PIN_6);
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
    gpio_af_set(GPIOC, GPIO_AF_4, GPIO_PIN_6);

    /* enable SPI1 interrupt in the NVIC interrupt controller */
    NVIC_EnableIRQ(SPI1_IRQn);
    /* enable SPI1/I2S clock */
    rcu_periph_clock_enable(RCU_SPI1);

    /* configure I2S1 to work in slave transmit mode */
    i2s_init(SPI1, I2S_MODE_SLAVETX, I2S_STD_PHILLIPS, I2S_CKPL_LOW);
    i2s_psc_config(SPI1, I2S_AUDIOSAMPLE_96K, I2S_FRAMEFORMAT_DT16B_CH16B, I2S_MCKOUT_DISABLE);
    i2s_enable(SPI1);
    spi_i2s_interrupt_enable(SPI1, SPI_I2S_INT_TBE);

    /* enable MFCOM interrupt in the NVIC interrupt controller */
    NVIC_EnableIRQ(MFCOM_IRQn);
    /* enable MFCOM clock */
    rcu_periph_clock_enable(RCU_MFCOM);
    mfcom_i2s_master_rx_init();
    mfcom_shifter_interrupt_enable(MFCOM_SHIFTER_1);
    /* enable mfcom */
    mfcom_enable();

    /* wait recieve data */
    while(mrx < ARRAYSIZE){}

    /* compare receive data with send data */
    if(SUCCESS == memory_compare(MRxBUF, STxBUF, ARRAYSIZE)){
        gd_eval_led_on(LED1);
        gd_eval_led_on(LED2);
    }else{
        gd_eval_led_off(LED1);
        gd_eval_led_off(LED2);
    }

    /* If MFCOM_SBUF is overflow or shortage, the led will be turn off */
    if(MFCOM_SERR != 0){
        gd_eval_led_off(LED1);
        gd_eval_led_off(LED2);
    }

    while(1){
    }
}

/*!
    \brief      initialize mfcom to work in i2s master receive mode 
    \param[in]  none
    \param[out] none
    \retval     none
*/
void mfcom_i2s_master_rx_init()
{
    mfcom_timer_parameter_struct mfcom_timer;
    mfcom_shifter_parameter_struct mfcom_shifter;

    /* configure MFCOM_SHIFTER_1 for receive */
    mfcom_shifter.timer_select    = MFCOM_SHIFTER_TIMER0;
    mfcom_shifter.timer_polarity  = MFCOM_SHIFTER_TIMPOL_ACTIVE_LOW;
    mfcom_shifter.pin_config      = MFCOM_TIMER_PINCFG_INPUT;
    mfcom_shifter.pin_select      = MFCOM_SHIFTER_PINSEL_PIN4;
    mfcom_shifter.pin_polarity    = MFCOM_SHIFTER_PINPOL_ACTIVE_HIGH;
    mfcom_shifter.mode            = MFCOM_SHIFTER_RECEIVE;
    mfcom_shifter.input_source    = MFCOM_SHIFTER_INSRC_PIN;
    mfcom_shifter.stopbit         = MFCOM_SHIFTER_STOPBIT_DISABLE;
    mfcom_shifter.startbit        = MFCOM_SHIFTER_STARTBIT_DISABLE;
    mfcom_shifter_init(MFCOM_SHIFTER_1, &mfcom_shifter);

    /* configure MFCOM_TIMER_0 for SCK */
    mfcom_timer.trigger_select       = MFCOM_TIMER_TRGSEL_SHIFTER1;
    mfcom_timer.trigger_polarity     = MFCOM_TIMER_TRGPOL_ACTIVE_LOW;
    mfcom_timer.pin_config           = MFCOM_TIMER_PINCFG_OUTPUT;
    mfcom_timer.pin_select           = MFCOM_TIMER_PINSEL_PIN2;
    mfcom_timer.pin_polarity         = MFCOM_TIMER_PINPOL_ACTIVE_LOW;
    mfcom_timer.mode                 = MFCOM_TIMER_BAUDMODE;
    mfcom_timer.output               = MFCOM_TIMER_OUT_HIGH_EN;
    mfcom_timer.decrement            = MFCOM_TIMER_DEC_CLK_SHIFT_OUT;
    mfcom_timer.reset                = MFCOM_TIMER_RESET_NEVER;
    mfcom_timer.disable              = MFCOM_TIMER_DISMODE_NEVER;
    mfcom_timer.enable               = MFCOM_TIMER_ENMODE_TRIGHIGH;
    mfcom_timer.stopbit              = MFCOM_TIMER_STOPBIT_DISABLE;
    mfcom_timer.startbit             = MFCOM_TIMER_STARTBIT_ENABLE;
    mfcom_timer.compare              = 0x00001F10;
    mfcom_timer_init(MFCOM_TIMER_0, &mfcom_timer);

    /* configure MFCOM_TIMER_1 for WS */
    mfcom_timer.trigger_select       = MFCOM_TIMER_TRGSEL_EXTERNAL0;
    mfcom_timer.trigger_polarity     = MFCOM_TIMER_TRGPOL_ACTIVE_HIGH;
    mfcom_timer.pin_config           = MFCOM_SHIFTER_PINCFG_OUTPUT;
    mfcom_timer.pin_select           = MFCOM_TIMER_PINSEL_PIN3;
    mfcom_timer.pin_polarity         = MFCOM_TIMER_PINPOL_ACTIVE_LOW;
    mfcom_timer.mode                 = MFCOM_TIMER_16BITCOUNTER;
    mfcom_timer.output               = MFCOM_TIMER_OUT_HIGH_EN;
    mfcom_timer.decrement            = MFCOM_TIMER_DEC_CLK_SHIFT_OUT;
    mfcom_timer.reset                = MFCOM_TIMER_RESET_NEVER;
    mfcom_timer.disable              = MFCOM_TIMER_DISMODE_NEVER;
    mfcom_timer.enable               = MFCOM_TIMER_ENMODE_PRE_TIMEN;
    mfcom_timer.stopbit              = MFCOM_TIMER_STOPBIT_DISABLE;
    mfcom_timer.startbit             = MFCOM_TIMER_STARTBIT_DISABLE;
    mfcom_timer.compare              = 0x0000021F;
    mfcom_timer_init(MFCOM_TIMER_1, &mfcom_timer);
}

/*!
    \brief      memory compare function
    \param[in]  src : source data pointer
    \param[in]  dst : destination data pointer
    \param[in]  length : the compare data length
    \param[out] none
    \retval     ErrStatus : ERROR or SUCCESS
*/
ErrStatus memory_compare(uint16_t *src, uint16_t *dst, uint8_t length)
{
    while(length--){
        if(*src++ != *dst++){
            return ERROR;
        }
    }
    return SUCCESS;
}
