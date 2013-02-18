#include "stm32f10x.h"
#include "systick.h"

/*����������*/
void Beep_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/*�������촦��count��ʾ��Ĵ�����time��ʾÿ����ļ�϶*/
void Beep(u8 count,u16 time)
{
	u8 i=0;
	for(i=0;i<count;i++)
	{
		GPIO_SetBits(GPIOC, GPIO_Pin_8);
		delay_ms(time);
		GPIO_ResetBits(GPIOC, GPIO_Pin_8);
		delay_ms(time);
	}
}
