#ifndef _NTC_H_
#define _NTC_H_



#define ADC_TIMEOUT 10000
#define NUM_READS 8



//-----------------------------------------------------------------------------------
//头文件
#include "gd32a50x.h"
//-----------------------------------------------------------------------------------
//宏定义     
#define NTC_ADC_ARRAY_NUM   (sizeof(ntc_adc)/sizeof(uint16_t))
#define INDEX_MAX           (NTC_ADC_ARRAY_NUM-3)                               /* 数组下标的最大可用取值，对应 80℃采样值下标 */
#define INDEX_MIN       3                                                       /* 数组下标的最小可用取值，对应-10℃采样值下标 */
#define TEMPER_REF      -40                                                     /* 温度基准点，即下标为0时对应的温度                    */
#define TEMPER_LAST     150                                                     /* 温度最后值，即数组最后一个对应的温度        */
#define TEMPER_ERROR    1060
//-----------------------------------------------------------------------------------
#define NTC_MAX 		66


//外部变量调用

//外部函数调用

//-----------------------------------------------------------------------------------
//静态函数声明
static int16_t temper_calc(int16_t index,uint16_t adc0_value) ;
static int16_t search_data(uint16_t search_value) ;
//-----------------------------------------------------------------------------------
//函数声明

uint8_t readTemper(uint16_t adc_filter,int16_t *Temp) ;
/////////////////////////////////////////////////////////////////////
uint16_t get_adc_value(void);                                                   //采样
float adc_mean_filter(void);                                                    //采样均值滤波

//-----------------------------------------------------------------------------------









#endif
