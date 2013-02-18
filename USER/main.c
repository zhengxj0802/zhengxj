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
	delay_init();				//延时函数初始化
	LCD_Configuration();		//液晶屏初始化
	Init_LCD();

	AT45DB161D_Configuration();	//AT45DB161D（SPI1）配置
	AT24C256_Configuration(); 	//AT24C256（I2C2）配置
	I2C2_Configuration();		//I2C1总线配置	硬件I2C使用
	ICCARD_Configuration();		//接触式逻辑加密卡槽（I2C1）
	I2C1_Configuration();		//I2C2总线配置	硬件I2C使用
  	//CPUCARD_Configuration();	//CPU卡配置
	KEY_Configuration(); 		//按键配置
	Beep_Configuration();		//蜂鸣器配置
	USART1_Configuration();		//串口1配置
	USART2_Configuration();		//串口2配置,用于指纹通信
	TIM2_Configuration(); 		//定时器2配置
	EXTI_Configuration();		//外部中断配置
	NVIC_Configuration();		//中断向量配置

	IWDG_Configuration();		//独立看门狗配置
	reboot=0;
/*************************************************************************/
/*************************************************************************/
	/*蜂鸣器测试,响一声，500ms*/
	Beep(1,500);
	/*液晶屏测试*/
	ClearLcd();
	DispOutString(0,4,8,"广州立秋",NORMALDISP);
	DispOutString(2,2,12,"硬件测试程序",NORMALDISP);
/*************************************************************************/
/*************************************************************************/
	/*按键测试*/
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
					DispOutString(4,0,16,"   确定键正常   ",NORMALDISP);
					break;
				case CancleKey:
					DispOutString(4,0,16,"   取消键正常   ",NORMALDISP);
					break;
				case DownKey:
					DispOutString(4,0,16,"   下翻键正常   ",NORMALDISP);
					break;
				case PowerKey:
					DispOutString(4,0,16,"   开机键正常   ",NORMALDISP);
					break;
				case UpKey:
					DispOutString(4,0,16,"   上翻键正常   ",NORMALDISP);
					break;
				default:break;
			}
			delay_ms(1000);
		}
	}
/*************************************************************************/
/*************************************************************************/
	/*AT45DB161D 测试*/
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
	/*24C256First测试,模拟I2C*/
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
	/*24C256Second测试,模拟I2C*/
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
	/*24C256First测试,硬件I2C*/
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
	/*24C256Second测试,硬件I2C*/
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
	/*接触式逻辑加密IC卡测试*/
	Result=ICCheckCardInsert();
	if(Result==C_OK)
	{
#if 0
		/*模拟I2C*/
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
		/*硬件I2C*/
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
//	/*接触式CPU卡测试*/
//	//启动CPU卡
//	for(i=0;i<15;i++)
//		readbuffer[i]=0;
//	Result=StartCPUCard(readbuffer);
//	if(Result==C_OK)
//	{
//		for(i=0;i<15;i++)//复旦微15，电科院4
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
//				writebuffer[i]=0;//外部验证原密码
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
//	//关闭CPU卡
//	StopCPUCard();	
/*************************************************************************/
/*************************************************************************/
	/*指纹测试*/
	CollectFPTrain();
/*************************************************************************/
/*************************************************************************/
	/*外部中断0测试*/
	DispOutString(4,0,16,"脉冲计数器:",NORMALDISP);
	while(gwSpeedPluseMile<10)
	{
		IntToString(gwSpeedPluseMile, gwSpeedPluseMileBuf,5);
		DispOutString(4,11,5,gwSpeedPluseMileBuf,NORMALDISP);
	}
/*************************************************************************/
/*************************************************************************/
	/*RTC测试*/
	RTC_Init();					//RTC配置
	RTCNVIC_Configuration();	//RTC中断向量配置
	RefreshTime=1;
/*************************************************************************/
/*************************************************************************/
	/*关闭显示屏背光灯*/
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
				reboot=1;//按下开机键，停止喂狗，等待看门狗复位重启
			}
		}
	}
}

/*独立看门狗配置*/
void IWDG_Configuration(void)
{
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); 	/* 写入0x5555,用于允许狗狗寄存器写入功能 */
  IWDG_SetPrescaler(IWDG_Prescaler_256);            /* 内部低速时钟256分频 40K/256=156HZ(6.4ms) */ 
  /* 看门狗定时器溢出时间 */
  IWDG_SetReload(781);							    /* 喂狗时间 5s/6.4MS=781 .注意不能大于0xfff*/
  IWDG_ReloadCounter();								/* 喂狗*/
  IWDG_Enable(); 									/* 使能看门狗*/
}
/*除RTC外中断向量配置*/
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
  /*串口中断*/
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	  
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  /*定时器2中断*/
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;	  
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//抢占式优先级
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;//响应式优先级	
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure); 
  /*外部中断0*/
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn; 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;   
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	      
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure); 
}
/*RTC中断向量配置*/
void RTCNVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0); 
  /*RTC中断*/
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure); 
}
