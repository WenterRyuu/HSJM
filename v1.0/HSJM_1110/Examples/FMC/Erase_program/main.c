/*!
    \file    main.c
    \brief   main flash program, erase example

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

#include "gd32a503v_eval.h"

#define FMC_PAGE_SIZE                ((uint16_t)0x400U)
#define FMC_WRITE_START_ADDR         ((uint32_t)0x08004000U)
#define FMC_WRITE_END_ADDR           ((uint32_t)0x08006000U)

#define FMC_PROGRAM_TYPE_DWORD       ((uint8_t)0x00U)
#define FMC_PROGRAM_TYPE_FAST        ((uint8_t)0x01U)

uint32_t address = 0x00U;
uint64_t data0   = 0x0123456701234567U;
led_typedef_enum lednum = LED2;

/* calculate the number of page to be programmed/erased */
uint32_t page_num = (FMC_WRITE_END_ADDR - FMC_WRITE_START_ADDR) / FMC_PAGE_SIZE;
/* calculate double word number to be programmed */
uint32_t dword_num = ((FMC_WRITE_END_ADDR - FMC_WRITE_START_ADDR) >> 3);

/* data buffer for fast programming */
static uint64_t data_buffer[DOUBLEWORD_CNT_IN_ROW] = {
    0x0000000000000000U, 0x1111111111111111U, 0x2222222222222222U, 0x3333333333333333U,
    0x4444444444444444U, 0x5555555555555555U, 0x6666666666666666U, 0x7777777777777777U,
    0x8888888888888888U, 0x9999999999999999U, 0xAAAAAAAAAAAAAAAAU, 0xBBBBBBBBBBBBBBBBU,
    0xCCCCCCCCCCCCCCCCU, 0xDDDDDDDDDDDDDDDDU, 0xEEEEEEEEEEEEEEEEU, 0xFFFFFFFFFFFFFFFFU,
    0x0011001100110011U, 0x2233223322332233U, 0x4455445544554455U, 0x6677667766776677U,
    0x8899889988998899U, 0xAABBAABBAABBAABBU, 0xCCDDCCDDCCDDCCDDU, 0xEEFFEEFFEEFFEEFFU,
    0x2200220022002200U, 0x3311331133113311U, 0x6644664466446644U, 0x7755775577557755U,
    0xAA88AA88AA88AA88U, 0xBB99BB99BB99BB99U, 0xEECCEECCEECCEECCU, 0xFFDDFFDDFFDDFFDDU
};

/* clear pengding flags */
void fmc_flags_clear(void);
/* erase fmc pages from FMC_WRITE_START_ADDR to FMC_WRITE_END_ADDR */
void fmc_erase_pages(void);
/* check fmc erase result */
void fmc_erase_pages_check(void);
/* program fmc word by word from FMC_WRITE_START_ADDR to FMC_WRITE_END_ADDR */
void fmc_program(uint8_t program_type);
/* check fmc erase result */
void fmc_erase_pages_check(void);
/* check fmc program result */
void fmc_program_check(uint8_t program_type);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* initialize led on the board */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);

    /* step1: erase pages and check if it is successful. If not, light the LED1. */
    fmc_erase_pages();
    fmc_erase_pages_check();

    /* step2: program and check if it is successful. If not, light the LED1. */
    fmc_program(FMC_PROGRAM_TYPE_DWORD);
    fmc_program_check(FMC_PROGRAM_TYPE_DWORD);

    /* step3: erase pages and check if it is successful. If not, light the LED1. */
    fmc_erase_pages();
    fmc_erase_pages_check();

    /* step4: fast program and check if it is successful. If not, light the LED1. */
    fmc_program(FMC_PROGRAM_TYPE_FAST);
    fmc_program_check(FMC_PROGRAM_TYPE_FAST);

    /* if all the operations are successful, light the LED2. */
    if(LED2 == lednum) {
        gd_eval_led_on(LED2);
    }

    while(1);
}

