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

#include	"APP_HSSPI.h"
#include	"STC32G_Clock.h"
#include	"STC32G_GPIO.h"
#include	"STC32G_SPI.h"
#include	"STC32G_UART.h"
#include	"STC32G_NVIC.h"

/*************	����˵��	**************

ͨ������SPI��PM25LV040/W25X40CL/W25Q80BV���ж�д���ԡ�

���ڷ�ָ���FLASH������������д�롢�����Ĳ���������ָ����ַ��

Ĭ�ϲ�����:  115200,8,N,1. ������"���ڳ�ʼ������"���޸�.

������������: (��ĸ�����ִ�Сд)
    E 0x001234              --> ����������ָ��ʮ�����Ƶ�ַ.
    W 0x001234 1234567890   --> д�������ָ��ʮ�����Ƶ�ַ������Ϊд������.
    R 0x001234 10           --> ����������ָ��ʮ�����Ƶ�ַ������Ϊ�����ֽ�. 
    C                       --> �����ⲻ��PM25LV040/W25X40CL/W25Q80BV, ����Cǿ���������.

ע�⣺Ϊ��ͨ�ã�����ʶ���ַ�Ƿ���Ч���û��Լ����ݾ�����ͺ���������

����ʱ, ѡ��ʱ�� 24MHz (�����������ļ�"config.h"���޸�).

******************************************/

//========================================================================
//                               ���س�������	
//========================================================================

#define BUF_LENGTH          107			//n+1
#define EE_BUF_LENGTH       50      //

/******************* FLASH��ض��� ************************/
#define SFC_WREN        0x06        //����Flash���
#define SFC_WRDI        0x04
#define SFC_RDSR        0x05
#define SFC_WRSR        0x01
#define SFC_READ        0x03
#define SFC_FASTREAD    0x0B
#define SFC_RDID        0xAB
#define SFC_PAGEPROG    0x02
#define SFC_RDCR        0xA1
#define SFC_WRCR        0xF1
#define SFC_SECTORER1   0xD7        //PM25LV040 ��������ָ��
#define SFC_SECTORER2   0x20        //W25Xxx ��������ָ��
#define SFC_BLOCKER     0xD8
#define SFC_CHIPER      0xC7

#define SPI_CE_High()   P_PM25LV040_CE  = 1     // set CE high
#define SPI_CE_Low()    P_PM25LV040_CE  = 0     // clear CE low
#define SPI_Hold()      P_SPI_Hold      = 0     // clear Hold pin
#define SPI_UnHold()    P_SPI_Hold      = 1     // set Hold pin
#define SPI_WP()        P_SPI_WP        = 0     // clear WP pin
#define SPI_UnWP()      P_SPI_WP        = 1     // set WP pin

sbit    P_PM25LV040_CE  = P2^2;     //PIN1
sbit    P_PM25LV040_SO  = P2^4;     //PIN2
sbit    P_PM25LV040_SI  = P2^3;     //PIN5
sbit    P_PM25LV040_SCK = P2^5;     //PIN6

//========================================================================
//                               ���ر�������
//========================================================================

u8  sst_byte;
u32 Flash_addr;

u8  B_FlashOK;                                //Flash״̬
u8  PM25LV040_ID, PM25LV040_ID1, PM25LV040_ID2;

//========================================================================
//                               ���غ�������
//========================================================================

void    FlashCheckID(void);
void    RX2_Check(void);
u8      CheckFlashBusy(void);
void    FlashWriteEnable(void);
void    FlashChipErase(void);
void    FlashSectorErase(u32 addr);
void    SPI_Read_Nbytes( u32 addr, u8 *buffer, u16 size);
u8      SPI_Read_Compare(u32 addr, u8 *buffer, u16 size);
void    SPI_Write_Nbytes(u32 addr, u8 *buffer,  u8 size);

//========================================================================
//                            �ⲿ�����ͱ�������
//========================================================================


u8  Hex2Ascii(u8 dat)
{
    dat &= 0x0f;
    if(dat < 10)    return (dat+'0');
    return (dat-10+'A');
}

