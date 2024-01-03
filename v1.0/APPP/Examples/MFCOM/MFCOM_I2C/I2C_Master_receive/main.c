/*!
    \file    main.c
    \brief   MFCOM_I2C master receive demo

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
#include "gd32a503v_eval.h"
#include <stdio.h>

#define ARRAYSIZE      8

uint8_t MTxBUF[8] = {0x78,0x89,0x9A,0xAB,0xBC,0xCD,0xDE,0xEF};
uint8_t MRxBUF[8] = {0};
const uint8_t STxBUF[8] = {0x98,0x87,0x76,0x65,0x54,0x43,0x32,0x21};
uint8_t mtx = 0;
uint8_t mrx = 0;
uint8_t stx = 0;
uint8_t status = 0;
uint8_t reload = 0;
uint8_t lastreload = 0;
    
uint8_t txRemainingBytes = 0;
uint8_t rxRemainingBytes = 0;
uint8_t receivemode = 0;
uint8_t addrReceived = 0;

void i2c_init(void);
void mfcom_i2c_init(void);
void mfcom_i2c_start(uint8_t * buffer, uint8_t address, uint8_t size, uint8_t receiveornot);
void mfcom_i2c_stop(void);
ErrStatus memory_compare(uint8_t *src, uint8_t *dst, uint8_t length);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* configure LED */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    
    /* MFCOM gpio config */
    rcu_periph_clock_enable(RCU_GPIOE);
    gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_2 | GPIO_PIN_3);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_2 | GPIO_PIN_3);
    gpio_af_set(GPIOE, GPIO_AF_6, GPIO_PIN_2 | GPIO_PIN_3);

    /* initialize i2c0 */
    i2c_init();
    /* initialize mfcom work in i2c master receive mode */
    mfcom_i2c_init();
    mfcom_i2c_start(MRxBUF, 0x23, 8, 1);

    /* set up mfcom_i2c receive data from i2c0 */
    i2c_data_transmit(I2C0, STxBUF[0]);
    while(!(I2C_STAT(I2C0) & I2C_FLAG_ADDSEND)){};
    I2C_STATC(I2C0) |= I2C_FLAG_ADDSEND;

    /* send data from the slave */
    for(stx = 1; stx < 8; stx++){
        while(!(I2C_STAT(I2C0) & I2C_FLAG_TI));
        i2c_data_transmit(I2C0, STxBUF[stx]);
    }

    /* determine the data transmission status */
    while(!(I2C_STAT(I2C0) & I2C_FLAG_STPDET)){};
    I2C_STATC(I2C0) |= I2C_FLAG_STPDET;

    while(status != 4){};

    /* transfer completed, check the data */
    mfcom_i2c_stop();

    /* compare receive data with send data */
    if(ERROR != memory_compare(MRxBUF, (uint8_t *)STxBUF, ARRAYSIZE)){
        gd_eval_led_on(LED1);
        gd_eval_led_on(LED2);
    }else{
        gd_eval_led_off(LED1);
        gd_eval_led_off(LED2);
    }

    while(1){
    }
}

