/*!
    \file    main.c
    \brief   TIMERs parallel synchro demo for gd32a50x

    \version 2023-06-20, V1.1.0, firmware for GD32A50x
*/

/*
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

/* configure the GPIO ports */
void gpio_config(void);
/* configure the TIMER peripheral */
void timer_config(void);
/* configure the TRIGSEL peripheral */
void trigsel_config(void);

/*!
    \brief      configure the GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOE);

    /*configure PE6(TIMER1 CH0) as alternate function*/
    gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
    gpio_af_set(GPIOE, GPIO_AF_1, GPIO_PIN_6);

    /*configure PB0(TIMER19 CH0) as alternate function*/
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_0);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
    gpio_af_set(GPIOB, GPIO_AF_1, GPIO_PIN_0);

    /*configure PA15(TIMER20 CH0) as alternate function*/
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_15);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_15);
}

/*!
    \brief      configure the TRIGSEL peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void trigsel_config(void)
{
    /* enable TRIGSEL clock */
    rcu_periph_clock_enable(RCU_TRIGSEL);
    /* select TIMER1_TRGO to trigger TIMER19_ITI0 */
    trigsel_init(TRIGSEL_OUTPUT_TIMER19_ITI0, TRIGSEL_INPUT_TIMER1_TRGO);
    /* select TIMER1_TRGO to trigger TIMER20_ITI0 */
    trigsel_init(TRIGSEL_OUTPUT_TIMER20_ITI0, TRIGSEL_INPUT_TIMER1_TRGO);

    /* lock trigger register */
    trigsel_register_lock_set(TRIGSEL_OUTPUT_TIMER19_ITI0);
    trigsel_register_lock_set(TRIGSEL_OUTPUT_TIMER20_ITI0);
}

/*!
    \brief      configure the TIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer_config(void)
{
    /* timers synchronisation in parallel mode ----------------------------
       1/TIMER1 is configured as master timer:
       - PWM mode is used.
       - The TIMER1 enable event is used as trigger output.

       2/TIMER19 is slave for TIMER1,
       - PWM mode is used.
       - The ITR0(TIMER1) is used as input trigger.

       3/TIMER20 is slave for TIMER1,
       - PWM mode is used.
       - The ITR0(TIMER1) is used as input trigger.
      -------------------------------------------------------------------- */
    timer_oc_parameter_struct timer_ocinitpara;
    timer_parameter_struct timer_initpara;

    /* enable the peripherals clock */
    rcu_periph_clock_enable(RCU_TIMER1);
    rcu_periph_clock_enable(RCU_TIMER19);
    rcu_periph_clock_enable(RCU_TIMER20);

    /* TIMER1 configuration */
    timer_deinit(TIMER1);
    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_initpara);
    timer_initpara.prescaler         = 4999;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 3999;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER1, &timer_initpara);

    /* initialize TIMER channel output parameter struct */
    timer_channel_output_struct_para_init(&timer_ocinitpara);
    /* CH0 configuration in PWM0 mode */
    timer_ocinitpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocinitpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocinitpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocinitpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocinitpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocinitpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    timer_channel_output_config(TIMER1, TIMER_CH_0, &timer_ocinitpara);

    /* configure TIMER channel output pulse value */
    timer_channel_output_pulse_value_config(TIMER1, TIMER_CH_0, 2000);
    /* CH0 configuration in PWM0 mode */
    timer_channel_output_mode_config(TIMER1, TIMER_CH_0, TIMER_OC_MODE_PWM0);
    /* configure TIMER channel output shadow function */
    timer_channel_output_shadow_config(TIMER1, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER1);
    /* select the master slave mode */
    timer_master_slave_mode_config(TIMER1, TIMER_MASTER_SLAVE_MODE_ENABLE);
    /* TIMER1 update event is used as trigger output */
    timer_master_output_trigger_source_select(TIMER1, TIMER_TRI_OUT_SRC_ENABLE);

    /* TIMER19 configuration */
    timer_deinit(TIMER19);
    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_initpara);
    timer_initpara.prescaler         = 4999;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 3999;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER19, &timer_initpara);

    /* initialize TIMER channel output parameter struct */
    timer_channel_output_struct_para_init(&timer_ocinitpara);
    /* configure TIMER channel output function */
    timer_ocinitpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocinitpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocinitpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocinitpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocinitpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocinitpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    timer_channel_output_config(TIMER19, TIMER_CH_0, &timer_ocinitpara);

    /* configure TIMER channel output pulse value */
    timer_channel_output_pulse_value_config(TIMER19, TIMER_CH_0, 2000);
    /* CH0 configuration in PWM mode 0 */
    timer_channel_output_mode_config(TIMER19, TIMER_CH_0, TIMER_OC_MODE_PWM0);
    /* configure TIMER channel output shadow function */
    timer_channel_output_shadow_config(TIMER19, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER19);
    /* TIMER19 output enable */
    timer_primary_output_config(TIMER19, ENABLE);
    /* slave mode selection: external clock mode 0 */
    timer_slave_mode_select(TIMER19, TIMER_SLAVE_MODE_EVENT);
    /* select TIMER input trigger source: internal trigger 0(ITI0) */
    timer_input_trigger_source_select(TIMER19, TIMER_SMCFG_TRGSEL_ITI0);
    /* select the master slave mode */
    timer_master_slave_mode_config(TIMER19, TIMER_MASTER_SLAVE_MODE_ENABLE);

    /* TIMER20 configuration */
    timer_deinit(TIMER20);
    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_initpara);
    timer_initpara.prescaler         = 4999;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 3999;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER20, &timer_initpara);

    /* initialize TIMER channel output parameter struct */
    timer_channel_output_struct_para_init(&timer_ocinitpara);
    /* configure TIMER channel output function */
    timer_ocinitpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocinitpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocinitpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocinitpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocinitpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocinitpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    timer_channel_output_config(TIMER20, TIMER_CH_0, &timer_ocinitpara);

    /* configure TIMER channel output pulse value */
    timer_channel_output_pulse_value_config(TIMER20, TIMER_CH_0, 2000);
    /* CH0 configuration in PWM0 mode */
    timer_channel_output_mode_config(TIMER20, TIMER_CH_0, TIMER_OC_MODE_PWM0);
    /* configure TIMER channel output shadow function */
    timer_channel_output_shadow_config(TIMER20, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER20);
    /* TIMER20 output enable */
    timer_primary_output_config(TIMER20, ENABLE);
    /* slave mode selection: external clock mode 0 */
    timer_slave_mode_select(TIMER20, TIMER_SLAVE_MODE_EVENT);
    /* select TIMER input trigger source: internal trigger 0(ITI0) */
    timer_input_trigger_source_select(TIMER20, TIMER_SMCFG_TRGSEL_ITI0);
    /* select the master slave mode */
    timer_master_slave_mode_config(TIMER20, TIMER_MASTER_SLAVE_MODE_ENABLE);

    /* TIMER counter enable */
    timer_enable(TIMER1);
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* configure the GPIO ports */
    gpio_config();
    /* configure the TRIGSEL peripheral */
    trigsel_config();
    /* configure the TIMER peripheral */
    timer_config();

    while(1);
}
