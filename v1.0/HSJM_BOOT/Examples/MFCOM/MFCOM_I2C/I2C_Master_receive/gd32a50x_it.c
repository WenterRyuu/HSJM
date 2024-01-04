/*!
    \file    gd32a50x_it.c
    \brief   interrupt service routines

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

#include "gd32a50x_it.h"
#include "systick.h"

extern uint8_t MTxBUF[8];
extern uint8_t MRxBUF[8];
extern uint8_t STxBUF[8];

extern uint8_t status;
extern uint8_t txRemainingBytes;
extern uint8_t rxRemainingBytes;
extern uint8_t receivemode;
extern uint8_t addrReceived;
extern uint8_t reload;
extern uint8_t mtx;
extern uint8_t mrx;
extern uint8_t lastreload;

extern void mfcom_i2c_start(uint8_t * buffer, uint8_t address, uint8_t size, uint8_t receiveornot);
extern void mfcom_i2c_stop(void);
/*!
    \brief      this function handles NMI exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void NMI_Handler(void)
{
}

/*!
    \brief      this function handles HardFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void HardFault_Handler(void)
{
    /* if Hard Fault exception occurs, go to infinite loop */
    while(1){
    }
}

/*!
    \brief      this function handles SVC exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SVC_Handler(void)
{
}

/*!
    \brief      this function handles PendSV exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void PendSV_Handler(void)
{
}

/*!
    \brief      this function handles SysTick exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SysTick_Handler(void)
{
    delay_decrement();
}

/*!
    \brief      this function handles MFCOM exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void MFCOM_IRQHandler(void)
{
    if(mfcom_shifter_error_interrupt_flag_get(MFCOM_SHIFTER_0) != RESET){
        mfcom_shifter_error_flag_clear(MFCOM_SHIFTER_0);
        /* underrun */
        status = 1; 
    }

    if(mfcom_shifter_error_interrupt_flag_get(MFCOM_SHIFTER_1) != RESET){
        /* device limitation: not possible to tell the difference between NACK and receive overflow */
        /*
         *  If there is a Tx event active it is an indication that module was not 
         *  serviced for a long time - chances are this is an overflow.
         *  It is not certain, and it is even possible to have both NACK and overflow,
         *  but there is no way to tell, so we chose the safe option (if it is an 
         *  overflow and we abort the transfer we may block the I2C bus).
        */
            /* receive nack */
            status = 2;
            mfcom_i2c_stop();
    }
    /* receive data handle */
    if(mfcom_shifter_interrupt_flag_get(MFCOM_SHIFTER_1) != RESET){
        uint8_t data;
        /* read data from rx shifter */
        data = (uint8_t)mfcom_buffer_read(MFCOM_SHIFTER_1, MFCOM_RWMODE_BITSWAP);

        if(addrReceived == 0){
            addrReceived = 1;
            if(receivemode == 1){
                mfcom_shifter_stopbit_set(MFCOM_SHIFTER_0, MFCOM_SHIFTER_STOPBIT_LOW);//send ack
            }
        }
        else{
            rxRemainingBytes--;
            if (receivemode == 1){
                MRxBUF[mrx++] = data;
            }
        }
        
        if((receivemode == 1) && (rxRemainingBytes == 1U)){
            /* send NACK for last byte */
            mfcom_shifter_stopbit_set(MFCOM_SHIFTER_0, MFCOM_SHIFTER_STOPBIT_HIGH);
            /* also instruct rx shifter to expect NACK */
            mfcom_shifter_stopbit_set(MFCOM_SHIFTER_1, MFCOM_SHIFTER_STOPBIT_HIGH);
        }
    }

    /* transmit data handle */
    if(mfcom_shifter_interrupt_flag_get(MFCOM_SHIFTER_0) != RESET){
        uint32_t data;
        txRemainingBytes--;

        if(txRemainingBytes == 0U){
            /* transmit stop condition */
            data = 0x0;
            mtx = 0;
        }
        else if(receivemode){
            data = 0xFFU;
        }
        else{
            data = MTxBUF[mtx];
            mtx++;
        }

        mfcom_buffer_write(MFCOM_SHIFTER_0, data, MFCOM_RWMODE_BITBYTESWAP);
    
        if(txRemainingBytes == 0U){
            mfcom_shifter_interrupt_disable(MFCOM_SHIFTER_0);
        }
    }

    /* check if the transfer is over */
    if (mfcom_timer_interrupt_flag_get(MFCOM_TIMER_0)){
        mfcom_timer_flag_clear(MFCOM_TIMER_0);
        reload--;

        if(reload == 2U){
            mfcom_timer_cmpvalue_set(MFCOM_TIMER_0, (mfcom_timer_cmpvalue_get(MFCOM_TIMER_0) & 0xFF)|((lastreload & 0xFF) << 8));
        }
        if(reload == 1U){
            mfcom_timer_dismode_set(MFCOM_TIMER_0, MFCOM_TIMER_DISMODE_COMPARE);
        }
        if(reload == 0U){
            mfcom_timer_dismode_set(MFCOM_TIMER_0, MFCOM_TIMER_DISMODE_NEVER);
            /* success */
            status = 4;
            /* end transfer. In case of race condition between Tx_shifter and timer_end events, 
               it is possible for the clock to be restarted. so we use forced stop to avoid this. */
            mfcom_i2c_stop();
        }
    }
}
