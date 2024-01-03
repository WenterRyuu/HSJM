/*!
    \file    main.c
    \brief   CAN Pretended Networking mode

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
#include "systick.h"
#include "string.h"

const uint8_t tx_data[8] = {0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8};
uint8_t rx_data[8];

can_mailbox_descriptor_struct transmit_message;
can_mailbox_descriptor_struct receive_message;
FlagStatus can0_match_flag, can0_timeout_flag;
uint8_t i = 0;

void can_gpio_config(void);
void bsp_board_config(void);
void can_config(void);
void can_pn_config(void);
void communication_check(void);

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
    /* configure board */
    bsp_board_config();
    /* configure GPIO */
    can_gpio_config();
    /* initialize CAN and filter */
    can_config();

    printf("\r\nPretended Networking mode test CAN0, please press TAMPER key to start! \r\n");

    can_struct_para_init(CAN_MDSC_STRUCT, &transmit_message);
    can_struct_para_init(CAN_MDSC_STRUCT, &receive_message);
    /* initialize transmit message */
    transmit_message.rtr = 0;
    transmit_message.ide = 0;
    transmit_message.code = CAN_MB_TX_STATUS_DATA;
    transmit_message.brs = 0;
    transmit_message.fdf = 0;
    transmit_message.prio = 0;
    transmit_message.data_bytes = 8;
    /* tx message content */
    transmit_message.data = (uint32_t *)(tx_data);
    transmit_message.id = 0x55;

    while(1) {
        /* test whether the TAMPER key is pressed */
        if(0 == gd_eval_key_state_get(KEY_TAMPER)) {
            delay_1ms(100);
            if(0 == gd_eval_key_state_get(KEY_TAMPER)) {
                /* enter Pretended Networking mode */
                can_operation_mode_enter(CAN0, CAN_PN_MODE);

                /* waiting for the TAMPER key up */
                while(0 == gd_eval_key_state_get(KEY_TAMPER));
            }
        }

        /* test whether the WAKEUP key is pressed */
        if(0 == gd_eval_key_state_get(KEY_WAKEUP)) {
            delay_1ms(100);
            if(0 == gd_eval_key_state_get(KEY_WAKEUP)) {
                /* transmit message */
                can_mailbox_config(CAN1, 1, &transmit_message);

                printf("\r\nCAN1 transmit data: \r\n");
                for(i = 0; i < 8; i++) {
                    printf("%02x\r\n", tx_data[i]);
                }

                /* waiting for the WAKEUP key up */
                while(0 == gd_eval_key_state_get(KEY_WAKEUP));
            }
        }
        communication_check();
    }
}

/*!
    \brief      configure GPIO
    \param[in]  none
    \param[out] none
    \retval     none
*/
void can_gpio_config(void)
{
    /* enable CAN clock */
    rcu_can_clock_config(CAN0, RCU_CANSRC_HXTAL);
    rcu_can_clock_config(CAN1, RCU_CANSRC_HXTAL);
    rcu_periph_clock_enable(RCU_CAN0);
    rcu_periph_clock_enable(RCU_CAN1);
    /* enable CAN port clock */
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOD);

    /* configure CAN0 GPIO */
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_13);
    gpio_af_set(GPIOB, GPIO_AF_6, GPIO_PIN_13);

    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_14);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_14);
    gpio_af_set(GPIOB, GPIO_AF_6, GPIO_PIN_14);

    /* configure CAN1 GPIO */
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6);
    gpio_af_set(GPIOD, GPIO_AF_6, GPIO_PIN_6);

    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_7);
    gpio_af_set(GPIOD, GPIO_AF_6, GPIO_PIN_7);
}

/*!
    \brief      configure BSP
    \param[in]  none
    \param[out] none
    \retval     none
*/
void bsp_board_config(void)
{
    /* configure USART */
    gd_eval_com_init(EVAL_COM);
    /* clock enable */
    rcu_periph_clock_enable(RCU_PMU);

    /* configure WAKEUP key */
    gd_eval_key_init(KEY_WAKEUP, KEY_MODE_GPIO);
    /* configure TAMPER key */
    gd_eval_key_init(KEY_TAMPER, KEY_MODE_GPIO);

    /* configure LEDs */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);

    gd_eval_led_off(LED1);
    gd_eval_led_off(LED2);
}

