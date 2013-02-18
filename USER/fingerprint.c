#include "stm32f10x.h"
#include "USART.h"
#include "comdef.h"
#include "globleexten.h"
#include "Beep.h"
#include "systick.h"
#include "12864lcd.h"

/*打开指纹模块电源*/
void FPPOWERON(void)
{
	 GPIO_SetBits(GPIOD, GPIO_Pin_15); //输出高电平
	 delay_ms(200);
}
/*关闭指纹模块电源*/
void FPPOWEROFF(void)
{
	 GPIO_ResetBits(GPIOD, GPIO_Pin_15); //输出低电平
	 delay_ms(10);
}
/*设置指纹参数*/
u8 PS_SetSysPara(u16 timeOut, u8 index, u8 Para)
{
	u8 cmdBuf[3];
	u8 cRetCode=0x00;
	int length;
	u8 PackageID;

	cmdBuf[0] = 0x0e;	//指令代码
	cmdBuf[1] =index;
	cmdBuf[2] =Para;		
	USART2SendPackage(CMDPACKAGE, 3, cmdBuf); //发送下载一个特征文件的命令包
	g_FPTimeOut=timeOut;
	while(g_FPTimeOut>0)
	{
		length=USART2RecvPackage(&PackageID);
		if(length!=-1)
		{
			if(PackageID==ASKPACKAGE)  cRetCode=PackageBuffer[0];	
			else cRetCode=0x1e;		//包类型错误等
			break;	
		}
	}
	if(g_FPTimeOut<=0)
		cRetCode=0x19;
	return cRetCode;
}
/*
* 函数功能: 从传感器上读入图像存于图像缓冲区
* 输入参数: 无
* 输出参数: 无
* 返回值  : C_OK
*/
u8 PS_GetImage(u16 timeOut)
{
	u8 cRetCode=0x00;
    u8 cmdBuf[1];
	int length;
	u8 PackageID;

	cmdBuf[0] = 0x01;
	g_FPTimeOut=timeOut;

	while(g_FPTimeOut>0)
	{	    	
		USART2SendPackage(CMDPACKAGE, 1, cmdBuf); //发送命令包,没有参数,命令长度
		length=USART2RecvPackage(&PackageID);
		if(length!=-1)
		{
			if(PackageID==ASKPACKAGE)
			{
				cRetCode=PackageBuffer[0];
				if(cRetCode==0)break;	
			}
		}
	}
	
	if(g_FPTimeOut<=0)
		cRetCode=0x19;

	return cRetCode;
}
/*
* 函数功能: 根据原始图像生成指纹特征存于CharBuffer1 或CharBuffer2,默认存放载charBuffer2中
* 输入参数: 无
* 输出参数: 无
* 返回值  : C_OK
*/
u8 PS_GenChar(u16 timeOut, u8 bufID)
{
	u8 cmdBuf[2];
	u8 cRetCode=0X00;
	int length;
	u8 PackageID;
	
	cmdBuf[0] = 0x02;
	if(bufID == 1)	 cmdBuf[1] = 0x01;	
	else cmdBuf[1] = 0x02;	//存放在charBurffer2中
    		
	USART2SendPackage(CMDPACKAGE, 2, cmdBuf); //发送命令包,没有参数,命令长度
	g_FPTimeOut=timeOut;
	while(g_FPTimeOut>0)
	{
		length=USART2RecvPackage(&PackageID);
		if(length!=-1)
		{
			if(PackageID==ASKPACKAGE) cRetCode=PackageBuffer[0];
			else cRetCode=0x1e;		//包类型错误等
			break;
		}
	}
	if(g_FPTimeOut<=0)
		cRetCode=0x19;
		
	return cRetCode;
}
/*
* 函数功能: 实现指纹特征采集
* 输入参数: index: 指示指纹特征存放的位置
* 输出参数: 无
* 返回值      : C_OK: 通过, 其他表示相应的错误码
* modify date     : 2009.04.01
*/
u8 FidGetTZ(u8 index)
{
	u8 cRetCode;
	
	//设置系统参数,数据包大小最大不能超过64B
	cRetCode=PS_SetSysPara(50, 6, 1);
	if(cRetCode!=C_OK) return (Mapped[cRetCode]);
	//设置安全级别
	cRetCode = PS_SetSysPara(50, 5, 3);
	if(cRetCode!=C_OK)	return (Mapped[cRetCode]);
	//挂擦手指采集指纹图像
	cRetCode=PS_GetImage(500);
	if(cRetCode!=C_OK) return (Mapped[cRetCode]);
    //生成特征,1秒钟超时
	cRetCode=PS_GenChar(50, index );
	if(cRetCode!=C_OK) return (Mapped[cRetCode]);
	return C_OK;
}
/*
* 函数功能: 将CharBuffer1 与CharBuffer2 中的特征文件合并生成模板存于CharBuffer2
* 输入参数: 无
* 输出参数: 无
* 返回值  : C_OK
*/
u8 PS_RegModel(u16 timeOut)//此函数还没有实现
{
	u8 cmdBuf[1];
	u8 cRetCode = 0x00;
	int length;
	u8 PackageID;
	
	cmdBuf[0] = 0x05;
	USART2SendPackage(CMDPACKAGE, 1, cmdBuf); //发送命令包,没有参数,命令长度
	g_FPTimeOut=timeOut;
	while(g_FPTimeOut>0)
	{
		length=USART2RecvPackage(&PackageID);
		if(length!=-1)
		{
			if(PackageID==ASKPACKAGE) cRetCode=PackageBuffer[0];
			else cRetCode=0x1e;	//包类型错误等
			break;
		}
	}
	if(g_FPTimeOut<=0)
		cRetCode=0x19;

	return cRetCode;
}
/*
* 函数功能: 精确匹配charBuffer1和charBuffer2中的指纹
* 输入参数: 无
* 输出参数: 无
* 返回值  : C_OK
*/
u8 PS_Match(u16 timeOut)
{
	u8 cmdBuf[1];
	u8 cRetCode=0X00;
	int length;
	u8 PackageID;

	cmdBuf[0] = 0x03;	//指令代码
	USART2SendPackage(CMDPACKAGE, 1, cmdBuf); //发送下载一个特征文件的命令包,没有参数,命令长度

	g_FPTimeOut=timeOut;
	while(g_FPTimeOut>0)
	{
		length=USART2RecvPackage(&PackageID);
		if(length!=-1)
		{
			if(PackageID==ASKPACKAGE)
				cRetCode=PackageBuffer[0];
			else
				cRetCode=0x1e;		//包类型错误等
			break;
		}
	}
	if(g_FPTimeOut<=0)
		cRetCode=0x19;

	return cRetCode;
}
/*指纹模块测试程序*/
void CollectFPTrain(void)
{
	u8 cRetCode;
	u8 cTimes=0;
	u8 cSingleTimes;

	Beep(2,50);
	USART1_SendNByte((u8*)"zhiwen ceshi\n",13);
	DispOutString(4,0,16,"    指纹测试    ",NORMALDISP);
	FPPOWERON();
	for (cTimes=0;cTimes<2;cTimes++) 
	{
		cSingleTimes =0;
		while (cSingleTimes<3) 
		{
			cRetCode = FidGetTZ(cTimes + 1);	//获取指纹特征点
			if (cRetCode!=C_OK) 
			{
				switch(cTimes)
				{
					case 0:
						USART1_SendNByte((u8*)"zhiwen ceshi1 fail\n",19);
						DispOutString(4,0,16,"  指纹1采集失败 ",NORMALDISP);
						delay_ms(1000);
   						break;
					case 1:
						USART1_SendNByte((u8*)"zhiwen ceshi2 fail\n",19);
						DispOutString(4,0,16,"  指纹2采集失败 ",NORMALDISP);
						delay_ms(1000);
   						break;
				}
				cSingleTimes++;
			}	
			else
			{
				switch(cTimes)
				{
					case 0:
						USART1_SendNByte((u8*)"zhiwen ceshi1 Success,and Continue zhiwen ceshi2\n",49);
						DispOutString(4,0,16,"  指纹1采集成功 ",NORMALDISP);
						delay_ms(1000);
	   					break;
					case 1:
						USART1_SendNByte((u8*)"zhiwen ceshi2 Success\n",23);
						DispOutString(4,0,16,"  指纹2采集成功 ",NORMALDISP);
						delay_ms(1000);
   						break;
				}
				break;
			}
		}
		if (cRetCode!=C_OK) 
		{
			USART1_SendNByte((u8*)"zhiwen fail\n",12);
			DispOutString(4,0,16,"  指纹采集失败  ",NORMALDISP);
			delay_ms(1000);
		}
	}
	if(cRetCode==C_OK)//在进行指纹采集成功的情况下才进行指纹的比较和合成
	{
		cRetCode=PS_Match(50);//指纹的比较
		if(cRetCode != C_OK)
		{
			USART1_SendNByte((u8*)"zhiwen compare fail\n",20);
			DispOutString(4,0,16,"  指纹比较失败  ",NORMALDISP);
			delay_ms(1000);	
		}
		else
		{
			USART1_SendNByte((u8*)"zhiwen compare Success\n",23);
			DispOutString(4,0,16,"  指纹比较成功  ",NORMALDISP);
			delay_ms(1000);
		}

		cRetCode = PS_RegModel(50);	//合成
		if(cRetCode != C_OK)
		{
			USART1_SendNByte((u8*)"zhiwen Synthesis fail\n",22);
			DispOutString(4,0,16,"  指纹合成失败  ",NORMALDISP); 
			delay_ms(1000);
		}
		else
		{
			USART1_SendNByte((u8*)"zhiwen Synthesis Success\n",25);	
			Beep(1,100);
			DispOutString(4,0,16,"  指纹合成成功  ",NORMALDISP);
			delay_ms(1000);
		}
	}
	else
	{
		USART1_SendNByte((u8*)"zhiwen Synthesis fail\n",22);
		DispOutString(4,0,16,"  指纹合成失败  ",NORMALDISP);
		delay_ms(1000);
	}
	FPPOWEROFF();
}
