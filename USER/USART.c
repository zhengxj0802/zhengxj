#include "stm32f10x.h"
#include "24C256.h"
#include "I2CLIB.h"
#include "systick.h"
#include "RTC_Time.h"
#include "globleexten.h"
#include "AT45DB161D.h"
#include "fingerprint.h"
#include "Beep.h"
#include "common.h"
#include "12864lcd.h"
#include "comdef.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*����1��ʼ��*/
void USART1_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	//RCCʹ��
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1,ENABLE);

	//IO������ USART1_TX -> PA9 , USART1_RX ->	PA10			
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);		   

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	        
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//���ڲ�������
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART1, &USART_InitStructure); 
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //���������ж�
	USART_Cmd(USART1, ENABLE);
}
/*����2��ʼ��������ָ��ͨ��*/
void USART2_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	//RCCʹ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOD,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

	//IO������ USART2_TX -> PA2 , USART2_RX ->PA3			
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);		   

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	        
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IN_FLOATING; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// PD15,ָ�Ƶ�Դ���ƽ�
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOD, GPIO_Pin_15); //����͵�ƽ,�ر�ָ��ģ���Դ

	//���ڲ�������
	USART_InitStructure.USART_BaudRate = 57600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure); 
	USART_Cmd(USART2, ENABLE);
}
/*���ڽ���һ���ֽڣ������ճ�ʱ����*/
u8 USARTRecvByte(USART_TypeDef* USARTx,u8 *Data,u32 Overtime)
{
	while(Overtime--)
	{
		if(USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) != RESET)
		{
			*Data =USART_ReceiveData(USARTx);  
			return C_OK;
		}
	}
	return 0x19;
}
/*���ڷ���һ���ֽڵ�����*/
void USARTSendByte(USART_TypeDef* USARTx,u8 data)
{	
	while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);//�������ݱ�־Ϊ�Ƿ�Ϊ��
	USART_SendData(USARTx,data);
}
/*����1���Ͷ���ֽڵ�����*/
void USART1_SendNByte(u8 *data,u16 len)
{
	u16 i;
	USART_ClearFlag(USART1,USART_FLAG_TC);//�����־λ�������1λ���ݻᶪʧ
	for (i=0; i<len; i++)
	{
		USARTSendByte(USART1,data[i]);	
	}
}
/*����1����һ�����ݣ�������ͷ*/
void SendResponse(unsigned char cmd[3],unsigned int ccRetCode,unsigned int length,unsigned char *data)
{
	unsigned char head[3]={0x1a,0x72,0x73};
	unsigned int i=0;
	unsigned char *buf=NULL;

	buf=(unsigned char*)malloc(length+11);
	if(buf==NULL)
	{
		free(buf);
		return;
	}
	buf[length+11-1]=0;
	//����ͷ
	for(i=0;i<3;i++)
	{
		buf[i]=head[i];
		buf[length+11-1]+=buf[i]; //�ۼ�У���
	}
	//���ݳ���
	buf[3]=(length+6)/256;
	buf[length+11-1]+=buf[3]; //�ۼ�У���
	buf[4]=(length+6)%256;
	buf[length+11-1]+=buf[4]; //�ۼ�У���

	//������
	for(i=0;i<3;i++)
	{
		buf[5+i]=cmd[i];
		buf[length+11-1]+=buf[5+i];//�ۼ�У���
	}

	//������
	buf[8]=(ccRetCode)/256;
	buf[length+11-1]+=buf[8];//�ۼ�У���
	buf[9]=(ccRetCode)%256;
	buf[length+11-1]+=buf[9];//�ۼ�У���

	//��Ч����
	for(i=0;i<length;i++)
	{
		buf[10+i]=data[i];
		buf[length+11-1]+=buf[10+i]; //�ۼ�У���
	}
	//��������
	USART1_SendNByte(buf,length+11);
	free(buf);
}
/*���³���*/
void CmdDownLoadProgram(u8*buf)
{
	u8 SymRestData[5]={0x1a,0x72,0x73,0x00,0x00};
	SymRestData[3]=buf[0];
	SymRestData[4]=buf[1];
	I2C_Write(I2C2,ADDR_First24C256_Write,0x10,SymRestData,5);
}
/*���������ֿ�*/
void CmdDownLoadziku(void)
{
	u32 cPage=0;
	u8 Buffer[528];
	u8 cCheckSum;
	u32 i;
	/*Һ������ʾ*/
	ClearLcd();
	DispOutString(2, 1, 14, "INIT DATAFLASH",NORMALDISP);
	/*
	*���Ϳ϶���Ӧ��Ϣ��Ȼ����λ����ʼ��������
	*һҳһҳ���ͣ�������DFPAGECOUNTҳ��ÿҳ���ճɹ���д����ȷ����'p'
	*/
	USARTSendByte(USART1,(u8)'p');

	for(cPage=0;cPage<DFPAGECOUNT;cPage++)
	{
		cCheckSum=0;
		for(i=0;i<528;i++)
		{ 
			if(USARTRecvByte(USART1,&Buffer[i],50000)!=C_OK)
				break;
			cCheckSum+=Buffer[i];
		}

		if(USARTRecvByte(USART1,&cCheckSum,50000)!=C_OK)
		{
			USARTSendByte(USART1,(u8)'q');
			break;
		}
		Write_AT45DB161D(cPage,0,528,Buffer,1);
		delay_ms(20);//��ʱ 20ms
		USARTSendByte(USART1,(u8)'p');
	}
	/*������Ӧ�ı�־����Ȼ������*/
	if(cPage==DFPAGECOUNT)
	{
		DispOutString(4, 4, 7, "SUCCEED",NORMALDISP);
	}
	else
	{
		DispOutString(4, 4, 7,"FAILURE",NORMALDISP);
	}
	delay_ms(2000);
}
/*����1�������ݼ�������Ϣ*/
void DealDataOfUSART1(void)
{
	u8 head[3]={0x0};
	u8 length[2];
	u8 cmd[3]={0xff,0xff,0xff};
	u8 *data=NULL;
	u8 sumcheck=0;
	u16 i=0;
	u16 leng=0;
	u8 sumreceive=0;

	head[0]=USART_ReceiveData(USART1); 
	/*��������*/
	for(i=1;i<3;i++)//��������ͷ
	{
		if(USARTRecvByte(USART1,&head[i],50000)!=C_OK)
		{
			return;
		}
	}
	if((head[0]!=0x1a) || (head[1]!=0x72) || (head[2]!=0x73))
	{
		return;
	}
	sumcheck+=head[0];
	sumcheck+=head[1];
	sumcheck+=head[2];

	for(i=0;i<2;i++)//�������ݳ���
	{
		if(USARTRecvByte(USART1,&length[i],50000)!=C_OK)
		 {	
			return;
		 }
		sumcheck+=length[i];
	}
	for(i=0;i<3;i++) //����������
	{
		if(USARTRecvByte(USART1,&cmd[i],50000)!=C_OK)
		{
			return;
		}
		sumcheck+=cmd[i];
	}
	leng=length[0]*256+length[1]-4;//�������ݳ��ȼ����У����1��,������3��������ݳ���
	if(leng>0)
	{
		data=(unsigned char*)malloc(leng); //��̬����ռ�
		if(data==NULL)
		{
			free(data);
			return;
		}
		for(i=0;i<leng;i++)//������Ч����
		{
			if(USARTRecvByte(USART1,&data[i],50000)!=C_OK)
			{	
			 	return;
			}
			sumcheck+=data[i];
		}
	}
	if(USARTRecvByte(USART1,&sumreceive,50000)!=C_OK)//����У���벢�Ƚ�У���
	{
		return;
	}
	else
	{
		if (cmd[0]!=0xb2)
		{
			if(sumcheck!=sumreceive)	
			{	 
				SendResponse(cmd,C_CRCERR,0,NULL);
				free(data);
				return;	
			}
		}
	}
	/*���ݴ���*/
	switch(cmd[0])
	{
		case 0xb2:
			if(cmd[1]==0x00) //���³���
			{
				CmdDownLoadProgram(data);
				SystemReset();
			}
			else if(cmd[1]==0x01) //�����ֿ�
			{
				CmdDownLoadziku();
				SystemReset();
			}
			break;
		//���Դ����жϳ���	
		case 0x01:
			SendResponse(cmd,0,0,NULL);
			break;
		default:break;
	}
	if(leng>0)
	{
		free(data);
	}
}
/*����1�жϷ������*/
void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET)
	{	
		USART_ClearITPendingBit(USART1,USART_IT_RXNE); //����жϱ�־λ
		USART_ITConfig(USART1, USART_IT_RXNE, DISABLE); //�رս����жϣ��ò�ѯ�ķ�ʽ����������
		//USARTInterruptFlag=1;	//�ô����жϱ�־λ
		DealDataOfUSART1();
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //���������ж�
	}
}
/****************************************************************************************************/
/*����ָ��ͨ��*/
/*USART2����һ����,ע��Ҫ���͵İ�����ֻ��������������ݰ��ͽ�����
*PackageLen:������У���������ֽ�*/
void USART2SendPackage(PackageType type, u16 PackageLen, u8 *PackageData)
{
	u16 i;
	u16 CRCcheck;

	//��������ͷ
	USARTSendByte(USART2,0xef);
	USARTSendByte(USART2,0x01);

	//����оƬ��ַ
	USARTSendByte(USART2,0xff);
	USARTSendByte(USART2,0xff);
	USARTSendByte(USART2,0xff);
	USARTSendByte(USART2,0xff);

	//����У���
	CRCcheck=0;

	//���Ͱ���ʶ
	USARTSendByte(USART2,(u8)type);
	CRCcheck+=(u8)type;
	
	//���Ͱ�����,����У����
	USARTSendByte(USART2,(PackageLen+2)/256);
	CRCcheck+=(PackageLen+2)/256;
	USARTSendByte(USART2,(PackageLen+2)%256);
	CRCcheck+=(PackageLen+2)%256;

	//����������
	for(i=0;i<PackageLen;i++)
	{
		USARTSendByte(USART2,PackageData[i]);
		CRCcheck+=PackageData[i];
	}
	USARTSendByte(USART2,CRCcheck/256);
	USARTSendByte(USART2,CRCcheck%256);
}
/*USART2����һ����
*���ذ�����Ч���ݵĳ��ȣ�����-1ʧ��*/
int USART2RecvPackage(u8 *PackageID)
{
	u8 cChar;
   	u16 wCmdLength=0;
   	u16 i;
   	u16 cCheckSum;
	u16 rCheckSum;
   	u8 gcpCmdHead[2]={0xef,0x01};
	u8 CmdHeadBuf[2];
	u8 ChipAddr[4]={0,0,0,0};
	u8 LenBuffer[2];
	
	if(USARTRecvByte(USART2,&CmdHeadBuf[0],50000)!=C_OK)	
		return -1;
	if(USARTRecvByte(USART2,&CmdHeadBuf[1],50000)!=C_OK)	
		return -1;
	switch (CmdHeadBuf[0])
	{
		case 0xef:
			if (CmdHeadBuf[1] != gcpCmdHead[1])
			{
	        	return -1;
			}

			for(i=0;i<4;i++)//��ʼ����оƬ��ַ					
			{
				if(USARTRecvByte(USART2,&ChipAddr[i],50000)!=C_OK)	
					return -1;
			}
			if(USARTRecvByte(USART2,&PackageID[0],50000)!=C_OK)// ��ʼ�������ݰ���ʶ	
				return -1;
			
			if(USARTRecvByte(USART2,&LenBuffer[0],50000)!=C_OK)// ��ʼ�������ݰ�����	
				return -1;
			if(USARTRecvByte(USART2,&LenBuffer[1],50000)!=C_OK)	
				return -1;
			
			cCheckSum = 0;					// �������
	       	cCheckSum+= *PackageID;
			cCheckSum+=LenBuffer[0]; 			
		    cCheckSum+=LenBuffer[1];

			wCmdLength=LenBuffer[0]*256+LenBuffer[1];
			wCmdLength-=2;					//��ȥ�����

			//�����������ݰ�����
   			for(i=0; i<wCmdLength; i++)
   			{	
				if(USARTRecvByte(USART2,&PackageBuffer[i],50000)!=C_OK)	
					return -1;
   				
   				cCheckSum+=PackageBuffer[i];
   			}

			if(USARTRecvByte(USART2,&cChar,50000)!=C_OK)	
				return -1;
			rCheckSum=cChar;

			if(USARTRecvByte(USART2,&cChar,50000)!=C_OK)	
				return -1;
			rCheckSum*=256;
			rCheckSum+=cChar;
			
	   		if(cCheckSum!=rCheckSum)//������Ƿ�һ��
	   		{
				return -1;
			}
			return wCmdLength;
		default:
			return -1;
   	} 
}
