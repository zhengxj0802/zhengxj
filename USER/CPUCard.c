#include "stm32f10x.h"
#include "CPUCard.h"
#include "systick.h"
#include "USART.h"
#include "des.h"
#include "3des.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*定义两个全局变量*/
SC_ADPU_Responce  SC_ResponceStatus;
SC_ADPU_Commands  SC_ADPU;

/*CPU卡管脚配置,模拟方式*/
void CPUCARD_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIOD*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOD| RCC_APB2Periph_GPIOC| RCC_APB2Periph_GPIOA, ENABLE);

	/*使能管脚智能卡供电*/
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode =GPIO_Mode_Out_PP;  
    GPIO_Init(GPIOC, &GPIO_InitStructure);
	POWEROFFICCARD();

	/*检测卡是否插入配置 */
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
/*判断CPU卡是否插入*/
u8 ICCheckCardInsert(void)
{
	POWERONICCARD();
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8)==0)
	{ 
		delay_ms(10);//延时10ms
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
/*IC卡槽上电*/
void POWERONICCARD(void)
{
    GPIO_SetBits(GPIOC , GPIO_Pin_9);
	delay_ms(5);//延时5ms
}
/**********************************************************************/
/*IC卡槽下电*/
void POWEROFFICCARD(void)
{
	GPIO_ResetBits(GPIOC, GPIO_Pin_9); 
	delay_ms(5);//延时5ms	
}
/**********************************************************************/
/*将输入的字节，按位反序，并输出,高位-->低位*/
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
/*接收数据，1个字节，3次机会*/
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
		   		if (!GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_8)) /*等待低电平，一个字节传输开始*/
		  		{
		        	goto start;
		    	}
			}
   		} 
		return 0x19; //超时
		start:
		delay_us(150);
		for(i=0;i<8;i++)
		{
			*data<<=1;
			if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_8))/*判断管脚是不是高电平*/
			{
		      	*data|=1;
				count++;/*累计1的个数*/
			}
			else
			{
				*data|=0;
			}
			delay_us(100);
		}
		if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_8))/*接收奇偶校验位*/
		{
			count++;/*累计1的个数*/
		}
		delay_us(100);
		/*判断奇偶校验是否正确*/
		if(count%2==0)
		{
			g_RCPUCARDTimeOut=5000;
			while(g_RCPUCARDTimeOut--) 
	   		{
				for (i=0;i<100;i++) 
				{
			  		if (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_8))/*等待高电平，一个字节传输结束*/
			   	 	{
			        	goto end;
			      	}
				}
	   		}
			return 0x19; //超时  
			end:
			*data=Reverse(*data);
			return 0;
		}
		else
		{
			/*输出低电平*/
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
/*发送数据，一个字节*/
u8 SendData(u8 data)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	u16 i=0;
	u8 count=0;
	u8 databuf=0;

	/*将数据进行倒序*/
	databuf=Reverse(data);
	
	/*输出高电平*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_SetBits(GPIOD , GPIO_Pin_8);

	/*延时0.5etu*/
	delay_us(50);	

	/*发送起始位，低电平*/
	GPIO_ResetBits(GPIOD,GPIO_Pin_8);
	
	/*延时1etu*/
	delay_us(100);
	
	/*发送8位数据位*/
	for(i=0;i<8;i++)
	{
		/*发送数据*/
		if(databuf&0x80)
		{
	      	GPIO_SetBits(GPIOD , GPIO_Pin_8);
			count++;
		}
	    else
	 	{
	  		GPIO_ResetBits(GPIOD , GPIO_Pin_8);
	  	}
			
		/*移位*/
		databuf<<=1;

		/*延时1etu*/
		delay_us(100);
	}
	/*发送奇偶校验位*/
	if(count%2==0)
	{
		GPIO_ResetBits(GPIOD , GPIO_Pin_8);/*输出低电平*/
	}
	else
	{
		GPIO_SetBits(GPIOD , GPIO_Pin_8);/*输出高电平*/
	}

	delay_us(100);

	/*将IO设置为接收*/
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
/*启动CPU卡*/
u8 StartCPUCard(u8 *Reset_ACK)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	u8 cRetCode=0xff;
	u8 count=0;
	u8 i=0;
	u8 ACK[128];

	/*判断卡是否插入*/
	cRetCode=ICCheckCardInsert();
	if(cRetCode!=0)
		return cRetCode;

	/******************冷复位***************************/
	/*给卡上电*/
	POWERONICCARD();
	
	/*RST输出低电平*/
	GPIO_ResetBits(GPIOD, GPIO_Pin_9);

	/*启动时钟*/
	GPIO_SetBits(GPIOF , GPIO_Pin_7);
	delay_us(5);
	
	/*IO为高，并且为输入*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/*延时*/
	delay_us(1000);
	
	/*RST输出高电平*/
	GPIO_SetBits(GPIOD , GPIO_Pin_9);

	/*接收TS*/
	cRetCode=ReceiveData(&ACK[0]);
	if(cRetCode!=0)
	{
		USARTSendByte(USART1,cRetCode);
		return cRetCode;
	}

	/*****************热复位***************************/
	if(ACK[0]!=0x3B && ACK[0]!=0x3F )
	{

	}
	/*******************************************************/
	/*开始接收应答*/
	if(ACK[0]==0x3B)//T0模式
	{
		count=2;
	}
	else if(ACK[0]==0x3f)//T1模式
	{
		
	}
	/*接收TO*/
	cRetCode=ReceiveData(&ACK[1]);
	if(cRetCode!=0)
	{
		USARTSendByte(USART1,cRetCode);
		return cRetCode;
	}

	/*接收剩余的字节*/
	for(i=0;i<count+(ACK[1]&0x0F);i++)
	{
		cRetCode=ReceiveData(&ACK[i+2]);
		if(cRetCode!=0)
		{
			USARTSendByte(USART1,cRetCode);
			return cRetCode;
		}
	}
	/*判断TCK，检测字符，T0模式下无此字节*/
	if(ACK[0]==0x3B)
	{
		delay_us(150);
		if(!GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_8))/*低电平，表示下一个字节的开始*/
			return 0xff;
	}
	
	for(i=0;i<count+2+(ACK[1]&0x0F);i++)
	{
		Reset_ACK[i]=ACK[i];
	}
	/****************************************************************/
	return 0;
}

