/*******************************************************************************
* 本文件实现基于RTC的日期功能，提供年月日的读写。（基于ANSI-C的time.h）
* RTC中保存的时间格式，是UNIX时间戳格式的。即一个32bit的time_t变量（实为u32）
* ANSI-C的标准库中，提供了两种表示时间的数据  型：
* time_t:   	UNIX时间戳（从1970-1-1起到某时间经过的秒数）
* 	typedef unsigned int time_t;
* struct tm:	Calendar格式（年月日形式）
*   tm结构如下：
*   struct tm {
*   	int tm_sec;   // 秒 seconds after the minute, 0 to 60
*   					 (0 - 60 allows for the occasional leap second)
*   	int tm_min;   // 分 minutes after the hour, 0 to 59
*		int tm_hour;  // 时 hours since midnight, 0 to 23
*		int tm_mday;  // 日 day of the month, 1 to 31
*		int tm_mon;   // 月 months since January, 0 to 11
*		int tm_year;  // 年 years since 1900
*		int tm_wday;  // 星期 days since Sunday, 0 to 6
*		int tm_yday;  // 从元旦起的天数 days since January 1, 0 to 365
* 		int tm_isdst; // 夏令时？？Daylight Savings Time flag
* 		...
* 	}
* 	其中wday，yday可以自动产生，软件直接读取
* 	mon的取值为0-11
*	***注意***：
*	tm_year:在time.h库中定义为1900年起的年份，即2008年应表示为2008-1900=108
* 	这种表示方法对用户来说不是十分友好，与现实有较大差异。
* 	所以在本文件中，屏蔽了这种差异。
* 	即外部调用本文件的函数时，tm结构体类型的日期，tm_year即为2008
* 	注意：若要调用系统库time.c中的函数，需要自行将tm_year-=1900
* 
* 成员函数说明：
* struct tm Time_ConvUnixToCalendar(time_t t);
* 	输入一个Unix时间戳（time_t），返回Calendar格式日期
* time_t Time_ConvCalendarToUnix(struct tm t);
* 	输入一个Calendar格式日期，返回Unix时间戳（time_t）
* time_t Time_GetUnixTime(void);
* 	从RTC取当前时间的Unix时间戳值
* struct tm Time_GetCalendarTime(void);
* 	从RTC取当前时间的日历时间
* void Time_SetUnixTime(time_t);
* 	输入UNIX时间戳格式时间，设置为当前RTC时间
* void Time_SetCalendarTime(struct tm t);
* 	输入Calendar格式时间，设置为当前RTC时间
* 
* 外部调用实例：
* 定义一个Calendar格式的日期变量：
* struct tm now;
* now.tm_year = 2008;
* now.tm_mon = 11;		//12月
* now.tm_mday = 20;
* now.tm_hour = 20;
* now.tm_min = 12;
* now.tm_sec = 30;
* 
* 获取当前日期时间：
* tm_now = Time_GetCalendarTime();
* 然后可以直接读tm_now.tm_wday获取星期数
* 
* 设置时间：
* Step1. tm_now.xxx = xxxxxxxxx;
* Step2. Time_SetCalendarTime(tm_now);
* 
* 计算两个时间的差
* struct tm t1,t2;
* t1_t = Time_ConvCalendarToUnix(t1);
* t2_t = Time_ConvCalendarToUnix(t2);
* dt = t1_t - t2_t;
* dt就是两个时间差的秒数
* dt_tm = mktime(dt);	//注意dt的年份匹配，ansi库中函数为相对年份，注意超限
* 另可以参考相关资料，调用ansi-c库的格式化输出等功能，ctime，strftime等
* 
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "RTC_Time.h" 
#include "globleexten.h"
#include "USART.h"
#include "comdef.h"
#include "time.h"
#include "common.h"
#include "12864lcd.h"
#include <stdio.h>


/*将UNIX时间戳转换为日历时间*/
struct tm Time_ConvUnixToCalendar(time_t t)
{
	struct tm *t_tm;
	t_tm = localtime(&t);
	t_tm->tm_year += 1900;	/* localtime转换结果的tm_year是相对值，需要转成绝对值 */
	return *t_tm;
}

/*将日历时间转换成UNIX时间戳*/
time_t Time_ConvCalendarToUnix(struct tm t)
{
	t.tm_year -= 1900;  /* 外部tm结构体存储的年份为2008格式	*/
						/* 而time.h中定义的年份格式为1900年开始的年份 */
						/* 所以，在日期转换时要考虑到这个因素。*/
	return mktime(&t);
}

/*从RTC取当前时间的Unix时间戳值*/
time_t Time_GetUnixTime(void)
{
	return (time_t)RTC_GetCounter();
}

/*从RTC取当前时间的日历时间*/
struct tm Time_GetCalendarTime(void)
{
	time_t t_t;
	struct tm t_tm;

	t_t = (time_t)RTC_GetCounter();
	t_tm = Time_ConvUnixToCalendar(t_t);
	return t_tm;
}

