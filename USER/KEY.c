#include "stm32f10x.h"
#include "systick.h"
#include "comdef.h"
#include "globleexten.h"
#include "Beep.h"
#include "fingerprint.h"
#include "USART.h"

/*key����*/
void KEY_Configuration(void)
{
	//JOY_UP -> PD4 , JOY_DOWN -> PD1 , JOY_LEFT -> PD2 , JOY_RIGHT -> PD3 , JOY_SEL -> PD0
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG , ENABLE); 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_4 | GPIO_Pin_3 | GPIO_Pin_2 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //GPIO_Mode_IPU��������
	GPIO_Init(GPIOG, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE); 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //GPIO_Mode_IPU��������
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}
/*��ֵɨ��*/
u8 ScanKey(void)
{
	KeyCuration=0; 	//�������������µ�ʱ��
	KeyPressflag=0;	//�����Ƿ��б�����
    if((GPIO_ReadInputData(GPIOG)&0xFFFF)!=0xFFFF)  //�ж��Ƿ��а���������
	{
		delay_ms(10);//��ʱ10ms
		if((GPIO_ReadInputData(GPIOG)&0xFFFF)!=0xFFFF)  //�ж��Ƿ��а���������
		{	
			if(!GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_2))	//ȷ����
			{	
				KeyPressflag=1;//�а�������
				KeyCuration=0;//����������
				while(!GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_2)&&(!(KeyCuration>KeyPressTime))); //�ȴ������ͷŻ��߰��°�������3��
				if(KeyCuration>KeyPressTime)//���°�����ʱ�䳬��3��(20ms*150)�����ڳ�������������ͬ
				{
					KeyCuration=0;//�������������µ�ʱ�����㣬�Ա��´δ��㿪ʼ����ͬ
					return(EnterLongKey);
				}
				else
				{
					KeyCuration=0;
					return(EnterKey);	
				}
			}	
			else if(!GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_3)) //�Ϸ���
			{
				KeyPressflag=1;//�а�������
				KeyCuration=0;//����������
				while((!GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_3))&&(!(KeyCuration>KeyPressTime))); //�ȴ������ͷŻ��߰��°�������3��
				if(KeyCuration>KeyPressTime)//���°�����ʱ�䳬��3��(20ms*150)�����ڳ�������������ͬ
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
			else if(!GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_4)) //�·���
			{
				KeyPressflag=1;//�а�������
				KeyCuration=0;//����������
				while((!GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_4))&&(!(KeyCuration>KeyPressTime))); //�ȴ������ͷŻ��߰��°�������3��
				if(KeyCuration>KeyPressTime)//���°�����ʱ�䳬��3��(20ms*150)�����ڳ�������������ͬ
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
			else if(!GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_5))//ȡ����
			{
				KeyPressflag=1;//�а�������
				KeyCuration=0;//����������
				while((!GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_5))&&(!(KeyCuration>KeyPressTime))); //�ȴ������ͷŻ��߰��°�������3��
				if(KeyCuration>KeyPressTime)//���°�����ʱ�䳬��3��(20ms*150)�����ڳ�������������ͬ
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
	if((GPIO_ReadInputData(GPIOC)&0xFFFF)!=0xFFFF)  //�ж��Ƿ��а���������
	{
		delay_ms(10);//��ʱ10ms
		if((GPIO_ReadInputData(GPIOC)&0xFFFF)!=0xFFFF)  //�ж��Ƿ��а���������
		{
		    if(!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7))//���ؼ���
			{
				KeyPressflag=1;//�а�������
				KeyCuration=0;//����������
				while((!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7))&&(!(KeyCuration>KeyPressTime))); //�ȴ������ͷŻ��߰��°�������3��
				if(KeyCuration>KeyPressTime)//���°�����ʱ�䳬��3��(20ms*150)�����ڳ�������������ͬ
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
