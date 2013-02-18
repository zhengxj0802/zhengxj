#include "stm32f10x.h"
#include "common.h"
#include "comdef.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

__asm void SystemReset(void)
{
 MOV R0, #1           //; 
 MSR FAULTMASK, R0    //; 清除FAULTMASK 禁止一切中断产生
 LDR R0, =0xE000ED0C  //;
 LDR R1, =0x05FA0004  //; 
 STR R1, [R0]         //; 系统软件复位   
 
deadloop
    B deadloop        //; 死循环使程序运行不到下面的代码
}

/*
*整数转换为字符串
*dwint:要转换的整数
*cpBuf:存放转换的结果字符串
*cLen:要转换的长度
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
*字符串转换为长整数
*cpBuf:存放要转换的源字符串
*cLen:要转换的长度
*cASCII:字符串的数字类型，10进制数字或者是ASCII码值
*返回一个4B的长整数
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
*将LONG型数据转换为HEX串
*cLen:拟转换的字符串的长度
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
/*计算CRC*/
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
