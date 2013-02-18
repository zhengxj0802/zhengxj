#include "stm32f10x.h"
#include "common.h"
#include "comdef.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

__asm void SystemReset(void)
{
 MOV R0, #1           //; 
 MSR FAULTMASK, R0    //; ���FAULTMASK ��ֹһ���жϲ���
 LDR R0, =0xE000ED0C  //;
 LDR R1, =0x05FA0004  //; 
 STR R1, [R0]         //; ϵͳ�����λ   
 
deadloop
    B deadloop        //; ��ѭ��ʹ�������в�������Ĵ���
}

/*
*����ת��Ϊ�ַ���
*dwint:Ҫת��������
*cpBuf:���ת���Ľ���ַ���
*cLen:Ҫת���ĳ���
*/
void  IntToString(int dwint,unsigned char *cpBuf,u8 cLen)
{
   	u8  i=0;
   	while(i<cLen) 
   	{
	      *(cpBuf+cLen-(i+1))= dwint%10+0x30;
	      dwint = dwint / 10;
	      i++;
   	}
}
/*
*�ַ���ת��Ϊ������
*cpBuf:���Ҫת����Դ�ַ���
*cLen:Ҫת���ĳ���
*cASCII:�ַ������������ͣ�10�������ֻ�����ASCII��ֵ
*����һ��4B�ĳ�����
*/
u32 ByteToLong(u8 *cpBuf,u8 cLen,u8 cASCII)
{
   	u32  dwLong=0;
   	u8  i;

   	if (cASCII==ISASCII) 
   	{
		for (i=0;i<cLen;i++) 
		{
			dwLong *= 10;
			dwLong += (*(cpBuf+i)-0x30);
		}
   	}
   	else 
   	{
		for (i=0;i<cLen;i++) 
		{
			dwLong = dwLong <<8;
			dwLong += *(cpBuf+i);
		}
   	}
   	return(dwLong);
}

/*
*��LONG������ת��ΪHEX��
*cLen:��ת�����ַ����ĳ���
*/
void  LongToHEX(u32 dwLong,u8*cpBuf,u8 cLen)
{
	u8 i=0;
   	while(i<cLen) 
	{
		*(cpBuf+cLen-(i+1))= dwLong%256;
    	dwLong = dwLong / 256;
    	i++;
   	}
}
/*����CRC*/
u16 calcrc(u8 *ptr, int count)
{
	u16 crc = 0;
	char i;
	while (--count >= 0)
	{
		crc = crc ^ (int) *ptr++ << 8;
		i = 8;
		do
		{
			if (crc & 0x8000)
			{
				crc = crc << 1 ^ 0x1021;
			}
			else
			{
				crc = crc << 1;
			}
		} 
		while(--i);
	}
	return (crc);
}