//========================================================================
// ����: HSSPI_init
// ����: �û���ʼ������.
// ����: None.
// ����: None.
// �汾: V1.0, 2021-05-27
//========================================================================
void HSSPI_init(void)
{
	SPI_InitTypeDef		SPI_InitStructure;
	COMx_InitDefine		COMx_InitStructure;					//�ṹ����

	//----------------------------------------------
	P2_MODE_IO_PU(GPIO_Pin_All);		//P2 ����Ϊ׼˫���
	P4_MODE_IO_PU(GPIO_Pin_6 | GPIO_Pin_7);		//P4.6,P4.7 ����Ϊ׼˫���
	P2_SPEED_HIGH(GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5); //��ƽת���ٶȿ죨���IO�ڷ�ת�ٶȣ�

	P_PM25LV040_SCK = 0;    // set clock to low initial state
	P_PM25LV040_SI = 1;
	P_PM25LV040_CE = 1;

	//----------------------------------------------
	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//ģʽ,   UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
//	COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;			//ѡ�����ʷ�����, BRT_Timer2 (ע��: ����2�̶�ʹ��BRT_Timer2, ���Բ���ѡ��)
	COMx_InitStructure.UART_BaudRate  = 115200ul;			//������,     110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//��������,   ENABLE �� DISABLE
	UART_Configuration(UART2, &COMx_InitStructure);		//��ʼ������2 UART1,UART2,UART3,UART4
	NVIC_UART2_Init(ENABLE,Priority_1);		//�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3

	PrintString2("��������:\r\n");
	PrintString2("E 0x001234            --> ��������  ʮ�����Ƶ�ַ\r\n");
	PrintString2("W 0x001234 1234567890 --> д�����  ʮ�����Ƶ�ַ  д������\r\n");
	PrintString2("R 0x001234 10         --> ��������  ʮ�����Ƶ�ַ  �����ֽ�\r\n");
	PrintString2("C                     --> �����ⲻ��PM25LV040/W25X40CL/W25Q80BV, ����Cǿ���������.\r\n\r\n");

	//----------------------------------------------
	SPI_InitStructure.SPI_Enable    = ENABLE;				//SPI����    ENABLE, DISABLE
	SPI_InitStructure.SPI_SSIG      = ENABLE;				//Ƭѡλ     ENABLE(����SS���Ź���), DISABLE(SSȷ�������ӻ�)
	SPI_InitStructure.SPI_FirstBit  = SPI_MSB;				//��λ����   SPI_MSB, SPI_LSB
	SPI_InitStructure.SPI_Mode      = SPI_Mode_Master;		//����ѡ��   SPI_Mode_Master, SPI_Mode_Slave
	SPI_InitStructure.SPI_CPOL      = SPI_CPOL_High;		//ʱ����λ   SPI_CPOL_High,   SPI_CPOL_Low
	SPI_InitStructure.SPI_CPHA      = SPI_CPHA_2Edge;		//���ݱ���   SPI_CPHA_1Edge,  SPI_CPHA_2Edge
	SPI_InitStructure.SPI_Speed     = SPI_Speed_4;			//SPI�ٶ�    SPI_Speed_4, SPI_Speed_8, SPI_Speed_16, SPI_Speed_2
	SPI_Init(&SPI_InitStructure);
	NVIC_SPI_Init(DISABLE,Priority_0);		//�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
	SPI_ClearFlag();   //��� SPIF��WCOL ��־

	HSPllClkConfig(MCLKSEL_HIRC,PLL_96M,4);    //ϵͳʱ��ѡ��,PLLʱ��ѡ��,ʱ�ӷ�Ƶϵ��
	HSSPI_Enable();

	FlashCheckID();
	FlashCheckID();
	
	if(!B_FlashOK)  PrintString2("δ��⵽PM25LV040/W25X40CL/W25Q80BV!\r\n");
	else
	{
			if(B_FlashOK == 1)
			{
					PrintString2("��⵽PM25LV040!\r\n");
			}
			else if(B_FlashOK == 2)
			{
					PrintString2("��⵽W25X40CL!\r\n");
			}
			else if(B_FlashOK == 3)
			{
					PrintString2("��⵽W25Q80BV!\r\n");
			}
			PrintString2("������ID1 = 0x");
			TX2_write2buff(Hex2Ascii(PM25LV040_ID1 >> 4));
			TX2_write2buff(Hex2Ascii(PM25LV040_ID1));
			PrintString2("\r\n      ID2 = 0x");
			TX2_write2buff(Hex2Ascii(PM25LV040_ID2 >> 4));
			TX2_write2buff(Hex2Ascii(PM25LV040_ID2));
			PrintString2("\r\n   �豸ID = 0x");
			TX2_write2buff(Hex2Ascii(PM25LV040_ID >> 4));
			TX2_write2buff(Hex2Ascii(PM25LV040_ID));
			PrintString2("\r\n");
	}
}

