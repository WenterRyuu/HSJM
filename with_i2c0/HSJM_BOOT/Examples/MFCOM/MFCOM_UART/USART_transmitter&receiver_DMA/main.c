/*!
    \file    main.c
    \brief   transmit/receive data using MFCOM emulated USART by DMA mode 

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
uint8_t MTxBUF[8] = {0x12,0x23,0x34,0x45,0x56,0x67,0x78,0x89};
uint8_t MRxBUF[8] = {0};
uint8_t STxBUF[8] = {0xFE,0xED,0xDC,0xCB,0xBA,0xA9,0x98,0x87};
uint8_t SRxBUF[8] = {0};

void mfcom_usart_init(void);
void mfcom_usart_send(uint8_t * buffer, uint32_t size);
void mfcom_usart_receive(uint8_t * buffer, uint32_t size);
ErrStatus memory_compare(uint8_t *src, uint8_t *dst, uint8_t length);

int main(void)
{
    /* for mfcom usart div */
    RCU_CFG0 |= RCU_AHB_CKSYS_DIV2;

    /* led config */
    /* configure LED */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOE);

    /* configure the D0/D1 GPIO port */ 
    gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4 | GPIO_PIN_5);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4 | GPIO_PIN_5);
    gpio_af_set(GPIOE, GPIO_AF_6, GPIO_PIN_4 | GPIO_PIN_5);   //PE5 D0 TX  PE4 D1 RX

    /* usart gpio config */
    rcu_periph_clock_enable(RCU_GPIOA);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_10 | GPIO_PIN_11);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10 | GPIO_PIN_11);
    gpio_af_set(GPIOA, GPIO_AF_5, GPIO_PIN_10 | GPIO_PIN_11);   //PA10 TX  PA11 RX
    
    /* USART configure */
    rcu_periph_clock_enable(RCU_USART0);
    rcu_usart_clock_config(USART0, RCU_USARTSRC_IRC8M);
    usart_deinit(USART0);
    usart_baudrate_set(USART0, 115200U);
    usart_data_first_config(USART0,USART_MSBF_LSB);

    /* USART and MFCOM enable */
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
    usart_enable(USART0);
    rcu_periph_clock_enable(RCU_MFCOM);

    /* initialize the mfcom_usart */
    mfcom_usart_init();
    mfcom_usart_send(MTxBUF, 8);
    mfcom_usart_receive(MRxBUF, 8);

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

    if(MFCOM_SERR != 0){
        gd_eval_led_off(LED1);
        gd_eval_led_off(LED2);
    }

    while(1){
    }
}

void mfcom_usart_init()
{  
    mfcom_timer_parameter_struct mfcom_timer;
    mfcom_shifter_parameter_struct mfcom_shifter;
   
    /* configure MFCOM_SHIFTER_1 for tx */
    mfcom_shifter.timer_select    = MFCOM_SHIFTER_TIMER0;
    mfcom_shifter.timer_polarity  = MFCOM_SHIFTER_TIMPOL_ACTIVE_HIGH;
    mfcom_shifter.pin_config      = MFCOM_SHIFTER_PINCFG_OUTPUT;
    mfcom_shifter.pin_select      = MFCOM_SHIFTER_PINSEL_PIN0;
    mfcom_shifter.pin_polarity    = MFCOM_SHIFTER_PINPOL_ACTIVE_HIGH;
    mfcom_shifter.mode            = MFCOM_SHIFTER_TRANSMIT;
    mfcom_shifter.input_source    = MFCOM_SHIFTER_INSRC_PIN;
    mfcom_shifter.stopbit         = MFCOM_SHIFTER_STOPBIT_HIGH;
    mfcom_shifter.startbit        = MFCOM_SHIFTER_STARTBIT_LOW;
    mfcom_shifter_init(MFCOM_SHIFTER_1, &mfcom_shifter);
    
    /* configure MFCOM_SHIFTER_3 for rx */
    mfcom_shifter.timer_select    = MFCOM_SHIFTER_TIMER2;
    mfcom_shifter.timer_polarity  = MFCOM_SHIFTER_TIMPOL_ACTIVE_LOW;
    mfcom_shifter.pin_config      = MFCOM_SHIFTER_PINCFG_INPUT;
    mfcom_shifter.pin_select      = MFCOM_SHIFTER_PINSEL_PIN1;
    mfcom_shifter.pin_polarity    = MFCOM_SHIFTER_PINPOL_ACTIVE_HIGH;
    mfcom_shifter.mode            = MFCOM_SHIFTER_RECEIVE;
    mfcom_shifter.input_source    = MFCOM_SHIFTER_INSRC_PIN;
    mfcom_shifter.stopbit         = MFCOM_SHIFTER_STOPBIT_HIGH;
    mfcom_shifter.startbit        = MFCOM_SHIFTER_STARTBIT_LOW;
    mfcom_shifter_init(MFCOM_SHIFTER_3, &mfcom_shifter);
    
    /* configure MFCOM_TIMER_0 for tx */
    mfcom_timer.trigger_select       = MFCOM_TIMER_TRGSEL_SHIFTER1;
    mfcom_timer.trigger_polarity     = MFCOM_TIMER_TRGPOL_ACTIVE_LOW;
    mfcom_timer.pin_config           = MFCOM_TIMER_PINCFG_OUTPUT;
    mfcom_timer.pin_select           = MFCOM_TIMER_PINSEL_PIN2;
    mfcom_timer.pin_polarity         = MFCOM_TIMER_PINPOL_ACTIVE_HIGH;
    mfcom_timer.mode                 = MFCOM_TIMER_BAUDMODE;
    mfcom_timer.output               = MFCOM_TIMER_OUT_HIGH_EN;
    mfcom_timer.decrement            = MFCOM_TIMER_DEC_CLK_SHIFT_OUT;
    mfcom_timer.reset                = MFCOM_TIMER_RESET_NEVER;
    mfcom_timer.disable              = MFCOM_TIMER_DISMODE_COMPARE;
    mfcom_timer.enable               = MFCOM_TIMER_ENMODE_TRIGHIGH;
    mfcom_timer.stopbit              = MFCOM_TIMER_STOPBIT_TIMDIS;
    mfcom_timer.startbit             = MFCOM_TIMER_STARTBIT_ENABLE;
    mfcom_timer.compare              = 0x0FD8;
    mfcom_timer_init(MFCOM_TIMER_0, &mfcom_timer);

    /* configure MFCOM_TIMER_2 for rx */
    mfcom_timer.trigger_select       = MFCOM_TIMER_TRGSEL_PIN1;
    mfcom_timer.trigger_polarity     = MFCOM_TIMER_TRGPOL_ACTIVE_LOW;
    mfcom_timer.pin_config           = MFCOM_TIMER_PINCFG_OUTPUT;
    mfcom_timer.pin_select           = MFCOM_TIMER_PINSEL_PIN3;
    mfcom_timer.pin_polarity         = MFCOM_TIMER_PINPOL_ACTIVE_HIGH;
    mfcom_timer.mode                 = MFCOM_TIMER_BAUDMODE;
    mfcom_timer.output               = MFCOM_TIMER_OUT_LOW_EN;
    mfcom_timer.decrement            = MFCOM_TIMER_DEC_CLK_SHIFT_OUT;
    mfcom_timer.reset                = MFCOM_TIMER_RESET_NEVER;
    mfcom_timer.disable              = MFCOM_TIMER_DISMODE_COMPARE;
    mfcom_timer.enable               = MFCOM_TIMER_ENMODE_TRIGRISING;
    mfcom_timer.stopbit              = MFCOM_TIMER_STOPBIT_DISABLE;
    mfcom_timer.startbit             = MFCOM_TIMER_STARTBIT_ENABLE;
    mfcom_timer.compare              = 0x0FDA;
    mfcom_timer_init(MFCOM_TIMER_2, &mfcom_timer);
    
    mfcom_enable();
}