/*将给定的Unix时间戳写入RTC*/
void Time_SetUnixTime(time_t t)
{
	RTC_WaitForLastTask();
	RTC_SetCounter((u32)t);
	RTC_WaitForLastTask();
	return;
}

/*将给定的Calendar格式时间转换成UNIX时间戳写入RTC*/
void Time_SetCalendarTime(struct tm t)
{
	Time_SetUnixTime(Time_ConvCalendarToUnix(t));
	return;
}

/*RTC配置*/
static void RTC_Configuration(void)
{
  /* Enable PWR and BKP clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Reset Backup Domain */
  BKP_DeInit();

  /* Enable LSE */
  RCC_LSEConfig(RCC_LSE_ON);
  /* Wait till LSE is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {}

  /* Select LSE as RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

  /* Enable RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC registers synchronization */
  RTC_WaitForSynchro();

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Enable the RTC Second */
  RTC_ITConfig(RTC_IT_SEC, ENABLE);

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Set RTC prescaler: set RTC period to 1sec */
  RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
}


/*设置时间*/
void Time_Regulate(void)
{
	u8 cmd[3]={0xea,0x01,0x00};
	//test
	time.tm_year=2012;
	time.tm_mon=11;
	time.tm_mday=29;
	time.tm_hour=20;
	time.tm_min=50;
	time.tm_sec=0;
	Time_SetCalendarTime(time); 
	return;

	again:SendResponse(cmd,0,0,NULL);//向上位机发送指令，要求更新时间
	while(!UpdateTimeFlag);
	UpdateTimeFlag=0;
	if(time.tm_year>2050||time.tm_year<2010
		||time.tm_mon>11	||time.tm_mon<0
		||time.tm_mday>31 	||time.tm_mday<1
		||time.tm_hour>23	||time.tm_min>59	||time.tm_sec>59)
	{
		SendResponse(cmd,0xff,0,NULL);//时间错误，要求上位机再次发送更新时间指令
		goto again;	
	}
	else
	{
		SendResponse(cmd,0x00,0,NULL);//时间大致正确，上位机收到此指令将不再发送更新时间指令
		Time_SetCalendarTime(time); 
	}
}

/* RTC初始化*/
void RTC_Init(void)
{
	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
	{
    	/* Backup data register value is not correct or not yet programmed (when
       the first time the program is executed) */
    	/* RTC Configuration */
		RTC_Configuration();
		Time_Regulate();
		/* Adjust time by values entred by the user on the hyperterminal */
    	BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
	}
  	else
  	{
	    /* Wait for RTC registers synchronization */
	    RTC_WaitForSynchro();
	    /* Enable the RTC Second */
	    RTC_ITConfig(RTC_IT_SEC, ENABLE);
	    /* Wait until last write operation on RTC registers has finished */
	    RTC_WaitForLastTask();
		time = Time_ConvUnixToCalendar(Time_GetUnixTime());//读取时间
		if(time.tm_year>2050||time.tm_year<2010
			||time.tm_mon>12	||time.tm_mon<1
			||time.tm_mday>31 	||time.tm_mday<1
			||time.tm_hour>23	||time.tm_min>59	||time.tm_sec>59) //大致判断日期时间是否错误
		{
			Time_Regulate();
		}
  	}
#ifdef RTCClockOutput_Enable
  /* Enable PWR and BKP clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Disable the Tamper Pin */
  BKP_TamperPinCmd(DISABLE); /* To output RTCCLK/64 on Tamper pin, the tamper
                                 functionality must be disabled */

  /* Enable RTC Clock Output on Tamper Pin */
  BKP_RTCOutputConfig(BKP_RTCOutputSource_CalibClock);
#endif
   /* Clear reset flags */
  RCC_ClearFlag();
  return;
}

/*******************************************************************************
* Function Name  : Time_Display
* Description    : Printf Time
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void Time_Display(void)
{
   u8 lcd_display_time[20];

   time = Time_ConvUnixToCalendar(Time_GetUnixTime());
   IntToString(time.tm_year,lcd_display_time,4);
   lcd_display_time[4]='-';
   IntToString(time.tm_mon+1,lcd_display_time+5,2);
   lcd_display_time[7]='-';
   IntToString(time.tm_mday,lcd_display_time+8,2);
   lcd_display_time[10]=' ';

   IntToString(time.tm_hour,lcd_display_time+11,2);
   lcd_display_time[13]=':';
   IntToString(time.tm_min,lcd_display_time+14,2);
   lcd_display_time[16]=':';
   IntToString(time.tm_sec,lcd_display_time+17,2);
   lcd_display_time[19]='\n';

   //USART1_SendNByte(lcd_display_time,20);//test
   /*LCD显示当前时间*/
   DispOutString(6,0,16,lcd_display_time,NORMALDISP);
}
/*RTC中断服务程序*/
void RTC_IRQHandler(void)
{
	if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
	{
    	/*Clear the RTC Second interrupt */
    	RTC_ClearITPendingBit(RTC_IT_SEC);
		if(RefreshTime)
    		Time_Display();
	}
}
