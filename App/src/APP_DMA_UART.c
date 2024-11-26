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

#include	"APP_DMA_UART.h"
#include	"STC32G_GPIO.h"
#include	"STC32G_UART.h"
#include	"STC32G_NVIC.h"
#include	"STC32G_DMA.h"


/*************	功能说明	**************

通过PC向MCU发送数据, MCU将收到的数据自动存入DMA空间.

当DMA空间存满设置大小的内容后，通过串口的DMA自动发送功能把存储空间的数据原样返回.

用定时器做波特率发生器，建议使用1T模式(除非低波特率用12T)，并选择可被波特率整除的时钟频率，以提高精度。

下载时, 选择时钟 24MHz (可以在配置文件"config.h"中修改).

******************************************/


//========================================================================
//                               本地常量声明	
//========================================================================


//========================================================================
//                               本地变量声明
//========================================================================

u8 xdata DmaBuffer[256];	//收发共用缓存，同时使用多路串口时每个串口需分别定义缓存，以免相互干扰

//========================================================================
//                               本地函数声明
//========================================================================


//========================================================================
//                            外部函数和变量声明
//========================================================================


//========================================================================
// 函数: DMA_UART_init
// 描述: 用户初始化程序.
// 参数: None.
// 返回: None.
// 版本: V1.0, 2021-05-27
//========================================================================
void DMA_UART_init(void)
{
	u16	i;
	COMx_InitDefine		COMx_InitStructure;					//结构定义
	DMA_UART_InitTypeDef		DMA_UART_InitStructure;		//结构定义
	
	//----------------------------------------------
//	P3_MODE_IO_PU(GPIO_Pin_0 | GPIO_Pin_1);	//P3.0,P3.1 设置为准双向口 - UART1
	P4_MODE_IO_PU(GPIO_Pin_6 | GPIO_Pin_7);		//P4.6,P4.7 设置为准双向口 - UART2
//	P0_MODE_IO_PU(GPIO_Pin_0 | GPIO_Pin_1);	//P0.0,P0.1 设置为准双向口 - UART3
//	P0_MODE_IO_PU(GPIO_Pin_2 | GPIO_Pin_3);	//P0.2,P0.3 设置为准双向口 - UART4
	
	//----------------------------------------------
	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;	//模式,   UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;			//选择波特率发生器, BRT_Timer1, BRT_Timer2 (注意: 串口2固定使用BRT_Timer2)
	COMx_InitStructure.UART_BaudRate  = 115200ul;			//波特率,     110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//接收允许,   ENABLE或DISABLE
//	UART_Configuration(UART1, &COMx_InitStructure);		//初始化串口 UART1,UART2,UART3,UART4
//	NVIC_UART1_Init(ENABLE,Priority_0);		//中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
	UART_Configuration(UART2, &COMx_InitStructure);		//初始化串口 UART1,UART2,UART3,UART4
	NVIC_UART2_Init(ENABLE,Priority_0);		//中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
//	UART_Configuration(UART3, &COMx_InitStructure);		//初始化串口 UART1,UART2,UART3,UART4
//	NVIC_UART3_Init(ENABLE,Priority_0);		//中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
//	UART_Configuration(UART4, &COMx_InitStructure);		//初始化串口 UART1,UART2,UART3,UART4
//	NVIC_UART4_Init(ENABLE,Priority_0);		//中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3

	printf("STC32G UART DMA Test Programme!\r\n");  //UART发送一个字符串

	//----------------------------------------------
	DMA_UART_InitStructure.DMA_TX_Length = 255;				//DMA传输总字节数  	(0~65535) + 1
	DMA_UART_InitStructure.DMA_TX_Buffer = (u16)DmaBuffer;	//发送数据存储地址
	DMA_UART_InitStructure.DMA_RX_Length = 255;				//DMA传输总字节数  	(0~65535) + 1
	DMA_UART_InitStructure.DMA_RX_Buffer = (u16)DmaBuffer;	//接收数据存储地址
	DMA_UART_InitStructure.DMA_TX_Enable = ENABLE;		//DMA使能  	ENABLE,DISABLE
	DMA_UART_InitStructure.DMA_RX_Enable = ENABLE;		//DMA使能  	ENABLE,DISABLE
//	DMA_UART_Inilize(UART1, &DMA_UART_InitStructure);	//初始化
	DMA_UART_Inilize(UART2, &DMA_UART_InitStructure);	//初始化
//	DMA_UART_Inilize(UART3, &DMA_UART_InitStructure);	//初始化
//	DMA_UART_Inilize(UART4, &DMA_UART_InitStructure);	//初始化

//	NVIC_DMA_UART1_Tx_Init(ENABLE,Priority_0,Priority_0);		//中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0~Priority_3; 总线优先级(低到高) Priority_0~Priority_3
//	NVIC_DMA_UART1_Rx_Init(ENABLE,Priority_0,Priority_0);		//中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0~Priority_3; 总线优先级(低到高) Priority_0~Priority_3
	NVIC_DMA_UART2_Tx_Init(ENABLE,Priority_0,Priority_0);		//中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0~Priority_3; 总线优先级(低到高) Priority_0~Priority_3
	NVIC_DMA_UART2_Rx_Init(ENABLE,Priority_0,Priority_0);		//中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0~Priority_3; 总线优先级(低到高) Priority_0~Priority_3
//	NVIC_DMA_UART3_Tx_Init(ENABLE,Priority_0,Priority_0);		//中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0~Priority_3; 总线优先级(低到高) Priority_0~Priority_3
//	NVIC_DMA_UART3_Rx_Init(ENABLE,Priority_0,Priority_0);		//中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0~Priority_3; 总线优先级(低到高) Priority_0~Priority_3
//	NVIC_DMA_UART4_Tx_Init(ENABLE,Priority_0,Priority_0);		//中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0~Priority_3; 总线优先级(低到高) Priority_0~Priority_3
//	NVIC_DMA_UART4_Rx_Init(ENABLE,Priority_0,Priority_0);		//中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0~Priority_3; 总线优先级(低到高) Priority_0~Priority_3

//	DMA_UR1R_CLRFIFO();		//清空 DMA FIFO
	DMA_UR2R_CLRFIFO();		//清空 DMA FIFO
//	DMA_UR3R_CLRFIFO();		//清空 DMA FIFO
//	DMA_UR4R_CLRFIFO();		//清空 DMA FIFO
	//----------------------------------------------
	DmaTx1Flag = 0;
	DmaRx1Flag = 0;
	DmaTx2Flag = 0;
	DmaRx2Flag = 0;
	DmaTx3Flag = 0;
	DmaRx3Flag = 0;
	DmaTx4Flag = 0;
	DmaRx4Flag = 0;
	for(i=0; i<256; i++)
	{
		DmaBuffer[i] = i;
	}
//	DMA_UR1T_TRIG();	//触发UART1发送功能
//	DMA_UR1R_TRIG();	//触发UART1接收功能
	DMA_UR2T_TRIG();	//触发UART2发送功能
	DMA_UR2R_TRIG();	//触发UART2接收功能
//	DMA_UR3T_TRIG();	//触发UART3发送功能
//	DMA_UR3R_TRIG();	//触发UART3接收功能
//	DMA_UR4T_TRIG();	//触发UART4发送功能
//	DMA_UR4R_TRIG();	//触发UART4接收功能
}

