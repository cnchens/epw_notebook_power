/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCMCU.com --------------------------------------------*/
/* --- Web: www.STCMCUDATA.com  ---------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* ���Ҫ�ڳ�����ʹ�ô˴���,���ڳ�����ע��ʹ����STC�����ϼ�����            */
/*---------------------------------------------------------------------*/

#ifndef __APP_H_
#define __APP_H_

//========================================================================
//                                ͷ�ļ�
//========================================================================

#include	"config.h"
#include	"APP_Lamp.h"
#include	"APP_AD_UART.h"
#include	"APP_INT_UART.h"
#include	"APP_RTC.h"
#include	"APP_I2C_PS.h"
#include	"APP_SPI_PS.h"
#include	"APP_WDT.h"
#include	"APP_PWM.h"
#include	"APP_EEPROM.h"
#include	"APP_DMA_AD.h"
#include	"APP_DMA_M2M.h"
#include	"APP_DMA_UART.h"
#include	"APP_DMA_SPI_PS.h"
#include	"APP_DMA_LCM.h"
#include	"APP_DMA_I2C.h"
#include	"APP_CAN.h"
#include	"APP_LIN.h"
#include	"APP_USART_LIN.h"
#include	"APP_USART2_LIN.h"
#include	"APP_HSSPI.h"
#include	"APP_HSPWM.h"

//========================================================================
//                               ���س�������	
//========================================================================

#define DIS_DOT     0x20
#define DIS_BLACK   0x10
#define DIS_        0x11

extern u8 code t_display[];

extern u8 code T_COM[];      //λ��

extern u8 code T_KeyTable[16];

//========================================================================
//                            �ⲿ�����ͱ�������
//========================================================================

extern u8  LED8[8];        //��ʾ����
extern u8  display_index;  //��ʾλ����

extern u8  IO_KeyState, IO_KeyState1, IO_KeyHoldCnt;    //���м��̱���
extern u8  KeyHoldCnt; //�����¼�ʱ
extern u8  KeyCode;    //���û�ʹ�õļ���
extern u8  cnt50ms;

extern u8  hour,minute,second; //RTC����
extern u16 msecond;

void APP_config(void);
void DisplayScan(void);

#endif
