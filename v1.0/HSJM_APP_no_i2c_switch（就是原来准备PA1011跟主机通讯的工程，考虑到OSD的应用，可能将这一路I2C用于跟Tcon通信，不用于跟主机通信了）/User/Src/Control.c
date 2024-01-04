#include "Control.h"




_heartbeat Heartbeat;
_ScreenLitAfterStartup ScreenLitAfterStartup;



void process_0x81(void)
{
	uint8_t index = 0;
	uint8_t id[4] = {0x13,0x14,0x15,0x16};

	for(index=0; index<4; index++)
	{
		ReadFrameTransmit(id[index]);
		while(!IRQ_HIGH_level_Detection());                 //高电平退出while
		IRQ_LOW_DOWN;
		IRQ_100ms = 100;
	}
}
