/*******************************************************************************
 * Project	:车载屏幕MCU开发_BOE成都
 * MCU	  	:GD32A503KCU3
 * Data		:2023/10/26
 * Files	:MPQ3367.c
 * Library	:V1.1.0, firmware for GD32A50x
 * Function	:LED Driver(MPQ3367)的功能应用函数
 * Author	:Liu Wei
 * Phone	:13349168457
 ******************************************************************************/
 #include "MPQ3367.h"



/*------------------------------------------------------------------------
*Function name		 :void McuRcuInitialization(void) 
*Function description:从MPQ3367的寄存器里面读取数据【标准读取流程】
*Ipunt				 :【1】器件地址【2】寄存器地址【3】存放读回数据的变量指针【4】要读的数据个数
*OutPut				 :None
*-------------------------------------------------------------------------*/
void I2C_Standard_Reading_Process(uint8_t I2c_Addr, uint8_t Reg_Addr, uint8_t *Buff, uint8_t Length)
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
                    i2c_bus_reset();
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
                i2c_data_transmit(I2C0, Reg_Addr);
                timeout = 0;
                state = I2C_RESTART;
            } else {
                timeout = 0;
                state = I2C_START;
                printf("i2c master sends data timeout in read!\n");
            }
            break;
        case I2C_RESTART:
            /* wait until the transmit data buffer is empty */
            while((!i2c_flag_get(I2C0, I2C_FLAG_TC)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                /* configure the EEPROM's internal address to write to : only one byte address */
                i2c_master_addressing(I2C0, I2c_Addr, I2C_MASTER_RECEIVE);
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
                        Buff++;
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

uint8_t Read_MPQ3367_Register(void)
{
	uint8_t read_data = 0;
	I2C_Standard_Reading_Process(MPQ3367_ADDRESS8, 0x02, &read_data, 1);
	return read_data;
}



