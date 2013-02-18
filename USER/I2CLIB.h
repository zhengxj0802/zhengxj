#ifndef _I2CLIB_h_
#define _I2CLIB_h_

#include "stm32f10x.h"

/***************************************************************************************************/
/*EEPROM*/
//模拟
#define ADDR_First24C256		0x02
#define ADDR_Second24C256		0x04

//硬件
#define ADDR_First24C256_Read		0xa3
#define ADDR_First24C256_Write		0xa2

#define ADDR_Second24C256_Read		0xa5
#define ADDR_Second24C256_Write		0xa4

/*IC*/
//模拟
#define ICCARD		0x00

//硬件
#define ICCARD_READ		0xA1
#define ICCARD_WRITE	0xA0
/***************************************************************************************************/
#define SCL1_H         GPIOB->BSRR = GPIO_Pin_6	 /* GPIO_SetBits(GPIOB , GPIO_Pin_6)   */
#define SCL1_L         GPIOB->BRR  = GPIO_Pin_6   /* GPIO_ResetBits(GPIOB , GPIO_Pin_6) */
   
#define SDA1_H         GPIOB->BSRR = GPIO_Pin_7	 /* GPIO_SetBits(GPIOB , GPIO_Pin_7)   */
#define SDA1_L         GPIOB->BRR  = GPIO_Pin_7	 /* GPIO_ResetBits(GPIOB , GPIO_Pin_7) */

#define SCL1_read      GPIOB->IDR  & GPIO_Pin_6   /* GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_6) */
#define SDA1_read      GPIOB->IDR  & GPIO_Pin_7	 /* GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_7) */ 

#define SCL2_H         GPIOB->BSRR = GPIO_Pin_10	 /* GPIO_SetBits(GPIOB , GPIO_Pin_10)   */
#define SCL2_L         GPIOB->BRR  = GPIO_Pin_10   /* GPIO_ResetBits(GPIOB , GPIO_Pin_11) */
   
#define SDA2_H         GPIOB->BSRR = GPIO_Pin_11	 /* GPIO_SetBits(GPIOB , GPIO_Pin_11)   */
#define SDA2_L         GPIOB->BRR  = GPIO_Pin_11	 /* GPIO_ResetBits(GPIOB , GPIO_Pin_11) */

#define SCL2_read      GPIOB->IDR  & GPIO_Pin_10   /* GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_10) */
#define SDA2_read      GPIOB->IDR  & GPIO_Pin_11	 /* GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_11) */ 

/***************************************************************************************************/
u8 I2C1_ReadNByte(uint8_t* pBuffer,   uint16_t length,   uint16_t ReadAddress,  uint8_t DeviceAddress);
u8 I2C1_WritePage(uint8_t* pBuffer, uint16_t length,uint16_t WriteAddress, uint8_t DeviceAddress);
u8 I2C1_WriteNByte(uint8_t* pBuffer, uint16_t length,uint16_t WriteAddress, uint8_t DeviceAddress);
u8 I2C1_WriteByte(u8 senddata,uint16_t WriteAddress, uint8_t DeviceAddress);

u8 I2C2_ReadNByte(uint8_t* pBuffer,   uint16_t length,   uint16_t ReadAddress,  uint8_t DeviceAddress);
u8 I2C2_WritePage(uint8_t* pBuffer, uint16_t length,uint16_t WriteAddress, uint8_t DeviceAddress);
u8 I2C2_WriteNByte(uint8_t* pBuffer, uint16_t length,uint16_t WriteAddress, uint8_t DeviceAddress);
u8 I2C2_WriteByte(u8 senddata,uint16_t WriteAddress, uint8_t DeviceAddress);

uint8_t I2C_Read(I2C_TypeDef *I2Cx,uint8_t I2C_Addr,uint8_t addr,uint8_t *buf,uint16_t num);
uint8_t I2C_Write(I2C_TypeDef *I2Cx,uint8_t I2C_Addr,uint8_t addr,uint8_t *buf,uint16_t num);

#endif
