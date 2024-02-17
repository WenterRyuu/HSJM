/*******************************************************************************
 * Project	:车载屏幕MCU开发_BOE成都
 * MCU	  	:GD32A503KCU3
 * Data		:2023/10/07
 * Files	:MAX96752.c
 * Library	:V1.1.0, firmware for GD32A50x
 * Function	:解串器的功能应用函数
 * Author	:Liu Wei
 * Phone	:13349168457
 ******************************************************************************/
#include "MAX96752.h"
#include <stdio.h>





ErrStatus I2c1_MasterWriteMulBytes(uint8_t I2c_Addr,uint16_t Reg_Addr,uint8_t *Buff,uint8_t Length)
{
    i2c_process_enum state = I2C_START;
    uint16_t timeout = 0;
    uint8_t end_flag = 0;

    while(!end_flag) {
        switch(state) {
        case I2C_START:
            /* configure slave address */
            i2c_master_addressing(I2C1, I2c_Addr, I2C_MASTER_TRANSMIT);
            /* configure number of bytes to be transferred */
            i2c_transfer_byte_number_config(I2C1, Length + 1);
            /* clear I2C_TDATA register */
            I2C_STAT(I2C1) |= I2C_STAT_TBE;
            /* enable I2C automatic end mode in master mode */
            i2c_automatic_end_enable(I2C1);
            /* i2c master sends start signal only when the bus is idle */
            while(i2c_flag_get(I2C1, I2C_FLAG_I2CBSY) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                i2c_start_on_bus(I2C1);
                timeout = 0;
                state = I2C_SEND_ADDRESS;
            } else {
                /* timeout, bus reset */
                I2C_BUS_Reset();
                timeout = 0;
                state = I2C_START;
                printf("i2c bus is busy in page write!\n");
            }
            break;
        case I2C_SEND_ADDRESS:
            /* wait until the transmit data buffer is empty */
            while((!i2c_flag_get(I2C1, I2C_FLAG_TBE)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                /* send the EEPROM's internal address to write to : only one byte address */
                i2c_data_transmit(I2C1, (Reg_Addr >> 8));
                timeout = 0;
				state = I2C_SEND_ADDRESS_LOW;
            } else {
                timeout = 0;
                state = I2C_START;
                printf("i2c master sends address timeout in page write!\n");
            }
            break;
///////////////////////////////////////////////////////////////////////////////////////////////////////////			
		case I2C_SEND_ADDRESS_LOW:	
            /* wait until the transmit data buffer is empty */
            while((!i2c_flag_get(I2C1, I2C_FLAG_TBE)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
				i2c_data_transmit(I2C1, (Reg_Addr & 0xFF));
                timeout = 0;
				state = I2C_TRANSMIT_DATA;
            } else {
                timeout = 0;
                state = I2C_START;
                printf("i2c master sends address timeout in page write!\n");
            }
            break;			
///////////////////////////////////////////////////////////////////////////////////////////////////////////////			
        case I2C_TRANSMIT_DATA:
            while(Length--) {
                /* wait until TI bit is set */
                while((!i2c_flag_get(I2C1, I2C_FLAG_TI)) && (timeout < I2C_TIME_OUT)) {
                    timeout++;
                }
                if(timeout < I2C_TIME_OUT) {
                    /* while there is data to be written */
                    i2c_data_transmit(I2C1, *Buff);
                    /* point to the next byte to be written */
                    Buff++;
                    timeout = 0;
                    state = I2C_STOP;
                } else {
                    /* wait TI timeout */
                    timeout = 0;
                    state = I2C_START;
                    printf("i2c master sends data timeout in page write!\n");
                    return ERROR;////////////////////////////////////////////////////////
                }
            }
            break;
        case I2C_STOP:
            /* wait until the stop condition is finished */
            while((!i2c_flag_get(I2C1, I2C_FLAG_STPDET)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                /* clear STPDET flag */
                i2c_flag_clear(I2C1, I2C_FLAG_STPDET);
                timeout = 0;
                state = I2C_END;
                end_flag = 1;
            } else {
                /* stop detect timeout */
                timeout = 0;
                state = I2C_START;
                printf("i2c master sends stop signal timeout in page write!\n");
            }
            break;
        default:
            /* default status */
            state = I2C_START;
            end_flag = 1;
            timeout = 0;
            printf("i2c master sends start signal in page write!\n");
            break;
        }
	}

	return SUCCESS;
}

ErrStatus I2c0_MasterWriteMulBytes(uint8_t I2c_Addr,uint16_t Reg_Addr,uint8_t *Buff,uint8_t Length)
{
    i2c_process_enum state = I2C_START;
    uint32_t timeout = 0;
    uint8_t end_flag = 0;

    while(!end_flag) {
        switch(state) {
        case I2C_START:
            /* configure slave address */
            i2c_master_addressing(I2C0, I2c_Addr, I2C_MASTER_TRANSMIT);
            /* configure number of bytes to be transferred */
            i2c_transfer_byte_number_config(I2C0, Length + 1);
            /* clear I2C_TDATA register */
            I2C_STAT(I2C0) |= I2C_STAT_TBE;
            /* enable I2C automatic end mode in master mode */
            i2c_automatic_end_enable(I2C0);
            /* i2c master sends start signal only when the bus is idle */
            while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                i2c_start_on_bus(I2C0);
                timeout = 0;
                state = I2C_SEND_ADDRESS;
            } else {
                /* timeout, bus reset */
                I2C_BUS_Reset();
                timeout = 0;
                state = I2C_START;
                printf("i2c bus is busy in page write!\n");
            }
            break;
        case I2C_SEND_ADDRESS:
            /* wait until the transmit data buffer is empty */
            while((!i2c_flag_get(I2C0, I2C_FLAG_TBE)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                /* send the EEPROM's internal address to write to : only one byte address */
                i2c_data_transmit(I2C0, (Reg_Addr >> 8));
                timeout = 0;
				state = I2C_SEND_ADDRESS_LOW;
            } else {
                timeout = 0;
                state = I2C_START;
                printf("i2c master sends address timeout in page write!\n");
            }
            break;
///////////////////////////////////////////////////////////////////////////////////////////////////////////			
		case I2C_SEND_ADDRESS_LOW:	
            /* wait until the transmit data buffer is empty */
            while((!i2c_flag_get(I2C0, I2C_FLAG_TBE)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
				i2c_data_transmit(I2C0, (Reg_Addr & 0xFF));
                timeout = 0;
				state = I2C_TRANSMIT_DATA;
            } else {
                timeout = 0;
                state = I2C_START;
                printf("i2c master sends address timeout in page write!\n");
            }
            break;			
///////////////////////////////////////////////////////////////////////////////////////////////////////////////			
        case I2C_TRANSMIT_DATA:
            while(Length--) {
                /* wait until TI bit is set */
                while((!i2c_flag_get(I2C0, I2C_FLAG_TI)) && (timeout < I2C_TIME_OUT)) {
                    timeout++;
                }
                if(timeout < I2C_TIME_OUT) {
                    /* while there is data to be written */
                    i2c_data_transmit(I2C0, *Buff);
                    /* point to the next byte to be written */
                    Buff++;
                    timeout = 0;
                    state = I2C_STOP;
                } else {
                    /* wait TI timeout */
                    timeout = 0;
                    state = I2C_START;
                    printf("i2c master sends data timeout in page write!\n");
                    return ERROR;////////////////////////////////////////////////////////
                }
            }
            break;
        case I2C_STOP:
            /* wait until the stop condition is finished */
            while((!i2c_flag_get(I2C0, I2C_FLAG_STPDET)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                /* clear STPDET flag */
                i2c_flag_clear(I2C0, I2C_FLAG_STPDET);
                timeout = 0;
                state = I2C_END;
                end_flag = 1;
            } else {
                /* stop detect timeout */
                timeout = 0;
                state = I2C_START;
                printf("i2c master sends stop signal timeout in page write!\n");
            }
            break;
        default:
            /* default status */
            state = I2C_START;
            end_flag = 1;
            timeout = 0;
            printf("i2c master sends start signal in page write!\n");
            break;
        }
	}

	return SUCCESS;

}


void MAX96752_Read(uint8_t I2c_Addr, uint16_t Reg_Addr, uint8_t *Buff, uint8_t Length)
{
    uint32_t nbytes_reload = 0;
    i2c_process_enum state = I2C_START;
    uint32_t timeout = 0;
    uint8_t end_flag = 0;
    uint8_t restart_flag = 0;
    uint8_t first_reload_flag = 1;
    uint8_t last_reload_flag = 0;

    while(!end_flag) {
        switch(state) {
        case I2C_START:
            if(0 == restart_flag) {
                /* clear I2C_TDATA register */
                I2C_STAT(I2C0) |= I2C_STAT_TBE;
                /* configure slave address */
                i2c_master_addressing(I2C0, I2c_Addr, I2C_MASTER_TRANSMIT);
                /* configure number of bytes to be transferred */
                i2c_transfer_byte_number_config(I2C0, 1);
                /* disable I2C automatic end mode in master mode */
                i2c_automatic_end_disable(I2C0);
                /* i2c master sends start signal only when the bus is idle */
                while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY) && (timeout < I2C_TIME_OUT)) {
                    timeout++;
                }
                if(timeout < I2C_TIME_OUT) {
                    i2c_start_on_bus(I2C0);
                    timeout = 0;
                    state = I2C_SEND_ADDRESS;
                } else {
                    /* timeout, bus reset */
                    I2C_BUS_Reset();
                    timeout = 0;
                    state = I2C_START;
                    printf("i2c bus is busy in read!\n");
                }
            } else {
                /* restart */
                i2c_start_on_bus(I2C0);
                restart_flag = 0;
                state = I2C_TRANSMIT_DATA;
            }
            break;
        case I2C_SEND_ADDRESS:
            /* wait until the transmit data buffer is empty */
            while((!i2c_flag_get(I2C0, I2C_FLAG_TBE)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                /* send the EEPROM's internal address to write to : only one byte address */
                i2c_data_transmit(I2C0, (Reg_Addr >> 8));
                timeout = 0;
				state = I2C_SEND_ADDRESS_LOW;
            } else {
                timeout = 0;
                state = I2C_START;
                printf("i2c master sends address timeout in page write!\n");
            }
            break;
		case I2C_SEND_ADDRESS_LOW:	
            /* wait until the transmit data buffer is empty */
            while((!i2c_flag_get(I2C0, I2C_FLAG_TBE)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
				i2c_data_transmit(I2C0, (Reg_Addr & 0xFF));
                timeout = 0;
				state = I2C_RESTART;
            } else {
                timeout = 0;
                state = I2C_START;
                printf("i2c master sends address timeout in page write!\n");
            }
            break;            
        case I2C_RESTART:
            /* wait until the transmit data buffer is empty */
            while((!i2c_flag_get(I2C0, I2C_FLAG_TC)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                /* configure the EEPROM's internal address to write to : only one byte address */
                i2c_master_addressing(I2C0, (I2c_Addr|0x01), I2C_MASTER_RECEIVE);
                /* enable I2C reload mode */
                i2c_reload_enable(I2C0);
                /* configure number of bytes to be transferred */
                timeout = 0;
                state = I2C_RELOAD;
                restart_flag = 1;
            } else {
                timeout = 0;
                state = I2C_START;
                printf("i2c master sends EEPROM's internal address timeout in read!\n");
            }
            break;
        case I2C_RELOAD:
            if(Length > MAX_RELOAD_SIZE) {
                Length = Length - MAX_RELOAD_SIZE;
                nbytes_reload = MAX_RELOAD_SIZE;
            } else {
                nbytes_reload = Length;
                last_reload_flag = 1;
            }
            if(1 == first_reload_flag) {
                /* configure number of bytes to be transferred */
                i2c_transfer_byte_number_config(I2C0, nbytes_reload);
                if(1 == last_reload_flag){
                    last_reload_flag = 0;
                    /* disable I2C reload mode */
                    if(Length <= MAX_RELOAD_SIZE) {
                        i2c_reload_disable(I2C0);
                        /* enable I2C automatic end mode in master mode */
                        i2c_automatic_end_enable(I2C0);
                    }
                }
                first_reload_flag = 0;
                state = I2C_START;
            } else {
                /* wait for TCR flag */
                while((!i2c_flag_get(I2C0, I2C_FLAG_TCR)) && (timeout < I2C_TIME_OUT)) {
                    timeout++;
                }
                if(timeout < I2C_TIME_OUT) {
                    /* configure number of bytes to be transferred */
                    i2c_transfer_byte_number_config(I2C0, nbytes_reload);
                    /* disable I2C reload mode */
                    if(Length <= MAX_RELOAD_SIZE) {
                        i2c_reload_disable(I2C0);
                        /* enable I2C automatic end mode in master mode */
                        i2c_automatic_end_enable(I2C0);
                    }
                    timeout = 0;
                    state = I2C_TRANSMIT_DATA;
                } else {
                    timeout = 0;
                    state = I2C_START;
                    printf("i2c master reload data timeout in read!\n");
                }
            }
            break;
        case I2C_TRANSMIT_DATA:
            /* wait until TI bit is set */
            while((!i2c_flag_get(I2C0, I2C_FLAG_TBE)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                while(nbytes_reload) {
                    /* wait until the RBNE bit is set and clear it */
                    if(i2c_flag_get(I2C0, I2C_FLAG_RBNE)) {
                        /* read a byte from the EEPROM */
                        *Buff = i2c_data_receive(I2C0);
                        /* point to the next location where the byte read will be saved */
                        Buff ++;
                        /* decrement the read bytes counter */
                        nbytes_reload--;
                    }
                }
                timeout = 0;
                /* check if the reload mode is enabled or not */
                if(I2C_CTL1(I2C0) & I2C_CTL1_RELOAD) {
                    timeout = 0;
                    state = I2C_RELOAD;
                } else {
                    timeout = 0;
                    state = I2C_STOP;
                }
            } else {
                /* wait TI timeout */
                timeout = 0;
                state = I2C_START;
                printf("i2c master read data timeout in read!\n");
            }
            break;
        case I2C_STOP:
            /* wait until the stop condition is finished */
            while((!i2c_flag_get(I2C0, I2C_FLAG_STPDET)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                /* clear STPDET flag */
                i2c_flag_clear(I2C0, I2C_FLAG_STPDET);
                timeout = 0;
                state = I2C_END;
                end_flag = 1;
            } else {
                timeout = 0;
                state = I2C_START;
                printf("i2c master sends stop signal timeout in read!\n");
            }
            break;
        default:
            /* default status */
            state = I2C_START;
            end_flag = 1;
            timeout = 0;
            printf("i2c master sends start signal in read!\n");
            break;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////

/*------------------------------------------------------------------------
*Function name		 :void McuGpioInitialization(void) 
*Function description:解串器的寄存器配置
*Ipunt				 :None
*OutPut				 :None
*-------------------------------------------------------------------------*/
void MAX96752_REG_Config(void)
{
	uint8_t data_tobe_write = 0;
    uint8_t data_from_read = 0;
/*
//	//-----------------------------------------------------------------------------
//	data_tobe_write = 0x32;
//	I2c1_MasterWriteMulBytes(MAX96752_ADDRESS8, 0x0001, &data_tobe_write, 2);
//	delay_1ms(1);
//	data_tobe_write = 0x03;
//	I2c1_MasterWriteMulBytes(MAX96752_ADDRESS8, 0x0218, &data_tobe_write, 2);
//	delay_1ms(1);
//	data_tobe_write = 0x28;
//	I2c1_MasterWriteMulBytes(MAX96752_ADDRESS8, 0x0219, &data_tobe_write, 2);
//	delay_1ms(1);
//	data_tobe_write = 0x08;
//	I2c1_MasterWriteMulBytes(MAX96752_ADDRESS8, 0x021A, &data_tobe_write, 2);	
//	delay_1ms(1);

//	//-----------------------------------------------------------------------------
//	data_tobe_write = 0x4F;
//	I2c1_MasterWriteMulBytes(MAX96752_ADDRESS8, 0x01CE, &data_tobe_write, 2);
//	data_tobe_write = 0x08;
//	I2c1_MasterWriteMulBytes(MAX96752_ADDRESS8, 0x01CF, &data_tobe_write, 2);
//	data_tobe_write = 0x00;
//	I2c1_MasterWriteMulBytes(MAX96752_ADDRESS8, 0x0050, &data_tobe_write, 2);
//	data_tobe_write = 0x00;
//	I2c1_MasterWriteMulBytes(MAX96752_ADDRESS8, 0x01D0, &data_tobe_write, 2);
//	data_tobe_write = 0x11;
//	I2c1_MasterWriteMulBytes(MAX96752_ADDRESS8, 0x01D1, &data_tobe_write, 2);
//	data_tobe_write = 0x22;
//	I2c1_MasterWriteMulBytes(MAX96752_ADDRESS8, 0x01D2, &data_tobe_write, 2);
//	data_tobe_write = 0x33;
//	I2c1_MasterWriteMulBytes(MAX96752_ADDRESS8, 0x01D3, &data_tobe_write, 2);
//	data_tobe_write = 0x44;
//	I2c1_MasterWriteMulBytes(MAX96752_ADDRESS8, 0x01D4, &data_tobe_write, 2);
*/
	//--------------------------------------------------------------------------
	data_tobe_write = 0x32;
	I2c0_MasterWriteMulBytes(MAX96752_ADDRESS8, 0x0001, &data_tobe_write, 2);
	delay_1ms(1);
	data_tobe_write = 0x03;
	I2c0_MasterWriteMulBytes(MAX96752_ADDRESS8, 0x0218, &data_tobe_write, 2);
	delay_1ms(1);
	data_tobe_write = 0x28;
	I2c0_MasterWriteMulBytes(MAX96752_ADDRESS8, 0x0219, &data_tobe_write, 2);
	delay_1ms(1);
	data_tobe_write = 0x08;
	I2c0_MasterWriteMulBytes(MAX96752_ADDRESS8, 0x021A, &data_tobe_write, 2);	
	delay_1ms(1);
//    MAX96752_Read(MAX96752_ADDRESS8, 0X0001, &data_from_read, 1);
//	delay_1ms(1);
//    MAX96752_Read(MAX96752_ADDRESS8, 0X0218, &data_from_read, 1);
//	delay_1ms(1);
//    MAX96752_Read(MAX96752_ADDRESS8, 0X0219, &data_from_read, 1);
//	delay_1ms(1);
//    MAX96752_Read(MAX96752_ADDRESS8, 0X021A, &data_from_read, 1);
//	delay_1ms(1);

	//--------------------------------------------------------------------------
	data_tobe_write = 0x4F;
	I2c0_MasterWriteMulBytes(MAX96752_ADDRESS8, 0x01CE, &data_tobe_write, 2);	
	delay_1ms(1);
	data_tobe_write = 0x08;
	I2c0_MasterWriteMulBytes(MAX96752_ADDRESS8, 0x01CF, &data_tobe_write, 2);	
	delay_1ms(1);
	data_tobe_write = 0x00;
	I2c0_MasterWriteMulBytes(MAX96752_ADDRESS8, 0x0050, &data_tobe_write, 2);	
	delay_1ms(1);
	data_tobe_write = 0x00;
	I2c0_MasterWriteMulBytes(MAX96752_ADDRESS8, 0x01D0, &data_tobe_write, 2);	
	delay_1ms(1);
	data_tobe_write = 0x11;
	I2c0_MasterWriteMulBytes(MAX96752_ADDRESS8, 0x01D1, &data_tobe_write, 2);	
	delay_1ms(1);
	data_tobe_write = 0x22;
	I2c0_MasterWriteMulBytes(MAX96752_ADDRESS8, 0x01D2, &data_tobe_write, 2);	
	delay_1ms(1);
	data_tobe_write = 0x33;
	I2c0_MasterWriteMulBytes(MAX96752_ADDRESS8, 0x01D3, &data_tobe_write, 2);	
	delay_1ms(1);
	data_tobe_write = 0x44;
	I2c0_MasterWriteMulBytes(MAX96752_ADDRESS8, 0x01D4, &data_tobe_write, 2);	
	delay_1ms(1);
}

