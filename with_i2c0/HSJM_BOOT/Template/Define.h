#ifndef _DEFINE_H_
#define _DEFINE_H_
#define I2C_REC_MAX                 50		//S19�ļ�һ�в�78���ַ�
      

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <gd32a50x.h>
#include "systick.h"
#include "gd32a50x_fmc.h"
#include "Update.h"
#include "fmc_App.h"
#include "i2c_App.h"



#define I2C_REC_MAX   50
#define I2C_SLAVE_REC_MAX_SIZE  50

#define FMC_PAGE_SIZE     ((uint16_t)0x400U)

#define BOOT_START_PAGE 	0
#define APP_START_PAGE		20
#define BACKUP_START_PAGE	130
#define UPDATE_FLAG_PAGE	240

#define BOOT_START _ADDRESS		(uint32_t)(0X08000000)
#define APP_START_ADDRESS		(uint32_t)(0x08005000)
#define BACKUP_START_ADDRESS	(uint32_t)(0x08020800)
#define UPDATE_FLAG_ADDRESS		(uint32_t)(0x0803C000)
#define APP2BOOT_FLAG_ADDRESS	(uint32_t)(0x0803C400)

#define BOOT_PAGE_NUM 	20
#define APP_PAGE_NUM	110
#define BACKUP_PAGE_NUM	110
#define UPDATE_PAGE_NUM	1

#define IRQ_LOW_DOWN 	gpio_bit_reset(GPIOB, GPIO_PIN_14)
#define IRQ_RELEASE 	gpio_bit_set(GPIOB, GPIO_PIN_14)

//typedef struct _FmcProc_Type{    
//    uint8_t Page ;          //�ڵڼ�ҳ
//    uint16_t Length ;       //д���ݳ���  
//    int16_t Addr ;          //�ڱ�ҳд���ݳ�ʼ��ַ                                                                 
//    uint16_t *Content ;     //д��������ָ��                                                                                     
//}FmcProc_Type ;
//extern FmcProc_Type FmcUpdate ;
typedef union  _uI2cFlag_Union{
    uint8_t Byte ;
    struct {
        uint8_t RecSuccess:1 ;
        uint8_t SendSucess:1 ;
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
extern tI2c_Type Update_tI2cSlave;

#define Reply_Length_Max 11
typedef struct
{
	uint8_t Header;
	uint8_t Direction;
	uint8_t Return_frame_length;
	uint8_t Data_length;
    uint8_t ReadData[Reply_Length_Max];//Lcd���ص�����
    uint8_t checksum;//checksum = ReadCmd_Id + ReadData[] + ExtLength
} Std_ReplyFrameStr;//��׼֡(��)�ṹ��,���ڷ��͸�������Ҫ������
#define Receive_Length_Max 255
typedef struct
{
	uint8_t Header;
	uint8_t Direction;
	uint8_t Return_frame_length;
	uint8_t Data_length;
    uint8_t ReadData[Receive_Length_Max];//Lcd���յ�����
    uint8_t checksum;//checksum = ReadCmd_Id + ReadData[] + ExtLength
} Std_ReceiveFrameStr;//��׼֡(��)�ṹ��,���ڷ��͸�������Ҫ������

typedef union _uUpFlag_uni{
    uint8_t Byte ;
    struct{
        uint8_t Sucess :1 ; //�洢�����ݿռ�ɹ�
        uint8_t Save   :1 ; //���Դ洢������
        uint8_t Erase  :1 ;
		uint8_t Finish :1 ;
        uint8_t UnUse  :4 ;
    }Bits ;
}uUpFlag_uni ;
//typedef struct _tUpdata_Type{
//	uUpFlag_uni Flag;
//    uint8_t Error ;         //�������ݷ�������
//    uint8_t CrcData ;       //4λ�����У��ֵ 
//    uint8_t CrcBin ;        //��������bin�ļ���У��ֵ
//    uint8_t CrcCal ;        //��������bin�ļ�У��ֵ
//    uint8_t XC8 ;
//    uint16_t PacketSum ;    //�����ܹ�����
//    uint16_t PackSize ;     //�����һ���⣬������������
//    uint16_t PackEndSize ;  //����һ����������
//    uint16_t Page ;         //ҳ��
//    uint16_t Addr ;         //һҳ�ĵ�ַ
//    uint16_t Size ;
//    uint16_t Length ;       //��Ҫд���ݵĳ���
//}tUpdata_Type ;
//extern tUpdata_Type tUpdata ;
typedef struct _tAgree_Type{
    tI2c_Type   tI2cProc ;
    uint8_t     FanNum ;
    uint16_t FmcNorSpeedAddr ; //�����ٶ���FLASH����ĵ�ַ
    uint16_t FmcOtherAddr ;     //�����ٶȣ�efuse������led״̬  
}tAgree_Type  ;



typedef struct{
	uint8_t CheckBootloaderStatus;
	uint8_t AppErase;
	uint8_t StartProgramming;
	uint8_t SendS19;
	uint8_t FlashChecksum;
}_Times;//���Դ���



#endif
