/*!
    \file    main.c
    \brief   MFCOM emulated spi and SPI fullduplex communication use dma

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
uint8_t MTxBUF[8] = {0x51,0x62,0x73,0x84,0x95,0xA6,0xB7,0xC8};
uint8_t MRxBUF[8] = {0};
uint8_t STxBUF[8] = {0xA8,0x97,0xC6,0xD5,0xE4,0xF3,0x02,0x11};
uint8_t SRxBUF[8] = {0};
void mfcom_spi_master_init(void);
void mfcom_spi_salve_init(void);
void mfcom_dma_config(void);
void spi_dma_config(void);
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
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15); /* PD13 ->NSS */
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    gpio_af_set(GPIOD, GPIO_AF_4, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);/* PD14 ->MOSI PD15 ->MISO */

    /* mfcom config*/
    rcu_periph_clock_enable(RCU_MFCOM);
    mfcom_dma_config();
    mfcom_spi_master_init();
    mfcom_enable();

    /* spi config*/
    rcu_periph_clock_enable(RCU_SPI1);
    spi_para.device_mode          = SPI_SLAVE;
    spi_para.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_para.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_para.nss                  = SPI_NSS_HARD;
    spi_para.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    spi_para.prescale             = SPI_PSC_4;
    spi_para.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI1,&spi_para);
    spi_enable(SPI1);

    /* enable dma function */
    spi_dma_config();
    spi_dma_enable(SPI1,SPI_DMA_RECEIVE);
    spi_dma_enable(SPI1,SPI_DMA_TRANSMIT);
    mfcom_shifter_dma_enable(MFCOM_SHIFTER_2);
    mfcom_shifter_dma_enable(MFCOM_SHIFTER_3);

    /* wait dma transmit complete */
    while(!dma_flag_get(DMA0, DMA_CH1, DMA_INT_FLAG_FTF));
    while(!dma_flag_get(DMA0, DMA_CH4, DMA_INT_FLAG_FTF));
    while(!dma_flag_get(DMA1, DMA_CH0, DMA_INT_FLAG_FTF));
    while(!dma_flag_get(DMA1, DMA_CH2, DMA_INT_FLAG_FTF));

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

    /* determine if the shifter has an error */
    if(MFCOM_SERR !=0){
        gd_eval_led_off(LED2);
        gd_eval_led_off(LED1);
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
    
    /* configure MFCOM_SHIFTER_2 */
    mfcom_shifter.timer_polarity  = MFCOM_SHIFTER_TIMPOL_ACTIVE_HIGH;
    mfcom_shifter.pin_config      = MFCOM_SHIFTER_PINCFG_INPUT;
    mfcom_shifter.pin_select      = MFCOM_SHIFTER_PINSEL_PIN5;
    mfcom_shifter.mode            = MFCOM_SHIFTER_RECEIVE;
    mfcom_shifter_init(MFCOM_SHIFTER_2,&mfcom_shifter);
    
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

    /* configure MFCOM_TIMER_1 */
    mfcom_timer.trigger_select    = MFCOM_TIMER_TRGSEL_EXTERNAL0;
    mfcom_timer.trigger_polarity  = MFCOM_TIMER_TRGPOL_ACTIVE_HIGH;
    mfcom_timer.pin_config        = MFCOM_TIMER_PINCFG_OUTPUT;
    mfcom_timer.pin_select        = MFCOM_TIMER_PINSEL_PIN3;
    mfcom_timer.pin_polarity      = MFCOM_TIMER_PINPOL_ACTIVE_LOW;
    mfcom_timer.mode              = MFCOM_TIMER_16BITCOUNTER;
    mfcom_timer.output            = MFCOM_TIMER_OUT_HIGH_EN;
    mfcom_timer.decrement         = MFCOM_TIMER_DEC_CLK_SHIFT_OUT;
    mfcom_timer.reset             = MFCOM_TIMER_RESET_NEVER;
    mfcom_timer.disable           = MFCOM_TIMER_DISMODE_PRE_TIMDIS;
    mfcom_timer.enable            = MFCOM_TIMER_ENMODE_PRE_TIMEN;
    mfcom_timer.stopbit           = MFCOM_TIMER_STOPBIT_DISABLE;
    mfcom_timer.startbit          = MFCOM_TIMER_STARTBIT_DISABLE;
    mfcom_timer.compare           = 0xFFFF;
    mfcom_timer_init(MFCOM_TIMER_1, &mfcom_timer);
}

