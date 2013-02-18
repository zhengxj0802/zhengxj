#include "stm32f10x.h"
#include "globleexten.h"
#include "USART.h"

/*��ʱ��2��ʼ��20mS*/
/*��ʱʱ��ļ���
(1+TIM_Prescale)*(1+TIM_Preriod)/72M
*/
void TIM2_Configuration(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
  TIM_DeInit(TIM2);
  TIM_TimeBaseStructure.TIM_Period=28-1;		 				/* �Զ���װ�ؼĴ������ڵ�ֵ(����ֵ) */
																/* �ۼ� TIM_Period��Ƶ�ʺ����һ�����»����ж� */
  TIM_TimeBaseStructure.TIM_Prescaler= (36000 - 1);				/* ʱ��Ԥ��Ƶ��   ���磺ʱ��Ƶ��=72MHZ/(ʱ��Ԥ��Ƶ+1) *///36000
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 		/* ������Ƶ */
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 	/* ���ϼ���ģʽ */
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  TIM_ClearFlag(TIM2, TIM_FLAG_Update);							/* �������жϱ�־ */
  TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
  TIM_Cmd(TIM2, ENABLE);										/* ����ʱ�� */
}
/*��ʱ��2�жϷ������*/
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2 , TIM_IT_Update) != RESET ) 
	{
		TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);
	
		if(reboot!=1)
			IWDG_ReloadCounter();	   //ι��

		if(KeyPressflag==1||WaitKeyPressflag==1)//�Ƿ��а������£��еĻ�����ȫ�ֱ���KeyCuration�Լ�1
		{
	   		KeyCuration++;
		} 
		if(g_FPTimeOut>0)	g_FPTimeOut--; //ָ�Ƴ�ʱ����

    }	
}
