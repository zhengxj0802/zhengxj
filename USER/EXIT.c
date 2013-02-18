#include "stm32f10x.h"
#include "USART.h"
#include "globleexten.h"

void EXTI_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO , ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING ; //иою╜
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	 
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource6);  
    /* Configure Button EXTI line */
	EXTI_ClearITPendingBit(EXTI_Line6);	
    EXTI_InitStructure.EXTI_Line = EXTI_Line6;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
}

/*******************************************************************************
* Function Name  : EXTI0_IRQHandler
* Description    : This function handles External lines 0 interrupt request.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void EXTI9_5_IRQHandler(void)					 
{
	if ( EXTI_GetITStatus(EXTI_Line6) != RESET ) 
	{
		EXTI_ClearITPendingBit(EXTI_Line6);
		gwSpeedPluseMile++;
		USARTSendByte(USART1,gwSpeedPluseMile);
	}				
}
