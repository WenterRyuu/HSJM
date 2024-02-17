#ifndef _BACKLIGHT_H_
#define _BACKLIGHT_H_



//头文件//////////////////////////////////////////////////////////////////////////////////////////////////
#include "Define.h"
#include <stdio.h>
#include <stdlib.h>


//创建调光结构体变量类型，其中用到了枚举类型///////////////////////////////////////////////////////////////
//为开机上电状态机的枚举状态，创建类型别名
typedef enum {
    STATE_IDLE,					//空闲状态
    STATE_ADJUSTING,			//调整中
    STATE_ADJUSTED,				//调整完了
	STATE_WAITING_for_20ms		//等待调整
} State;

typedef struct {
    uint8_t currDutyCycle;
    uint8_t targetDutyCycle;
    int8_t stepDutyCycle;
	int8_t laststepDutyCycle;
    State state;
} BacklightController;

typedef struct
{
	int16_t Final_Temperature;		//最终算出的温度
	uint16_t Backlight_PWM;			//这个数在0~100之间，真正直接控制亮度的实际变量
	uint8_t Brightness_Counter;		//用于20计数的
	BacklightController blc;		//调光结构体全局变量	
}_Backlight;
extern _Backlight Backlight;
//外部文件要引用的变量////////////////////////////////////////////////////////////////////////////////////

//函数声明////////////////////////////////////////////////////////////////////////////////////////////////
uint16_t Brightness_control(uint16_t Target_Brightness);
void Backlight_Brightness_Init(BacklightController* blc, uint8_t initialDutyCycle);
void SetTargetDutyCycle(BacklightController* blc, uint8_t target);
uint8_t UpdateBacklightController(BacklightController* blc);
void Modify_PWM_Directly(uint8_t Brightness);
void Update_Brightness(uint8_t target_brightness);




#endif