//========================================================================
// ����: Sample_HSSPI
// ����: �û�Ӧ�ó���.
// ����: None.
// ����: None.
// �汾: V1.0, 2021-05-27
//========================================================================
void Sample_HSSPI(void)
{
	if(COM2.RX_TimeOut > 0)
	{
		if(--COM2.RX_TimeOut == 0)  //��ʱ,�򴮿ڽ��ս���
		{
			if(COM2.RX_Cnt > 0)
			{
				RX2_Check();    //����1��������
			}
			COM2.RX_Cnt = 0;
		}
	}
}

/**************** ASCII��תBIN ****************************/
u8  CheckData(u8 dat)
{
    if((dat >= '0') && (dat <= '9'))        return (dat-'0');
    if((dat >= 'A') && (dat <= 'F'))        return (dat-'A'+10);
    return 0xff;
}

/**************** ��ȡд���ַ ****************************/
u32 GetAddress(void)
{
    u32 address;
    u8  i,j;
    
    address = 0;
    if((RX2_Buffer[2] == '0') && (RX2_Buffer[3] == 'X'))
    {
        for(i=4; i<10; i++)
        {
            j = CheckData(RX2_Buffer[i]);
            if(j >= 0x10)   return 0x80000000;  //error
            address = (address << 4) + j;
        }
        return (address);
    }
    return  0x80000000; //error
}

/**************** ��ȡҪ�������ݵ��ֽ��� ****************************/
u8  GetDataLength(void)
{
    u8  i;
    u8  length;
    
    length = 0;
    for(i=11; i<COM2.RX_Cnt; i++)
    {
        if(CheckData(RX2_Buffer[i]) >= 10)  break;
        length = length * 10 + CheckData(RX2_Buffer[i]);
    }
    return (length);
}


/**************** ����2������ ****************************/

void RX2_Check(void)
{
    u8  i,j;
    u8  tmp[EE_BUF_LENGTH];

    if((COM2.RX_Cnt == 1) && (RX2_Buffer[0] == 'C'))    //����Cǿ���������
    {
        B_FlashOK = 1;
        PrintString2("ǿ���������FLASH!\r\n");
    }

    if(!B_FlashOK)
    {
        PrintString2("PM25LV040/W25X40CL/W25Q80BV������, ���ܲ���FLASH!\r\n");
        return;
    }
    
    F0 = 0;
    if((COM2.RX_Cnt >= 10) && (RX2_Buffer[1] == ' '))   //�������Ϊ10���ֽ�
    {
//			printf("�յ��������£� ");
//			for(i=0; i<COM2.RX_Cnt; i++)    printf("%c", RX2_Buffer[i]);    //���յ�������ԭ������,���ڲ���
//			printf("\r\n");

        for(i=0; i<10; i++)
        {
            if((RX2_Buffer[i] >= 'a') && (RX2_Buffer[i] <= 'z'))    RX2_Buffer[i] = RX2_Buffer[i] - 'a' + 'A';//Сдת��д
        }
        Flash_addr = GetAddress();
        if(Flash_addr < 0x80000000)
        {
            if(RX2_Buffer[0] == 'E')    //����
            {
                FlashSectorErase(Flash_addr);
                PrintString2("�Ѳ���һ����������!\r\n");
                F0 = 1;
            }

            else if((RX2_Buffer[0] == 'W') && (COM2.RX_Cnt >= 12) && (RX2_Buffer[10] == ' '))   //д��N���ֽ�
            {
                j = COM2.RX_Cnt - 11;
                for(i=0; i<j; i++)  tmp[i] = 0xff;      //���Ҫд��Ŀռ��Ƿ�Ϊ��
                i = SPI_Read_Compare(Flash_addr,tmp,j);
                if(i > 0)
                {
                    PrintString2("Ҫд��ĵ�ַΪ�ǿ�,����д��,���Ȳ���!\r\n");
                }
                else
                {
                    SPI_Write_Nbytes(Flash_addr,&RX2_Buffer[11],j);     //дN���ֽ� 
                    i = SPI_Read_Compare(Flash_addr,&RX2_Buffer[11],j); //�Ƚ�д�������
                    if(i == 0)
                    {
                        PrintString2("��д��");
                        if(j >= 100)    {TX2_write2buff((u8)(j/100+'0'));   j = j % 100;}
                        if(j >= 10)     {TX2_write2buff((u8)(j/10+'0'));    j = j % 10;}
                        TX2_write2buff((u8)(j%10+'0'));
                        PrintString2("�ֽ�����!\r\n");
                    }
                    else        PrintString2("д�����!\r\n");
                }
                F0 = 1;
            }
            else if((RX2_Buffer[0] == 'R') && (COM2.RX_Cnt >= 12) && (RX2_Buffer[10] == ' '))   //����N���ֽ�
            {
                j = GetDataLength();
                if((j > 0) && (j < EE_BUF_LENGTH))
                {
                    SPI_Read_Nbytes(Flash_addr,tmp,j);
                    PrintString2("����");
                    if(j>=100)  TX2_write2buff((u8)(j/100+'0'));
                    TX2_write2buff((u8)(j%100/10+'0'));
                    TX2_write2buff((u8)(j%10+'0'));
                    PrintString2("���ֽ��������£�\r\n");
                    for(i=0; i<j; i++)  TX2_write2buff(tmp[i]);
                    TX2_write2buff(0x0d);
                    TX2_write2buff(0x0a);
                    F0 = 1;
                }
            }
        }
    }
    if(!F0) PrintString2("�������!\r\n");
}

