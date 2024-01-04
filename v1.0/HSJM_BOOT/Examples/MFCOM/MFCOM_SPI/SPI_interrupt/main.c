/*!
    \file    main.c
    \brief   MFCOM emulated spi master send and slave receive data use interrupt mode

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
#include "gd32a50x_it.h"

#define ARRAYSIZE         8
uint8_t MTxBUF[8] = {0x51,0x62,0x73,0x84,0x95,0xA6,0xB7,0xC8};
uint8_t SRxBUF[8] = {0};
extern uint32_t srx;
void mfcom_spi_master_init(void);
void mfcom_spi_salve_init(void);
ErrStatus memory_compare(uint8_t *src, uint8_t *dst, uint8_t length);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    spi_parameter_struct spi_para;

    /* configure LED */
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED1);

    /* configure the D2~D5 GPIO port */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOE);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_10 | GPIO_PIN_11);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10 | GPIO_PIN_11);
    gpio_af_set(GPIOA, GPIO_AF_6, GPIO_PIN_10 | GPIO_PIN_11);
    gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_2 | GPIO_PIN_3);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2 | GPIO_PIN_3);
    gpio_af_set(GPIOE, GPIO_AF_6, GPIO_PIN_2 | GPIO_PIN_3);
    
    /* SPI1 gpio config */
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOE);
    gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5); 
    gpio_af_set(GPIOE, GPIO_AF_4, GPIO_PIN_5);/* PE5 ->SPI1_SCK */
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE,  GPIO_PIN_15 ); 
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15 );
    gpio_af_set(GPIOD, GPIO_AF_4, GPIO_PIN_15);/* PD15 ->MISO */
    
    /* mfcom config*/
    nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);
    nvic_irq_enable(MFCOM_IRQn, 1, 1);
    rcu_periph_clock_enable(RCU_MFCOM);
    mfcom_spi_master_init();

    /* spi config*/
    nvic_irq_enable(SPI1_IRQn, 0, 1);
    rcu_periph_clock_enable(RCU_SPI1);
    spi_para.device_mode          = SPI_SLAVE;
    spi_para.trans_mode           = SPI_TRANSMODE_BDRECEIVE;
    spi_para.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_para.nss                  = SPI_NSS_SOFT;
    spi_para.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    spi_para.prescale             = SPI_PSC_8;
    spi_para.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI1, &spi_para);

    spi_i2s_interrupt_enable(SPI1, SPI_I2S_INT_RBNE);
    mfcom_shifter_interrupt_enable(MFCOM_SHIFTER_3);
    spi_enable(SPI1);
    mfcom_enable();

    /* wait transmit complete */
    while(srx < ARRAYSIZE){
        mfcom_shifter_flag_clear(MFCOM_SHIFTER_3);
    }

    /* compare receive data with send data */
    if(ERROR != memory_compare(MTxBUF, SRxBUF, ARRAYSIZE)){
        gd_eval_led_on(LED1);
        gd_eval_led_on(LED2);
    }else{
        gd_eval_led_off(LED1);
        gd_eval_led_off(LED2);
    }

    /*  determine if the shifter has an error */
    if(MFCOM_SERR !=0){
        gd_eval_led_off(LED1);
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
void mfcom_spi_master_init(void)
{  
    mfcom_timer_parameter_struct mfcom_timer;
    mfcom_shifter_parameter_struct mfcom_shifter;

    /* configure MFCOM_SHIFTER_3 */
    mfcom_shifter.timer_select    = MFCOM_SHIFTER_TIMER0;
    mfcom_shifter.timer_polarity  = MFCOM_SHIFTER_TIMPOL_ACTIVE_LOW;
    mfcom_shifter.pin_config      = MFCOM_SHIFTER_PINCFG_OUTPUT;
    mfcom_shifter.pin_select      = MFCOM_SHIFTER_PINSEL_PIN4;
    mfcom_shifter.pin_polarity    = MFCOM_SHIFTER_PINPOL_ACTIVE_HIGH;
    mfcom_shifter.mode            = MFCOM_SHIFTER_TRANSMIT;
    mfcom_shifter.input_source    = MFCOM_SHIFTER_INSRC_PIN;
    mfcom_shifter.stopbit         = MFCOM_SHIFTER_STOPBIT_DISABLE;
    mfcom_shifter.startbit        = MFCOM_SHIFTER_STARTBIT_DISABLE;
    mfcom_shifter_init(MFCOM_SHIFTER_3,&mfcom_shifter);

    /* configure MFCOM_TIMER_0 */
    mfcom_timer.trigger_select    = MFCOM_TIMER_TRGSEL_SHIFTER3;
    mfcom_timer.trigger_polarity  = MFCOM_TIMER_TRGPOL_ACTIVE_LOW;
    mfcom_timer.pin_config        = MFCOM_TIMER_PINCFG_OUTPUT;
    mfcom_timer.pin_select        = MFCOM_TIMER_PINSEL_PIN2;
    mfcom_timer.pin_polarity      = MFCOM_TIMER_PINPOL_ACTIVE_HIGH;
    mfcom_timer.mode              = MFCOM_TIMER_BAUDMODE;
    mfcom_timer.output            = MFCOM_TIMER_OUT_LOW_EN;
    mfcom_timer.decrement         = MFCOM_TIMER_DEC_CLK_SHIFT_OUT;
    mfcom_timer.reset             = MFCOM_TIMER_RESET_NEVER;
    mfcom_timer.disable           = MFCOM_TIMER_DISMODE_COMPARE;
    mfcom_timer.enable            = MFCOM_TIMER_ENMODE_TRIGHIGH;
    mfcom_timer.stopbit           = MFCOM_TIMER_STOPBIT_TIMDIS;
    mfcom_timer.startbit          = MFCOM_TIMER_STARTBIT_ENABLE;
    mfcom_timer.compare           = 0x0F08;
    mfcom_timer_init(MFCOM_TIMER_0, &mfcom_timer);
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
