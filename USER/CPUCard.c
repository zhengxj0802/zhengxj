#include "stm32f10x.h"
#include "CPUCard.h"
#include "systick.h"
#include "USART.h"
#include "des.h"
#include "3des.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*��������ȫ�ֱ���*/
SC_ADPU_Responce  SC_ResponceStatus;
SC_ADPU_Commands  SC_ADPU;

/*CPU���ܽ�����,ģ�ⷽʽ*/
void CPUCARD_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIOD*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOD| RCC_APB2Periph_GPIOC| RCC_APB2Periph_GPIOA, ENABLE);

	/*ʹ�ܹܽ����ܿ�����*/
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode =GPIO_Mode_Out_PP;  
    GPIO_Init(GPIOC, &GPIO_InitStructure);
	POWEROFFICCARD();

	/*��⿨�Ƿ�������� */
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IPU;  
    GPIO_Init(GPIOA, &GPIO_InitStructure);

	/*io*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	/*rst*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	/*clk*/
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode =GPIO_Mode_Out_PP;  
    GPIO_Init(GPIOF, &GPIO_InitStructure);
}
/**********************************************************************/
/*�ж�CPU���Ƿ����*/
u8 ICCheckCardInsert(void)
{
	POWERONICCARD();
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8)==0)
	{ 
		delay_ms(10);//��ʱ10ms
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8)==0)
		{
			//POWEROFFICCARD();
			return 0;
		}
	}
	//POWEROFFICCARD();
	return C_NOIC;  
}
/**********************************************************************/
/*IC�����ϵ�*/
void POWERONICCARD(void)
{
    GPIO_SetBits(GPIOC , GPIO_Pin_9);
	delay_ms(5);//��ʱ5ms
}
/**********************************************************************/
/*IC�����µ�*/
void POWEROFFICCARD(void)
{
	GPIO_ResetBits(GPIOC, GPIO_Pin_9); 
	delay_ms(5);//��ʱ5ms	
}
/**********************************************************************/
/*��������ֽڣ���λ���򣬲����,��λ-->��λ*/
u8 Reverse(u8 input)
{
	u8 tmp=0;
	u8 i=0,Mask=0x80;
	for(i=0;i<8;i++)
	{	
		if(input&Mask)
			tmp|=0x80;
		
		Mask>>=1;
		if(i!=7)
			tmp>>=1;	
	}
	return tmp;
}
/**********************************************************************/
/*�������ݣ�1���ֽڣ�3�λ���*/
u8 ReceiveData(u8 *data)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	u32 i=0,j=0;
	u8 count=0;
	u32 g_RCPUCARDTimeOut=0;
	
	for(j=3;j>0;j--)
	{
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  		GPIO_Init(GPIOD, &GPIO_InitStructure);

		g_RCPUCARDTimeOut=5000;
		while(g_RCPUCARDTimeOut--) 
   		{
			for (i=0;i<100;i++) 
		 	{
		   		if (!GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_8)) /*�ȴ��͵�ƽ��һ���ֽڴ��俪ʼ*/
		  		{
		        	goto start;
		    	}
			}
   		} 
		return 0x19; //��ʱ
		start:
		delay_us(150);
		for(i=0;i<8;i++)
		{
			*data<<=1;
			if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_8))/*�жϹܽ��ǲ��Ǹߵ�ƽ*/
			{
		      	*data|=1;
				count++;/*�ۼ�1�ĸ���*/
			}
			else
			{
				*data|=0;
			}
			delay_us(100);
		}
		if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_8))/*������żУ��λ*/
		{
			count++;/*�ۼ�1�ĸ���*/
		}
		delay_us(100);
		/*�ж���żУ���Ƿ���ȷ*/
		if(count%2==0)
		{
			g_RCPUCARDTimeOut=5000;
			while(g_RCPUCARDTimeOut--) 
	   		{
				for (i=0;i<100;i++) 
				{
			  		if (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_8))/*�ȴ��ߵ�ƽ��һ���ֽڴ������*/
			   	 	{
			        	goto end;
			      	}
				}
	   		}
			return 0x19; //��ʱ  
			end:
			*data=Reverse(*data);
			return 0;
		}
		else
		{
			/*����͵�ƽ*/
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  			GPIO_Init(GPIOD, &GPIO_InitStructure);
			GPIO_ResetBits(GPIOD , GPIO_Pin_8);
			delay_us(100);
		}
	}
	return 0xff;
}
/**********************************************************************/
/*�������ݣ�һ���ֽ�*/
u8 SendData(u8 data)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	u16 i=0;
	u8 count=0;
	u8 databuf=0;

	/*�����ݽ��е���*/
	databuf=Reverse(data);
	
	/*����ߵ�ƽ*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_SetBits(GPIOD , GPIO_Pin_8);

	/*��ʱ0.5etu*/
	delay_us(50);	

	/*������ʼλ���͵�ƽ*/
	GPIO_ResetBits(GPIOD,GPIO_Pin_8);
	
	/*��ʱ1etu*/
	delay_us(100);
	
	/*����8λ����λ*/
	for(i=0;i<8;i++)
	{
		/*��������*/
		if(databuf&0x80)
		{
	      	GPIO_SetBits(GPIOD , GPIO_Pin_8);
			count++;
		}
	    else
	 	{
	  		GPIO_ResetBits(GPIOD , GPIO_Pin_8);
	  	}
			
		/*��λ*/
		databuf<<=1;

		/*��ʱ1etu*/
		delay_us(100);
	}
	/*������żУ��λ*/
	if(count%2==0)
	{
		GPIO_ResetBits(GPIOD , GPIO_Pin_8);/*����͵�ƽ*/
	}
	else
	{
		GPIO_SetBits(GPIOD , GPIO_Pin_8);/*����ߵ�ƽ*/
	}

	delay_us(100);

	/*��IO����Ϊ����*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	for(i=0;i<100;i++)
	{
		if(!GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_8))
		{
			USARTSendByte(USART1,i);
			return 0xff;
		}
		delay_us(1);
	}  
	return 0;
}
/**********************************************************************/
/*����CPU��*/
u8 StartCPUCard(u8 *Reset_ACK)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	u8 cRetCode=0xff;
	u8 count=0;
	u8 i=0;
	u8 ACK[128];

	/*�жϿ��Ƿ����*/
	cRetCode=ICCheckCardInsert();
	if(cRetCode!=0)
		return cRetCode;

	/******************�临λ***************************/
	/*�����ϵ�*/
	POWERONICCARD();
	
	/*RST����͵�ƽ*/
	GPIO_ResetBits(GPIOD, GPIO_Pin_9);

	/*����ʱ��*/
	GPIO_SetBits(GPIOF , GPIO_Pin_7);
	delay_us(5);
	
	/*IOΪ�ߣ�����Ϊ����*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/*��ʱ*/
	delay_us(1000);
	
	/*RST����ߵ�ƽ*/
	GPIO_SetBits(GPIOD , GPIO_Pin_9);

	/*����TS*/
	cRetCode=ReceiveData(&ACK[0]);
	if(cRetCode!=0)
	{
		USARTSendByte(USART1,cRetCode);
		return cRetCode;
	}

	/*****************�ȸ�λ***************************/
	if(ACK[0]!=0x3B && ACK[0]!=0x3F )
	{

	}
	/*******************************************************/
	/*��ʼ����Ӧ��*/
	if(ACK[0]==0x3B)//T0ģʽ
	{
		count=2;
	}
	else if(ACK[0]==0x3f)//T1ģʽ
	{
		
	}
	/*����TO*/
	cRetCode=ReceiveData(&ACK[1]);
	if(cRetCode!=0)
	{
		USARTSendByte(USART1,cRetCode);
		return cRetCode;
	}

	/*����ʣ����ֽ�*/
	for(i=0;i<count+(ACK[1]&0x0F);i++)
	{
		cRetCode=ReceiveData(&ACK[i+2]);
		if(cRetCode!=0)
		{
			USARTSendByte(USART1,cRetCode);
			return cRetCode;
		}
	}
	/*�ж�TCK������ַ���T0ģʽ���޴��ֽ�*/
	if(ACK[0]==0x3B)
	{
		delay_us(150);
		if(!GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_8))/*�͵�ƽ����ʾ��һ���ֽڵĿ�ʼ*/
			return 0xff;
	}
	
	for(i=0;i<count+2+(ACK[1]&0x0F);i++)
	{
		Reset_ACK[i]=ACK[i];
	}
	/****************************************************************/
	return 0;
}