/************************************************
���Flash�Ƿ�׼������
��ڲ���: ��
���ڲ���:
    0 : û�м�⵽��ȷ��Flash
    1 : Flash׼������
************************************************/
void FlashCheckID(void)
{
    SPI_CE_Low();
    SPI_WriteByte(SFC_RDID);        //���Ͷ�ȡID����
    SPI_WriteByte(0x00);            //�ն�3���ֽ�
    SPI_WriteByte(0x00);
    SPI_WriteByte(0x00);
    PM25LV040_ID1 = SPI_ReadByte();         //��ȡ������ID1
    PM25LV040_ID  = SPI_ReadByte();         //��ȡ�豸ID
    PM25LV040_ID2 = SPI_ReadByte();         //��ȡ������ID2
    SPI_CE_High();

//    TX2_write2buff(PM25LV040_ID1);
//    TX2_write2buff(PM25LV040_ID);
//    TX2_write2buff(PM25LV040_ID2);
	
    if((PM25LV040_ID1 == 0x9d) && (PM25LV040_ID2 == 0x7f))  B_FlashOK = 1;  //����Ƿ�ΪPM25LVxxϵ�е�Flash
    else if(PM25LV040_ID == 0x12)  B_FlashOK = 2;                           //����Ƿ�ΪW25X4xϵ�е�Flash
    else if(PM25LV040_ID == 0x13)  B_FlashOK = 3;                           //����Ƿ�ΪW25X8xϵ�е�Flash
    else                                                    B_FlashOK = 0;
}

/************************************************
���Flash��æ״̬
��ڲ���: ��
���ڲ���:
    0 : Flash���ڿ���״̬
    1 : Flash����æ״̬
************************************************/
u8 CheckFlashBusy(void)
{
    u8  dat;

    SPI_CE_Low();
    SPI_WriteByte(SFC_RDSR);        //���Ͷ�ȡ״̬����
    dat = SPI_ReadByte();           //��ȡ״̬
    SPI_CE_High();

    return (dat);                   //״ֵ̬��Bit0��Ϊæ��־
}

/************************************************
ʹ��Flashд����
��ڲ���: ��
���ڲ���: ��
************************************************/
void FlashWriteEnable(void)
{
    while(CheckFlashBusy() > 0);    //Flashæ���
    SPI_CE_Low();
    SPI_WriteByte(SFC_WREN);        //����дʹ������
    SPI_CE_High();
}

