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

#include	"APP_DMA_SPI_PS.h"
#include	"STC32G_GPIO.h"
#include	"STC32G_SPI.h"
#include	"STC32G_UART.h"
#include	"STC32G_NVIC.h"
#include	"STC32G_DMA.h"

/*************	����˵��	**************

UART_DMA, M2M_DMA, SPI_DMA �ۺ�ʹ����ʾ����.

ͨ�����ڷ������ݸ�MCU1��MCU1�����յ���������SPI���͸�MCU2��MCU2��ͨ�����ڷ��ͳ�ȥ.

ͨ�����ڷ������ݸ�MCU2��MCU2�����յ���������SPI���͸�MCU1��MCU1��ͨ�����ڷ��ͳ�ȥ.

MCU1/MCU2: UART���� -> UART Rx DMA -> M2M -> SPI Tx DMA -> SPI����

MCU2/MCU1: SPI���� -> SPI Rx DMA -> M2M -> UART Tx DMA -> UART����

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

#define BUF_LENGTH          107			//n+1

//========================================================================
//                               ���ر�������
//========================================================================

u8 xdata UartTxBuffer[256];
u8 xdata UartRxBuffer[256];
u8 xdata SpiTxBuffer[256];
u8 xdata SpiRxBuffer[256];

//========================================================================
//                               ���غ�������
//========================================================================


//========================================================================
//                            �ⲿ�����ͱ�������
//========================================================================


//========================================================================
// ����: DMA_SPI_PS_init
// ����: �û���ʼ������.
// ����: None.
// ����: None.
// �汾: V1.0, 2021-05-27
//========================================================================
void DMA_SPI_PS_init(void)
{
	SPI_InitTypeDef		SPI_InitStructure;
	COMx_InitDefine		COMx_InitStructure;				//�ṹ����
	DMA_M2M_InitTypeDef		DMA_M2M_InitStructure;		//�ṹ����
	DMA_SPI_InitTypeDef		DMA_SPI_InitStructure;		//�ṹ����
	DMA_UART_InitTypeDef	DMA_UART_InitStructure;		//�ṹ����

	//----------------------------------------------
	P2_MODE_IO_PU(GPIO_Pin_All);		//P2 ����Ϊ׼˫���
	P4_MODE_IO_PU(GPIO_Pin_6 | GPIO_Pin_7);		//P4.6,P4.7 ����Ϊ׼˫���

	SPI_SS_2 = 1;

	//----------------------------------------------
	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//ģʽ,   UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
//	COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;			//ѡ�����ʷ�����, BRT_Timer2 (ע��: ����2�̶�ʹ��BRT_Timer2, ���Բ���ѡ��)
	COMx_InitStructure.UART_BaudRate  = 115200ul;			//������,     110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//��������,   ENABLE �� DISABLE
	UART_Configuration(UART2, &COMx_InitStructure);		//��ʼ������2 UART1,UART2,UART3,UART4
	NVIC_UART2_Init(ENABLE,Priority_1);		//�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
	printf("STC32G UART-DMA-SPI��Ϊ����͸������.\r\n");

	//----------------------------------------------
	SPI_InitStructure.SPI_Enable    = ENABLE;				//SPI����    ENABLE, DISABLE
	SPI_InitStructure.SPI_SSIG      = DISABLE;				//Ƭѡλ     ENABLE(����SS���Ź���), DISABLE(SSȷ�������ӻ�)
	SPI_InitStructure.SPI_FirstBit  = SPI_MSB;				//��λ����   SPI_MSB, SPI_LSB
	SPI_InitStructure.SPI_Mode      = SPI_Mode_Slave;		//����ѡ��   SPI_Mode_Master, SPI_Mode_Slave
	SPI_InitStructure.SPI_CPOL      = SPI_CPOL_Low;			//ʱ����λ   SPI_CPOL_High,   SPI_CPOL_Low
	SPI_InitStructure.SPI_CPHA      = SPI_CPHA_1Edge;		//���ݱ���   SPI_CPHA_1Edge,  SPI_CPHA_2Edge
	SPI_InitStructure.SPI_Speed     = SPI_Speed_16;			//SPI�ٶ�    SPI_Speed_4, SPI_Speed_8, SPI_Speed_16, SPI_Speed_2
	SPI_Init(&SPI_InitStructure);
	NVIC_SPI_Init(DISABLE,Priority_0);		//�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3

	//----------------------------------------------
	DMA_UART_InitStructure.DMA_TX_Length = BUF_LENGTH;	//DMA�������ֽ���  	(0~65535) + 1
	DMA_UART_InitStructure.DMA_TX_Buffer = (u16)UartTxBuffer;	//�������ݴ洢��ַ
	DMA_UART_InitStructure.DMA_RX_Length = BUF_LENGTH;	//DMA�������ֽ���  	(0~65535) + 1
	DMA_UART_InitStructure.DMA_RX_Buffer = (u16)UartRxBuffer;	//�������ݴ洢��ַ
	DMA_UART_InitStructure.DMA_TX_Enable = ENABLE;		//DMAʹ��  	ENABLE,DISABLE
	DMA_UART_InitStructure.DMA_RX_Enable = ENABLE;		//DMAʹ��  	ENABLE,DISABLE
	DMA_UART_Inilize(UART2, &DMA_UART_InitStructure);	//��ʼ��

	NVIC_DMA_UART2_Tx_Init(ENABLE,Priority_0,Priority_0);		//�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0~Priority_3; �������ȼ�(�͵���) Priority_0~Priority_3
	NVIC_DMA_UART2_Rx_Init(ENABLE,Priority_0,Priority_0);		//�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0~Priority_3; �������ȼ�(�͵���) Priority_0~Priority_3
	DMA_UR2R_CLRFIFO();		//��� DMA FIFO
	DMA_UR2R_TRIG();	//����UART���չ���

	//----------------------------------------------
	DMA_M2M_InitStructure.DMA_Enable = ENABLE;			//DMAʹ��  	ENABLE,DISABLE
	DMA_M2M_InitStructure.DMA_Length = BUF_LENGTH;			//DMA�������ֽ���  	(0~65535) + 1
	DMA_M2M_InitStructure.DMA_Tx_Buffer = (u16)UartRxBuffer;	//�������ݴ洢��ַ
	DMA_M2M_InitStructure.DMA_Rx_Buffer = (u16)SpiTxBuffer;	//�������ݴ洢��ַ
	DMA_M2M_InitStructure.DMA_SRC_Dir = M2M_ADDR_INC;		//����Դ��ַ�ı䷽��  	M2M_ADDR_INC,M2M_ADDR_DEC
	DMA_M2M_InitStructure.DMA_DEST_Dir = M2M_ADDR_INC;	//����Ŀ���ַ�ı䷽�� 	M2M_ADDR_INC,M2M_ADDR_DEC
	DMA_M2M_Inilize(&DMA_M2M_InitStructure);		//��ʼ��
	NVIC_DMA_M2M_Init(ENABLE,Priority_0,Priority_0);		//�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0~Priority_3; �������ȼ�(�͵���) Priority_0~Priority_3

	//----------------------------------------------
	DMA_SPI_InitStructure.DMA_Enable = DISABLE;					//DMAʹ��  	ENABLE,DISABLE
	DMA_SPI_InitStructure.DMA_Tx_Enable = ENABLE;				//DMA��������ʹ��  	ENABLE,DISABLE
	DMA_SPI_InitStructure.DMA_Rx_Enable = ENABLE;				//DMA��������ʹ��  	ENABLE,DISABLE
	DMA_SPI_InitStructure.DMA_Length = BUF_LENGTH;			//DMA�������ֽ���  	(0~65535) + 1
	DMA_SPI_InitStructure.DMA_Tx_Buffer = (u16)SpiTxBuffer;	//�������ݴ洢��ַ
	DMA_SPI_InitStructure.DMA_Rx_Buffer = (u16)SpiRxBuffer;	//�������ݴ洢��ַ
	DMA_SPI_InitStructure.DMA_SS_Sel = SPI_SS_P22;			//�Զ�����SS��ѡ�� 	SPI_SS_P12,SPI_SS_P22,SPI_SS_P74,SPI_SS_P35
	DMA_SPI_InitStructure.DMA_AUTO_SS = DISABLE;				//�Զ�����SS��ʹ��  	ENABLE,DISABLE
	DMA_SPI_Inilize(&DMA_SPI_InitStructure);		//��ʼ��
	SET_DMA_SPI_CR(DMA_ENABLE | SPI_TRIG_S | CLR_FIFO);	//bit7 1:ʹ�� SPI_DMA, bit5 1:��ʼ SPI_DMA �ӻ�ģʽ, bit0 1:��� SPI_DMA FIFO
	NVIC_DMA_SPI_Init(ENABLE,Priority_0,Priority_0);		//�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0~Priority_3; �������ȼ�(�͵���) Priority_0~Priority_3
}

void M2M_UART_SPI(u16 txbuf, u16 rxbuf)
{
	DMA_M2M_CLR_STA();
	SET_M2M_TX_FIFO(txbuf);
	SET_M2M_RX_FIFO(rxbuf);
	DMA_M2M_TRIG();
}

void M2M_SPI_UART(u16 txbuf, u16 rxbuf)
{
	DMA_M2M_CLR_STA();
	SET_M2M_TX_FIFO(txbuf);
	SET_M2M_RX_FIFO(rxbuf);
	DMA_M2M_TRIG();
}

void UART_DMA_Tx(void)
{
	DMA_UR2T_TRIG();
}

void UART_DMA_Rx(void)
{
	DMA_UR2R_TRIG();
}

void SPI_DMA_Master(void)
{
	SET_DMA_SPI_CR(0);
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	SPI_SS_2 = 0;
	SPCTL = 0xd2;   //ʹ�� SPI ����ģʽ������SS���Ź���
	SET_DMA_SPI_CR(DMA_ENABLE | SPI_TRIG_M);	//bit7 1:ʹ�� SPI_DMA, bit6 1:��ʼ SPI_DMA ����ģʽ
}

void SPI_DMA_Slave(void)
{
	SET_DMA_SPI_CR(0);
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	SPCTL = 0x42;  //��������Ϊ�ӻ�����
	SET_DMA_SPI_CR(DMA_ENABLE | SPI_TRIG_S);	//bit7 1:ʹ�� SPI_DMA, bit5 1:��ʼ SPI_DMA �ӻ�ģʽ
}

//========================================================================
// ����: Sample_DMA_SPI_PS
// ����: �û�Ӧ�ó���.
// ����: None.
// ����: None.
// �汾: V1.0, 2021-05-27
//========================================================================
void Sample_DMA_SPI_PS(void)
{
	//UART���� -> UART DMA -> SPI DMA -> SPI����
	if(DmaRx2Flag)
	{
		DmaRx2Flag = 0;
		u2sFlag = 1;
		M2M_UART_SPI((u16)UartRxBuffer,(u16)SpiTxBuffer);			//UART Memory -> SPI Memory
	}

	if(SpiSendFlag)
	{
		SpiSendFlag = 0;
		UART_DMA_Rx();			//UART Recive Continue
		SPI_DMA_Master();		//SPI Send Memory
	}

	if(SpiTxFlag)
	{
		SpiTxFlag = 0;
		SPI_DMA_Slave();		//SPI Slave Mode
	}

	
	//SPI���� -> SPI DMA -> UART DMA -> UART����
	if(SpiRxFlag)
	{
		SpiRxFlag = 0;
		s2uFlag = 1;
		M2M_SPI_UART((u16)SpiRxBuffer, (u16)UartTxBuffer);			//SPI Memory -> UART Memory
	}

	if(UartSendFlag)
	{
		UartSendFlag = 0;
		SPI_DMA_Slave();		//SPI Slave Mode
		UART_DMA_Tx();			//UART Send Memory
	}
}
