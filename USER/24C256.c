#include "stm32f10x.h"
#include "24C256.h"
#include "comdef.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*AT24C256管脚配置*/
void AT24C256_Configuration(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure; 
	/* Configure I2C2 pins: PB10->SCL and PB11->SDA */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOG, ENABLE);
	
	//IO口配置
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;	//模拟I2C使用
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;		//硬件I2C使用
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//EEP1_EN PG8 配置
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOG, GPIO_Pin_8); //输出低电平
}
/*I2C2配置*/
void I2C2_Configuration(void)
{
	I2C_InitTypeDef  I2C_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);

	I2C_DeInit(I2C2);
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x0;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = 100000;//100K速度
    
	I2C_Cmd(I2C2, ENABLE);
	I2C_Init(I2C2, &I2C_InitStructure);
	/*允许1字节1应答模式*/
	I2C_AcknowledgeConfig(I2C2, ENABLE);
}
