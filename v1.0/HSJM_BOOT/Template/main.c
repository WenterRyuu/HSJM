#include "gd32a50x.h"
#include "systick.h"
#include <stdio.h>
#include "Define.h"
#include "gd32a50x_rcu.h"
#include "fmc_App.h"
#include "i2c_App.h"
#include "MAX96752.h"
#include "Update.h"


uint64_t *pBL_State = (uint64_t)UPDATE_FLAG_ADDRESS;
uint64_t *pApp_Once = (uint64_t)APP2BOOT_FLAG_ADDRESS;



void Timer1_Generate_1ms_Interrupt(void)
{
    timer_parameter_struct timer_initpara;

    /* enable the peripherals clock */
    rcu_periph_clock_enable(RCU_TIMER1);

    /* deinit a TIMER */
    timer_deinit(TIMER1);
    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_initpara);
    /* TIMER1 configuration */
    timer_initpara.prescaler         = 9999;	//
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 9;//99999;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_init(TIMER1, &timer_initpara);

    /* enable the TIMER interrupt */
    timer_interrupt_enable(TIMER1, TIMER_INT_UP);
    /* enable a TIMER */
    timer_enable(TIMER1);
	
	nvic_irq_enable(TIMER1_IRQn, 0, 0);
}

int main(void)
{
    nvic_vector_table_set(NVIC_VECTTAB_FLASH, 0x0000);
	memset(&S19_Fire, 0, sizeof(S19_Fire));	
	if(*pApp_Once != (uint64_t)READY)//�ϵ�֮��ֱ�Ӵ�boot����app������û����bootloader
	{
		JumpAddress = *(__IO uint32_t*)(APP_START_ADDRESS + 4);
		Jump_To_Application = (pFunction)JumpAddress;
		__set_MSP(*(__IO uint32_t*) APP_START_ADDRESS);
		Jump_To_Application();
	}
	else
	if(*pApp_Once == (uint64_t)READY)
	{
		//SCB->AIRCR = (0x5FA << SCB_AIRCR_VECTKEY_Pos) | SCB_AIRCR_SYSRESETREQ_Msk;
		//-------------------------------------------------------------------------------------��λ->��ʼ��
        
		systick_config();
		//rcu_periph_clock_enable(RCU_GPIOA);
		rcu_periph_clock_enable(RCU_GPIOB);
		//rcu_periph_clock_enable(RCU_I2C0);
		rcu_periph_clock_enable(RCU_I2C1);
		rcu_periph_clock_enable(RCU_TIMER1);
		//iic
		//McuI2cInitialization();
        I2C_Gpio_Config();
//        I2C_Master_Config();
//        MAX96752_REG_Config();
        I2C_Slave_Config();
        
        memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.InputAccessKey2, sizeof(Std_Replay_Arr.InputAccessKey2));
        Update_tI2cSlave.SendSize = 7;
        IRQ_LOW_DOWN;//�����ж�֪ͨ��������
        Counter_1ms.IRQ_Update = 100;
        
		//IRQ
		gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_14);
		gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_14);		//PB14,������SOC���ж��ź�
		IRQ_RELEASE;	
		//Timer
		Timer1_Generate_1ms_Interrupt();
		
		//--------------------------------------------------------------------------------------------
//		gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_6);
//		gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
//		gpio_bit_set(GPIOA, GPIO_PIN_6);
		BootloaderStatus = QueryBootloaderStatus;
		Updating();
		//��������ɹ��ˣ���ֱ�ӵ���app���������ˡ�Ҫ������ʧ���ˣ��Ż��ߵ������while(1)����
	}

	
	
	
	
    while(1)//����ʧ���ˣ��Ż��ߵ����while(1)����
	{
		//��⵽�µĿ�ʼ
		if(compareArrays(Update_tI2cSlave.RecBuff, Std_Receive_Arr.RequestBootloaderAccess, sizeof(Std_Receive_Arr.RequestBootloaderAccess)) == true)
		{
			//������־
			fmc_erase_pages(UPDATE_FLAG_ADDRESS,1);
			fmc_erase_pages(APP2BOOT_FLAG_ADDRESS,1);
			//׼����Ҫ���͵����ݣ�ÿ��װһ�����ݣ�Ҫ����.SendSize
			memcpy(Update_tI2cSlave.Send_Buff, Std_Replay_Arr.RequestBootloaderAccess, sizeof(Std_Replay_Arr.RequestBootloaderAccess));
			Update_tI2cSlave.SendSize = 7;
			IRQ_LOW_DOWN;//�����ж�֪ͨ��������
			Counter_1ms.IRQ_Update = 100;
			BootloaderStatus = InputAccessKey1;
		}
		Updating();
		return 0;
    }
	
	
}
