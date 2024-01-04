#ifndef _TIMER_APP_H_
#define _TIMER_APP_H_


#include "Define.h"
#include "gd32a50x_timer.h"



void Timer1_Generate_1ms_Interrupt(void);
void Timer19_Generate_PWM_config(void);

void Timer5_Generate_2s_Interrupt(void);
void McuTimerInitialization(void);












#endif
