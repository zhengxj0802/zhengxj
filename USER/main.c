#include "stm32f10x.h"
#include "AT45DB161D.h"
#include "USART.h"
#include "systick.h"
#include "KEY.h"
#include "TIM.h"
#include "Beep.h"
#include "RTC_Time.h"
#include "comdef.h"
#include "fingerprint.h"
#include "globleexten.h"
#include "24C256.h"
#include "I2CLIB.h"
#include "ICCARD.h"
#include "CPUCard.h"
#include "des.h"
#include "3des.h"
#include "EXIT.h"
#include "12864lcd.h"
#include "common.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void NVIC_Configuration(void);
void RTCNVIC_Configuration(void);
void IWDG_Configuration(void);
void delay_ms(uint16_t nms);

int main(void)
{
	u8 Result=0;
	u8 writebuffer[528]={0x01,0X02,0X03,0X04,0X05,0X06,0X07,0X08,0X09,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x20};
	u8 readbuffer[528]={0x0};
	u8 gwSpeedPluseMileBuf[20];
	u8 keyvalue=0;
	u8 flag=1;
	u32 i; 
/*************************************************************************************/
	delay_init();				//��ʱ������ʼ��
	LCD_Configuration();		//Һ������ʼ��
	Init_LCD();

	AT45DB161D_Configuration();	//AT45DB161D��SPI1������
	AT24C256_Configuration(); 	//AT24C256��I2C2������
	I2C2_Configuration();		//I2C1��������	Ӳ��I2Cʹ��
	ICCARD_Configuration();		//�Ӵ�ʽ�߼����ܿ��ۣ�I2C1��
	I2C1_Configuration();		//I2C2��������	Ӳ��I2Cʹ��
  	//CPUCARD_Configuration();	//CPU������
	KEY_Configuration(); 		//��������
	Beep_Configuration();		//����������
	USART1_Configuration();		//����1����
	USART2_Configuration();		//����2����,����ָ��ͨ��
	TIM2_Configuration(); 		//��ʱ��2����
	EXTI_Configuration();		//�ⲿ�ж�����
	NVIC_Configuration();		//�ж���������

	IWDG_Configuration();		//�������Ź�����
	reboot=0;
/*************************************************************************/
/*************************************************************************/
	/*����������,��һ����500ms*/
	Beep(1,500);
	/*Һ��������*/
	ClearLcd();
	DispOutString(0,4,8,"��������",NORMALDISP);
	DispOutString(2,2,12,"Ӳ�����Գ���",NORMALDISP);
/*************************************************************************/
/*************************************************************************/
	/*��������*/
	flag=1;
	while(flag)
	{
		keyvalue=ScanKey();	
		if(KeyPressflag==1)
		{
			KeyPressflag=0;
			USARTSendByte(USART1,keyvalue);
			switch(keyvalue)
			{
				case EnterKey:
					flag=0;
					DispOutString(4,0,16,"   ȷ��������   ",NORMALDISP);
					break;
				case CancleKey:
					DispOutString(4,0,16,"   ȡ��������   ",NORMALDISP);
					break;
				case DownKey:
					DispOutString(4,0,16,"   �·�������   ",NORMALDISP);
					break;
				case PowerKey:
					DispOutString(4,0,16,"   ����������   ",NORMALDISP);
					break;
				case UpKey:
					DispOutString(4,0,16,"   �Ϸ�������   ",NORMALDISP);
					break;
				default:break;
			}
			delay_ms(1000);
		}
	}
/*************************************************************************/
/*************************************************************************/
	/*AT45DB161D ����*/
	Write_AT45DB161D(0,0,10,writebuffer,1);
	Read_AT45DB161D(0,0,10,readbuffer);
	if(memcmp(writebuffer,readbuffer,10)==0)
	{
		USART1_SendNByte((u8*)"AT45DB161D OK\n",15);
		DispOutString(4,0,16,"  AT45DB161D OK ",NORMALDISP);
	}
	else
	{
		USART1_SendNByte((u8*)"AT45DB161D NO OK\n",18);
		DispOutString(4,0,16,"AT45DB161D NO OK",NORMALDISP);
	}
	delay_ms(1000);
/*************************************************************************/
/*************************************************************************/
#if 0
	/*24C256First����,ģ��I2C*/
	for(i=0;i<10;i++)
	{
		readbuffer[i]=0;	
	}
	I2C2_WriteNByte(writebuffer,10,0,ADDR_First24C256);
	I2C2_ReadNByte(readbuffer,10,0, ADDR_First24C256); 
	if(memcmp(writebuffer,readbuffer,10)==0)
	{
		USART1_SendNByte((u8*)"EEPROM1 OK\n",12);
		DispOutString(4,0,16,"   EEPROM1 OK   ",NORMALDISP);
	}
	else
	{
		USART1_SendNByte((u8*)"EEPROM1 NO OK\n",15);
		DispOutString(4,0,16,"  EEPROM2 NO OK ",NORMALDISP);
	}
	delay_ms(1000);
	/*24C256Second����,ģ��I2C*/
	for(i=0;i<10;i++)
	{
		readbuffer[i]=0;	
	}
    I2C2_WriteNByte(writebuffer,10,100,ADDR_Second24C256);
	I2C2_ReadNByte(readbuffer,10,100, ADDR_Second24C256); 
	if(memcmp(writebuffer,readbuffer,10)==0)
	{
		USART1_SendNByte((u8*)"EEPROM2 OK\n",12);
		DispOutString(4,0,16,"   EEPROM2 OK   ",NORMALDISP);
	}
	else
	{
		USART1_SendNByte((u8*)"EEPROM2 NO OK\n",15);
		DispOutString(4,0,16,"  EEPROM2 NO OK ",NORMALDISP);
	}
	delay_ms(1000);
#else
	/*24C256First����,Ӳ��I2C*/
	for(i=0;i<10;i++)
	{
		readbuffer[i]=0;	
	}
	I2C_Write(I2C2,ADDR_First24C256_Write,0,writebuffer,10);
	I2C_Read(I2C2,ADDR_First24C256_Read,0,readbuffer,10);
	if(memcmp(writebuffer,readbuffer,10)==0)
	{
		USART1_SendNByte((u8*)"EEPROM1 OK\n",12);
		DispOutString(4,0,16,"   EEPROM1 OK   ",NORMALDISP);
	}
	else
	{
		USART1_SendNByte((u8*)"EEPROM1 NO OK\n",15);
		DispOutString(4,0,16,"  EEPROM1 NO OK ",NORMALDISP);
	}
	delay_ms(1000);
	/*24C256Second����,Ӳ��I2C*/
	for(i=0;i<10;i++)
	{
		readbuffer[i]=0;	
	}
	I2C_Write(I2C2,ADDR_Second24C256_Write,100,writebuffer,10);
	I2C_Read(I2C2,ADDR_Second24C256_Read,100,readbuffer,10);
	if(memcmp(writebuffer,readbuffer,10)==0)
	{
		USART1_SendNByte((u8*)"EEPROM2 OK\n",12);
		DispOutString(4,0,16,"   EEPROM2 OK   ",NORMALDISP);
	}
	else
	{
		USART1_SendNByte((u8*)"EEPROM2 NO OK\n",15);
		DispOutString(4,0,16,"  EEPROM2 NO OK ",NORMALDISP);
	}
	delay_ms(1000);
#endif
/*************************************************************************/
/*************************************************************************/
	/*�Ӵ�ʽ�߼�����IC������*/
	Result=ICCheckCardInsert();
	if(Result==C_OK)
	{
#if 0
		/*ģ��I2C*/
		for(i=0;i<10;i++)
		{
			readbuffer[i]=0;	
		}
		I2C1_WriteNByte(writebuffer,10,0,ICCARD);
        I2C1_ReadNByte(readbuffer,10,0, ICCARD);
		if(memcmp(writebuffer,readbuffer,10)==0)
		{
			USART1_SendNByte((u8*)"ICCARD OK\n",9);
			DispOutString(4,0,16,"    ICCARD OK   ",NORMALDISP);
		}
		else
		{
			USART1_SendNByte((u8*)"ICCARD NO OK\n",12);
			DispOutString(4,0,16,"  ICCARD NO OK  ",NORMALDISP);
		}
		delay_ms(1000);
#else
		/*Ӳ��I2C*/
		for(i=0;i<10;i++)
		{
			readbuffer[i]=0;	
		}
		I2C_Write(I2C1,ICCARD_WRITE,50,writebuffer,10);
		I2C_Read(I2C1,ICCARD_READ,50,readbuffer,10);	
		if(memcmp(writebuffer,readbuffer,10)==0)
		{
			USART1_SendNByte((u8*)"ICCARD OK\n",11);
			DispOutString(4,0,16,"    ICCARD OK   ",NORMALDISP);
		}
		else
		{
			USART1_SendNByte((u8*)"ICCARD NO OK\n",14);
			DispOutString(4,0,16,"  ICCARD NO OK  ",NORMALDISP);
		}
		delay_ms(1000);
#endif
	}
	else
	{
		USART1_SendNByte((u8*)"ICCARD NO INSERT\n",18);
		DispOutString(4,0,16,"ICCARD NO INSERT",NORMALDISP);
		delay_ms(1000);
	}
/*************************************************************************/
/*************************************************************************/
//	/*�Ӵ�ʽCPU������*/
//	//����CPU��
//	for(i=0;i<15;i++)
//		readbuffer[i]=0;
//	Result=StartCPUCard(readbuffer);
//	if(Result==C_OK)
//	{
//		for(i=0;i<15;i++)//����΢15�����Ժ4
//		{
//	  		USARTSendByte(USART1,readbuffer[i]);
//		}
//		USART1_SendNByte((u8*)"CPUCARD OK\n",12);
//		DispOutString(4,0,16,"   CPUCARD OK   ",NORMALDISP);
//		delay_ms(1000);	
//	}
//	else if(Result==C_NOIC)
//	{
//		USART1_SendNByte((u8*)"CPUCARD NO INSERT\n",19);
//		DispOutString(4,0,16,"CPUCARD NO INSERT",NORMALDISP);
//		delay_ms(1000);	
//	}
//	else
//	{
//		USART1_SendNByte((u8*)"CPUCARD EER\n",13);
//		DispOutString(4,0,16,"  CPUCARD EER  ",NORMALDISP);
//		delay_ms(1000);
//	}
//	if(Result==C_OK)
//	{
//		USART1_SendNByte((u8*)"ADPU TEST\n",11);
//		Result=Get_Challenge(readbuffer,8);
//		if(Result==C_OK)
//		{
//			for(i=0;i<16;i++)
//				writebuffer[i]=0;//�ⲿ��֤ԭ����
//			Result=External_Authenticate(readbuffer,8,writebuffer,16,1);
//			if(Result==C_OK)
//			{
//				USART1_SendNByte((u8*)"Extern Verification Pass\n",26);
//			}
//			else
//			{
//				USART1_SendNByte((u8*)"Extern Verification no Pass\n",29);	
//			}
//		}
//		else
//		{
//			USART1_SendNByte((u8*)"Get Challenge fail\n",20);
//		}
//	}
//	//�ر�CPU��
//	StopCPUCard();	
/*************************************************************************/
/*************************************************************************/
	/*ָ�Ʋ���*/
	CollectFPTrain();
/*************************************************************************/
/*************************************************************************/
	/*�ⲿ�ж�0����*/
	DispOutString(4,0,16,"���������:",NORMALDISP);
	while(gwSpeedPluseMile<10)
	{
		IntToString(gwSpeedPluseMile, gwSpeedPluseMileBuf,5);
		DispOutString(4,11,5,gwSpeedPluseMileBuf,NORMALDISP);
	}
/*************************************************************************/
/*************************************************************************/
	/*RTC����*/
	RTC_Init();					//RTC����
	RTCNVIC_Configuration();	//RTC�ж���������
	RefreshTime=1;
/*************************************************************************/
/*************************************************************************/
	/*�ر���ʾ�������*/
	CloseLcdLight();
/*************************************************************************/
/*************************************************************************/
	while(1)
	{
		keyvalue=ScanKey();	
		if(KeyPressflag==1)
		{
			KeyPressflag=0;
			if(keyvalue==PowerKey)
			{
				reboot=1;//���¿�������ֹͣι�����ȴ����Ź���λ����
			}
		}
	}
}

