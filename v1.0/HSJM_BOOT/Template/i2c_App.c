#include "i2c_App.h"



//全局变量
//tI2c_Type Update_tI2cSlave ;



void McuI2cInitialization(void)
{
	i2c_deinit(I2C_INDEX);
	I2C_Gpio_Config();
	I2C_Config();
}

void I2C_Gpio_Config(void)
{
    gpio_af_set(I2C_PORT, I2C_GPIO_AF_INDEX, I2C_SCL_PIN|I2C_SDA_PIN);
    gpio_mode_set(I2C_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, I2C_SCL_PIN|I2C_SDA_PIN);
    gpio_output_options_set(I2C_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, I2C_SCL_PIN|I2C_SDA_PIN);	
	gpio_mode_set(I2C1_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_7);	//PB7浮空输入	
//    gpio_af_set(GPIOB, GPIO_AF_5, GPIO_PIN_7 | GPIO_PIN_8);
//    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_7 | GPIO_PIN_8);
//    gpio_output_options_set(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_7 | GPIO_PIN_8);		
}


void I2C_Config(void)
{
//	I2C_Master_Config();
	I2C_Slave_Config();
}

//释放引脚
void I2C_Gpio_Release(void)
{
	gpio_mode_set(I2C0_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_10);	
	gpio_mode_set(I2C0_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_11);	
	gpio_mode_set(I2C1_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_7);	
}

//调试的时候，这个函数，包括注释，不允许更改
void I2C_Master_Config(void)
{
	//MASTER
    i2c_timing_config(I2C_INDEX, 0x550, 0xC0, 0);
    i2c_master_clock_config(I2C_INDEX, 0x2BB0, 0xE0);
    i2c_enable(I2C_INDEX);
}

//调试的时候，这个函数，包括注释，不允许更改
void I2C_Slave_Config(void)
{
    /* configure I2C timing */
    i2c_timing_config(I2C_INDEX, 0x0, 0x5, 0);
    i2c_master_clock_config(I2C_INDEX, 0x1D, 0x59);

    /* configure I2C address */
    i2c_address_config(I2C_INDEX, 0x34, I2C_ADDFORMAT_7BITS);
    i2c_enable(I2C_INDEX);
    
    i2c_interrupt_enable(I2C_INDEX, I2C_INT_ERR | I2C_INT_STPDET | I2C_INT_ADDM | I2C_INT_TI | I2C_INT_RBNE | I2C_INT_NACK); 
    nvic_irq_enable(I2C0_EV_IRQn,0,1);   
    nvic_irq_enable(I2C0_ER_IRQn,0,1); 
	nvic_irq_enable(I2C1_EV_IRQn,0,1);   
    nvic_irq_enable(I2C1_ER_IRQn,0,1); 
}


void I2C_BUS_Reset(void)
{
    /* configure SDA/SCL for GPIO */
    GPIO_BC(I2C_PORT) |= I2C_SCL_PIN;
    GPIO_BC(I2C_PORT) |= I2C_SDA_PIN;
    /* reset SCL and SDA pins */
    I2C_GPIO_Reset();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    /* stop signal */
    GPIO_BOP(I2C_PORT) |= I2C_SCL_PIN;
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    GPIO_BOP(I2C_PORT) |= I2C_SDA_PIN;
    gpio_mode_set(I2C_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, I2C_SCL_PIN);
    gpio_output_options_set(I2C_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, I2C_SCL_PIN);
    gpio_mode_set(I2C_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, I2C_SDA_PIN);
    gpio_output_options_set(I2C_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, I2C_SDA_PIN);
}
void I2C_GPIO_Reset(void)
{
    /* reset SCL and SDA pins */
    gpio_mode_set(I2C_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, I2C_SCL_PIN);
    gpio_output_options_set(I2C_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, I2C_SCL_PIN);
    gpio_mode_set(I2C_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, I2C_SDA_PIN);
    gpio_output_options_set(I2C_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, I2C_SDA_PIN);
}


////////////////////////////////////////////以上是I2C相关设置函数/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////以下是I2C中断函数/////////////////////////////////////////////////////////////////////////////