/*!
    \brief      configure mfcom to work in dma mode. 
    \param[in]  none
    \param[out] none
    \retval     none
*/
void mfcom_dma_config(void)
{
    dma_parameter_struct dma_para;

    rcu_periph_clock_enable(RCU_DMA0);
    rcu_periph_clock_enable(RCU_DMA1);
    rcu_periph_clock_enable(RCU_DMAMUX);
    
    /* configure MFCOM_SPI transmit dma:DMA_CH1 */
    dma_deinit(DMA0,DMA_CH1);  
    dma_struct_para_init(&dma_para);
    dma_para.direction = DMA_MEMORY_TO_PERIPHERAL;
    dma_para.memory_addr = (uint32_t)MTxBUF;
    dma_para.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_para.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_para.number = 8;
    dma_para.periph_addr = MFCOM + 0x00000380U + 3*4;
    dma_para.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_para.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_para.priority = DMA_PRIORITY_LOW;
    dma_para.request = DMA_REQUESR_SSTAT3;
    dma_init(DMA0, DMA_CH1, &dma_para);
    dma_channel_enable(DMA0, DMA_CH1);
    
    /* configure MFCOM_SPI transmit dma:DMA_CH0 */
    dma_deinit(DMA1,DMA_CH0);
    dma_struct_para_init(&dma_para);
    dma_para.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_para.memory_addr = (uint32_t)MRxBUF;
    dma_para.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_para.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_para.number = 8;
    dma_para.periph_addr = MFCOM + 0x00000280U + 2*4;
    dma_para.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_para.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_para.priority = DMA_PRIORITY_LOW;
    dma_para.request = DMA_REQUESR_SSTAT2;
    dma_init(DMA1, DMA_CH0, &dma_para);
    dma_channel_enable(DMA1, DMA_CH0);
}

/*!
    \brief      configure spi1 to work in dma mode. 
    \param[in]  none
    \param[out] none
    \retval     none
*/
void spi_dma_config(void)
{
    dma_parameter_struct dma_para;

    rcu_periph_clock_enable(RCU_DMA0);
    rcu_periph_clock_enable(RCU_DMA1);
    rcu_periph_clock_enable(RCU_DMAMUX);
    
    /* configure SPI1 transmit dma:DMA_CH2 */
    dma_deinit(DMA1,DMA_CH2);  
    dma_struct_para_init(&dma_para);
    dma_para.direction = DMA_MEMORY_TO_PERIPHERAL;
    dma_para.memory_addr = (uint32_t)STxBUF;
    dma_para.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_para.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_para.number = 8;
    dma_para.periph_addr = (SPI1) + 0x0CU;
    dma_para.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_para.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_para.priority = DMA_PRIORITY_LOW;
    dma_para.request = DMA_REQUEST_SPI1_TX;
    dma_init(DMA1,DMA_CH2,&dma_para);
    dma_channel_enable(DMA1,DMA_CH2);
    
    /* configure SPI1 transmit dma:DMA_CH4 */
    dma_deinit(DMA0,DMA_CH4);
    dma_struct_para_init(&dma_para);
    dma_para.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_para.memory_addr = (uint32_t)SRxBUF;
    dma_para.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_para.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_para.number = 8;
    dma_para.periph_addr = (SPI1) + 0x0CU;
    dma_para.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_para.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_para.priority = DMA_PRIORITY_LOW;
    dma_para.request = DMA_REQUEST_SPI1_RX;
    dma_init(DMA0,DMA_CH4,&dma_para);
    dma_channel_enable(DMA0,DMA_CH4);
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
