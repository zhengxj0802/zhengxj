#ifndef _common_h_
#define _common_h_

#include "stm32f10x.h"

void  IntToString(int dwint,unsigned char *cpBuf,u8 cLen);//����ת��Ϊ�ַ���
u32 ByteToLong(u8 *cpBuf,u8 cLen,u8 cASCII); //�ַ���ת��Ϊ������
void  LongToHEX(u32 dwLong,u8*cpBuf,u8 cLen);//��LONG������ת��ΪHEX��
u16 calcrc(u8 *ptr, int count);//����CRC
void SystemReset(void);//��λ
#endif
