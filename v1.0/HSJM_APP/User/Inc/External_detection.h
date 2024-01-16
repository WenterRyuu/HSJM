#ifndef _EXTERNAL_DETECTION_H_
#define _EXTERNAL_DETECTION_H_

#include "Define.h"

//电平检测状态枚举===============================================================
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


//函数声明=======================================================================
ErrStatus PIN_level_Detection(uint32_t gpio_periph, uint32_t pin, Voltage_level_Detection need_to_be_detected_level);


#endif
