#ifndef _12864LCD_H_
#define _12864LCD_H_

#include "stm32f10x.h"

#define CLR_CS()     	GPIO_ResetBits(GPIOG , GPIO_Pin_13)
#define SET_CS()     	GPIO_SetBits(GPIOG , GPIO_Pin_13)
#define CLR_RES()    	GPIO_ResetBits(GPIOG , GPIO_Pin_12)
#define SET_RES()    	GPIO_SetBits(GPIOG , GPIO_Pin_12)
#define CLR_A0()     	GPIO_ResetBits(GPIOG , GPIO_Pin_11)
#define SET_A0()     	GPIO_SetBits(GPIOG , GPIO_Pin_11)
#define CLR_RW()     	GPIO_ResetBits(GPIOG , GPIO_Pin_10)
#define SET_RW()     	GPIO_SetBits(GPIOG , GPIO_Pin_10)
#define CLR_E()      	GPIO_ResetBits(GPIOG , GPIO_Pin_9)
#define SET_E()      	GPIO_SetBits(GPIOG , GPIO_Pin_9)
#define NORMALDISP		0
#define REVERSEDISP    	1


/*lcd管脚配置*/
void LCD_Configuration(void);
/*打开液晶屏背光灯*/
void OpenLcdLight(void);
/*关闭显示屏背光灯*/
void CloseLcdLight(void);
/*初始化LCD控制器*/
void Init_LCD(void);
/*清空LCD*/
void ClearLcd(void); 
/*显示一个西文字符*/
void DispOutENGInRow(unsigned char cRow, unsigned char cCol, unsigned char ENGchar, unsigned char dispMode);
void DispOutString(unsigned char cPage, unsigned char cCol, unsigned char cLen, unsigned char *cpText, unsigned char dispMode);
#endif


