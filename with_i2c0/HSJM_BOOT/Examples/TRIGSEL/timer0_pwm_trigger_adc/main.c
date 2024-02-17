/*!
    \file    main.c
    \brief   TRIGSEL select TIMER0_CH2 as trigger source to trigger ADC example

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
#include "systick.h"
#include <stdio.h>
#include "gd32a503v_eval.h"

uint16_t adc_value[4] = {0};

/* configure GPIO peripheral */
void gpio_config(void);
void adc_config(void);
void timer_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* configure systick */
    systick_config();
    /* initilize USART */
    gd_eval_com_init(EVAL_COM);

    /* enable TRIGSEL clock */
    rcu_periph_clock_enable(RCU_TRIGSEL);
    /* select TIMER0_CH2 to trigger ADC0 */
    trigsel_init(TRIGSEL_OUTPUT_ADC0_RTTRG, TRIGSEL_INPUT_TIMER0_CH2);
    /* lock trigger register */
    trigsel_register_lock_set(TRIGSEL_OUTPUT_ADC0_RTTRG);

    /* configure GPIO */
    gpio_config();

    /* configure ADC */
    adc_config();

    /* configure TIMER */
    timer_config();

    while(1){
        delay_1ms(1000);
        printf("\n adc_value[0] = %08X \n", adc_value[0]);
        printf("\n adc_value[1] = %08X \n", adc_value[1]);
        printf("\n adc_value[2] = %08X \n", adc_value[2]);
        printf("\n adc_value[3] = %08X \n", adc_value[3]);
    }
}

/*!
    \brief      gpio configure function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOD);

    /* config the GPIO as analog mode */
    /* PD10-ADC0_IN2,   PD9-ADC0_IN3 */
    gpio_mode_set(GPIOD, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_10|GPIO_PIN_9);
    gpio_mode_set(GPIOE, GPIO_MODE_ANALOG,GPIO_PUPD_NONE,GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14);
}

/*!
    \brief      adc configure function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_config(void)
{
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC0);
    /* enable DMA clock */
    rcu_periph_clock_enable(RCU_DMA0);
    /* enable DMAMUX clock */
    rcu_periph_clock_enable(RCU_DMAMUX);

    /* configure ADC clock to 10M */    
    rcu_adc_clock_config(RCU_CKADC_CKAHB_DIV6);

    /* configure the GPIO as analog mode */
    gpio_mode_set(GPIOD, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_10 | GPIO_PIN_9);

    /* reset ADC */
    adc_deinit(ADC0);

    /* initialize ADC */
    adc_mode_config(ADC_MODE_FREE);
    /* ADC contineous function enable */
    adc_special_function_config(ADC0, ADC_SCAN_MODE, ENABLE);
    /* ADC trigger by TRGSEL */
    adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC0_1_EXTTRIG_REGULAR_TRIGSEL); 
    /* ADC data alignment config */
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
    /* ADC channel length config */
    adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, 4U);

    /* ADC temperature and Vrefint enable */
    adc_tempsensor_enable();
    adc_vrefint_enable();
    
    /* ADC regular channel config */
    adc_regular_channel_config(ADC0, 0U, ADC_CHANNEL_16, ADC_SAMPLETIME_55POINT5);
    adc_regular_channel_config(ADC0, 1U, ADC_CHANNEL_17, ADC_SAMPLETIME_55POINT5);
    adc_regular_channel_config(ADC0, 2U, ADC_CHANNEL_3,  ADC_SAMPLETIME_55POINT5); 
    adc_regular_channel_config(ADC0, 3U, ADC_CHANNEL_2,  ADC_SAMPLETIME_55POINT5);
    
    adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, ENABLE);

    /* ADC DMA configuration */
    /* ADC_DMA_channel configuration */
    dma_parameter_struct dma_data_parameter;

    /* ADC DMA_channel configuration */
    dma_deinit(DMA0, DMA_CH0);    
    /* initialize DMA single data mode */
    dma_data_parameter.request      = DMA_REQUEST_ADC;
    dma_data_parameter.periph_addr  = (uint32_t)(&ADC_RDATA(ADC0));
    dma_data_parameter.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_data_parameter.memory_addr  = (uint32_t)(&adc_value);
    dma_data_parameter.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_data_parameter.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_data_parameter.memory_width = DMA_MEMORY_WIDTH_16BIT;  
    dma_data_parameter.direction    = DMA_PERIPHERAL_TO_MEMORY;
    dma_data_parameter.number       = 4U;
    dma_data_parameter.priority     = DMA_PRIORITY_HIGH;
    dma_init(DMA0, DMA_CH0, &dma_data_parameter);

    dma_memory_to_memory_disable(DMA0, DMA_CH0);
    dma_circulation_enable(DMA0, DMA_CH0);
    /* enable DMA channel */
    dma_channel_enable(DMA0, DMA_CH0);
    /* ADC DMA function enable */
    adc_dma_mode_enable(ADC0);

    /* enable ADC interface */
    adc_enable(ADC0);

    delay_1ms(2);

    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC0);
}

/*!
    \brief      timer configure function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer_config(void)
{
    timer_parameter_struct timer_initpara;
    timer_oc_parameter_struct timer_ocinitpara;

    /* enable TIMER clock */
    rcu_periph_clock_enable(RCU_TIMER0);

    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_initpara);
    /* TIMER configuration */
    timer_initpara.prescaler         = 99;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 999;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    
    /* initialize TIMER channel output parameter struct */
    timer_channel_output_struct_para_init(&timer_ocinitpara);
    /* CH2 configuration in PWM0 mode */
    timer_ocinitpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocinitpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocinitpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocinitpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocinitpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocinitpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

    timer_deinit(TIMER0);

    timer_init(TIMER0, &timer_initpara);
    timer_channel_output_config(TIMER0, TIMER_CH_2, &timer_ocinitpara);

    /* configure TIMER channel output pulse value */
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_2, 499);
    /* CH2 configuration in PWM0 mode */
    timer_channel_output_mode_config(TIMER0, TIMER_CH_2, TIMER_OC_MODE_PWM0);
    /* configure TIMER channel output shadow function */
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_2, TIMER_OC_SHADOW_DISABLE);
    
    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER0);
    /* TIMER0 output enable */
    timer_primary_output_config(TIMER0, ENABLE);

    timer_enable(TIMER0);
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM, USART_FLAG_TBE));
    return ch;
}
