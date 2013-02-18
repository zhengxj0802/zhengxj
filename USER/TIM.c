#include "stm32f10x.h"
#include "globleexten.h"
#include "USART.h"

/*定时器2初始化20mS*/
/*定时时间的计算
(1+TIM_Prescale)*(1+TIM_Preriod)/72M
*/
void TIM2_Configuration(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
  TIM_DeInit(TIM2);
  TIM_TimeBaseStructure.TIM_Period=28-1;		 				/* 自动重装载寄存器周期的值(计数值) */
																/* 累计 TIM_Period个频率后产生一个更新或者中断 */
  TIM_TimeBaseStructure.TIM_Prescaler= (36000 - 1);				/* 时钟预分频数   例如：时钟频率=72MHZ/(时钟预分频+1) *///36000
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 		/* 采样分频 */
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 	/* 向上计数模式 */
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  TIM_ClearFlag(TIM2, TIM_FLAG_Update);							/* 清除溢出中断标志 */
  TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
  TIM_Cmd(TIM2, ENABLE);										/* 开启时钟 */
}
/*定时器2中断服务程序*/
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2 , TIM_IT_Update) != RESET ) 
	{
		TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);
	
		if(reboot!=1)
			IWDG_ReloadCounter();	   //喂狗

		if(KeyPressflag==1||WaitKeyPressflag==1)//是否有按键按下，有的话，将全局变量KeyCuration自加1
		{
	   		KeyCuration++;
		} 
		if(g_FPTimeOut>0)	g_FPTimeOut--; //指纹超时控制

    }	
}
