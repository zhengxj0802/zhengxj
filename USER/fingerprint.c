#include "stm32f10x.h"
#include "USART.h"
#include "comdef.h"
#include "globleexten.h"
#include "Beep.h"
#include "systick.h"
#include "12864lcd.h"

/*��ָ��ģ���Դ*/
void FPPOWERON(void)
{
	 GPIO_SetBits(GPIOD, GPIO_Pin_15); //����ߵ�ƽ
	 delay_ms(200);
}
/*�ر�ָ��ģ���Դ*/
void FPPOWEROFF(void)
{
	 GPIO_ResetBits(GPIOD, GPIO_Pin_15); //����͵�ƽ
	 delay_ms(10);
}
/*����ָ�Ʋ���*/
u8 PS_SetSysPara(u16 timeOut, u8 index, u8 Para)
{
	u8 cmdBuf[3];
	u8 cRetCode=0x00;
	int length;
	u8 PackageID;

	cmdBuf[0] = 0x0e;	//ָ�����
	cmdBuf[1] =index;
	cmdBuf[2] =Para;		
	USART2SendPackage(CMDPACKAGE, 3, cmdBuf); //��������һ�������ļ��������
	g_FPTimeOut=timeOut;
	while(g_FPTimeOut>0)
	{
		length=USART2RecvPackage(&PackageID);
		if(length!=-1)
		{
			if(PackageID==ASKPACKAGE)  cRetCode=PackageBuffer[0];	
			else cRetCode=0x1e;		//�����ʹ����
			break;	
		}
	}
	if(g_FPTimeOut<=0)
		cRetCode=0x19;
	return cRetCode;
}
/*
* ��������: �Ӵ������϶���ͼ�����ͼ�񻺳���
* �������: ��
* �������: ��
* ����ֵ  : C_OK
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
		USART2SendPackage(CMDPACKAGE, 1, cmdBuf); //���������,û�в���,�����
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
* ��������: ����ԭʼͼ������ָ����������CharBuffer1 ��CharBuffer2,Ĭ�ϴ����charBuffer2��
* �������: ��
* �������: ��
* ����ֵ  : C_OK
*/
u8 PS_GenChar(u16 timeOut, u8 bufID)
{
	u8 cmdBuf[2];
	u8 cRetCode=0X00;
	int length;
	u8 PackageID;
	
	cmdBuf[0] = 0x02;
	if(bufID == 1)	 cmdBuf[1] = 0x01;	
	else cmdBuf[1] = 0x02;	//�����charBurffer2��
    		
	USART2SendPackage(CMDPACKAGE, 2, cmdBuf); //���������,û�в���,�����
	g_FPTimeOut=timeOut;
	while(g_FPTimeOut>0)
	{
		length=USART2RecvPackage(&PackageID);
		if(length!=-1)
		{
			if(PackageID==ASKPACKAGE) cRetCode=PackageBuffer[0];
			else cRetCode=0x1e;		//�����ʹ����
			break;
		}
	}
	if(g_FPTimeOut<=0)
		cRetCode=0x19;
		
	return cRetCode;
}
/*
* ��������: ʵ��ָ�������ɼ�
* �������: index: ָʾָ��������ŵ�λ��
* �������: ��
* ����ֵ      : C_OK: ͨ��, ������ʾ��Ӧ�Ĵ�����
* modify date     : 2009.04.01
*/
u8 FidGetTZ(u8 index)
{
	u8 cRetCode;
	
	//����ϵͳ����,���ݰ���С����ܳ���64B
	cRetCode=PS_SetSysPara(50, 6, 1);
	if(cRetCode!=C_OK) return (Mapped[cRetCode]);
	//���ð�ȫ����
	cRetCode = PS_SetSysPara(50, 5, 3);
	if(cRetCode!=C_OK)	return (Mapped[cRetCode]);
	//�Ҳ���ָ�ɼ�ָ��ͼ��
	cRetCode=PS_GetImage(500);
	if(cRetCode!=C_OK) return (Mapped[cRetCode]);
    //��������,1���ӳ�ʱ
	cRetCode=PS_GenChar(50, index );
	if(cRetCode!=C_OK) return (Mapped[cRetCode]);
	return C_OK;
}
/*
* ��������: ��CharBuffer1 ��CharBuffer2 �е������ļ��ϲ�����ģ�����CharBuffer2
* �������: ��
* �������: ��
* ����ֵ  : C_OK
*/
u8 PS_RegModel(u16 timeOut)//�˺�����û��ʵ��
{
	u8 cmdBuf[1];
	u8 cRetCode = 0x00;
	int length;
	u8 PackageID;
	
	cmdBuf[0] = 0x05;
	USART2SendPackage(CMDPACKAGE, 1, cmdBuf); //���������,û�в���,�����
	g_FPTimeOut=timeOut;
	while(g_FPTimeOut>0)
	{
		length=USART2RecvPackage(&PackageID);
		if(length!=-1)
		{
			if(PackageID==ASKPACKAGE) cRetCode=PackageBuffer[0];
			else cRetCode=0x1e;	//�����ʹ����
			break;
		}
	}
	if(g_FPTimeOut<=0)
		cRetCode=0x19;

	return cRetCode;
}
/*
* ��������: ��ȷƥ��charBuffer1��charBuffer2�е�ָ��
* �������: ��
* �������: ��
* ����ֵ  : C_OK
*/
u8 PS_Match(u16 timeOut)
{
	u8 cmdBuf[1];
	u8 cRetCode=0X00;
	int length;
	u8 PackageID;

	cmdBuf[0] = 0x03;	//ָ�����
	USART2SendPackage(CMDPACKAGE, 1, cmdBuf); //��������һ�������ļ��������,û�в���,�����

	g_FPTimeOut=timeOut;
	while(g_FPTimeOut>0)
	{
		length=USART2RecvPackage(&PackageID);
		if(length!=-1)
		{
			if(PackageID==ASKPACKAGE)
				cRetCode=PackageBuffer[0];
			else
				cRetCode=0x1e;		//�����ʹ����
			break;
		}
	}
	if(g_FPTimeOut<=0)
		cRetCode=0x19;

	return cRetCode;
}
/*ָ��ģ����Գ���*/
void CollectFPTrain(void)
{
	u8 cRetCode;
	u8 cTimes=0;
	u8 cSingleTimes;

	Beep(2,50);
	USART1_SendNByte((u8*)"zhiwen ceshi\n",13);
	DispOutString(4,0,16,"    ָ�Ʋ���    ",NORMALDISP);
	FPPOWERON();
	for (cTimes=0;cTimes<2;cTimes++) 
	{
		cSingleTimes =0;
		while (cSingleTimes<3) 
		{
			cRetCode = FidGetTZ(cTimes + 1);	//��ȡָ��������
			if (cRetCode!=C_OK) 
			{
				switch(cTimes)
				{
					case 0:
						USART1_SendNByte((u8*)"zhiwen ceshi1 fail\n",19);
						DispOutString(4,0,16,"  ָ��1�ɼ�ʧ�� ",NORMALDISP);
						delay_ms(1000);
   						break;
					case 1:
						USART1_SendNByte((u8*)"zhiwen ceshi2 fail\n",19);
						DispOutString(4,0,16,"  ָ��2�ɼ�ʧ�� ",NORMALDISP);
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
						DispOutString(4,0,16,"  ָ��1�ɼ��ɹ� ",NORMALDISP);
						delay_ms(1000);
	   					break;
					case 1:
						USART1_SendNByte((u8*)"zhiwen ceshi2 Success\n",23);
						DispOutString(4,0,16,"  ָ��2�ɼ��ɹ� ",NORMALDISP);
						delay_ms(1000);
   						break;
				}
				break;
			}
		}
		if (cRetCode!=C_OK) 
		{
			USART1_SendNByte((u8*)"zhiwen fail\n",12);
			DispOutString(4,0,16,"  ָ�Ʋɼ�ʧ��  ",NORMALDISP);
			delay_ms(1000);
		}
	}
	if(cRetCode==C_OK)//�ڽ���ָ�Ʋɼ��ɹ�������²Ž���ָ�ƵıȽϺͺϳ�
	{
		cRetCode=PS_Match(50);//ָ�ƵıȽ�
		if(cRetCode != C_OK)
		{
			USART1_SendNByte((u8*)"zhiwen compare fail\n",20);
			DispOutString(4,0,16,"  ָ�ƱȽ�ʧ��  ",NORMALDISP);
			delay_ms(1000);	
		}
		else
		{
			USART1_SendNByte((u8*)"zhiwen compare Success\n",23);
			DispOutString(4,0,16,"  ָ�ƱȽϳɹ�  ",NORMALDISP);
			delay_ms(1000);
		}

		cRetCode = PS_RegModel(50);	//�ϳ�
		if(cRetCode != C_OK)
		{
			USART1_SendNByte((u8*)"zhiwen Synthesis fail\n",22);
			DispOutString(4,0,16,"  ָ�ƺϳ�ʧ��  ",NORMALDISP); 
			delay_ms(1000);
		}
		else
		{
			USART1_SendNByte((u8*)"zhiwen Synthesis Success\n",25);	
			Beep(1,100);
			DispOutString(4,0,16,"  ָ�ƺϳɳɹ�  ",NORMALDISP);
			delay_ms(1000);
		}
	}
	else
	{
		USART1_SendNByte((u8*)"zhiwen Synthesis fail\n",22);
		DispOutString(4,0,16,"  ָ�ƺϳ�ʧ��  ",NORMALDISP);
		delay_ms(1000);
	}
	FPPOWEROFF();
}
