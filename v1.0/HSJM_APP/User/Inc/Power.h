#ifndef _POWER_H_
#define _POWER_H_


#include "Define.h"

typedef struct
{
	uint16_t PowerOn_Counter;
	uint16_t PowerOff_Counter;	
}_Power;
extern _Power Power;


//--------------------------------------------------------------
//º¯ÊýÉùÃ÷
void PowerOn_Order(void);
void PowerOff_Order(void);
void PowerOn_Sequence(void);
void PowerOff_Sequence(void);

#endif
