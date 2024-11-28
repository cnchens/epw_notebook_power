/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/* --- Web: www.STCMCUDATA.com  ---------------------------------------*/
/* --- BBS: www.STCAIMCU.com  -----------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* ���Ҫ�ڳ�����ʹ�ô˴���,���ڳ�����ע��ʹ����STC�����ϼ�����            */
/*---------------------------------------------------------------------*/

#include	"APP_CAN.h"
#include	"STC32G_CAN.h"
#include	"STC32G_GPIO.h"
#include	"STC32G_UART.h"
#include	"STC32G_NVIC.h"
#include	"STC32G_Switch.h"

/*************   ����˵��   ***************

CAN1��CAN2����ͬʱʹ���շ���������.

DCAN��һ��֧��CAN2.0BЭ��Ĺ��ܵ�Ԫ��

�յ�һ֡���ݺ�, ͨ������1��P3.0,P3.1�����ͳ�ȥ�����ڲ�����115200,N,8,1.

��Ҫ��"STC32G_UART.h"�����ã� #define	PRINTF_SELECT  UART1

MCUÿ���Ӵ�CAN1��CAN2����һ֡�̶�����.

Ĭ��CAN���߲�����500KHz, �û��������޸�.

����ʱ, ѡ��ʱ�� 24MHz (�û�����"config.h"�޸�Ƶ��).

******************************************/

//========================================================================
//                               ���س�������	
//========================================================================


//========================================================================
//                               ���ر�������
//========================================================================


//========================================================================
//                               ���غ�������
//========================================================================


//========================================================================
//                            �ⲿ�����ͱ�������
//========================================================================

extern u16 msecond;

//========================================================================
// ����: CAN_init
// ����: �û���ʼ������.
// ����: None.
// ����: None.
// �汾: V1.0, 2022-03-27
//========================================================================
void CAN_init(void)
{
	COMx_InitDefine		COMx_InitStructure;					//�ṹ����
	CAN_InitTypeDef	CAN_InitStructure;					//�ṹ����

	CAN_InitStructure.CAN_Enable = ENABLE;		//CAN����ʹ��   ENABLE��DISABLE
	CAN_InitStructure.CAN_IMR    = CAN_ALLIM;	//CAN�жϼĴ��� 	CAN_DOIM,CAN_BEIM,CAN_TIM,CAN_RIM,CAN_EPIM,CAN_EWIM,CAN_ALIM,CAN_ALLIM,DISABLE
	CAN_InitStructure.CAN_SJW    = 0;					//����ͬ����Ծ���  0~3
	CAN_InitStructure.CAN_SAM    = 0;					//���ߵ�ƽ��������  0:����1��; 1:����3��

	//CAN���߲�����=Fclk/((1+(TSG1+1)+(TSG2+1))*(BRP+1)*2)
	CAN_InitStructure.CAN_TSG1   = 2;					//ͬ��������1       0~15
	CAN_InitStructure.CAN_TSG2   = 1;					//ͬ��������2       1~7 (TSG2 ��������Ϊ0)
	CAN_InitStructure.CAN_BRP    = 3;					//�����ʷ�Ƶϵ��    0~63
	//24000000/((1+3+2)*4*2)=500KHz

	CAN_InitStructure.CAN_ListenOnly = DISABLE;	//Listen Onlyģʽ   ENABLE,DISABLE
	CAN_InitStructure.CAN_Filter  = DUAL_FILTER;//�˲�ѡ��  DUAL_FILTER(˫�˲�),SINGLE_FILTER(���˲�)
	CAN_InitStructure.CAN_ACR0    = 0x00;			//�������մ���Ĵ��� 0~0xFF
	CAN_InitStructure.CAN_ACR1    = 0x00;
	CAN_InitStructure.CAN_ACR2    = 0x00;
	CAN_InitStructure.CAN_ACR3    = 0x00;
	CAN_InitStructure.CAN_AMR0    = 0xff;			//�����������μĴ��� 0~0xFF
	CAN_InitStructure.CAN_AMR1    = 0xff;
	CAN_InitStructure.CAN_AMR2    = 0xff;
	CAN_InitStructure.CAN_AMR3    = 0xff;
	CAN_Inilize(CAN1,&CAN_InitStructure);			//CAN1 ��ʼ��
	CAN_Inilize(CAN2,&CAN_InitStructure);			//CAN2 ��ʼ��
	
	NVIC_CAN_Init(CAN1,ENABLE,Priority_1);		//�ж�ʹ��, CAN1/CAN2; ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
	NVIC_CAN_Init(CAN2,ENABLE,Priority_1);		//�ж�ʹ��, CAN1/CAN2; ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
	
	P5_MODE_IO_PU(GPIO_Pin_LOW);		//P5.0~P5.3 ����Ϊ׼˫���

	CAN1_SW(CAN1_P50_P51);				//CAN1_P00_P01,CAN1_P50_P51,CAN1_P42_P45,CAN1_P70_P71
	CAN2_SW(CAN2_P52_P53);				//CAN2_P02_P03,CAN2_P52_P53,CAN2_P46_P47,CAN2_P72_P73

	//====��ʼ������=====
	CAN1_Tx.FF = STANDARD_FRAME;    //��׼֡
	CAN1_Tx.RTR = 0;                //0������֡��1��Զ��֡
	CAN1_Tx.DLC = 0x08;             //���ݳ���
	CAN1_Tx.ID = 0x0567;            //CAN ID
	CAN1_Tx.DataBuffer[0] = 0x11;   //��������
	CAN1_Tx.DataBuffer[1] = 0x12;
	CAN1_Tx.DataBuffer[2] = 0x13;
	CAN1_Tx.DataBuffer[3] = 0x14;
	CAN1_Tx.DataBuffer[4] = 0x15;
	CAN1_Tx.DataBuffer[5] = 0x16;
	CAN1_Tx.DataBuffer[6] = 0x17;
	CAN1_Tx.DataBuffer[7] = 0x18;
	
	CAN2_Tx.FF = EXTENDED_FRAME;    //��չ֡
	CAN2_Tx.RTR = 0;                //0������֡��1��Զ��֡
	CAN2_Tx.DLC = 0x08;             //���ݳ���
	CAN2_Tx.ID = 0x03456789;        //CAN ID
	CAN2_Tx.DataBuffer[0] = 0x21;   //��������
	CAN2_Tx.DataBuffer[1] = 0x22;
	CAN2_Tx.DataBuffer[2] = 0x23;
	CAN2_Tx.DataBuffer[3] = 0x24;
	CAN2_Tx.DataBuffer[4] = 0x25;
	CAN2_Tx.DataBuffer[5] = 0x26;
	CAN2_Tx.DataBuffer[6] = 0x27;
	CAN2_Tx.DataBuffer[7] = 0x28;

    //��ӡ���ڳ�ʼ��
	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//ģʽ,   UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;			//ѡ�����ʷ�����, BRT_Timer2 (ע��: ����2�̶�ʹ��BRT_Timer2, ���Բ���ѡ��)
	COMx_InitStructure.UART_BaudRate  = 115200ul;			//������,     110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//��������,   ENABLE��DISABLE
	UART_Configuration(UART1, &COMx_InitStructure);		//��ʼ������ USART1,USART2,USART3,USART4
	NVIC_UART1_Init(ENABLE,Priority_1);		//�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3

	P3_MODE_IO_PU(GPIO_Pin_0 | GPIO_Pin_1);		//P3.0,P3.1 ����Ϊ׼˫���
	UART1_SW(UART1_SW_P30_P31);		//UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17,UART1_SW_P43_P44
}

