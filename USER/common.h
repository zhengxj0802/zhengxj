#ifndef _common_h_
#define _common_h_

#include "stm32f10x.h"

void  IntToString(int dwint,unsigned char *cpBuf,u8 cLen);//整数转换为字符串
u32 ByteToLong(u8 *cpBuf,u8 cLen,u8 cASCII); //字符串转换为长整数
void  LongToHEX(u32 dwLong,u8*cpBuf,u8 cLen);//将LONG型数据转换为HEX串
u16 calcrc(u8 *ptr, int count);//计算CRC
void SystemReset(void);//复位
#endif