/*!
    \brief      initialize CAN function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void can_config(void)
{
    can_parameter_struct can_parameter;

    /* initialize CAN register */
    can_deinit(CAN0);
    can_deinit(CAN1);
    /* initialize CAN */
    can_struct_para_init(CAN_INIT_STRUCT, &can_parameter);

    /* initialize CAN parameters */
    can_parameter.internal_counter_source = CAN_TIMER_SOURCE_BIT_CLOCK;
    can_parameter.self_reception = DISABLE;
    can_parameter.mb_tx_order = CAN_TX_HIGH_PRIORITY_MB_FIRST;
    can_parameter.mb_tx_abort_enable = ENABLE;
    can_parameter.local_priority_enable = DISABLE;
    can_parameter.mb_rx_ide_rtr_type = CAN_IDE_RTR_FILTERED;
    can_parameter.mb_remote_frame = CAN_STORE_REMOTE_REQUEST_FRAME;
    can_parameter.rx_private_filter_queue_enable = DISABLE;
    can_parameter.edge_filter_enable = DISABLE;
    can_parameter.protocol_exception_enable = DISABLE;
    can_parameter.rx_filter_order = CAN_RX_FILTER_ORDER_MAILBOX_FIRST;
    can_parameter.memory_size = CAN_MEMSIZE_32_UNIT;
    /* filter configuration */
    can_parameter.mb_public_filter = 0x0;
    can_parameter.resync_jump_width = 1;
    can_parameter.prop_time_segment = 2;
    can_parameter.time_segment_1 = 4;
    can_parameter.time_segment_2 = 3;
    /* 800Kbps */
    can_parameter.prescaler = 1;

    /* initialize CAN */
    can_init(CAN0, &can_parameter);
    can_init(CAN1, &can_parameter);

    /* configure Pretended Networking mode */
    can_pn_config();

    /* configure CAN0 NVIC */
    nvic_irq_enable(CAN0_WKUP_IRQn, 0, 0);

    /* EXTI configuration */
    exti_deinit();
    exti_init(EXTI_18, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    can_flag_clear(CAN0, CAN_FLAG_WAKEUP_MATCH);
    can_flag_clear(CAN0, CAN_FLAG_WAKEUP_TIMEOUT);
    exti_interrupt_flag_clear(EXTI_18);
    exti_interrupt_enable(EXTI_18);

    /* enable CAN WAKEUP interrupt */
    can_interrupt_enable(CAN0, CAN_INT_WAKEUP_MATCH);
    can_interrupt_enable(CAN0, CAN_INT_WAKEUP_TIMEOUT);

    can_operation_mode_enter(CAN1, CAN_NORMAL_MODE);
}

/*!
    \brief      initialize CAN Pretended Networking function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void can_pn_config(void)
{
    can_pn_mode_config_struct can_pn_parameter;
    can_pn_mode_filter_struct filter_parameter[2];

    /* initialize CAN Pretended Networking */
    can_struct_para_init(CAN_PN_MODE_INIT_STRUCT, &can_pn_parameter);
    /* initialize CAN Pretended Networking parameters */
    can_pn_parameter.timeout_int = ENABLE;
    can_pn_parameter.match_int = ENABLE;
    can_pn_parameter.num_matches = 1;
    can_pn_parameter.match_timeout = 0xFFFF;
    can_pn_parameter.frame_filter = CAN_PN_FRAME_FILTERING_ID;
    can_pn_parameter.id_filter = CAN_PN_ID_FILTERING_EXACT;

    /* initialize CAN Pretended Networking mode */
    can_pn_mode_config(CAN0, &can_pn_parameter);

    /* initialize CAN Pretended Networking filter */
    can_struct_para_init(CAN_PN_MODE_FILTER_STRUCT, &filter_parameter[0]);
    can_struct_para_init(CAN_PN_MODE_FILTER_STRUCT, &filter_parameter[1]);
    /* initialize CAN Pretended Networking filter parameters */
    filter_parameter[0].remote_frame = RESET;
    filter_parameter[0].extended_frame = RESET;
    filter_parameter[0].id = 0x55;
    filter_parameter[1].remote_frame = RESET;
    filter_parameter[1].extended_frame = RESET;
    filter_parameter[1].id = 0x1FFFFFFF;

    /* initialize CAN Pretended Networking filter */
    can_pn_mode_filter_config(CAN0, &filter_parameter[0], &filter_parameter[1]);
}

/*!
    \brief      check received data
    \param[in]  none
    \param[out] none
    \retval     none
*/
void communication_check(void)
{
    /* CAN0 wakeup match event */
    if(SET == can0_match_flag) {
        can0_match_flag = RESET;

        /* check the received message */
        receive_message.data = (uint32_t *)(rx_data);
        can_pn_mode_data_read(CAN0, 0, &receive_message);
        if(0 == memcmp(rx_data, tx_data, 8)) {
            printf("\r\nCAN0 receive data: \r\n");
            for(i = 0; i < 8; i++) {
                printf("%02x\r\n", rx_data[i]);
            }
            gd_eval_led_toggle(LED1);
        }
        can_pn_mode_exit(CAN0);
    }

    /* CAN0 wakeup timeout */
    if(SET == can0_timeout_flag) {
        can0_timeout_flag = RESET;
        gd_eval_led_toggle(LED2);
        can_pn_mode_exit(CAN0);
    }
}

/* retarget the C library printf function to the usart */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM, USART_FLAG_TBE));
    return ch;
}