void I2C0_EV_IRQHandler(void)
{
    if(i2c_interrupt_flag_get(I2C_INDEX, I2C_INT_FLAG_ADDSEND)) 
    {
        Update_tI2cSlave.uFlag.Bits.DirState=i2c_flag_get(I2C_INDEX,I2C_FLAG_TR);
        if(Update_tI2cSlave.uFlag.Bits.DirState==RESET)
        {
            Update_tI2cSlave.RecCount=0;
        }
        else
        {
            /* clear I2C_TDATA register */
            I2C_STAT(I2C_INDEX) |= I2C_STAT_TBE;
            Update_tI2cSlave.SendCount=0;
        }            
        /* clear the ADDSEND bit */
        i2c_interrupt_flag_clear(I2C_INDEX, I2C_INT_FLAG_ADDSEND);
    } 
    else if(i2c_interrupt_flag_get(I2C_INDEX, I2C_INT_FLAG_RBNE)) 
    {
        gpio_bit_set(I2C_PORT, GPIO_PIN_14) ;
        if(Update_tI2cSlave.RecCount < I2C_SLAVE_REC_MAX_SIZE)
        {
            Update_tI2cSlave.RecBuff[Update_tI2cSlave.RecCount]= i2c_data_receive(I2C_INDEX) ;
            if(Update_tI2cSlave.RecBuff[0] != 0xfe)
            {
                Update_tI2cSlave.SendSize = 0x00 ;
            }
            ++Update_tI2cSlave.RecCount ;       
        }
        else
        {
            i2c_data_receive(I2C_INDEX);        
        }
    }
    else if((i2c_interrupt_flag_get(I2C_INDEX, I2C_INT_FLAG_TI))) 
    {
        /* send a data byte */
        if(Update_tI2cSlave.SendCount < Update_tI2cSlave.SendSize)
        {
            /* if reception data register is not empty, I2C_INDEX will read a data from I2C_RDATA */
            i2c_data_transmit(I2C_INDEX,Update_tI2cSlave.Send_Buff[Update_tI2cSlave.SendCount]) ;
            Update_tI2cSlave.SendCount++ ;
        }
        else
        {
            i2c_data_transmit(I2C_INDEX,0xff);        
        }
    }
    else if(i2c_interrupt_flag_get(I2C_INDEX, I2C_INT_FLAG_STPDET)) 
    {
        /* clear STPDET interrupt flag */
        i2c_interrupt_flag_clear(I2C_INDEX, I2C_INT_FLAG_STPDET);
        
        if(Update_tI2cSlave.uFlag.Bits.DirState==RESET)
        {
            Update_tI2cSlave.uFlag.Bits.RecSuccess = SUCCESS ; 
//            tBuiltIn.tI2c = Update_tI2cSlave ;
//            Update_tI2cSlave.RecCount = 0x00 ;   //计数值清0  从头开始接收数据
        }
        else
        {
            Update_tI2cSlave.uFlag.Bits.SendSucess = SUCCESS; 
      //      tBuiltIn.tI2c = Update_tI2cSlave ;
            
            Update_tI2cSlave.SendSize = 0x00 ;   //避免没有接收到命令重新发数据
            Update_tI2cSlave.SendCount = 0x00 ;  //计数值清零
        }
        /* disable I2C_INDEX interrupt */
    }
    else if(i2c_interrupt_flag_get(I2C_INDEX, I2C_INT_FLAG_NACK)) 
    {
        /* clear STPDET interrupt flag */
        i2c_interrupt_flag_clear(I2C_INDEX, I2C_INT_FLAG_NACK);
//        Update_tI2cSlave.uFlag.Bits.SendSucess = SUCCESS;            
    }    
}


