/*!
    \file    main.c
    \brief   main flash program, write_protection

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

typedef enum {FAILED = 0, PASSED = !FAILED} test_state;

#define FLASH_PAGE_PROGRAM
//#define WRITE_PROTECTION_ENABLE
#define WRITE_PROTECTION_DISABLE

#define BANK0_BASE_ADDR        ((uint32_t)0x08000000U)
#define MAIN_FLASH_SIZE        ((REG32(0x1FFFF7E0) & 0x0000FFFF)*0x400)
#define FLASH_BANK0_SIZE       ((MAIN_FLASH_SIZE>256)?(256*0x400): MAIN_FLASH_SIZE)
#define FLASH_PAGE_SIZE        ((uint16_t)0x400U)
#define FLASH_SECTOR_SIZE      (FLASH_BANK0_SIZE / 32)

/* protected sectors of BK0WP[31:0] is sector 5 and 6 */
#define FMC_PAGES_PROTECTED    ((uint32_t)0x000000060)
#define BANK_WRITE_START_ADDR  (BANK0_BASE_ADDR + FLASH_SECTOR_SIZE * 5)
#define BANK_WRITE_END_ADDR    (BANK0_BASE_ADDR + FLASH_SECTOR_SIZE * 7)

uint32_t erase_counter = 0x0U, address = 0x0U;
uint64_t data = 0x123456789ABCDEFU;
__IO uint32_t wp_value = 0xFFFFFFFFU, protected_sectors = 0x0U;
uint32_t page_num;
__IO  fmc_state_enum fmc_state = FMC_READY;
__IO test_state program_state = PASSED;

/* clear pengding flags */
void fmc_flags_clear(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    uint32_t program_area;
    
    /* initialize leds on the board */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);

    /* unlock the flash program/erase controller */
    fmc_unlock();
    ob_unlock();
    fmc_flags_clear();
    /* get pages write protection status */
    wp_value = ob_write_protection_get();
#ifdef WRITE_PROTECTION_DISABLE
    /* get pages already write protected */
    protected_sectors = ~(wp_value | FMC_PAGES_PROTECTED);

    /* check if desired pages are already write protected */
    if((wp_value | (~FMC_PAGES_PROTECTED)) != 0xFFFFFFFF) {
        /* erase all the option bytes */
        fmc_state = ob_erase();

        /* check if there is write protected pages */
        if(protected_sectors != 0x0) {
            /* restore write protected pages */
            fmc_state = ob_write_protection_enable(BANK0_AREA, protected_sectors);
        }
        /* reload the option byte 0 and generate a system reset */
        ob_reset();
    }

#elif defined WRITE_PROTECTION_ENABLE
    /* get current write protected pages and the new pages to be protected */
    protected_sectors = (~wp_value) | FMC_PAGES_PROTECTED;

    /* check if desired pages are not yet write protected */
    if(((~wp_value) & FMC_PAGES_PROTECTED) != FMC_PAGES_PROTECTED) {

        /* erase all the option byte 0 because if a program operation is
        performed on a protected page, the flash memory returns a
        protection error */
        fmc_state = ob_erase();
        /* enable the pages write protection */
        fmc_state = ob_write_protection_enable(BANK0_AREA, protected_sectors);

        /* reload the option byte 0 and generate a system reset */
        ob_reset();
    }
#endif /* WRITE_PROTECTION_DISABLE */

#ifdef FLASH_PAGE_PROGRAM
    program_area = BANK_WRITE_END_ADDR;
    program_area -= BANK_WRITE_START_ADDR;
    /* get the number of pages to be erased */
    page_num = program_area / FLASH_PAGE_SIZE;

    /* the selected pages are not write protected */
    if((wp_value & FMC_PAGES_PROTECTED) != 0x00) {
        /* clear all pending flags */
        fmc_flags_clear();
        
        /* erase the FLASH pages */
        for(erase_counter = 0; (erase_counter < page_num) && (fmc_state == FMC_READY); erase_counter++) {
            fmc_state = fmc_page_erase(BANK_WRITE_START_ADDR + (FLASH_PAGE_SIZE * erase_counter));
        }

        /* FLASH double word program of data at addresses defined by BANK1_WRITE_START_ADDR and BANK1_WRITE_END_ADDR */
        address = BANK_WRITE_START_ADDR;

        while((address < BANK_WRITE_END_ADDR) && (fmc_state == FMC_READY)) {
            fmc_state = fmc_doubleword_program(address, data);
            address = address + 8;
        }

        /* check the correctness of written data */
        address = BANK_WRITE_START_ADDR;

        while((address < BANK_WRITE_END_ADDR) && (program_state != FAILED)) {
            if((*(__IO uint64_t *) address) != data) {
                program_state = FAILED;
            }
            address += 8;
        }
        gd_eval_led_on(LED1);
    } else {
        /* error to program the flash : the desired pages are write protected */
        program_state = FAILED;
        gd_eval_led_on(LED2);

    }
#endif /* FLASH_PAGE_PROGRAM */
    while(1) {
    }
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
