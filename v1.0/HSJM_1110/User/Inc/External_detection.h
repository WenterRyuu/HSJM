#ifndef _EXTERNAL_DETECTION_H_
#define _EXTERNAL_DETECTION_H_

#include "Define.h"

//电平检测状态枚举//////////////////////////////////////////////////////////
typedef enum //_Voltage_level_Detection
{
	LOW = 0,
	HIGH = 1,
	IDLEE,
	WAIT,
	CHECK
}Voltage_level_Detection;

typedef struct
{
	uint8_t Detection_Counter;
}_IO_Detection;
extern _IO_Detection IO_Detection;

//外部要调用的变量////////////////////////////////////////////////////////////////////////////


extern void ReadFrameTransmit(uint8_t cmd_id) ;
//函数声明/////////////////////////////////////////////////////////////////
ErrStatus PIN_level_Detection(uint32_t gpio_periph, uint32_t pin, Voltage_level_Detection need_to_be_detected_level);
ErrStatus LOCK_Detection(void);
ErrStatus Backlight_level_Error_Detection(void);
ErrStatus LCD_level_error_Detection(void);
ErrStatus Source_ic_Error_Detection(void);
ErrStatus Update_level_Detection(void);
ErrStatus IRQ_HIGH_level_Detection(void);
void power_4_1(void);
ErrStatus power4_1(void);
void DATA_Send(void);

void handshake_process(void);
ErrStatus handshake(void);
extern	uint8_t id[4];
extern	uint8_t index;
#endif
