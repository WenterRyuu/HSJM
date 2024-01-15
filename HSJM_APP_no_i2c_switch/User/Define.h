#ifndef _DEFINE_H_
#define _DEFINE_H_

//用于屏蔽调试代码，调试过程中打开这个宏定义，调试结束注释掉这一句/////////////////////////////////////////////////////////////
//#define DEBUG_USE_I2C_IN_MAIN
//#define DEBUG_USE_EVAL_BOARD
//#define DEBUG_USE_I2C_ADDRESS8
//#define DEBUG_MAX96752
//#define DEBUG_USE_PA10_PA11_instead_of_PB7_PB8_due_to_swclk_pin


//用于修改参数/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define Temperature_Measurement_Cycle	500//500ms去测一次温度
#define READ_DATA_LOADING_CYCLE			500//【往读数据的数据帧里填入数据】


//全局变量/////////////////////////////////////////////////////////////



//头文件//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//其他头文件
#include "systick.h"
#include <stdbool.h>

//Lib\Inc文件夹里的头文件
#include "gpio_App.h"
#include "rcu_App.h"
#include "timer_App.h"
#include "adc_App.h"
#include "spi_App.h"
#include "i2c_App.h"

//User\Inc文件夹里的头文件
#include "Initialization.h"
#include "Power.h"
#include "MAX96752.h"
#include "Backlight.h"
#include "NTC.h"
#include "MPQ3367.h"
#include "External_detection.h"
#include "Tcon.h"
#include "Control.h"

//DATAFRAME
#include "ReadDataFrame.h"
#include "WriteDataFrame.h"


//为开机上电状态机的枚举状态，创建类型别名
typedef enum {
    START,
	Waiting_for_RESET_ACTION,
    RESET_ACTION,
    Waiting_for_STBYB_ACTION,
	STBYB_ACTION,
	Waiting_for_LedDriverEn_ACTION,
    LedDriverEn_ACTION,
	END
}Power_State;

//io口宏定义
#define 	RESET_PORT				GPIOE
#define		RESET_PIN				GPIO_PIN_14
#define		STBYB_PORT				GPIOA
#define		STBYB_PIN				GPIO_PIN_4
#define 	LED_DRIVER_EN_PORT		GPIOB
#define 	LED_DRIVER_EN_PIN		GPIO_PIN_3

//上电时序io口控制函数封装
#define		RESET_SET_HIGH			gpio_bit_set(GPIOE, GPIO_PIN_14)
#define		RESET_SET_LOW			gpio_bit_reset(GPIOE, GPIO_PIN_14)
#define		STBYB_SET_HIGH			gpio_bit_set(GPIOA, GPIO_PIN_4)
#define		STBYB_SET_LOW			gpio_bit_reset(GPIOA, GPIO_PIN_4)
#define		LED_DRIVER_ENABLE		gpio_bit_set(GPIOB, GPIO_PIN_3)
#define		LED_DRIVER_DISABLE		gpio_bit_reset(GPIOB, GPIO_PIN_3)

#define		BEEP_ON			gpio_bit_set(GPIOA, GPIO_PIN_7)
#define		BEEP_OFF		gpio_bit_reset(GPIOA, GPIO_PIN_7)


#define I2C_REC_MAX   50
#define I2C_SLAVE_REC_MAX_SIZE  50
//i2c结构体定义
typedef union  _uI2cFlag_Union{
    uint8_t Byte ;
    struct {
        uint8_t RecSuccess:1 ;
        uint8_t SendSucess:1 ;//
        uint8_t DirState  :1 ;
        uint8_t Update    :1 ;
        uint8_t Unused    :4 ;
    }Bits ;
}uI2cFlag_Union ;

typedef struct _tI2c_Type{
    uI2cFlag_Union  uFlag ;
    uint8_t RecBuff[I2C_REC_MAX] ;
    uint8_t Send_Buff[255] ;
    uint16_t SendSize ;
    uint16_t SendAddr ;
    uint16_t SendCount ;
    uint16_t RecCount ; 
}tI2c_Type ;
extern tI2c_Type tI2cSlave ;
extern tI2c_Type Update_tI2cSlave;

#define LENGTH_of_(array) (sizeof(array) / sizeof(array[0]))
	

typedef struct{
	uint16_t NTC;
	uint16_t LOCK;
	uint16_t RData_Loading_Cycle;
	uint8_t IRQ_0X13;
	uint8_t IRQ_0X11;
	//.下面三个用不到
	uint16_t Timeout_in_Updating;
	uint16_t Waiting_in_Updating;
	uint8_t IRQ_Update;
}_Counter_1ms;
extern _Counter_1ms Counter_1ms;

//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
//---------------------------以下是升级要用的-----------------------------------------------------------------------------

#include "Update.h"

#define FMC_PAGE_SIZE     ((uint16_t)0x400U)

#define BOOT_START_PAGE 	0
#define APP_START_PAGE		20
#define BACKUP_START_PAGE	130
#define UPDATE_FLAG_PAGE	240

#define BOOT_START_ADDRESS		(uint32_t)(0X08000000)
#define APP_START_ADDRESS		(uint32_t)(0x08005000)
#define BACKUP_START_ADDRESS	(uint32_t)(0x08020800)
#define UPDATE_FLAG_ADDRESS		(uint32_t)(0x0803C000)
#define APP2BOOT_FLAG_ADDRESS	(uint32_t)(0x0803C400)

#define BOOT_PAGE_NUM 	20
#define APP_PAGE_NUM	110
#define BACKUP_PAGE_NUM	110
#define UPDATE_PAGE_NUM	1




#endif
