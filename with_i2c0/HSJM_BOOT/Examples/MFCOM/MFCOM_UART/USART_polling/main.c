/*!
    \file    main.c
    \brief   emulating uart using MFCOM

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

void mfcom_usart_init(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    uint8_t rxbufer[10];

    /* configure LED */
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED1);

    /* enable the D7~D0 GPIO clock */
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

    /* enable the MFCOM clock */
    rcu_periph_clock_enable(RCU_MFCOM);
    /* keep MFCOM timer counter when the mcu is in debug mode */
    dbg_periph_enable(DBG_MFCOM_HOLD);

    /* configure mfcom work in usart mode */
    mfcom_usart_init();
    mfcom_enable();

    /* wait the SSTAT bits set */
    while(RESET == mfcom_shifter_flag_get(MFCOM_SHIFTER_0));
    mfcom_buffer_write(MFCOM_SHIFTER_0, 0x45, MFCOM_RWMODE_BITBYTESWAP);
    while(RESET == mfcom_shifter_flag_get(MFCOM_SHIFTER_2));
    rxbufer[0] = mfcom_buffer_read(MFCOM_SHIFTER_2, MFCOM_RWMODE_BITSWAP);

    /* After the first data has been transmited , transmit second data */
    while(RESET == mfcom_shifter_flag_get(MFCOM_SHIFTER_0));
    mfcom_buffer_write(MFCOM_SHIFTER_0, 0x6D, MFCOM_RWMODE_BITBYTESWAP);
    while(RESET == mfcom_shifter_flag_get(MFCOM_SHIFTER_2));
    rxbufer[1] = mfcom_buffer_read(MFCOM_SHIFTER_2, MFCOM_RWMODE_BITSWAP);

     /* check the read data, the LED will be turn on when the data is correct,
        otherwise, the LED will be turn off */
    if((rxbufer[0]==0x45)&&(rxbufer[1]==0x6D)){
        gd_eval_led_on(LED2);
        gd_eval_led_on(LED1);
    }
    else{
        gd_eval_led_off(LED2);
        gd_eval_led_off(LED1);
    }

    /* If MFCOM_SBUF is overflow or shortage,the LED will be reset */
    if(MFCOM_SERR !=0){
        gd_eval_led_off(LED2);
        gd_eval_led_off(LED1);
    }

    while(1){
    }
}

/*!
    \brief      initialize mfcom to work in usart mode 
    \param[in]  none
    \param[out] none
    \retval     none
*/
void mfcom_usart_init()
{
    mfcom_timer_parameter_struct mfcom_timer;
    mfcom_shifter_parameter_struct mfcom_shifter;

    /* configure MFCOM_TIMER_0 */
    mfcom_timer.trigger_select       = MFCOM_TIMER_TRGSEL_SHIFTER0;
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
    mfcom_timer.compare              = 0x0F33;
    mfcom_timer_init(MFCOM_TIMER_0,&mfcom_timer);

    /* configure MFCOM_TIMER_2 */
    mfcom_timer.trigger_select       = MFCOM_TIMER_TRGSEL_EXTERNAL0;
    mfcom_timer.trigger_polarity     = MFCOM_TIMER_TRGPOL_ACTIVE_HIGH;
    mfcom_timer.pin_config           = MFCOM_TIMER_PINCFG_INPUT;
    mfcom_timer.pin_select           = MFCOM_TIMER_PINSEL_PIN1;
    mfcom_timer.pin_polarity         = MFCOM_TIMER_PINPOL_ACTIVE_LOW;
    mfcom_timer.mode                 = MFCOM_TIMER_BAUDMODE;
    mfcom_timer.output               = MFCOM_TIMER_OUT_HIGH_EN_RESET;
    mfcom_timer.decrement            = MFCOM_TIMER_DEC_CLK_SHIFT_OUT;
    mfcom_timer.reset                = MFCOM_TIMER_RESET_PIN_RISING;
    mfcom_timer.disable              = MFCOM_TIMER_DISMODE_COMPARE;
    mfcom_timer.enable               = MFCOM_TIMER_ENMODE_PINRISING;
    mfcom_timer.stopbit              = MFCOM_TIMER_STOPBIT_TIMDIS;
    mfcom_timer.startbit             = MFCOM_TIMER_STARTBIT_ENABLE;
    mfcom_timer.compare              = 0x0F32;
    mfcom_timer_init(MFCOM_TIMER_2,&mfcom_timer);

    /* configure MFCOM_SHIFTER_0 */
    mfcom_shifter.timer_select       = MFCOM_SHIFTER_TIMER0;
    mfcom_shifter.timer_polarity     = MFCOM_SHIFTER_TIMPOL_ACTIVE_HIGH;
    mfcom_shifter.pin_config         = MFCOM_SHIFTER_PINCFG_OUTPUT;
    mfcom_shifter.pin_select         = MFCOM_SHIFTER_PINSEL_PIN0;
    mfcom_shifter.pin_polarity       = MFCOM_SHIFTER_PINPOL_ACTIVE_HIGH;
    mfcom_shifter.mode               = MFCOM_SHIFTER_TRANSMIT;
    mfcom_shifter.input_source       = MFCOM_SHIFTER_INSRC_PIN;
    mfcom_shifter.stopbit            = MFCOM_SHIFTER_STOPBIT_HIGH;
    mfcom_shifter.startbit           = MFCOM_SHIFTER_STARTBIT_LOW;
    mfcom_shifter_init(MFCOM_SHIFTER_0,&mfcom_shifter);

    /* configure MFCOM_SHIFTER_2 */
    mfcom_shifter.timer_select       = MFCOM_SHIFTER_TIMER2;
    mfcom_shifter.timer_polarity     = MFCOM_SHIFTER_TIMPOL_ACTIVE_LOW;
    mfcom_shifter.pin_config         = MFCOM_SHIFTER_PINCFG_INPUT;
    mfcom_shifter.pin_select         = MFCOM_SHIFTER_PINSEL_PIN1;
    mfcom_shifter.pin_polarity       = MFCOM_SHIFTER_PINPOL_ACTIVE_HIGH;
    mfcom_shifter.mode               = MFCOM_SHIFTER_RECEIVE;
    mfcom_shifter.input_source       = MFCOM_SHIFTER_INSRC_PIN;
    mfcom_shifter.stopbit            = MFCOM_SHIFTER_STOPBIT_HIGH;
    mfcom_shifter.startbit           = MFCOM_SHIFTER_STARTBIT_LOW;
    mfcom_shifter_init(MFCOM_SHIFTER_2,&mfcom_shifter);
}
