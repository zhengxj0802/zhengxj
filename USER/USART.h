#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"
#include "comdef.h"

void USART1_Configuration(void);
void USART2_Configuration(void);
u8 USARTRecvByte(USART_TypeDef* USARTx,u8 *Data,u16 Overtime);
void USARTSendByte(USART_TypeDef* USARTx,u8 data);
void USART1_SendNByte(u8 *data,u16 len);
void SendResponse(unsigned char cmd[3],unsigned int ccRetCode,unsigned int length,unsigned char *data);
void DealDataOfUSART1(void);
void USART2SendPackage(PackageType type, u16 PackageLen, u8 *PackageData);
int USART2RecvPackage(u8 *PackageID);

#endif 
