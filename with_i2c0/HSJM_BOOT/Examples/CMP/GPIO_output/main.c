/*!
    \file    main.c
    \brief   comparator GPIO output example

    \version 2023-06-20, V1.1.0, firmware for GD32A50x
*/

/*
    Copyright (c) 2021, GigaDevice Semiconductor Inc.

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
#include "systick.h"

void gpio_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    systick_config();
    /* initialize LED */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    /* turn off LED */
    gd_eval_led_off(LED1);
    gd_eval_led_off(LED2);

    /* configure comparator GPIO */
    gpio_config();

    /* enable CMP clock */
    rcu_periph_clock_enable(RCU_CMP);

    /* initialize comparator, since using 1/2VREFINT as inverting input, the voltage bridge and voltage scaler must be enable */
    cmp_disable();
    cmp_voltage_scaler_enable();
    cmp_scaler_bridge_enable();
    cmp_mode_init(CMP_HIGHSPEED, CMP_1_2VREFINT, CMP_IP_PA3, CMP_HYSTERESIS_NO);
    cmp_output_init(CMP_OUTPUT_NONE, CMP_OUTPUT_POLARITY_NOINVERTED);

    cmp_enable();
    delay_1ms(1);

    while(1) {
        if(CMP_OUTPUTLEVEL_HIGH == cmp_output_level_get()) {
            gd_eval_led_on(LED1);
            gd_eval_led_off(LED2);
        } else {
            gd_eval_led_off(LED1);
            gd_eval_led_on(LED2);
        }
    }
}

/*!
    \brief      configure GPIO for comparator input and output
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);

    /* configure comparator plus input: PA3 */
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_3);
}