/*!
    \brief      initialize mfcom to work in i2c master receive mode 
    \param[in]  none
    \param[out] none
    \retval     none
*/
void mfcom_i2c_init(void)
{
    mfcom_timer_parameter_struct mfcom_timer;
    mfcom_shifter_parameter_struct mfcom_shifter;

    /* enable mfcom interrupt */
    rcu_periph_clock_enable(RCU_MFCOM);
    NVIC_EnableIRQ(MFCOM_IRQn);
    
    /* configure the timer 0 is used to config baudrate */
    mfcom_timer.trigger_select     = MFCOM_TIMER_TRGSEL_SHIFTER0;
    mfcom_timer.trigger_polarity   = MFCOM_TIMER_TRGPOL_ACTIVE_LOW;
    mfcom_timer.pin_config         = MFCOM_TIMER_PINCFG_OPENDRAIN;
    mfcom_timer.pin_select         = MFCOM_TIMER_PINSEL_PIN2;
    mfcom_timer.pin_polarity       = MFCOM_TIMER_PINPOL_ACTIVE_HIGH;
    mfcom_timer.mode               = MFCOM_TIMER_DISABLE;
    mfcom_timer.output             = MFCOM_TIMER_OUT_LOW_EN;
    mfcom_timer.decrement          = MFCOM_TIMER_DEC_CLK_SHIFT_OUT;
    mfcom_timer.reset              = MFCOM_TIMER_RESET_PIN_TIMOUT;
    mfcom_timer.disable            = MFCOM_TIMER_DISMODE_NEVER;
    mfcom_timer.enable             = MFCOM_TIMER_ENMODE_TRIGHIGH;
    mfcom_timer.stopbit            = MFCOM_TIMER_STOPBIT_TIMDIS;
    mfcom_timer.startbit           = MFCOM_TIMER_STARTBIT_ENABLE;
    mfcom_timer.compare            = 0x00FF;
    mfcom_timer_init(MFCOM_TIMER_0, &mfcom_timer);
    
    /* configure the timer 1 for controlling shifters. */
    mfcom_timer.trigger_select     = MFCOM_TIMER_TRGSEL_PIN0;
    mfcom_timer.trigger_polarity   = MFCOM_TIMER_TRGPOL_ACTIVE_HIGH;
    mfcom_timer.pin_config         = MFCOM_TIMER_PINCFG_INPUT;
    mfcom_timer.pin_select         = MFCOM_TIMER_PINSEL_PIN2;
    mfcom_timer.pin_polarity       = MFCOM_TIMER_PINPOL_ACTIVE_LOW;
    mfcom_timer.mode               = MFCOM_TIMER_DISABLE;
    mfcom_timer.output             = MFCOM_TIMER_OUT_HIGH_EN;
    mfcom_timer.decrement          = MFCOM_TIMER_DEC_PIN_SHIFT_PIN;
    mfcom_timer.reset              = MFCOM_TIMER_RESET_NEVER;
    mfcom_timer.disable            = MFCOM_TIMER_DISMODE_PRE_TIMDIS;
    mfcom_timer.enable             = MFCOM_TIMER_ENMODE_PRE_TIMEN;
    mfcom_timer.stopbit            = MFCOM_TIMER_STOPBIT_TIMCMP;
    mfcom_timer.startbit           = MFCOM_TIMER_STARTBIT_ENABLE;
    mfcom_timer.compare            = 0xF;
    mfcom_timer_init(MFCOM_TIMER_1, &mfcom_timer);

    /* configure the shifter 0 for transfer data */
    mfcom_shifter.timer_select    = MFCOM_SHIFTER_TIMER1;
    mfcom_shifter.timer_polarity  = MFCOM_SHIFTER_TIMPOL_ACTIVE_HIGH;
    mfcom_shifter.pin_config      = MFCOM_SHIFTER_PINCFG_INPUT;
    mfcom_shifter.pin_select      = MFCOM_SHIFTER_PINSEL_PIN3;
    mfcom_shifter.pin_polarity    = MFCOM_SHIFTER_PINPOL_ACTIVE_LOW;
    mfcom_shifter.mode            = MFCOM_SHIFTER_DISABLE;
    mfcom_shifter.input_source    = MFCOM_SHIFTER_INSRC_PIN;
    mfcom_shifter.stopbit         = MFCOM_SHIFTER_STOPBIT_HIGH;
    mfcom_shifter.startbit        = MFCOM_SHIFTER_STARTBIT_LOW;
    mfcom_shifter_init(MFCOM_SHIFTER_0, &mfcom_shifter);

    /* configure the shifter 1 for receive data */
    mfcom_shifter.timer_select    = MFCOM_SHIFTER_TIMER1;
    mfcom_shifter.timer_polarity  = MFCOM_SHIFTER_TIMPOL_ACTIVE_LOW;
    mfcom_shifter.pin_config      = MFCOM_SHIFTER_PINCFG_INPUT;
    mfcom_shifter.pin_select      = MFCOM_SHIFTER_PINSEL_PIN3;
    mfcom_shifter.pin_polarity    = MFCOM_SHIFTER_PINPOL_ACTIVE_HIGH;
    mfcom_shifter.mode            = MFCOM_SHIFTER_DISABLE;
    mfcom_shifter.input_source    = MFCOM_SHIFTER_INSRC_PIN;
    mfcom_shifter.stopbit         = MFCOM_SHIFTER_STOPBIT_LOW;
    mfcom_shifter.startbit        = MFCOM_SHIFTER_STARTBIT_DISABLE;
    mfcom_shifter_init(MFCOM_SHIFTER_1, &mfcom_shifter);

    mfcom_enable();
}

