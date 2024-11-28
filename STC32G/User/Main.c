/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCMCU.com --------------------------------------------*/
/* --- Web: www.STCMCUDATA.com  ---------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序            */
/*---------------------------------------------------------------------*/

#include "config.h"
#include "Task.h"
#include "System_init.h"\
#include "STC32G_GPIO.h"
#include "STC32G_NVIC.h"
#include "STC32G_I2C.h"
#include "STC32G_DMA.h"

// Define I2C Addresses
#define MCU_EEPROM 0xAE
#define CRPS_MCU 0xB0
#define CRPS_EEPROM 0xA0
#define OLED 0x78

//========================================================================
// Define: void start_gpio(void)
// Desc: Init MCU GPIOs
// Data: x
// Return: x
// Ver: V1.0
//========================================================================
void start_gpio(void)
{
	P0_MODE_IO_PU(GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2);
	P1_MODE_IO_PU(GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);
	P2_MODE_OUT_OD(GPIO_Pin_1);
	P3_MODE_IO_PU(GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2);
	P4_MODE_OUT_PP(GPIO_Pin_7);
	P4_MODE_IO_PU(GPIO_Pin_4);
	P5_MODE_IO_PU(GPIO_Pin_4);
}

//========================================================================
// Define: void start_i2c(void)
// Desc: Init MCU master I2C
// Data: x
// Return: x
// Ver: V1.0
//========================================================================
void start_i2c(void)
{
	I2C_InitTypeDef I2C_InitStruct;
	I2C_InitStruct.I2C_Enable = ENABLE;
	I2C_InitStruct.I2C_Mode = I2C_Mode_Master;
	I2C_InitStruct.I2C_MS_WDTA = DISABLE;
	I2C_Init(&I2C_InitStruct);
	NVIC_I2C_Init(I2C_Mode_Master, ENABLE, Priority_0);
}

//========================================================================
// Define: void start_dma(void)
// Desc: Init MCU DMA
// Data: x
// Return: x
// Ver: V1.0
//========================================================================
void start_dma(void)
{
	DMA_I2C_InitTypeDef DMA_I2C_InitStruct;
	DMA_InitStruct.DMA_Enable = ENABLE

//========================================================================
// Define: void read_pmbus_eeprom(void)
// Desc: Read CRPS EEPROM 
// Data: 
// Return: 
// Ver: V1.0
//========================================================================
void read_pmbus_eeprom()
{
	I2C_ReadNbyte(CRPS_EEPROM, )
}

//========================================================================
// 函数: void	main(void)
// 描述: 主函数程序.
// 参数: None.
// 返回: None.
// 版本: V1.0, 2012-10-22
//========================================================================
void main(void)
{
	WTST = 0;		//设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
	EAXSFR();		//扩展SFR(XFR)访问使能 
	CKCON = 0;      //提高访问XRAM速度

	SYS_Init();
	
	start_gpio();
	start_i2c();
	EA = 1;
	
	
}