void I2C0_ER_IRQHandler(void)
{
    /* bus error */
    if(i2c_interrupt_flag_get(I2C_INDEX, I2C_INT_FLAG_BERR)) {
        i2c_interrupt_flag_clear(I2C_INDEX, I2C_INT_FLAG_BERR);
    }

    /* arbitration lost */
    if(i2c_interrupt_flag_get(I2C_INDEX, I2C_INT_FLAG_LOSTARB)) {
        i2c_interrupt_flag_clear(I2C_INDEX, I2C_INT_FLAG_LOSTARB);
    }

    /* over-run or under-run when SCL stretch is disabled */
    if(i2c_interrupt_flag_get(I2C_INDEX, I2C_INT_FLAG_OUERR)) {
        i2c_interrupt_flag_clear(I2C_INDEX, I2C_INT_FLAG_OUERR);
    }

    /* PEC error */
    if(i2c_interrupt_flag_get(I2C_INDEX, I2C_INT_FLAG_PECERR)) {
        i2c_interrupt_flag_clear(I2C_INDEX, I2C_INT_FLAG_PECERR);
    }

    /* timeout error */
    if(i2c_interrupt_flag_get(I2C_INDEX, I2C_INT_FLAG_TIMEOUT)) {
        i2c_interrupt_flag_clear(I2C_INDEX, I2C_INT_FLAG_TIMEOUT);
    }

    /* SMBus alert */
    if(i2c_interrupt_flag_get(I2C_INDEX, I2C_INT_FLAG_SMBALT)) {
        i2c_interrupt_flag_clear(I2C_INDEX, I2C_INT_FLAG_SMBALT);
    }
    else if(i2c_interrupt_flag_get(I2C_INDEX, I2C_INT_FLAG_NACK)) 
    {
        /* clear STPDET interrupt flag */
        i2c_interrupt_flag_clear(I2C_INDEX, I2C_INT_FLAG_NACK);
        Update_tI2cSlave.uFlag.Bits.SendSucess = SUCCESS;            
    }

    Update_tI2cSlave.RecCount = 0x00 ;   //计数值清0  从头开始接收数据
    Update_tI2cSlave.uFlag.Bits.RecSuccess = ERROR ;  
   
    Update_tI2cSlave.SendSize = 0x00 ;   //避免没有接收到命令重新发数据
    Update_tI2cSlave.SendCount = 0x00 ;  //计数值清零
    Update_tI2cSlave.uFlag.Bits.SendSucess = ERROR;  
}


void I2C1_EV_IRQHandler(void)
{
    if(i2c_interrupt_flag_get(I2C_INDEX, I2C_INT_FLAG_ADDSEND)) 
    {
        Update_tI2cSlave.uFlag.Bits.DirState=i2c_flag_get(I2C_INDEX,I2C_FLAG_TR);
        if(Update_tI2cSlave.uFlag.Bits.DirState==RESET)
        {
            Update_tI2cSlave.RecCount=0;
        }
        else
        {
            /* clear I2C_TDATA register */
            I2C_STAT(I2C_INDEX) |= I2C_STAT_TBE;
            Update_tI2cSlave.SendCount=0;
        }            
        /* clear the ADDSEND bit */
        i2c_interrupt_flag_clear(I2C_INDEX, I2C_INT_FLAG_ADDSEND);
    } 
    else if(i2c_interrupt_flag_get(I2C_INDEX, I2C_INT_FLAG_RBNE)) 
    {
        gpio_bit_set(I2C_PORT, GPIO_PIN_14) ;
        if(Update_tI2cSlave.RecCount < I2C_SLAVE_REC_MAX_SIZE)
        {
            Update_tI2cSlave.RecBuff[Update_tI2cSlave.RecCount]= i2c_data_receive(I2C_INDEX) ;
            if(Update_tI2cSlave.RecBuff[0] != 0xfe)
            {
                Update_tI2cSlave.SendSize = 0x00 ;
            }
            ++Update_tI2cSlave.RecCount ;       
        }
        else
        {
            i2c_data_receive(I2C_INDEX);        
        }
    }
    else if((i2c_interrupt_flag_get(I2C_INDEX, I2C_INT_FLAG_TI))) 
    {
        /* send a data byte */
        if(Update_tI2cSlave.SendCount < Update_tI2cSlave.SendSize)
        {
            /* if reception data register is not empty, I2C_INDEX will read a data from I2C_RDATA */
            i2c_data_transmit(I2C_INDEX,Update_tI2cSlave.Send_Buff[Update_tI2cSlave.SendCount]) ;
            Update_tI2cSlave.SendCount++ ;
        }
        else
        {
            i2c_data_transmit(I2C_INDEX,0xff);        
        }
    }
    else if(i2c_interrupt_flag_get(I2C_INDEX, I2C_INT_FLAG_STPDET)) 
    {
        /* clear STPDET interrupt flag */
        i2c_interrupt_flag_clear(I2C_INDEX, I2C_INT_FLAG_STPDET);
        
        if(Update_tI2cSlave.uFlag.Bits.DirState==RESET)
        {
            Update_tI2cSlave.uFlag.Bits.RecSuccess = SUCCESS ; 
//            tBuiltIn.tI2c = Update_tI2cSlave ;
//            Update_tI2cSlave.RecCount = 0x00 ;   //计数值清0  从头开始接收数据
        }
        else
        {
            Update_tI2cSlave.uFlag.Bits.SendSucess = SUCCESS; 
      //      tBuiltIn.tI2c = Update_tI2cSlave ;
            
            Update_tI2cSlave.SendSize = 0x00 ;   //避免没有接收到命令重新发数据
            Update_tI2cSlave.SendCount = 0x00 ;  //计数值清零
        }
        /* disable I2C_INDEX interrupt */
    }
    else if(i2c_interrupt_flag_get(I2C_INDEX, I2C_INT_FLAG_NACK)) 
    {
        /* clear STPDET interrupt flag */
        i2c_interrupt_flag_clear(I2C_INDEX, I2C_INT_FLAG_NACK);
//        Update_tI2cSlave.uFlag.Bits.SendSucess = SUCCESS;            
    }    
}


