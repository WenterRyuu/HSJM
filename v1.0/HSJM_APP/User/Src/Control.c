#include "Control.h"




_heartbeat Heartbeat;
_ScreenLitAfterStartup ScreenLitAfterStartup;


uint8_t flag = 0;
void process_0x81(void)
{
	uint8_t index = 0;
	uint8_t id[4] = {0x13,0x14,0x15,0x16}; 
    
	for(index=0; index<4; index++)
	{
        while(gpio_input_bit_get(GPIOB, GPIO_PIN_14) == RESET);
        ReadFrameTransmit(id[index]);
        IRQ_RELEASE;
        delay_1ms(10);		
        IRQ_LOW_DOWN;
		IRQ_100ms = 100;
	}
    
    if(flag == 0)
    {
        nvic_irq_enable(TIMER5_DAC_IRQn, 0, 0);
        delay_1ms(1000);
        nvic_irq_enable(TIMER6_IRQn, 0, 0);  
        flag = 1;
    }
    

}

