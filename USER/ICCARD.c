#include "stm32f10x.h"
#include "comdef.h"
#include "common.h"
#include "I2CLIB.h"
#include "globleexten.h"
#include "ICCARD.h"
#include "USART.h"
#include "systick.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*�߼����ܿ����۹ܽ�����*/
void ICCARD_Configuration(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure; 
	/* Configure I2C2 pins: PB10->SCL and PB11->SDA */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |RCC_APB2Periph_GPIOD, ENABLE);

	//IO������
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//GPIO_InitStructure.GPIO_Mode =GPIO_Mode_Out_OD;	//ģ��I2Cʹ��
    GPIO_InitStructure.GPIO_Mode =GPIO_Mode_AF_OD;	//Ӳ��I2Cʹ�� 
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//ʹ�ܹܽ����ܿ�����
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode =GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

	//��⿨�Ƿ��������
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IPU;  
    GPIO_Init(GPIOA, &GPIO_InitStructure);

	//��λ�ܽ�����
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode =GPIO_Mode_Out_PP;  
    GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_SetBits(GPIOD , GPIO_Pin_9);
}
/*I2C1����*/
void I2C1_Configuration(void)
{

	I2C_InitTypeDef  I2C_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
  
	I2C_DeInit(I2C1);
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x0;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = 100000;//100K�ٶ�
    
	I2C_Cmd(I2C1, ENABLE);
	I2C_Init(I2C1, &I2C_InitStructure);
	/*����1�ֽ�1Ӧ��ģʽ*/
	I2C_AcknowledgeConfig(I2C1, ENABLE);
}
/**********************************************************************/
/*�жϿ��Ƿ����*/
//u8 ICCheckCardInsert(void)
//{
//	POWERONICCARD();
//	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8)==0)
//	{ 
//		delay_ms(5);//��ʱ5ms
//		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8)==0)
//		{
//			//POWEROFFICCARD();
//			return C_OK;
//		}
//	}
//	//POWEROFFICCARD();
//	return C_NOIC;
//}
///**********************************************************************/
///*IC�����ϵ�*/
//void POWERONICCARD(void)
//{
//	GPIO_SetBits(GPIOC , GPIO_Pin_9);
//	delay_ms(5);//��ʱ5ms
//}
///**********************************************************************/
///*IC�����µ�*/
//void POWEROFFICCARD(void)
//{
//	GPIO_ResetBits(GPIOC, GPIO_Pin_9); 
//	delay_ms(5);//��ʱ5ms
//}
/*******************************************************************************************/
/*******************************************************************************************/
/*******************************************************************************************
IC��ҵ���߼�����
********************************************************************************************/
/*******************************************************************************************/
/*******************************************************************************************/