/*!
    \brief      configure mfcom_i2c start condition
    \param[in]  none
    \param[out] none
    \retval     none
*/
void mfcom_i2c_start(uint8_t * buffer, uint8_t address, uint8_t size, uint8_t receiveornot)
{
    uint8_t edge;
    uint8_t counter;
    rxRemainingBytes = size;

    /* stop condition */
    txRemainingBytes = size + 1;
    receivemode = receiveornot;
    addrReceived = 0;
    edge = txRemainingBytes * 18 + 2;
    reload = (edge + 255) / 256;
    counter = (edge + (reload - 1)) / reload;
    lastreload = edge - (reload - 1) * counter - 1;

    /* config and enable mfcom */
    mfcom_timer_cmpvalue_set(MFCOM_TIMER_0, (mfcom_timer_cmpvalue_get(MFCOM_TIMER_0) & 0xFF)|((counter - 1) << 8));
    if(reload == 1){
        mfcom_timer_dismode_set(MFCOM_TIMER_0, MFCOM_TIMER_DISMODE_COMPARE);
    }
    
    mfcom_shifter_enable(MFCOM_SHIFTER_0, MFCOM_SHIFTER_TRANSMIT);
    mfcom_shifter_enable(MFCOM_SHIFTER_1, MFCOM_SHIFTER_RECEIVE);
    mfcom_timer_enable(MFCOM_TIMER_0, MFCOM_TIMER_BAUDMODE);
    mfcom_timer_enable(MFCOM_TIMER_1, MFCOM_TIMER_16BITCOUNTER);
    mfcom_shifter_pin_config(MFCOM_SHIFTER_0, MFCOM_SHIFTER_PINCFG_OPENDRAIN);
    
    /* send address before interrupt enable */
    mfcom_buffer_write(MFCOM_SHIFTER_0, (address << 1 | receiveornot), MFCOM_RWMODE_BITBYTESWAP);
    mfcom_shifter_interrupt_enable(MFCOM_SHIFTER_0);
    mfcom_shifter_interrupt_enable(MFCOM_SHIFTER_1);
    mfcom_shifter_error_interrupt_enable(MFCOM_SHIFTER_0);
    mfcom_shifter_error_interrupt_enable(MFCOM_SHIFTER_1);
    
    /* it is no need when reload == 1 */
    mfcom_timer_interrupt_enable(MFCOM_TIMER_0);
    
    if(reload == 2){
        mfcom_timer_cmpvalue_set(MFCOM_TIMER_0, (mfcom_timer_cmpvalue_get(MFCOM_TIMER_0) & 0xFF)|((lastreload & 0xFF) << 8));
    }
}

/*!
    \brief      mfcom_i2c stop configure
    \param[in]  none
    \param[out] none
    \retval     none
*/
void mfcom_i2c_stop(void)
{
    /* disable timer and shifter */
    mfcom_shifter_pin_config(MFCOM_SHIFTER_0, MFCOM_SHIFTER_PINCFG_INPUT);
    mfcom_shifter_disable(MFCOM_SHIFTER_0);
    mfcom_shifter_disable(MFCOM_SHIFTER_1);
    mfcom_timer_disable(MFCOM_TIMER_0);
    mfcom_timer_disable(MFCOM_TIMER_1);

    /* clear shifter and timer flag */
    mfcom_shifter_error_flag_clear(MFCOM_SHIFTER_0);
    mfcom_shifter_error_flag_clear(MFCOM_SHIFTER_1);
    mfcom_shifter_flag_clear(MFCOM_SHIFTER_1);
    mfcom_timer_flag_clear(MFCOM_TIMER_0);
    
    /* set the shifter stopbit */
    mfcom_shifter_stopbit_set(MFCOM_SHIFTER_1, MFCOM_SHIFTER_STOPBIT_LOW);
    mfcom_shifter_stopbit_set(MFCOM_SHIFTER_0, MFCOM_SHIFTER_STOPBIT_HIGH);
    
    /* disable all shifter and timer interrupt */
    mfcom_shifter_interrupt_disable(MFCOM_SHIFTER_0);
    mfcom_shifter_interrupt_disable(MFCOM_SHIFTER_1);
    mfcom_shifter_error_interrupt_disable(MFCOM_SHIFTER_0);
    mfcom_shifter_error_interrupt_disable(MFCOM_SHIFTER_1);
    mfcom_timer_interrupt_disable(MFCOM_TIMER_0); 
}

/*!
    \brief      initialize the I2C0
    \param[in]  none
    \param[out] none
    \retval     none
*/
void i2c_init(void)
{
    /* enable GPIOA/I2C0 clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_I2C0);

    /* configure the I2C0 GPIO ports */
    gpio_af_set(GPIOA, GPIO_AF_3, GPIO_PIN_10);
    gpio_af_set(GPIOA, GPIO_AF_3, GPIO_PIN_11);
    /* configure GPIO pins of I2C0 */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_10);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_11);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_11);

    /* configure I2C address */
    i2c_address_config(I2C0, 0x46, I2C_ADDFORMAT_7BITS);
    /* enable I2C0 */
    i2c_enable(I2C0);
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