//========================================================================
// 函数: Sample_DMA_UART
// 描述: 用户应用程序.
// 参数: None.
// 返回: None.
// 版本: V1.0, 2021-05-27
//========================================================================
void Sample_DMA_UART(void)
{
	if((DmaTx1Flag) && (DmaRx1Flag))	//数据需要及时处理，使用延时或者定时器周期性处理，可能重新触发不及时导致外部连续收发的部分数据丢失
	{
		DmaTx1Flag = 0;
		DmaRx1Flag = 0;
		DMA_UR1T_TRIG();	//重新触发UART1发送功能，可在DMA中断里及时处理
		DMA_UR1R_TRIG();	//重新触发UART1接收功能，可在DMA中断里及时处理
	}

	if((DmaTx2Flag) && (DmaRx2Flag))	//数据需要及时处理，使用延时或者定时器周期性处理，可能重新触发不及时导致外部连续收发的部分数据丢失
	{
		DmaTx2Flag = 0;
		DmaRx2Flag = 0;
		DMA_UR2T_TRIG();	//重新触发UART2发送功能，可在DMA中断里及时处理
		DMA_UR2R_TRIG();	//重新触发UART2接收功能，可在DMA中断里及时处理
	}

	if((DmaTx3Flag) && (DmaRx3Flag))	//数据需要及时处理，使用延时或者定时器周期性处理，可能重新触发不及时导致外部连续收发的部分数据丢失
	{
		DmaTx3Flag = 0;
		DmaRx3Flag = 0;
		DMA_UR3T_TRIG();	//重新触发UART3发送功能，可在DMA中断里及时处理
		DMA_UR3R_TRIG();	//重新触发UART3接收功能，可在DMA中断里及时处理
	}

	if((DmaTx4Flag) && (DmaRx4Flag))	//数据需要及时处理，使用延时或者定时器周期性处理，可能重新触发不及时导致外部连续收发的部分数据丢失
	{
		DmaTx4Flag = 0;
		DmaRx4Flag = 0;
		DMA_UR4T_TRIG();	//重新触发UART4发送功能，可在DMA中断里及时处理
		DMA_UR4R_TRIG();	//重新触发UART4接收功能，可在DMA中断里及时处理
	}
}



