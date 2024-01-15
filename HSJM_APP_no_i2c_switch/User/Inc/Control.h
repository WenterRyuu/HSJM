#ifndef _CONTROL_H_
#define _CONTROL_H_




#include "Define.h"

typedef struct{
	uint16_t Counter;
	uint8_t	irq_Counter;
}_heartbeat;
extern _heartbeat Heartbeat;


typedef struct{
	bool finish;
}_ScreenLitAfterStartup;
extern _ScreenLitAfterStartup ScreenLitAfterStartup;


void Startup_Lock_Check(void);

void process_0x81(void);
#endif
