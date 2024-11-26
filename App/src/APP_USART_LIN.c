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

#include	"APP_USART_LIN.h"
#include	"STC32G_USART_LIN.h"
#include	"STC32G_GPIO.h"
#include	"STC32G_UART.h"
#include	"STC32G_NVIC.h"
#include	"STC32G_Switch.h"

/*************   ����˵��   ***************

Lin����ģʽ�շ����ԣ�
��һ��P32�ڰ���, ������������һ֡����.
��һ��P33�ڰ���, ��������֡ͷ����ȡ�ӻ�Ӧ�����ݣ��ϲ���һ��������֡��.

Lin�ӻ�ģʽ�շ����ԣ�
�յ�һ���Ǳ���Ӧ�������֡��ͨ������2���.
�յ�һ������Ӧ���֡ͷ��(���磺ID=0x12), ���ͻ������ݽ���Ӧ��.
��Ҫ�޸�ͷ�ļ� "STC32G_UART.h" ��Ķ��� "#define	PRINTF_SELECT  UART2"��ͨ������2��ӡ��Ϣ

Ĭ�ϴ������ʣ�9600������, �û��������޸�.

����ʱ, ѡ��ʱ�� 24MHz (�û�����"config.h"�޸�Ƶ��).

******************************************/

sbit SLP_N  = P5^2;     //0: Sleep

//========================================================================
//                               ���س�������	
//========================================================================

#define	USART1_LIN_MASTER_MODE     1    //0: �ӻ�ģʽ; 1: ����ģʽ

//========================================================================
//                               ���ر�������
//========================================================================

u8 ULin_ID;
u8 USART_BUF[8];

//========================================================================
//                               ���غ�������
//========================================================================


//========================================================================
//                            �ⲿ�����ͱ�������
//========================================================================

extern bit B_ULinRX1_Flag;

extern u8 Key1_cnt;
extern u8 Key2_cnt;
extern bit Key1_Flag;
extern bit Key2_Flag;

//========================================================================
// ����: LIN_init
// ����: �û���ʼ������.
// ����: None.
// ����: None.
// �汾: V1.0, 2022-03-27
//========================================================================
void USART_LIN_init(void)
{
	USARTx_LIN_InitDefine	LIN_InitStructure;				//�ṹ����
	COMx_InitDefine COMx_InitStructure;				//�ṹ����

	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//ģʽ,   UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
//	COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;			//ѡ�����ʷ�����, BRT_Timer2 (ע��: ����2�̶�ʹ��BRT_Timer2, ���Բ���ѡ��)
	COMx_InitStructure.UART_BaudRate  = 115200ul;			//������,     110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//��������,   ENABLE �� DISABLE
	UART_Configuration(UART2, &COMx_InitStructure);		//��ʼ������2 UART1,UART2,UART3,UART4
	NVIC_UART2_Init(ENABLE,Priority_1);		//�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3

#if(USART1_LIN_MASTER_MODE==1)
	LIN_InitStructure.LIN_Mode = LinMasterMode;	//LIN����ģʽ  	LinMasterMode,LinSlaveMode
	LIN_InitStructure.LIN_AutoSync = DISABLE;		//�Զ�ͬ��ʹ��  	ENABLE,DISABLE
#else
	LIN_InitStructure.LIN_Mode = LinSlaveMode;	//LIN����ģʽ  	LinMasterMode,LinSlaveMode
	LIN_InitStructure.LIN_AutoSync = ENABLE;		//�Զ�ͬ��ʹ��  	ENABLE,DISABLE
#endif
	LIN_InitStructure.LIN_Enable   = ENABLE;		//LIN����ʹ��  	ENABLE,DISABLE
	LIN_InitStructure.LIN_Baudrate = 9600;			//LIN������
	UASRT_LIN_Configuration(USART1,&LIN_InitStructure);						//LIN ��ʼ��

	NVIC_UART1_Init(ENABLE,Priority_1);		//�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3

	P4_MODE_IO_PU(GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_6 | GPIO_Pin_7);		//P4.3,P4.4,P4.6,P4.7 ����Ϊ׼˫���
	P5_MODE_IO_PU(GPIO_Pin_2);		//P5.2 ����Ϊ׼˫���
	
	UART1_SW(UART1_SW_P43_P44);		//UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17,UART1_SW_P43_P44

	//====��ʼ������=====
	SLP_N = 1;
	ULin_ID = 0x32;
	USART_BUF[0] = 0x81;
	USART_BUF[1] = 0x22;
	USART_BUF[2] = 0x33;
	USART_BUF[3] = 0x44;
	USART_BUF[4] = 0x55;
	USART_BUF[5] = 0x66;
	USART_BUF[6] = 0x77;
	USART_BUF[7] = 0x88;
}

//========================================================================
// ����: Sample_LIN
// ����: �û�Ӧ�ó���.
// ����: None.
// ����: None.
// �汾: V1.0, 2022-03-27
//========================================================================
void Sample_USART_LIN(void)
{
	u8 i;

#if(USART1_LIN_MASTER_MODE==1)
	if(!P32)
	{
		if(!Key1_Flag)
		{
			Key1_cnt++;
			if(Key1_cnt > 50)
			{
				Key1_Flag = 1;
				UsartLinSendFrame(USART1,ULin_ID, USART_BUF, FRAME_LEN);  //����һ����������
			}
		}
	}
	else
	{
		Key1_cnt = 0;
		Key1_Flag = 0;
	}

	if(!P33)
	{
		if(!Key2_Flag)
		{
			Key2_cnt++;
			if(Key2_cnt > 50)
			{
				Key2_Flag = 1;
				UsartLinSendHeader(USART1,0x13);  //����֡ͷ����ȡ����֡�����һ��������֡
			}
		}
	}
	else
	{
		Key2_cnt = 0;
		Key2_Flag = 0;
	}
#else
	if((B_ULinRX1_Flag) && (COM1.RX_Cnt >= 2))
	{
		B_ULinRX1_Flag = 0;

		if((RX1_Buffer[0] == 0x55) && ((RX1_Buffer[1] & 0x3f) == 0x12)) //PID -> ID
		{
			UsartLinSendData(USART1,USART_BUF, FRAME_LEN);
			UsartLinSendChecksum(USART1,USART_BUF, FRAME_LEN);
		}
	}
#endif

	if(COM1.RX_TimeOut > 0)     //��ʱ����
	{
		if(--COM1.RX_TimeOut == 0)
		{
			printf("Read Cnt = %d.\r\n",COM1.RX_Cnt);
			for(i=0; i<COM1.RX_Cnt; i++)    printf("0x%02x ",RX1_Buffer[i]);    //�Ӵ�������յ��Ĵӻ�����
			COM1.RX_Cnt  = 0;   //����ֽ���
			printf("\r\n");
		}
	}
}