/*!
    \brief      clear pengding flags
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_flags_clear(void)
{
    fmc_flag_clear(FMC_BANK0_FLAG_END);
    fmc_flag_clear(FMC_BANK0_FLAG_WPERR);
    fmc_flag_clear(FMC_BANK0_FLAG_PGAERR);
    fmc_flag_clear(FMC_BANK0_FLAG_PGERR);
    fmc_flag_clear(FMC_BANK0_FLAG_PGSERR);
    fmc_flag_clear(FMC_BANK0_FLAG_CBCMDERR);
    fmc_flag_clear(FMC_BANK0_FLAG_RSTERR);
}

/*!
    \brief      erase fmc pages from FMC_WRITE_START_ADDR to FMC_WRITE_END_ADDR
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_erase_pages(void)
{
    uint32_t page_cnt;

    /* unlock the flash program/erase controller */
    fmc_unlock();

    /* clear all pending flags */
    fmc_flags_clear();
    /* erase the flash pages */
    for(page_cnt = 0; page_cnt < page_num; page_cnt++) {
        fmc_page_erase(FMC_WRITE_START_ADDR + (FMC_PAGE_SIZE * page_cnt));
        fmc_flags_clear();
    }

    /* lock the main FMC after the erase operation */
    fmc_lock();
}

/*!
    \brief      program fmc word by word from FMC_WRITE_START_ADDR to FMC_WRITE_END_ADDR
    \param[in]  program_type: 
      \arg        FMC_PROGRAM_TYPE_DWORD: double word program mode
      \arg        FMC_PROGRAM_TYPE_FAST: fast program mode
    \param[out] none
    \retval     none
*/
void fmc_program(uint8_t program_type)
{
    /* unlock the flash program/erase controller */
    fmc_unlock();

    address = FMC_WRITE_START_ADDR;

    /* program flash */
    while(address < FMC_WRITE_END_ADDR) {
        if(FMC_PROGRAM_TYPE_DWORD == program_type) {
            fmc_doubleword_program(address, data0);
            address += sizeof(uint64_t);
        } else if(FMC_PROGRAM_TYPE_FAST == program_type) {
            fmc_fast_program(address, data_buffer);
            address += DOUBLEWORD_CNT_IN_ROW * sizeof(uint64_t);
        }
        fmc_flags_clear();
    }

    /* lock the main FMC after the program operation */
    fmc_lock();
}

/*!
    \brief      check fmc erase result
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_erase_pages_check(void)
{
    uint32_t i;
    uint64_t *ptrd;

    ptrd = (uint64_t *)FMC_WRITE_START_ADDR;

    /* check flash whether has been erased */
    for(i = 0; i < dword_num; i++) {
        if(0xFFFFFFFFFFFFFFFF != (*ptrd)) {
            lednum = LED1;
            gd_eval_led_on(lednum);
            break;
        } else {
            ptrd++;
        }
    }
}

/*!
    \brief      check fmc program result
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_program_check(uint8_t program_type)
{
    uint32_t i;
    uint64_t *ptrd;

    if(FMC_PROGRAM_TYPE_DWORD == program_type) {
        ptrd = (uint64_t *)FMC_WRITE_START_ADDR;
        /* check flash whether has been programmed */
        for(i = 0; i < dword_num; i++) {
            if((*ptrd) != data0) {
                lednum = LED1;
                gd_eval_led_on(lednum);
                break;
            } else {
                ptrd++;
            }
        }
    } else if(FMC_PROGRAM_TYPE_FAST == program_type) {
        ptrd = (uint64_t *)FMC_WRITE_START_ADDR;
        /* check flash whether has been programmed */
        for(i = 0; i < dword_num; i++) {
            if((*ptrd) != data_buffer[i % DOUBLEWORD_CNT_IN_ROW]) {
                lednum = LED1;
                gd_eval_led_on(lednum);
                break;
            } else {
                ptrd++;
            }
        }
    }
}
