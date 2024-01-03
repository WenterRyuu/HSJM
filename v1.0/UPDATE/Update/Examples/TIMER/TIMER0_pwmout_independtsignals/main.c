/*!
    \file    main.c
    \brief   TIMER0 pwmout independent signals demo for gd32a50x

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
#include <stdio.h>

void gpio_config(void);
void timer_config(void);

/*!
    \brief      configure the GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOC);

    /*configure PC8/PC7(TIMER0 CH0/MCH0) as alternate function*/
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_7 | GPIO_PIN_8);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7 | GPIO_PIN_8);
    gpio_af_set(GPIOC, GPIO_AF_1, GPIO_PIN_7 | GPIO_PIN_8);

    /*configure PA4/PA3(TIMER0 CH1/MCH1) as alternate function*/
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3 | GPIO_PIN_4);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3 | GPIO_PIN_4);
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_3 | GPIO_PIN_4);
}

/*!
    \brief      configure the TIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer_config(void)
{
    /* -----------------------------------------------------------------------
        TIMER0 configuration to:
        generate 2 pair PWM signals with 4 different duty cycles:
        - TIMER0CLK is fixed to systemcoreclock, the TIMER0 prescaler is equal
          to 5000 so the TIMER0 counter clock used is 20KHz.
        - the TIMER0_MCH0 is independent of TIMER0_CH0
        - the TIMER0_MCH1 is independent of TIMER0_CH1
        - the four duty cycles are computed as the following description:
           the channel 0 duty cycle is set to 20%
           the multi mode channel 0 duty cycle is set to 60%.
           the channel 1 duty cycle is set to 40%
           the multi mode channel 1 duty cycle is set to 80%.
      ----------------------------------------------------------------------- */
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;
    timer_omc_parameter_struct timer_omcpara;

    rcu_periph_clock_enable(RCU_TIMER0);

    timer_deinit(TIMER0);
    /* TIMER0 configuration */
    timer_struct_para_init(&timer_initpara);
    timer_initpara.prescaler         = 4999;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 9999;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER0, &timer_initpara);

    /* CH0/CH1 configuration in PWM mode 0 */
    timer_channel_output_struct_para_init(&timer_ocintpara);
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_ENABLE;
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    timer_channel_output_config(TIMER0, TIMER_CH_0, &timer_ocintpara);
    timer_channel_output_config(TIMER0, TIMER_CH_1, &timer_ocintpara);

    /* CH0 configuration in PWM mode 0 */
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, 2000);
    timer_channel_output_mode_config(TIMER0, TIMER_CH_0, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);
    /* CH1 configuration in PWM mode 0 */
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_1, 4000);
    timer_channel_output_mode_config(TIMER0, TIMER_CH_1, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_1, TIMER_OC_SHADOW_DISABLE);

    /* MCH0/MCH1 configuration in independently mode */
    timer_multi_mode_channel_output_parameter_struct_init(&timer_omcpara);
    timer_omcpara.outputmode   = TIMER_MCH_MODE_INDEPENDENTLY;
    timer_omcpara.outputstate  = TIMER_MCCX_ENABLE;
    timer_omcpara.ocpolarity   = TIMER_OMC_POLARITY_HIGH;
    timer_multi_mode_channel_output_config(TIMER0, TIMER_MCH_0, &timer_omcpara);
    timer_multi_mode_channel_output_config(TIMER0, TIMER_MCH_1, &timer_omcpara);
    /* MCH0 configuration in PWM mode 0 */
    timer_channel_output_pulse_value_config(TIMER0, TIMER_MCH_0, 6000);
    timer_channel_output_mode_config(TIMER0, TIMER_MCH_0, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0, TIMER_MCH_0, TIMER_OMC_SHADOW_DISABLE);
    /* MCH1 configuration in PWM mode 0 */
    timer_channel_output_pulse_value_config(TIMER0, TIMER_MCH_1, 8000);
    timer_channel_output_mode_config(TIMER0, TIMER_MCH_1, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0, TIMER_MCH_1, TIMER_OMC_SHADOW_DISABLE);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER0);
    timer_primary_output_config(TIMER0, ENABLE);
    timer_enable(TIMER0);
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    gpio_config();
    timer_config();

    while(1);
}
