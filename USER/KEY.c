#include "stm32f10x.h"
#include "systick.h"
#include "comdef.h"
#include "globleexten.h"
#include "Beep.h"
#include "fingerprint.h"
#include "USART.h"

/*key配置*/
void KEY_Configuration(void)
{
	//JOY_UP -> PD4 , JOY_DOWN -> PD1 , JOY_LEFT -> PD2 , JOY_RIGHT -> PD3 , JOY_SEL -> PD0
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG , ENABLE); 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_4 | GPIO_Pin_3 | GPIO_Pin_2 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //GPIO_Mode_IPU上拉输入
	GPIO_Init(GPIOG, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE); 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //GPIO_Mode_IPU上拉输入
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}
/*键值扫描*/
u8 ScanKey(void)
{
	KeyCuration=0; 	//按键被持续按下的时间
	KeyPressflag=0;	//按键是否有被按下
    if((GPIO_ReadInputData(GPIOG)&0xFFFF)!=0xFFFF)  //判断是否有按键被按下
	{
		delay_ms(10);//延时10ms
		if((GPIO_ReadInputData(GPIOG)&0xFFFF)!=0xFFFF)  //判断是否有按键被按下
		{	
			if(!GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_2))	//确定键
			{	
				KeyPressflag=1;//有按键按下
				KeyCuration=0;//计数器清零
				while(!GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_2)&&(!(KeyCuration>KeyPressTime))); //等待按键释放或者按下按键超过3秒
				if(KeyCuration>KeyPressTime)//按下按键的时间超过3秒(20ms*150)就属于长按按键处理，下同
				{
					KeyCuration=0;//按键被持续按下的时间清零，以便下次从零开始，下同
					return(EnterLongKey);
				}
				else
				{
					KeyCuration=0;
					return(EnterKey);	
				}
			}	
			else if(!GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_3)) //上翻键
			{
				KeyPressflag=1;//有按键按下
				KeyCuration=0;//计数器清零
				while((!GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_3))&&(!(KeyCuration>KeyPressTime))); //等待按键释放或者按下按键超过3秒
				if(KeyCuration>KeyPressTime)//按下按键的时间超过3秒(20ms*150)就属于长按按键处理，下同
				{
					KeyCuration=0;
					return(UpLongKey);
				}
				else
				{
					KeyCuration=0;
					return(UpKey);
				}
			}
			else if(!GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_4)) //下翻键
			{
				KeyPressflag=1;//有按键按下
				KeyCuration=0;//计数器清零
				while((!GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_4))&&(!(KeyCuration>KeyPressTime))); //等待按键释放或者按下按键超过3秒
				if(KeyCuration>KeyPressTime)//按下按键的时间超过3秒(20ms*150)就属于长按按键处理，下同
				{
					KeyCuration=0;
					return(DownLongKey);
				}
				else
				{
					KeyCuration=0;
					return(DownKey);
				}
			}
			else if(!GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_5))//取消键
			{
				KeyPressflag=1;//有按键按下
				KeyCuration=0;//计数器清零
				while((!GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_5))&&(!(KeyCuration>KeyPressTime))); //等待按键释放或者按下按键超过3秒
				if(KeyCuration>KeyPressTime)//按下按键的时间超过3秒(20ms*150)就属于长按按键处理，下同
				{
					KeyCuration=0;
					return(CancleLongKey);
				}
				else
				{
					KeyCuration=0;
					return(CancleKey);	
				}
			}
		}
	}
	if((GPIO_ReadInputData(GPIOC)&0xFFFF)!=0xFFFF)  //判断是否有按键被按下
	{
		delay_ms(10);//延时10ms
		if((GPIO_ReadInputData(GPIOC)&0xFFFF)!=0xFFFF)  //判断是否有按键被按下
		{
		    if(!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7))//开关键键
			{
				KeyPressflag=1;//有按键按下
				KeyCuration=0;//计数器清零
				while((!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7))&&(!(KeyCuration>KeyPressTime))); //等待按键释放或者按下按键超过3秒
				if(KeyCuration>KeyPressTime)//按下按键的时间超过3秒(20ms*150)就属于长按按键处理，下同
				{
					KeyCuration=0;
					return(PowerLongKey);
				}
				else
				{
					KeyCuration=0;
					return(PowerKey);	
				}
			}
		
		}
	}
	return 0xff;
}
