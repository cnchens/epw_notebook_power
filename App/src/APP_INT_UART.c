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

#include	"APP_INT_UART.h"
#include	"STC32G_GPIO.h"
#include	"STC32G_Exti.h"
#include	"STC32G_UART.h"
#include	"STC32G_Delay.h"
#include	"STC32G_NVIC.h"

/*************	����˵��	**************

��ʾINT0~INT4 5������Դ��MCU�����߻���.

�Ӵ����������Դ�����Ѵ�����115200,N,8,1.

�ö�ʱ���������ʷ�����������ʹ��1Tģʽ(���ǵͲ�������12T)����ѡ��ɱ�������������ʱ��Ƶ�ʣ�����߾��ȡ�

����ʱ, ѡ��ʱ�� 22.1184MHz (�û�����"config.h"�޸�Ƶ��).

******************************************/


//========================================================================
//                               ���س�������	
//========================================================================

sbit INT0 = P3^2;
sbit INT1 = P3^3;
sbit INT2 = P3^6;
sbit INT3 = P3^7;
sbit INT4 = P3^0;

//========================================================================
//                               ���ر�������
//========================================================================

u8 WakeUpCnt;

//========================================================================
//                               ���غ�������
//========================================================================


//========================================================================
//                            �ⲿ�����ͱ�������
//========================================================================


//========================================================================
// ����: INTtoUART_init
// ����: �û���ʼ������.
// ����: None.
// ����: None.
// �汾: V1.0, 2020-09-28
//========================================================================
void INTtoUART_init(void)
{
	EXTI_InitTypeDef	Exti_InitStructure;					//�ṹ����
	COMx_InitDefine		COMx_InitStructure;					//�ṹ����

	P3_MODE_IO_PU(GPIO_Pin_All);		//P3.0~P3.7 ����Ϊ׼˫���
	P4_MODE_IO_PU(GPIO_Pin_6 | GPIO_Pin_7);	//P4.6,P4.7 ����Ϊ׼˫���
	P3_PULL_UP_ENABLE(GPIO_Pin_All);//P3 ���ڲ���������ʹ��

	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//ģʽ,   UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;			//ѡ�����ʷ�����, BRT_Timer2 (ע��: ����2�̶�ʹ��BRT_Timer2)
	COMx_InitStructure.UART_BaudRate  = 115200ul;			//������,     110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//��������,   ENABLE��DISABLE
	UART_Configuration(UART2, &COMx_InitStructure);		//��ʼ������ UART1,UART2,UART3,UART4
	NVIC_UART2_Init(ENABLE,Priority_1);		//�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
	//------------------------------------------------
	Exti_InitStructure.EXTI_Mode      = EXT_MODE_Fall;//�ж�ģʽ,   EXT_MODE_RiseFall,EXT_MODE_Fall
	Ext_Inilize(EXT_INT0,&Exti_InitStructure);				//��ʼ��
	NVIC_INT0_Init(ENABLE,Priority_0);		//�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3

	Exti_InitStructure.EXTI_Mode      = EXT_MODE_Fall;//�ж�ģʽ,   EXT_MODE_RiseFall,EXT_MODE_Fall
	Ext_Inilize(EXT_INT1,&Exti_InitStructure);				//��ʼ��
	NVIC_INT1_Init(ENABLE,Priority_0);		//�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3

	NVIC_INT2_Init(ENABLE,NULL);		//�ж�ʹ��, ENABLE/DISABLE; �����ȼ�
	NVIC_INT3_Init(ENABLE,NULL);		//�ж�ʹ��, ENABLE/DISABLE; �����ȼ�
	NVIC_INT4_Init(ENABLE,NULL);		//�ж�ʹ��, ENABLE/DISABLE; �����ȼ�
	
	PrintString2("���ж�INT���Ѳ���\r\n");
}

//========================================================================
// ����: Sample_INTtoUART
// ����: �û�Ӧ�ó���.
// ����: None.
// ����: None.
// �汾: V1.0, 2020-09-24
//========================================================================
void Sample_INTtoUART(void)
{
	if(!INT0) return;	//�ȴ����ж�Ϊ�ߵ�ƽ
	if(!INT1) return;	//�ȴ����ж�Ϊ�ߵ�ƽ
	if(!INT2) return;	//�ȴ����ж�Ϊ�ߵ�ƽ
	if(!INT3) return;	//�ȴ����ж�Ϊ�ߵ�ƽ
	if(!INT4) return;	//�ȴ����ж�Ϊ�ߵ�ƽ
	delay_ms(10);	//delay 10ms

	if(!INT0) return;	//�ȴ����ж�Ϊ�ߵ�ƽ
	if(!INT1) return;	//�ȴ����ж�Ϊ�ߵ�ƽ
	if(!INT2) return;	//�ȴ����ж�Ϊ�ߵ�ƽ
	if(!INT3) return;	//�ȴ����ж�Ϊ�ߵ�ƽ
	if(!INT4) return;	//�ȴ����ж�Ϊ�ߵ�ƽ

	WakeUpSource = 0;

	PrintString2("MCU��������״̬��\r\n");
	PD = 1;		//Sleep
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	delay_ms(1);	//delay 1ms
	
	if(WakeUpSource == 1)	PrintString2("���ж�INT0����  ");
	if(WakeUpSource == 2)	PrintString2("���ж�INT1����  ");
	if(WakeUpSource == 3)	PrintString2("���ж�INT2����  ");
	if(WakeUpSource == 4)	PrintString2("���ж�INT3����  ");
	if(WakeUpSource == 5)	PrintString2("���ж�INT4����  ");
	
	WakeUpCnt++;
	TX2_write2buff((u8)(WakeUpCnt/100+'0'));
	TX2_write2buff((u8)(WakeUpCnt%100/10+'0'));
	TX2_write2buff((u8)(WakeUpCnt%10+'0'));
	PrintString2("�λ���\r\n");
}

