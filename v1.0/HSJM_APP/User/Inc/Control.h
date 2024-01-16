#ifndef _CONTROL_H_
#define _CONTROL_H_




#include "Define.h"


typedef struct{
	bool finish;
}_ScreenLitAfterStartup;
extern _ScreenLitAfterStartup ScreenLitAfterStartup;


extern void ReadFrameTransmit(uint8_t cmd_id) ;

void process_0x81(void);
void handshake_process(void);
ErrStatus handshake(void);

extern	uint8_t index;
#endif