/*�������Ź�����*/
void IWDG_Configuration(void)
{
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); 	/* д��0x5555,�����������Ĵ���д�빦�� */
  IWDG_SetPrescaler(IWDG_Prescaler_256);            /* �ڲ�����ʱ��256��Ƶ 40K/256=156HZ(6.4ms) */ 
  /* ���Ź���ʱ�����ʱ�� */
  IWDG_SetReload(781);							    /* ι��ʱ�� 5s/6.4MS=781 .ע�ⲻ�ܴ���0xfff*/
  IWDG_ReloadCounter();								/* ι��*/
  IWDG_Enable(); 									/* ʹ�ܿ��Ź�*/
}
/*��RTC���ж���������*/
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

#ifdef  VECT_TAB_RAM  
  /* Set the Vector Table base location at 0x20000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else  /* VECT_TAB_FLASH  */
  /* Set the Vector Table base location at 0x08000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x3000);
#endif

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); 
  /*�����ж�*/
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	  
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  /*��ʱ��2�ж�*/
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;	  
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//��ռʽ���ȼ�
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;//��Ӧʽ���ȼ�	
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure); 
  /*�ⲿ�ж�0*/
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn; 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;   
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	      
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure); 
}
/*RTC�ж���������*/
void RTCNVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0); 
  /*RTC�ж�*/
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure); 
}