//========================================================================
// ����: Sample_CAN
// ����: �û�Ӧ�ó���.
// ����: None.
// ����: None.
// �汾: V1.0, 2022-03-27
//========================================================================
void Sample_CAN(void)
{
	u8 sr;
	u8 n,i,j;

		//------------------����CAN1ģ��-----------------------
	if(++msecond >= 1000)   //1�뵽
	{
		msecond = 0;

		CANSEL = CAN1;		//ѡ��CAN1ģ��
		sr = CanReadReg(SR);

		if(sr & 0x01)		//�ж��Ƿ��� BS:BUS-OFF״̬
		{
			CANAR = MR;
			CANDR &= ~0x04;  //��� Reset Mode, ��BUS-OFF״̬�˳�
		}
		else
		{
            CanSendMsg(&CAN1_Tx);   //����һ֡����
		}

		//------------------����CAN2ģ��-----------------------
		CANSEL = CAN2;		//ѡ��CAN2ģ��
		sr = CanReadReg(SR);

		if(sr & 0x01)		//�ж��Ƿ��� BS:BUS-OFF״̬
		{
			CANAR = MR;
			CANDR &= ~0x04;  //��� Reset Mode, ��BUS-OFF״̬�˳�
		}
		else
		{
            CanSendMsg(&CAN2_Tx);   //����һ֡����
		}
	}

	if(B_Can1Read)
	{
		B_Can1Read = 0;
		
		CANSEL = CAN1;		//ѡ��CAN1ģ��
        n = CanReadMsg(CAN1_Rx);    //��ȡ��������
        if(n>0)
        {
            for(i=0;i<n;i++)
            {
//                CanSendMsg(&CAN1_Rx[i]);  //CAN����ԭ������
                    printf("CAN1 ID=0x%08lX DLC=%d FF=%d RTR=%d ",CAN1_Rx[i].ID,CAN1_Rx[i].DLC,CAN1_Rx[i].FF,CAN1_Rx[i].RTR);
                for(j=0;j<CAN1_Rx[i].DLC;j++)
                {
                    printf("0x%02X ",CAN1_Rx[i].DataBuffer[j]);    //�Ӵ�������յ�������
                }
                printf("\r\n");
            }
        }
	}

	if(B_Can2Read)
	{
		B_Can2Read = 0;
		
		CANSEL = CAN2;		//ѡ��CAN2ģ��
        n = CanReadMsg(CAN2_Rx);    //��ȡ��������
        if(n>0)
        {
            for(i=0;i<n;i++)
            {
//                CanSendMsg(CAN2_Rx);  //CAN����ԭ������
                    printf("CAN2 ID=0x%08lX DLC=%d FF=%d RTR=%d ",CAN2_Rx[i].ID,CAN2_Rx[i].DLC,CAN2_Rx[i].FF,CAN2_Rx[i].RTR);
                for(j=0;j<CAN2_Rx[i].DLC;j++)
                {
                    printf("0x%02X ",CAN2_Rx[i].DataBuffer[j]);    //�Ӵ�������յ�������
                }
                printf("\r\n");
            }
        }
	}
}