/*停止CPU卡*/
void StopCPUCard(void)
{
	/*释放RST*/
	GPIO_ResetBits(GPIOD , GPIO_Pin_9);
	
	/*关闭时钟*/
	GPIO_ResetBits(GPIOF , GPIO_Pin_7);

	/*给卡掉电*/
	POWEROFFICCARD();
}

/*发送ADPU指令*/
//u8 SendADPUData(SC_ADPU_Commands *SC_ADPU,SC_ADPU_Responce *SC_ResponceStatus)
u8 SendADPUData(void)
{
	u16 i = 0;
	u8 SCData=0;
	u8 cRetCode=0;
	
	/*判断卡是否插入*/
	cRetCode=ICCheckCardInsert();
	if(cRetCode!=0)
		return cRetCode;
	
	/*数据初始化*/
	for(i = 0; i < LCmax; i++)
	{
		SC_ResponceStatus.Data[i] = 0;
	}
	SC_ResponceStatus.SW1 = 0x00;
	SC_ResponceStatus.SW2 = 0x00;
	
	/*组织数据，并发送*/
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
   	/*开始接收数据*/
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
/****************************************详细ADPU指令传输**************************/
/*取随机数*/
/*data:存放取回的随机数*/
/*length:随机数的字节数，4或者8*/
u8 Get_Challenge(u8 *data,u8 length)
{
	u8 Result=0;
	u8 i=0;
	/*组织ADPU报文*/
	SC_ADPU.Header.CLA = 0x00;
	SC_ADPU.Header.INS = 0x84;
	SC_ADPU.Header.P1  = 0x00;
	SC_ADPU.Header.P2  = 0x00; 
	SC_ADPU.Body.LE = length;
	/*发送ADPU报文*/
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
			Result=SC_ResponceStatus.SW1;//错误应答
		}
	}
	return Result;	
}
/*外部验证*/
/*data:传入的随机数*/
/*length:传入的随机数的长度*/
/*key:外部验证密匙*/
/*keyleng:外部验证密匙的长度*/
/*type:加密算法的类型*/
u8 External_Authenticate(u8 *data,u8 length,u8 *key,u8 keyleng,u8 type)
{
	u8 Result=0;
	u8 keybuf[16];
	u8 i=0;
	/*加密运算*/
	if(type==0)//des
	{
		encrypt(key,data,1);
		memcpy(keybuf,data,length);  
	}
	else if(type==1)//3des
	{
		DES3_CRYPT(1,key,data,keybuf,length);//加密
	}
	for(i=0;i<length;i++)
	{
		USARTSendByte(USART1,keybuf[i]);	
	}
	/*组织ADPU报文*/
	SC_ADPU.Header.CLA = 0x00;
	SC_ADPU.Header.INS = 0x82;
	SC_ADPU.Header.P1  = 0x00;
	SC_ADPU.Header.P2  = 0x00; 
	SC_ADPU.Body.LC = 0x08;
	for(i=0;i<8;i++)
		SC_ADPU.Body.Data[i]=keybuf[i];
	/*发送ADPU报文*/
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
			Result=SC_ResponceStatus.SW1;//错误应答
		}
	}
	return Result;
}
/*选择文件*/
u8 Select_file(u8 *file)
{
	u8 Result=0;
	u8 i=0;
	/*组织ADPU报文*/
	SC_ADPU.Header.CLA = 0x00;
	SC_ADPU.Header.INS = 0xa4;
	SC_ADPU.Header.P1  = file[0];
	SC_ADPU.Header.P2  = file[1]; 
	SC_ADPU.Body.LC = file[2];
	for(i=0;i<file[2];i++)
		SC_ADPU.Body.Data[i]=file[3+i]; //文件名

	/*发送ADPU报文*/
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
			Result=SC_ResponceStatus.SW1;//错误应答
		}
	}
	return Result; 	
}
/*删除所有文件*/
u8 Erase_DF(void)
{
	u8 Result=0;
	/*组织ADPU报文*/
	SC_ADPU.Header.CLA = 0xbf;
	SC_ADPU.Header.INS = 0xce;
	SC_ADPU.Header.P1  = 0x00;
	SC_ADPU.Header.P2  = 0x00; 
	SC_ADPU.Body.LC = 0x00;
	/*发送ADPU报文*/
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
			Result=SC_ResponceStatus.SW1;//错误应答
		}
	}
	return Result;	
}
/*建立文件*/
u8 Create_File(u8 *file)
{
	u8 Result=0;

	return Result;
}
/*建立主控密匙(外部验证密匙)*/
u8 Create_ExternalKey(u8 *key)
{
	u8 Result=0;

	return Result; 	
}
/*修改主控密匙(外部验证密匙)*/
u8 Update_ExternalKey(u8 *key)
{
	u8 Result=0;

	return Result; 	
}
/*建立口令*/
u8 Create_Pin(u8 *Pin)
{
	u8 Result=0;

	return Result; 	
}
/*验证口令*/
u8 Verify_Pin(u8 *Pin)
{
	u8 Result=0;

	return Result; 	
} 
/*写记录文件*/
u8 Write_Record(u8 *data)
{
	u8 Result=0;

	return Result;	
}
/*更新记录文件*/
u8 Update_Record(u8 *data)
{
	u8 Result=0;

	return Result;	
}
/*读记录文件*/
u8 Read_Record(u8 *data)
{
	u8 Result=0;

	return Result;	
}
/*写二进制文件*/
u8 Write_Binary(u8 *data)
{
	u8 Result=0;

	return Result;	
}
/*更新二进制文件*/
u8 Update_Binary(u8 *data)
{
	u8 Result=0;

	return Result;	
}
/*读二进制文件*/
u8 Read_Binary(u8 *data)
{
	u8 Result=0;

	return Result;	
}
/*取响应数据*/
u8 get_response(u8 *data)
{
	u8 Result=0;

	return Result;	
}
