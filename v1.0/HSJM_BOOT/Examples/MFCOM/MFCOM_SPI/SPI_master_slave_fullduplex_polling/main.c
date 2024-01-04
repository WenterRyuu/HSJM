/*!
    \file    main.c
    \brief   master and slave fullduplex communication use MFCOM emulated spi

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

#define  ARRAYSIZE         8
uint8_t MTxBUF[8] = {0x51,0x62,0x73,0x84,0x95,0xA6,0xB7,0xC8};
uint8_t MRxBUF[8] = {0};
uint8_t STxBUF[8] = {0xA8,0x97,0xC6,0xD5,0xE4,0xF3,0x02,0x11};
uint8_t SRxBUF[8] = {0};

void init_mfcom_spi_master(void);
void init_mfcom_spi_slave(void);
ErrStatus memory_compare(uint8_t *src, uint8_t *dst, uint8_t length);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    volatile uint16_t i = 0;

    /* configure LED */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);

    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOE);
    
    /* configure the D7~D4 GPIO port */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11);
    gpio_af_set(GPIOA, GPIO_AF_5, GPIO_PIN_8 | GPIO_PIN_9);
    gpio_af_set(GPIOA, GPIO_AF_6, GPIO_PIN_10 | GPIO_PIN_11);

    /* configure the D3~D0 GPIO port */ 
    gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);
    gpio_af_set(GPIOE, GPIO_AF_6, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);

    /* mfcom config*/
    rcu_periph_clock_enable(RCU_MFCOM);
    init_mfcom_spi_slave();
    init_mfcom_spi_master();
    mfcom_enable();

    /* polling transmission data in LSB mode */
    for(i = 0; i < ARRAYSIZE; i++){
        while(mfcom_shifter_flag_get(MFCOM_SHIFTER_0)==RESET);
        while(mfcom_shifter_flag_get(MFCOM_SHIFTER_2)==RESET);
        mfcom_buffer_write(MFCOM_SHIFTER_2, STxBUF[i], MFCOM_RWMODE_NORMAL);
        mfcom_buffer_write(MFCOM_SHIFTER_0, MTxBUF[i], MFCOM_RWMODE_NORMAL);

        while(mfcom_shifter_flag_get(MFCOM_SHIFTER_1)==RESET);
        while(mfcom_shifter_flag_get(MFCOM_SHIFTER_3)==RESET);
        MRxBUF[i] = mfcom_buffer_read(MFCOM_SHIFTER_1, MFCOM_RWMODE_BYTESWAP);
        SRxBUF[i] = mfcom_buffer_read(MFCOM_SHIFTER_3, MFCOM_RWMODE_BYTESWAP);
    }

    /* compare receive data with send data */
    if(ERROR != memory_compare(MTxBUF, SRxBUF, ARRAYSIZE)){
        gd_eval_led_on(LED1);
    }else{
        gd_eval_led_off(LED1);
    }

    if(ERROR != memory_compare(MRxBUF, STxBUF, ARRAYSIZE)){
        gd_eval_led_on(LED2);
    }else{
        gd_eval_led_off(LED2);
    }

    while(1){
    }
}
/*!
    \brief      initialize mfcom to work in spi master mode. 
    \param[in]  none
    \param[out] none
    \retval     none
*/
void init_mfcom_spi_master(void)
{
    mfcom_timer_parameter_struct mfcom_timer;
    mfcom_shifter_parameter_struct mfcom_shifter;

    /* configure MFCOM_TIMER_0 for shift clock */
    mfcom_timer.trigger_select       = MFCOM_TIMER_TRGSEL_SHIFTER0;
    mfcom_timer.trigger_polarity     = MFCOM_TIMER_TRGPOL_ACTIVE_LOW;
    mfcom_timer.pin_config           = MFCOM_TIMER_PINCFG_OUTPUT;
    mfcom_timer.pin_select           = MFCOM_TIMER_PINSEL_PIN2;
    mfcom_timer.pin_polarity         = MFCOM_TIMER_PINPOL_ACTIVE_HIGH;
    mfcom_timer.mode                 = MFCOM_TIMER_BAUDMODE;
    mfcom_timer.output               = MFCOM_TIMER_OUT_LOW_EN;
    mfcom_timer.decrement            = MFCOM_TIMER_DEC_CLK_SHIFT_OUT;
    mfcom_timer.reset                = MFCOM_TIMER_RESET_NEVER;
    mfcom_timer.disable              = MFCOM_TIMER_DISMODE_COMPARE;
    mfcom_timer.enable               = MFCOM_TIMER_ENMODE_TRIGHIGH;
    mfcom_timer.stopbit              = MFCOM_TIMER_STOPBIT_TIMDIS;
    mfcom_timer.startbit             = MFCOM_TIMER_STARTBIT_DISABLE;
    mfcom_timer.compare              = 0x0F2F;
    mfcom_timer_init(MFCOM_TIMER_0, &mfcom_timer);

    /* configure MFCOM_TIMER_1 for nss */
    mfcom_timer.trigger_select       = MFCOM_TIMER_TRGSEL_PIN3;
    mfcom_timer.trigger_polarity     = MFCOM_TIMER_TRGPOL_ACTIVE_HIGH;
    mfcom_timer.pin_config           = MFCOM_TIMER_PINCFG_OUTPUT;
    mfcom_timer.pin_select           = MFCOM_TIMER_PINSEL_PIN3;
    mfcom_timer.pin_polarity         = MFCOM_TIMER_PINPOL_ACTIVE_LOW;
    mfcom_timer.mode                 = MFCOM_TIMER_16BITCOUNTER;
    mfcom_timer.output               = MFCOM_TIMER_OUT_HIGH_EN;
    mfcom_timer.decrement            = MFCOM_TIMER_DEC_CLK_SHIFT_OUT;
    mfcom_timer.reset                = MFCOM_TIMER_RESET_NEVER;
    mfcom_timer.disable              = MFCOM_TIMER_DISMODE_PRE_TIMDIS;
    mfcom_timer.enable               = MFCOM_TIMER_ENMODE_PRE_TIMEN;
    mfcom_timer.stopbit              = MFCOM_TIMER_STOPBIT_DISABLE;
    mfcom_timer.startbit             = MFCOM_TIMER_STARTBIT_DISABLE;
    mfcom_timer.compare              = 0xFFFF;
    mfcom_timer_init(MFCOM_TIMER_1, &mfcom_timer);

    /* configure MFCOM_SHIFTER_0 for tx */
    mfcom_shifter.timer_select       = MFCOM_SHIFTER_TIMER0;
    mfcom_shifter.timer_polarity     = MFCOM_SHIFTER_TIMPOL_ACTIVE_LOW;
    mfcom_shifter.pin_config         = MFCOM_SHIFTER_PINCFG_OUTPUT;
    mfcom_shifter.pin_select         = MFCOM_SHIFTER_PINSEL_PIN0;
    mfcom_shifter.pin_polarity       = MFCOM_SHIFTER_PINPOL_ACTIVE_HIGH;
    mfcom_shifter.mode               = MFCOM_SHIFTER_TRANSMIT;
    mfcom_shifter.input_source       = MFCOM_SHIFTER_INSRC_PIN;
    mfcom_shifter.stopbit            = MFCOM_SHIFTER_STOPBIT_DISABLE;
    mfcom_shifter.startbit           = MFCOM_SHIFTER_STARTBIT_DISABLE;
    mfcom_shifter_init(MFCOM_SHIFTER_0, &mfcom_shifter);

    /* configure MFCOM_SHIFTER_1 for rx */
    mfcom_shifter.timer_select       = MFCOM_SHIFTER_TIMER0;
    mfcom_shifter.timer_polarity     = MFCOM_SHIFTER_TIMPOL_ACTIVE_HIGH;
    mfcom_shifter.pin_config         = MFCOM_SHIFTER_PINCFG_INPUT;
    mfcom_shifter.pin_select         = MFCOM_SHIFTER_PINSEL_PIN1;
    mfcom_shifter.pin_polarity       = MFCOM_SHIFTER_PINPOL_ACTIVE_HIGH;
    mfcom_shifter.mode               = MFCOM_SHIFTER_RECEIVE;
    mfcom_shifter.input_source       = MFCOM_SHIFTER_INSRC_PIN;
    mfcom_shifter.stopbit            = MFCOM_SHIFTER_STOPBIT_DISABLE;
    mfcom_shifter.startbit           = MFCOM_SHIFTER_STARTBIT_DISABLE;
    mfcom_shifter_init(MFCOM_SHIFTER_1, &mfcom_shifter);
}

