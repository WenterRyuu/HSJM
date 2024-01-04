/*!
    \file    main.c
    \brief   TIMER0 deadtime break demo for gd32a50x

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
    rcu_periph_clock_enable(RCU_GPIOD);

    /*configure PC8/PC7(TIMER0 CH0/MCH0) as alternate function*/
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_7 | GPIO_PIN_8);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7 | GPIO_PIN_8);
    gpio_af_set(GPIOC, GPIO_AF_1, GPIO_PIN_7 | GPIO_PIN_8);

    /*configure PA4/PA3(TIMER0 CH1/MCH1) as alternate function*/
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3 | GPIO_PIN_4);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3 | GPIO_PIN_4);
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_3 | GPIO_PIN_4);

    /*configure PD5(TIMER0 BRKIN0) as alternate function*/
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5);
    gpio_af_set(GPIOD, GPIO_AF_1, GPIO_PIN_5);

    /*configure PD10(TIMER0 BRKIN1) as alternate function*/
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_10);
    gpio_af_set(GPIOD, GPIO_AF_2, GPIO_PIN_10);
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
    TIMER0 configuration:
    - generate 1 complementary PWM signal.
    - TIMER0CLK is fixed to systemcoreclock, the TIMER0 prescaler is equal to
      100, so the TIMER0 counter clock used is 1MHz.
    - the duty cycle is computed as the following description:
      the CH0 duty cycle is set to 40% so MCH0 is set to 60%.
      the CH1 duty cycle is set to 80% so MCH1 is set to 20%.
    - the deadtime of CH0/MCH0 and CH1/MCH1 with different values.
    - configure the break feature, BRKIN0 and BRKIN1 active at high level, and
      using the automatic output enable feature.
    - use the locking parameters level0.
    ----------------------------------------------------------------------- */
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;
    timer_break_parameter_struct timer_breakpara;
    timer_free_complementary_parameter_struct freecompara;
    timer_break_ext_input_struct breakinpara;

    rcu_periph_clock_enable(RCU_TIMER0);

    timer_deinit(TIMER0);
    /* TIMER0 configuration */
    timer_initpara.prescaler         = 99;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 999;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV4;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER0, &timer_initpara);

    /* CH0/MCH0 and CH1/MCH1 configuration in PWM mode 0 */
    timer_channel_output_struct_para_init(&timer_ocintpara);
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_ENABLE;
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_HIGH;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    timer_channel_output_config(TIMER0, TIMER_CH_0, &timer_ocintpara);
    timer_channel_output_config(TIMER0, TIMER_CH_1, &timer_ocintpara);

    /* CH0/MCH0 configuration in PWM mode 0 */
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, 400);
    timer_channel_output_mode_config(TIMER0, TIMER_CH_0, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);
    /* CH1/MCH1 configuration in PWM mode 0 */
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_1, 800);
    timer_channel_output_mode_config(TIMER0, TIMER_CH_1, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_1, TIMER_OC_SHADOW_DISABLE);

    /* initialize TIMER break external input parameter struct */
    timer_break_external_input_struct_para_init(&breakinpara);
    breakinpara.filter   = 15;
    breakinpara.enable   = ENABLE;
    breakinpara.polarity = TIMER_BRKIN_POLARITY_HIGH;
    timer_break_external_input_config(TIMER0, TIMER_BREAKINPUT_BRK0, &breakinpara);
    timer_break_external_input_config(TIMER0, TIMER_BREAKINPUT_BRK1, &breakinpara);

    /* initialize TIMER CH0/CH1 free complementary parameter struct */
    timer_free_complementary_struct_para_init(&freecompara);
    freecompara.freecomstate = TIMER_FCCHP_STATE_ENABLE;
    freecompara.runoffstate  = TIMER_ROS_STATE_ENABLE;
    freecompara.ideloffstate = TIMER_IOS_STATE_ENABLE;
    freecompara.deadtime     = 164;
    timer_channel_free_complementary_config(TIMER0, TIMER_CH_0, &freecompara);
    freecompara.deadtime     = 200;
    timer_channel_free_complementary_config(TIMER0, TIMER_CH_1, &freecompara);

    /* configure the TIMER channel break function */
    timer_channel_break_control_config(TIMER0, TIMER_CH_0, ENABLE);
    timer_channel_break_control_config(TIMER0, TIMER_CH_1, ENABLE);
    /* configure the TIMER channel dead time function */
    timer_channel_dead_time_config(TIMER0, TIMER_CH_0, ENABLE);
    timer_channel_dead_time_config(TIMER0, TIMER_CH_1, ENABLE);

    /* automatic output enable, break, dead time and lock configuration */
    timer_break_struct_para_init(&timer_breakpara);
    timer_breakpara.runoffstate      = TIMER_ROS_STATE_DISABLE;
    timer_breakpara.ideloffstate     = TIMER_IOS_STATE_DISABLE ;
    timer_breakpara.deadtime         = 255;
    timer_breakpara.breakpolarity    = TIMER_BREAK_POLARITY_HIGH;
    timer_breakpara.outputautostate  = TIMER_OUTAUTO_ENABLE;
    timer_breakpara.protectmode      = TIMER_CCHP_PROT_0;
    timer_breakpara.breakstate       = TIMER_BREAK_ENABLE;
    timer_break_config(TIMER0, &timer_breakpara);

    /* TIMER0 primary output function enable */
    timer_primary_output_config(TIMER0, ENABLE);
    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER0);

    /* TIMER0 counter enable */
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
