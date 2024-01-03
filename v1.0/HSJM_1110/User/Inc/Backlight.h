#ifndef _BACKLIGHT_H_
#define _BACKLIGHT_H_



//ͷ�ļ�//////////////////////////////////////////////////////////////////////////////////////////////////
#include "Define.h"
#include <stdio.h>
#include <stdlib.h>


//��������ṹ��������ͣ������õ���ö������///////////////////////////////////////////////////////////////
//Ϊ�����ϵ�״̬����ö��״̬���������ͱ���
typedef enum {
    STATE_IDLE,					//����״̬
    STATE_ADJUSTING,			//������
    STATE_ADJUSTED,				//��������
	STATE_WAITING_for_20ms		//�ȴ�����
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
	int16_t Final_Temperature;		//����������¶�
	uint16_t Backlight_PWM;			//�������0~100֮�䣬����ֱ�ӿ������ȵ�ʵ�ʱ���
	uint8_t Brightness_Counter;		//����20������
	BacklightController blc;		//����ṹ��ȫ�ֱ���	
}_Backlight;
extern _Backlight Backlight;
//�ⲿ�ļ�Ҫ���õı���////////////////////////////////////////////////////////////////////////////////////

//��������////////////////////////////////////////////////////////////////////////////////////////////////
uint16_t Brightness_control(uint16_t Target_Brightness);
void Backlight_Brightness_Init(BacklightController* blc, uint8_t initialDutyCycle);
void SetTargetDutyCycle(BacklightController* blc, uint8_t target);
uint8_t UpdateBacklightController(BacklightController* blc);
void Modify_PWM_Directly(uint8_t Brightness);
void Update_Brightness(uint8_t target_brightness);




#endif
