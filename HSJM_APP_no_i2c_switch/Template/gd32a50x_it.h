#ifndef GD32A50X_IT_H
#define GD32A50X_IT_H

#include "gd32a50x.h"

/* function declarations */
/* this function handles NMI exception */
void NMI_Handler(void);
/* this function handles HardFault exception */
void HardFault_Handler(void);
/* this function handles MemManage exception */
void MemManage_Handler(void);
/* this function handles BusFault exception */
void BusFault_Handler(void);
/* this function handles UsageFault exception */
void UsageFault_Handler(void);
/* this function handles SVC exception */
void SVC_Handler(void);
/* this function handles DebugMon exception */
void DebugMon_Handler(void);
/* this function handles PendSV exception */
void PendSV_Handler(void);
/* this function handles SysTick exception */
void SysTick_Handler(void);


void TIMER1_IRQHandler(void);



extern uint16_t IRQ_100ms;
#endif /* GD32A50X_IT_H */
