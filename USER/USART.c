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

/*串口1初始化*/
void USART1_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	//RCC使能
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1,ENABLE);

	//IO口配置 USART1_TX -> PA9 , USART1_RX ->	PA10			
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);		   

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	        
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//串口参数配置
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART1, &USART_InitStructure); 
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //开启接收中断
	USART_Cmd(USART1, ENABLE);
}
/*串口2初始化，用于指纹通信*/
void USART2_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	//RCC使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOD,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

	//IO口配置 USART2_TX -> PA2 , USART2_RX ->PA3			
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);		   

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	        
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IN_FLOATING; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// PD15,指纹电源控制脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOD, GPIO_Pin_15); //输出低电平,关闭指纹模块电源

	//串口参数配置
	USART_InitStructure.USART_BaudRate = 57600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure); 
	USART_Cmd(USART2, ENABLE);
}
/*串口接收一个字节，含接收超时处理*/
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
/*串口发送一个字节的数据*/
void USARTSendByte(USART_TypeDef* USARTx,u8 data)
{	
	while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);//发送数据标志为是否为空
	USART_SendData(USARTx,data);
}
/*串口1发送多个字节的数据*/
void USART1_SendNByte(u8 *data,u16 len)
{
	u16 i;
	USART_ClearFlag(USART1,USART_FLAG_TC);//清除标志位，否则第1位数据会丢失
	for (i=0; i<len; i++)
	{
		USARTSendByte(USART1,data[i]);	
	}
}
/*串口1发送一串数据，含命令头*/
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
	//命令头
	for(i=0;i<3;i++)
	{
		buf[i]=head[i];
		buf[length+11-1]+=buf[i]; //累加校验和
	}
	//数据长度
	buf[3]=(length+6)/256;
	buf[length+11-1]+=buf[3]; //累加校验和
	buf[4]=(length+6)%256;
	buf[length+11-1]+=buf[4]; //累加校验和

	//命令字
	for(i=0;i<3;i++)
	{
		buf[5+i]=cmd[i];
		buf[length+11-1]+=buf[5+i];//累加校验和
	}

	//处理结果
	buf[8]=(ccRetCode)/256;
	buf[length+11-1]+=buf[8];//累加校验和
	buf[9]=(ccRetCode)%256;
	buf[length+11-1]+=buf[9];//累加校验和

	//有效数据
	for(i=0;i<length;i++)
	{
		buf[10+i]=data[i];
		buf[length+11-1]+=buf[10+i]; //累加校验和
	}
	//发送数据
	USART1_SendNByte(buf,length+11);
	free(buf);
}
/*更新程序*/
void CmdDownLoadProgram(u8*buf)
{
	u8 SymRestData[5]={0x1a,0x72,0x73,0x00,0x00};
	SymRestData[3]=buf[0];
	SymRestData[4]=buf[1];
	I2C_Write(I2C2,ADDR_First24C256_Write,0x10,SymRestData,5);
}
/*下载中文字库*/
void CmdDownLoadziku(void)
{
	u32 cPage=0;
	u8 Buffer[528];
	u8 cCheckSum;
	u32 i;
	/*液晶屏显示*/
	ClearLcd();
	DispOutString(2, 1, 14, "INIT DATAFLASH",NORMALDISP);
	/*
	*发送肯定回应信息，然后上位机开始发送数据
	*一页一页发送，共发送DFPAGECOUNT页，每页接收成功并写入正确后发送'p'
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
		delay_ms(20);//延时 20ms
		USARTSendByte(USART1,(u8)'p');
	}
	/*设置相应的标志区，然后重启*/
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
/*串口1接收数据及处理信息*/
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
	/*接收数据*/
	for(i=1;i<3;i++)//接收命令头
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

	for(i=0;i<2;i++)//接收数据长度
	{
		if(USARTRecvByte(USART1,&length[i],50000)!=C_OK)
		 {	
			return;
		 }
		sumcheck+=length[i];
	}
	for(i=0;i<3;i++) //接收命令体
	{
		if(USARTRecvByte(USART1,&cmd[i],50000)!=C_OK)
		{
			return;
		}
		sumcheck+=cmd[i];
	}
	leng=length[0]*256+length[1]-4;//根据数据长度计算除校验码1个,命令字3个后的数据长度
	if(leng>0)
	{
		data=(unsigned char*)malloc(leng); //动态分配空间
		if(data==NULL)
		{
			free(data);
			return;
		}
		for(i=0;i<leng;i++)//接收有效数据
		{
			if(USARTRecvByte(USART1,&data[i],50000)!=C_OK)
			{	
			 	return;
			}
			sumcheck+=data[i];
		}
	}
	if(USARTRecvByte(USART1,&sumreceive,50000)!=C_OK)//接收校验码并比较校验和
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
	/*数据处理*/
	switch(cmd[0])
	{
		case 0xb2:
			if(cmd[1]==0x00) //更新程序
			{
				CmdDownLoadProgram(data);
				SystemReset();
			}
			else if(cmd[1]==0x01) //下载字库
			{
				CmdDownLoadziku();
				SystemReset();
			}
			break;
		//测试串口中断程序	
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
/*串口1中断服务程序*/
void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET)
	{	
		USART_ClearITPendingBit(USART1,USART_IT_RXNE); //清除中断标志位
		USART_ITConfig(USART1, USART_IT_RXNE, DISABLE); //关闭接收中断，用查询的方式来接收数据
		//USARTInterruptFlag=1;	//置串口中断标志位
		DealDataOfUSART1();
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //开启接收中断
	}
}
/****************************************************************************************************/
/*用于指纹通信*/
/*USART2发送一个包,注意要发送的包类型只能是命令包，数据包和结束包
*PackageLen:不包括校验码两个字节*/
void USART2SendPackage(PackageType type, u16 PackageLen, u8 *PackageData)
{
	u16 i;
	u16 CRCcheck;

	//发送命令头
	USARTSendByte(USART2,0xef);
	USARTSendByte(USART2,0x01);

	//发送芯片地址
	USARTSendByte(USART2,0xff);
	USARTSendByte(USART2,0xff);
	USARTSendByte(USART2,0xff);
	USARTSendByte(USART2,0xff);

	//计算校验和
	CRCcheck=0;

	//发送包标识
	USARTSendByte(USART2,(u8)type);
	CRCcheck+=(u8)type;
	
	//发送包长度,加上校验码
	USARTSendByte(USART2,(PackageLen+2)/256);
	CRCcheck+=(PackageLen+2)/256;
	USARTSendByte(USART2,(PackageLen+2)%256);
	CRCcheck+=(PackageLen+2)%256;

	//发送命令体
	for(i=0;i<PackageLen;i++)
	{
		USARTSendByte(USART2,PackageData[i]);
		CRCcheck+=PackageData[i];
	}
	USARTSendByte(USART2,CRCcheck/256);
	USARTSendByte(USART2,CRCcheck%256);
}
/*USART2接收一个包
*返回包的有效数据的长度，返回-1失败*/
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

			for(i=0;i<4;i++)//开始接收芯片地址					
			{
				if(USARTRecvByte(USART2,&ChipAddr[i],50000)!=C_OK)	
					return -1;
			}
			if(USARTRecvByte(USART2,&PackageID[0],50000)!=C_OK)// 开始接收数据包标识	
				return -1;
			
			if(USARTRecvByte(USART2,&LenBuffer[0],50000)!=C_OK)// 开始接收数据包长度	
				return -1;
			if(USARTRecvByte(USART2,&LenBuffer[1],50000)!=C_OK)	
				return -1;
			
			cCheckSum = 0;					// 计算检查和
	       	cCheckSum+= *PackageID;
			cCheckSum+=LenBuffer[0]; 			
		    cCheckSum+=LenBuffer[1];

			wCmdLength=LenBuffer[0]*256+LenBuffer[1];
			wCmdLength-=2;					//减去检验和

			//继续接收数据包内容
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
			
	   		if(cCheckSum!=rCheckSum)//检验和是否一致
	   		{
				return -1;
			}
			return wCmdLength;
		default:
			return -1;
   	} 
}
