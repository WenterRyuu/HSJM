#include "Tcon.h"



/*------------------------------------------------------------------------
*Function name		 :void McuRcuInitialization(void) 
*Function description:从TCON的寄存器里面读取数据
*Ipunt				 :【1】器件地址【2】寄存器地址【3】存放读回数据的变量指针【4】要读的数据个数
*OutPut				 :None
*-------------------------------------------------------------------------*/
void I2C0_Read_TCON_Process(uint8_t I2c_Addr, uint16_t Reg_Addr, uint8_t *Buff, uint8_t Length)
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
                I2C_STAT(I2C_INDEX) |= I2C_STAT_TBE;
                /* configure slave address */
                i2c_master_addressing(I2C_INDEX, I2c_Addr, I2C_MASTER_TRANSMIT);
                /* configure number of bytes to be transferred */
                i2c_transfer_byte_number_config(I2C_INDEX, 3);/////////////////////////////////////////////?
                /* disable I2C automatic end mode in master mode */
                i2c_automatic_end_disable(I2C_INDEX);
                /* i2c master sends start signal only when the bus is idle */
                while(i2c_flag_get(I2C_INDEX, I2C_FLAG_I2CBSY) && (timeout < I2C_TIME_OUT)) {
                    timeout++;
                }
                if(timeout < I2C_TIME_OUT) {
                    i2c_start_on_bus(I2C_INDEX);
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
                i2c_start_on_bus(I2C_INDEX);
                restart_flag = 0;
                state = I2C_TRANSMIT_DATA;
            }
            break;
        case I2C_SEND_ADDRESS://发高8位地址
            /* wait until the transmit data buffer is empty */
            while((!i2c_flag_get(I2C_INDEX, I2C_FLAG_TBE)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                /* send the EEPROM's internal address to write to : only one byte address */
                i2c_data_transmit(I2C_INDEX, Reg_Addr>>8);
                timeout = 0;
                state = I2C_SEND_ADDRESS_LOW;
            } else {
                timeout = 0;
                state = I2C_START;
                printf("i2c master sends data timeout in read!\n");
            }
            break;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		case I2C_SEND_ADDRESS_LOW:	//发低8位地址
            /* wait until the transmit data buffer is empty */
            while((!i2c_flag_get(I2C_INDEX, I2C_FLAG_TBE)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
				i2c_data_transmit(I2C_INDEX, (Reg_Addr & 0xFF));
                timeout = 0;
				state = I2C_SEND_LENGTH;
            } else {
                timeout = 0;
                state = I2C_START;
                printf("i2c master sends address timeout in page write!\n");
            }
            break;						
		case I2C_SEND_LENGTH://发送要读的数据长度
            /* wait until the transmit data buffer is empty */
            while((!i2c_flag_get(I2C_INDEX, I2C_FLAG_TBE)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
				i2c_data_transmit(I2C_INDEX, 0x00);
                timeout = 0;
				state = I2C_STOOP;
            } else {
                timeout = 0;
                state = I2C_START;
                printf("i2c master sends address timeout in page write!\n");
            }
            break;				
		case I2C_STOOP:
            /* wait until the stop condition is finished */
            while((!i2c_flag_get(I2C_INDEX, I2C_FLAG_STPDET)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
				i2c_stop_on_bus(I2C_INDEX);///////////////////////////////////////////?
                timeout = 0;
                state = I2C_RESTART;
            } else {
                timeout = 0;
                state = I2C_START;
                printf("i2c master sends stop signal timeout in page write!\n");
            }
            break;			
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        case I2C_RESTART:
            /* wait until the transmit data buffer is empty */
            while((!i2c_flag_get(I2C_INDEX, I2C_FLAG_TC)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                /* configure the EEPROM's internal address to write to : only one byte address */
                i2c_master_addressing(I2C_INDEX, (I2c_Addr|0x01), I2C_MASTER_RECEIVE);
                /* enable I2C reload mode */
                i2c_reload_enable(I2C_INDEX);
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
			//----------------------
			i2c_transfer_byte_number_config(I2C_INDEX,nbytes_reload);
			while(i2c_flag_get(I2C_INDEX, I2C_FLAG_I2CBSY));
			i2c_start_on_bus(I2C_INDEX);
			//-------------------------
            if(1 == first_reload_flag) {
                /* configure number of bytes to be transferred */
                i2c_transfer_byte_number_config(I2C_INDEX, nbytes_reload);
                if(1 == last_reload_flag){
                    last_reload_flag = 0;
                    /* disable I2C reload mode */
                    if(Length <= MAX_RELOAD_SIZE) {
                        i2c_reload_disable(I2C_INDEX);
                        /* enable I2C automatic end mode in master mode */
                        i2c_automatic_end_enable(I2C_INDEX);
                    }
                }
                first_reload_flag = 0;
                state = I2C_START;
            } else {
                /* wait for TCR flag */
                while((!i2c_flag_get(I2C_INDEX, I2C_FLAG_TCR)) && (timeout < I2C_TIME_OUT)) {
                    timeout++;
                }
                if(timeout < I2C_TIME_OUT) {
                    /* configure number of bytes to be transferred */
                    i2c_transfer_byte_number_config(I2C_INDEX, nbytes_reload);
                    /* disable I2C reload mode */
                    if(Length <= MAX_RELOAD_SIZE) {
                        i2c_reload_disable(I2C_INDEX);
                        /* enable I2C automatic end mode in master mode */
                        i2c_automatic_end_enable(I2C_INDEX);
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
            while((!i2c_flag_get(I2C_INDEX, I2C_FLAG_TBE)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                while(nbytes_reload) {
                    /* wait until the RBNE bit is set and clear it */
                    if(i2c_flag_get(I2C_INDEX, I2C_FLAG_RBNE)) {
                        /* read a byte from the EEPROM */
                        *Buff = i2c_data_receive(I2C_INDEX);
                        /* point to the next location where the byte read will be saved */
                        Buff++;
                        /* decrement the read bytes counter */
                        nbytes_reload--;
                    }
                }
                timeout = 0;
                /* check if the reload mode is enabled or not */
                if(I2C_CTL1(I2C_INDEX) & I2C_CTL1_RELOAD) {
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
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		case I2C_CRC:
//			break;		
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        case I2C_STOP:
            /* wait until the stop condition is finished */
            while((!i2c_flag_get(I2C_INDEX, I2C_FLAG_STPDET)) && (timeout < I2C_TIME_OUT)) {
                timeout++;
            }
            if(timeout < I2C_TIME_OUT) {
                /* clear STPDET flag */
                i2c_flag_clear(I2C_INDEX, I2C_FLAG_STPDET);
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













//全局变量，存储从Tcon读来的寄存器值
struct BitFields Tcon_Register;


/*------------------------------------------------------------------------
*Function name		 :void Read_TCON_Register(void) 
*Function description:保存从TCON读到的寄存器数据（单位：bit）
*Ipunt				 :None
*OutPut				 :None
*-------------------------------------------------------------------------*/
void Read_TCON_Register(void)
{	
	uint8_t read_tcon_reg[7] = {0};
	I2C0_Read_TCON_Process(TCON_ADDRESS8, 0x041D, &read_tcon_reg[REG_041D], 1);
	I2C0_Read_TCON_Process(TCON_ADDRESS8, 0x041E, &read_tcon_reg[REG_041E], 1);
	I2C0_Read_TCON_Process(TCON_ADDRESS8, 0x041F, &read_tcon_reg[REG_041F], 1);
	I2C0_Read_TCON_Process(TCON_ADDRESS8, 0x0420, &read_tcon_reg[REG_0420], 1);
	I2C0_Read_TCON_Process(TCON_ADDRESS8, 0x0421, &read_tcon_reg[REG_0421], 1);
	I2C0_Read_TCON_Process(TCON_ADDRESS8, 0x0422, &read_tcon_reg[REG_0422], 1);
	I2C0_Read_TCON_Process(TCON_ADDRESS8, 0xFC00, &read_tcon_reg[REG_FC00], 1);
	
	Tcon_Register.R041D_1 = (bool)((read_tcon_reg[REG_041D]&0x01)>>1);
	Tcon_Register.R041D_2 = (bool)((read_tcon_reg[REG_041D]&0x04)>>2);
	Tcon_Register.R041D_6 = (bool)((read_tcon_reg[REG_041D]&0x40)>>6);
	Tcon_Register.R041D_7 = (bool)((read_tcon_reg[REG_041D]&0x80)>>7);
	Tcon_Register.R041E_3 = (bool)((read_tcon_reg[REG_041E]&0x08)>>3);
	Tcon_Register.R041F_7 = (bool)((read_tcon_reg[REG_041F]&0x80)>>7);
	Tcon_Register.R0420_1 = (bool)((read_tcon_reg[REG_0420]&0x02)>>1);
	Tcon_Register.R0420_6 = (bool)((read_tcon_reg[REG_0420]&0x40)>>6);	
	Tcon_Register.R0420_7 = (bool)((read_tcon_reg[REG_0420]&0x80)>>7);
	Tcon_Register.R0421_0 = (bool)((read_tcon_reg[REG_0421]&0x01)>>0);
	Tcon_Register.R0422_4 = (bool)((read_tcon_reg[REG_0422]&0x10)>>4);
	Tcon_Register.RFC00_0 = (bool)((read_tcon_reg[REG_FC00]&0x01)>>0);
	Tcon_Register.RFC00_1 = (bool)((read_tcon_reg[REG_FC00]&0x02)>>1);
	Tcon_Register.RFC00_2 = (bool)((read_tcon_reg[REG_FC00]&0x04)>>2);
	Tcon_Register.RFC00_3 = (bool)((read_tcon_reg[REG_FC00]&0x08)>>3);
	
}







