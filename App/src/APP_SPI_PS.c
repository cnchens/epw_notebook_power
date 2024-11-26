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

#include	"APP.h"
#include	"STC32G_GPIO.h"
#include	"STC32G_SPI.h"
#include	"STC32G_UART.h"
#include	"STC32G_NVIC.h"

/*************	����˵��	**************

ͨ�����ڷ������ݸ�MCU1��MCU1�����յ���������SPI���͸�MCU2��MCU2��ͨ�����ڷ��ͳ�ȥ.

���÷��� 2��
�����豸��ʼ��ʱ������ SSIG Ϊ 0��MSTR ����Ϊ0����ʱ�����豸���ǲ����� SS �Ĵӻ�ģʽ��
������һ���豸��Ҫ��������ʱ���ȼ�� SS �ܽŵĵ�ƽ�����ʱ��ߵ�ƽ��
�ͽ��Լ����óɺ��� SS ����ģʽ���Լ��� SS ������͵�ƽ�����ͶԷ��� SS �ţ����ɽ������ݴ��䡣

         MCU1                          MCU2
  |-----------------|           |-----------------|
  |            MISO |-----------| MISO            |
--| TX         MOSI |-----------| MOSI         TX |--
  |            SCLK |-----------| SCLK            |
--| RX           SS |-----------| SS           RX |--
  |-----------------|           |-----------------|


����ʱ, ѡ��ʱ�� 24MHz (�����������ļ�"config.h"���޸�).

******************************************/

//========================================================================
//                               ���س�������	
//========================================================================


//========================================================================
//                               ���ر�������
//========================================================================

bit UartReceived=0;

//========================================================================
//                               ���غ�������
//========================================================================


//========================================================================
//                            �ⲿ�����ͱ�������
//========================================================================


//========================================================================
// ����: SPI_PS_init
// ����: �û���ʼ������.
// ����: None.
// ����: None.
// �汾: V1.0, 2020-09-27
//========================================================================
void SPI_PS_init(void)
{
	SPI_InitTypeDef		SPI_InitStructure;
	COMx_InitDefine		COMx_InitStructure;					//�ṹ����

	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//ģʽ,   UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
//	COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;			//ѡ�����ʷ�����, BRT_Timer2 (ע��: ����2�̶�ʹ��BRT_Timer2, ���Բ���ѡ��)
	COMx_InitStructure.UART_BaudRate  = 115200ul;			//������,     110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//��������,   ENABLE �� DISABLE
	UART_Configuration(UART2, &COMx_InitStructure);		//��ʼ������2 UART1,UART2,UART3,UART4
	NVIC_UART2_Init(ENABLE,Priority_1);		//�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3

	SPI_InitStructure.SPI_Enable    = ENABLE;				//SPI����    ENABLE, DISABLE
	SPI_InitStructure.SPI_SSIG      = DISABLE;				//Ƭѡλ     ENABLE(����SS���Ź���), DISABLE(SSȷ�������ӻ�)
	SPI_InitStructure.SPI_FirstBit  = SPI_MSB;				//��λ����   SPI_MSB, SPI_LSB
	SPI_InitStructure.SPI_Mode      = SPI_Mode_Slave;		//����ѡ��   SPI_Mode_Master, SPI_Mode_Slave
	SPI_InitStructure.SPI_CPOL      = SPI_CPOL_Low;			//ʱ����λ   SPI_CPOL_High,   SPI_CPOL_Low
	SPI_InitStructure.SPI_CPHA      = SPI_CPHA_2Edge;		//���ݱ���   SPI_CPHA_1Edge,  SPI_CPHA_2Edge
	SPI_InitStructure.SPI_Speed     = SPI_Speed_4;			//SPI�ٶ�    SPI_Speed_4, SPI_Speed_8, SPI_Speed_16, SPI_Speed_2
	SPI_Init(&SPI_InitStructure);
	NVIC_SPI_Init(ENABLE,Priority_3);		//�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3

	P2_MODE_IO_PU(GPIO_Pin_All);		//P2 ����Ϊ׼˫���
	P4_MODE_IO_PU(GPIO_Pin_6 | GPIO_Pin_7);		//P4.6,P4.7 ����Ϊ׼˫���

	SPI_SS_2 = 1;
}

//========================================================================
// ����: Sample_SPI_PS
// ����: �û�Ӧ�ó���.
// ����: None.
// ����: None.
// �汾: V1.0, 2020-09-27
//========================================================================
void Sample_SPI_PS(void)
{
	u8 i;
	
	if(COM2.RX_TimeOut > 0)
	{
		if(--COM2.RX_TimeOut == 0)
		{
			if(COM2.RX_Cnt > 0)
			{
				UartReceived = 1;   //���ô��ڽ��ձ�־
			}
		}
	}
	if((UartReceived) && (SPI_SS_2))
	{
		SPI_SS_2 = 0;     //���ʹӻ� SS �ܽ�
		SPI_SetMode(SPI_Mode_Master);
		for(i=0;i<COM2.RX_Cnt;i++)
		{
			SPI_WriteByte(RX2_Buffer[i]); //���ʹ�������
		}
		SPI_SS_2 = 1;    //���ߴӻ��� SS �ܽ�
		SPI_SetMode(SPI_Mode_Slave);
		COM2.RX_Cnt = 0;
		UartReceived = 0;
	}
	
	if(SPI_RxTimerOut > 0)
	{
		if(--SPI_RxTimerOut == 0)
		{
			if(SPI_RxCnt > 0)
			{
				for(i=0; i<SPI_RxCnt; i++)	TX2_write2buff(SPI_RxBuffer[i]);
			}
			SPI_RxCnt = 0;
		}
	}
}