/*ֹͣCPU��*/
void StopCPUCard(void)
{
	/*�ͷ�RST*/
	GPIO_ResetBits(GPIOD , GPIO_Pin_9);
	
	/*�ر�ʱ��*/
	GPIO_ResetBits(GPIOF , GPIO_Pin_7);

	/*��������*/
	POWEROFFICCARD();
}

/*����ADPUָ��*/
//u8 SendADPUData(SC_ADPU_Commands *SC_ADPU,SC_ADPU_Responce *SC_ResponceStatus)
u8 SendADPUData(void)
{
	u16 i = 0;
	u8 SCData=0;
	u8 cRetCode=0;
	
	/*�жϿ��Ƿ����*/
	cRetCode=ICCheckCardInsert();
	if(cRetCode!=0)
		return cRetCode;
	
	/*���ݳ�ʼ��*/
	for(i = 0; i < LCmax; i++)
	{
		SC_ResponceStatus.Data[i] = 0;
	}
	SC_ResponceStatus.SW1 = 0x00;
	SC_ResponceStatus.SW2 = 0x00;
	
	/*��֯���ݣ�������*/
	SCData = SC_ADPU.Header.CLA;
	cRetCode=SendData(SCData);
	if(cRetCode!=0) return cRetCode;
	SCData = SC_ADPU.Header.INS;
	cRetCode=SendData(SCData);
	if(cRetCode!=0) return cRetCode;
	SCData = SC_ADPU.Header.P1;
	cRetCode=SendData(SCData);
	if(cRetCode!=0) return cRetCode;
	SCData = SC_ADPU.Header.P2;
	cRetCode=SendData(SCData);
	if(cRetCode!=0) return cRetCode;
	if(SC_ADPU.Body.LC)
	{
		SCData = SC_ADPU.Body.LC;
		cRetCode=SendData(SCData);
		if(cRetCode!=0) return cRetCode;
	}
	else if(SC_ADPU.Body.LE)
	{ 
		SCData = SC_ADPU.Body.LE;
		cRetCode=SendData(SCData);
		if(cRetCode!=0) return cRetCode;
	}
	/* --------------------------------------------------------
    Wait Procedure byte from card:
    1 - ACK
    2 - NULL
    3 - SW1; SW2
   -------------------------------------------------------- */
	//USARTSendByte(USART1,'r');//test
   	/*��ʼ��������*/
	cRetCode=ReceiveData(&SCData);
	if(cRetCode!=0)
		return cRetCode;
	if(((SCData & 0xF0) == 0x60) || ((SCData & 0xF0) == 0x90))
	{
		/* SW1 received */
		SC_ResponceStatus.SW1 = SCData;
		
		/* SW2 received */
		cRetCode=ReceiveData(&SCData);
		if(cRetCode!=0)
			return cRetCode;
		
		SC_ResponceStatus.SW2 = SCData;
	}
	else if (((SCData &0xFE) == ((~(SC_ADPU.Header.INS)) & 0xFE))||((SCData & 0xFE) == (SC_ADPU.Header.INS & 0xFE)))
	{
		SC_ResponceStatus.Data[0] = SCData;/*ACK received */
	}
	if(SC_ResponceStatus.SW1 == 0x00)
	{
    	/* Send body data to SC--------------------------------------------------*/
		if (SC_ADPU.Body.LC)
		{
  			for(i = 0; i < SC_ADPU.Body.LC; i++)
  			{
    			SCData = SC_ADPU.Body.Data[i];
				cRetCode=SendData(SCData);
				if(cRetCode!=0) return cRetCode;
  			}
   		 }
		/* Or receive body data from SC ------------------------------------------*/
		else if (SC_ADPU.Body.LE)
		{
  			for(i = 0; i < SC_ADPU.Body.LE; i++)
  			{
      			cRetCode= ReceiveData(&SC_ResponceStatus.Data[i]);
				if(cRetCode!=0)
					return cRetCode;
    		}
  		}
		/* Wait SW1 --------------------------------------------------------------*/
    	cRetCode=ReceiveData(&SCData);
		if(cRetCode!=0)
			return cRetCode;
		SC_ResponceStatus.SW1=SCData;
    	/* Wait SW2 --------------------------------------------------------------*/   
    	cRetCode=ReceiveData(&SCData);
		if(cRetCode!=0)
			return cRetCode;
		SC_ResponceStatus.SW2=SCData ;
	}
	return 0;
}
/****************************************��ϸADPUָ���**************************/
/*ȡ�����*/
/*data:���ȡ�ص������*/
/*length:��������ֽ�����4����8*/
u8 Get_Challenge(u8 *data,u8 length)
{
	u8 Result=0;
	u8 i=0;
	/*��֯ADPU����*/
	SC_ADPU.Header.CLA = 0x00;
	SC_ADPU.Header.INS = 0x84;
	SC_ADPU.Header.P1  = 0x00;
	SC_ADPU.Header.P2  = 0x00; 
	SC_ADPU.Body.LE = length;
	/*����ADPU����*/
	Result=SendADPUData();
	if(Result==0)
	{
		USARTSendByte(USART1,SC_ResponceStatus.SW1);
		USARTSendByte(USART1,SC_ResponceStatus.SW2);
		if((SC_ResponceStatus.SW1==0x90) && (SC_ResponceStatus.SW2==0x00))
		{	
			memcpy(data,&SC_ResponceStatus.Data[i],length);
			for(i=0;i<length;i++)
			{
				USARTSendByte(USART1,SC_ResponceStatus.Data[i]);
			}
		}
		else
		{
			Result=SC_ResponceStatus.SW1;//����Ӧ��
		}
	}
	return Result;	
}
/*�ⲿ��֤*/
/*data:����������*/
/*length:�����������ĳ���*/
/*key:�ⲿ��֤�ܳ�*/
/*keyleng:�ⲿ��֤�ܳ׵ĳ���*/
/*type:�����㷨������*/
u8 External_Authenticate(u8 *data,u8 length,u8 *key,u8 keyleng,u8 type)
{
	u8 Result=0;
	u8 keybuf[16];
	u8 i=0;
	/*��������*/
	if(type==0)//des
	{
		encrypt(key,data,1);
		memcpy(keybuf,data,length);  
	}
	else if(type==1)//3des
	{
		DES3_CRYPT(1,key,data,keybuf,length);//����
	}
	for(i=0;i<length;i++)
	{
		USARTSendByte(USART1,keybuf[i]);	
	}
	/*��֯ADPU����*/
	SC_ADPU.Header.CLA = 0x00;
	SC_ADPU.Header.INS = 0x82;
	SC_ADPU.Header.P1  = 0x00;
	SC_ADPU.Header.P2  = 0x00; 
	SC_ADPU.Body.LC = 0x08;
	for(i=0;i<8;i++)
		SC_ADPU.Body.Data[i]=keybuf[i];
	/*����ADPU����*/
	Result=SendADPUData();
	if(Result==0)
	{
		USARTSendByte(USART1,SC_ResponceStatus.SW1);
		USARTSendByte(USART1,SC_ResponceStatus.SW2);
		if((SC_ResponceStatus.SW1==0x90) && (SC_ResponceStatus.SW2==0x00))
		{
			Result=0;
		}
		else
		{
			Result=SC_ResponceStatus.SW1;//����Ӧ��
		}
	}
	return Result;
}
/*ѡ���ļ�*/
u8 Select_file(u8 *file)
{
	u8 Result=0;
	u8 i=0;
	/*��֯ADPU����*/
	SC_ADPU.Header.CLA = 0x00;
	SC_ADPU.Header.INS = 0xa4;
	SC_ADPU.Header.P1  = file[0];
	SC_ADPU.Header.P2  = file[1]; 
	SC_ADPU.Body.LC = file[2];
	for(i=0;i<file[2];i++)
		SC_ADPU.Body.Data[i]=file[3+i]; //�ļ���

	/*����ADPU����*/
	Result=SendADPUData();
	if(Result==0)
	{
		USARTSendByte(USART1,SC_ResponceStatus.SW1);
		USARTSendByte(USART1,SC_ResponceStatus.SW2);
		if((SC_ResponceStatus.SW1==0x90) && (SC_ResponceStatus.SW2==0x00))
		{	
			Result=0;	
		}
		else
		{
			Result=SC_ResponceStatus.SW1;//����Ӧ��
		}
	}
	return Result; 	
}
/*ɾ�������ļ�*/
u8 Erase_DF(void)
{
	u8 Result=0;
	/*��֯ADPU����*/
	SC_ADPU.Header.CLA = 0xbf;
	SC_ADPU.Header.INS = 0xce;
	SC_ADPU.Header.P1  = 0x00;
	SC_ADPU.Header.P2  = 0x00; 
	SC_ADPU.Body.LC = 0x00;
	/*����ADPU����*/
	Result=SendADPUData();
	if(Result==0)
	{
		USARTSendByte(USART1,SC_ResponceStatus.SW1);
		USARTSendByte(USART1,SC_ResponceStatus.SW2);
		if((SC_ResponceStatus.SW1==0x90) && (SC_ResponceStatus.SW2==0x00))
		{	
			Result=0;	
		}
		else
		{
			Result=SC_ResponceStatus.SW1;//����Ӧ��
		}
	}
	return Result;	
}
/*�����ļ�*/
u8 Create_File(u8 *file)
{
	u8 Result=0;

	return Result;
}
/*���������ܳ�(�ⲿ��֤�ܳ�)*/
u8 Create_ExternalKey(u8 *key)
{
	u8 Result=0;

	return Result; 	
}
/*�޸������ܳ�(�ⲿ��֤�ܳ�)*/
u8 Update_ExternalKey(u8 *key)
{
	u8 Result=0;

	return Result; 	
}
/*��������*/
u8 Create_Pin(u8 *Pin)
{
	u8 Result=0;

	return Result; 	
}
/*��֤����*/
u8 Verify_Pin(u8 *Pin)
{
	u8 Result=0;

	return Result; 	
} 
/*д��¼�ļ�*/
u8 Write_Record(u8 *data)
{
	u8 Result=0;

	return Result;	
}
/*���¼�¼�ļ�*/
u8 Update_Record(u8 *data)
{
	u8 Result=0;

	return Result;	
}
/*����¼�ļ�*/
u8 Read_Record(u8 *data)
{
	u8 Result=0;

	return Result;	
}
/*д�������ļ�*/
u8 Write_Binary(u8 *data)
{
	u8 Result=0;

	return Result;	
}
/*���¶������ļ�*/
u8 Update_Binary(u8 *data)
{
	u8 Result=0;

	return Result;	
}
/*���������ļ�*/
u8 Read_Binary(u8 *data)
{
	u8 Result=0;

	return Result;	
}
/*ȡ��Ӧ����*/
u8 get_response(u8 *data)
{
	u8 Result=0;

	return Result;	
}