void mfcom_usart_send(uint8_t * buffer, uint32_t size)
{
    uint32_t i;
    dma_parameter_struct dma_para;
    rcu_periph_clock_enable(RCU_DMA0);
    rcu_periph_clock_enable(RCU_DMA1);
    rcu_periph_clock_enable(RCU_DMAMUX);
    dma_deinit(DMA1, DMA_CH1);
    
    /* configure DMA1 */
    dma_struct_para_init(&dma_para);
    dma_para.direction = DMA_MEMORY_TO_PERIPHERAL;
    dma_para.memory_addr = (uint32_t)buffer;
    dma_para.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_para.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_para.number = size;
    dma_para.periph_addr = MFCOM + 0x00000200U + 1*4;
    dma_para.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_para.periph_width = DMA_PERIPHERAL_WIDTH_32BIT;
    dma_para.priority = DMA_PRIORITY_LOW;
    dma_para.request = DMA_REQUESR_SSTAT1;
    dma_init(DMA1, DMA_CH1, &dma_para);
    dma_channel_enable(DMA1, DMA_CH1);
    
    mfcom_shifter_dma_enable(MFCOM_SHIFTER_1);

    /* usart receive */
    for(i=0; i<size; i++){
        while(RESET == usart_flag_get(USART0, USART_FLAG_RBNE));
        SRxBUF[i] = usart_data_receive(USART0);
    }

    /* wait for receive complete */
    while(dma_flag_get(DMA1,DMA_CH1, DMA_FLAG_FTF) == RESET){};
    dma_deinit(DMA1, DMA_CH1);
    mfcom_shifter_dma_disable(MFCOM_SHIFTER_1);
}

void mfcom_usart_receive(uint8_t * buffer, uint32_t size)
{
    uint32_t i;
    dma_parameter_struct dma_para;
    rcu_periph_clock_enable(RCU_DMA0);
    rcu_periph_clock_enable(RCU_DMA1);
    rcu_periph_clock_enable(RCU_DMAMUX);
    dma_deinit(DMA0, DMA_CH5);

    /* configure DMA0 */
    dma_struct_para_init(&dma_para);
    dma_para.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_para.memory_addr = (uint32_t)buffer;
    dma_para.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_para.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_para.number = size;
    dma_para.periph_addr = MFCOM + 0x00000300U + 3*4;
    dma_para.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_para.periph_width = DMA_PERIPHERAL_WIDTH_32BIT;
    dma_para.priority = DMA_PRIORITY_LOW;
    dma_para.request = DMA_REQUESR_SSTAT3;
    dma_init(DMA0, DMA_CH5, &dma_para);
    dma_channel_enable(DMA0, DMA_CH5);
    mfcom_shifter_dma_enable(MFCOM_SHIFTER_3);

    /* usart send */
    for(i=0; i<size; i++){
        usart_data_transmit(USART0, (uint8_t)(STxBUF[i]));
        while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
    }

    /* wait for transmit complete */
    while(dma_flag_get(DMA0, DMA_CH5, DMA_FLAG_FTF) == RESET){};
    dma_deinit(DMA0, DMA_CH5);
    mfcom_shifter_dma_disable(MFCOM_SHIFTER_3);
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