void I2C1_ER_IRQHandler(void)
{
    /* bus error */
    if(i2c_interrupt_flag_get(I2C_INDEX, I2C_INT_FLAG_BERR)) {
        i2c_interrupt_flag_clear(I2C_INDEX, I2C_INT_FLAG_BERR);
    }

    /* arbitration lost */
    if(i2c_interrupt_flag_get(I2C_INDEX, I2C_INT_FLAG_LOSTARB)) {
        i2c_interrupt_flag_clear(I2C_INDEX, I2C_INT_FLAG_LOSTARB);
    }

    /* over-run or under-run when SCL stretch is disabled */
    if(i2c_interrupt_flag_get(I2C_INDEX, I2C_INT_FLAG_OUERR)) {
        i2c_interrupt_flag_clear(I2C_INDEX, I2C_INT_FLAG_OUERR);
    }

    /* PEC error */
    if(i2c_interrupt_flag_get(I2C_INDEX, I2C_INT_FLAG_PECERR)) {
        i2c_interrupt_flag_clear(I2C_INDEX, I2C_INT_FLAG_PECERR);
    }

    /* timeout error */
    if(i2c_interrupt_flag_get(I2C_INDEX, I2C_INT_FLAG_TIMEOUT)) {
        i2c_interrupt_flag_clear(I2C_INDEX, I2C_INT_FLAG_TIMEOUT);
    }

    /* SMBus alert */
    if(i2c_interrupt_flag_get(I2C_INDEX, I2C_INT_FLAG_SMBALT)) {
        i2c_interrupt_flag_clear(I2C_INDEX, I2C_INT_FLAG_SMBALT);
    }
    else if(i2c_interrupt_flag_get(I2C_INDEX, I2C_INT_FLAG_NACK)) 
    {
        /* clear STPDET interrupt flag */
        i2c_interrupt_flag_clear(I2C_INDEX, I2C_INT_FLAG_NACK);
        Update_tI2cSlave.uFlag.Bits.SendSucess = SUCCESS;            
    }

    Update_tI2cSlave.RecCount = 0x00 ;   //计数值清0  从头开始接收数据
    Update_tI2cSlave.uFlag.Bits.RecSuccess = ERROR ;  
   
    Update_tI2cSlave.SendSize = 0x00 ;   //避免没有接收到命令重新发数据
    Update_tI2cSlave.SendCount = 0x00 ;  //计数值清零
    Update_tI2cSlave.uFlag.Bits.SendSucess = ERROR;  
}