/*!
    \brief      initialize mfcom to work in spi slave mode.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void init_mfcom_spi_slave(void)
{
    mfcom_timer_parameter_struct mfcom_timer;
    mfcom_shifter_parameter_struct mfcom_shifter;

    /* configure MFCOM_TIMER_2 for shift clock */
    mfcom_timer.trigger_select     = MFCOM_TIMER_TRGSEL_PIN7;
    mfcom_timer.trigger_polarity   = MFCOM_TIMER_TRGPOL_ACTIVE_LOW;
    mfcom_timer.pin_config         = MFCOM_TIMER_PINCFG_INPUT;
    mfcom_timer.pin_select         = MFCOM_TIMER_PINSEL_PIN6;
    mfcom_timer.pin_polarity       = MFCOM_TIMER_PINPOL_ACTIVE_HIGH;
    mfcom_timer.mode               = MFCOM_TIMER_16BITCOUNTER;
    mfcom_timer.output             = MFCOM_TIMER_OUT_HIGH_EN;
    mfcom_timer.decrement          = MFCOM_TIMER_DEC_PIN_SHIFT_PIN;
    mfcom_timer.reset              = MFCOM_TIMER_RESET_NEVER;  
    mfcom_timer.disable            = MFCOM_TIMER_DISMODE_TRIGFALLING;
    mfcom_timer.enable             = MFCOM_TIMER_ENMODE_TRIGRISING; 
    mfcom_timer.stopbit            = MFCOM_TIMER_STOPBIT_DISABLE;
    mfcom_timer.startbit           = MFCOM_TIMER_STARTBIT_ENABLE;
    mfcom_timer.compare            = 0xFFFF;
    mfcom_timer_init(MFCOM_TIMER_2, &mfcom_timer);

    /* configure MFCOM_SHIFTER_2 for tx */
    mfcom_shifter.timer_select    = MFCOM_SHIFTER_TIMER2;
    mfcom_shifter.timer_polarity  = MFCOM_SHIFTER_TIMPOL_ACTIVE_LOW;
    mfcom_shifter.pin_config      = MFCOM_SHIFTER_PINCFG_OUTPUT;
    mfcom_shifter.pin_select      = MFCOM_SHIFTER_PINSEL_PIN5;
    mfcom_shifter.pin_polarity    = MFCOM_SHIFTER_PINPOL_ACTIVE_HIGH;
    mfcom_shifter.mode            = MFCOM_SHIFTER_TRANSMIT;
    mfcom_shifter.input_source    = MFCOM_SHIFTER_INSRC_PIN;
    mfcom_shifter.stopbit         = MFCOM_SHIFTER_STOPBIT_DISABLE;
    mfcom_shifter.startbit        = MFCOM_SHIFTER_STARTBIT_DISABLE;
    mfcom_shifter_init(MFCOM_SHIFTER_2, &mfcom_shifter);

    /* configure MFCOM_SHIFTER_3 for rx */
    mfcom_shifter.timer_select    = MFCOM_SHIFTER_TIMER2;
    mfcom_shifter.timer_polarity  = MFCOM_SHIFTER_TIMPOL_ACTIVE_HIGH;
    mfcom_shifter.pin_config      = MFCOM_SHIFTER_PINCFG_INPUT;
    mfcom_shifter.pin_select      = MFCOM_SHIFTER_PINSEL_PIN4;
    mfcom_shifter.pin_polarity    = MFCOM_SHIFTER_PINPOL_ACTIVE_HIGH;
    mfcom_shifter.mode            = MFCOM_SHIFTER_RECEIVE;
    mfcom_shifter.input_source    = MFCOM_SHIFTER_INSRC_PIN;
    mfcom_shifter.stopbit         = MFCOM_SHIFTER_STOPBIT_DISABLE;
    mfcom_shifter.startbit        = MFCOM_SHIFTER_STARTBIT_DISABLE;
    mfcom_shifter_init(MFCOM_SHIFTER_3, &mfcom_shifter);
}

/*!
    \brief      memory compare function
    \param[in]  src : source data pointer
    \param[in]  dst : destination data pointer
    \param[in]  length : the compare data length
    \param[out] none
    \retval     ErrStatus : ERROR or SUCCESS
*/
ErrStatus memory_compare(uint8_t *src, uint8_t *dst, uint8_t length)
{
    while(length--){
        if(*src++ != *dst++){
            return ERROR;
        }
    }
    return SUCCESS;
}