/************************************************
������ƬFlash
��ڲ���: ��
���ڲ���: ��
************************************************/
/*
void FlashChipErase(void)
{
    if(B_FlashOK)
    {
        FlashWriteEnable();             //ʹ��Flashд����
        SPI_CE_Low();
        SPI_WriteByte(SFC_CHIPER);      //����Ƭ��������
        SPI_CE_High();
    }
}
*/

/************************************************
��������, һ������4KB
��ڲ���: ��
���ڲ���: ��
************************************************/
void FlashSectorErase(u32 addr)
{
    if(B_FlashOK)
    {
        FlashWriteEnable();             //ʹ��Flashд����
        SPI_CE_Low();
        if(B_FlashOK == 1)
        {
            SPI_WriteByte(SFC_SECTORER1);    //����������������
        }
        else
        {
            SPI_WriteByte(SFC_SECTORER2);    //����������������
        }
        SPI_WriteByte(((u8 *)&addr)[1]);           //������ʼ��ַ
        SPI_WriteByte(((u8 *)&addr)[2]);
        SPI_WriteByte(((u8 *)&addr)[3]);
        SPI_CE_High();
    }
}

/************************************************
��Flash�ж�ȡ����
��ڲ���:
    addr   : ��ַ����
    buffer : �����Flash�ж�ȡ������
    size   : ���ݿ��С
���ڲ���:
    ��
************************************************/
void SPI_Read_Nbytes(u32 addr, u8 *buffer, u16 size)
{
    if(size == 0)   return;
    if(!B_FlashOK)  return;
    while(CheckFlashBusy() > 0);        //Flashæ���

    SPI_CE_Low();                       //enable device
    SPI_WriteByte(SFC_READ);            //read command

    SPI_WriteByte(((u8 *)&addr)[1]);    //������ʼ��ַ
    SPI_WriteByte(((u8 *)&addr)[2]);
    SPI_WriteByte(((u8 *)&addr)[3]);

    do{
        *buffer = SPI_ReadByte();       //receive byte and store at buffer
        buffer++;
    }while(--size);                     //read until no_bytes is reached
    SPI_CE_High();                      //disable device
}

/************************************************************************
����n���ֽ�,��ָ�������ݽ��бȽ�, ���󷵻�1,��ȷ����0
************************************************************************/
u8 SPI_Read_Compare(u32 addr, u8 *buffer, u16 size)
{
    u8  j;
    if(size == 0)   return 2;
    if(!B_FlashOK)  return 2;
    while(CheckFlashBusy() > 0);            //Flashæ���

    j = 0;
    SPI_CE_Low();                           //enable device
    SPI_WriteByte(SFC_READ);                //read command
    SPI_WriteByte(((u8 *)&addr)[1]);        //������ʼ��ַ
    SPI_WriteByte(((u8 *)&addr)[2]);
    SPI_WriteByte(((u8 *)&addr)[3]);
    do
    {
        if(*buffer != SPI_ReadByte())       //receive byte and store at buffer
        {
            j = 1;
            break;
        }
        buffer++;
    }while(--size);         //read until no_bytes is reached
    SPI_CE_High();          //disable device
    return j;
}


/************************************************
д���ݵ�Flash��
��ڲ���:
    addr   : ��ַ����
    buffer : ������Ҫд��Flash������
    size   : ���ݿ��С
���ڲ���: ��
************************************************/
void SPI_Write_Nbytes(u32 addr, u8 *buffer, u8 size)
{
    if(size == 0)   return;
    if(!B_FlashOK)  return;
    while(CheckFlashBusy() > 0);        //Flashæ���


    FlashWriteEnable();                 //ʹ��Flashд����

    SPI_CE_Low();                       // enable device
    SPI_WriteByte(SFC_PAGEPROG);        // ����ҳ�������
    SPI_WriteByte(((u8 *)&addr)[1]);    //������ʼ��ַ
    SPI_WriteByte(((u8 *)&addr)[2]);
    SPI_WriteByte(((u8 *)&addr)[3]);
    do{
        SPI_WriteByte(*buffer++);       //����ҳ��д
        addr++;
        if ((addr & 0xff) == 0) break;
    }while(--size);
    SPI_CE_High();                      // disable device
}
