/*******************************************************************************
* ���ļ�ʵ�ֻ���RTC�����ڹ��ܣ��ṩ�����յĶ�д��������ANSI-C��time.h��
* RTC�б����ʱ���ʽ����UNIXʱ�����ʽ�ġ���һ��32bit��time_t������ʵΪu32��
* ANSI-C�ı�׼���У��ṩ�����ֱ�ʾʱ�������  �ͣ�
* time_t:   	UNIXʱ�������1970-1-1��ĳʱ�侭����������
* 	typedef unsigned int time_t;
* struct tm:	Calendar��ʽ����������ʽ��
*   tm�ṹ���£�
*   struct tm {
*   	int tm_sec;   // �� seconds after the minute, 0 to 60
*   					 (0 - 60 allows for the occasional leap second)
*   	int tm_min;   // �� minutes after the hour, 0 to 59
*		int tm_hour;  // ʱ hours since midnight, 0 to 23
*		int tm_mday;  // �� day of the month, 1 to 31
*		int tm_mon;   // �� months since January, 0 to 11
*		int tm_year;  // �� years since 1900
*		int tm_wday;  // ���� days since Sunday, 0 to 6
*		int tm_yday;  // ��Ԫ��������� days since January 1, 0 to 365
* 		int tm_isdst; // ����ʱ����Daylight Savings Time flag
* 		...
* 	}
* 	����wday��yday�����Զ����������ֱ�Ӷ�ȡ
* 	mon��ȡֵΪ0-11
*	***ע��***��
*	tm_year:��time.h���ж���Ϊ1900�������ݣ���2008��Ӧ��ʾΪ2008-1900=108
* 	���ֱ�ʾ�������û���˵����ʮ���Ѻã�����ʵ�нϴ���졣
* 	�����ڱ��ļ��У����������ֲ��졣
* 	���ⲿ���ñ��ļ��ĺ���ʱ��tm�ṹ�����͵����ڣ�tm_year��Ϊ2008
* 	ע�⣺��Ҫ����ϵͳ��time.c�еĺ�������Ҫ���н�tm_year-=1900
* 
* ��Ա����˵����
* struct tm Time_ConvUnixToCalendar(time_t t);
* 	����һ��Unixʱ�����time_t��������Calendar��ʽ����
* time_t Time_ConvCalendarToUnix(struct tm t);
* 	����һ��Calendar��ʽ���ڣ�����Unixʱ�����time_t��
* time_t Time_GetUnixTime(void);
* 	��RTCȡ��ǰʱ���Unixʱ���ֵ
* struct tm Time_GetCalendarTime(void);
* 	��RTCȡ��ǰʱ�������ʱ��
* void Time_SetUnixTime(time_t);
* 	����UNIXʱ�����ʽʱ�䣬����Ϊ��ǰRTCʱ��
* void Time_SetCalendarTime(struct tm t);
* 	����Calendar��ʽʱ�䣬����Ϊ��ǰRTCʱ��
* 
* �ⲿ����ʵ����
* ����һ��Calendar��ʽ�����ڱ�����
* struct tm now;
* now.tm_year = 2008;
* now.tm_mon = 11;		//12��
* now.tm_mday = 20;
* now.tm_hour = 20;
* now.tm_min = 12;
* now.tm_sec = 30;
* 
* ��ȡ��ǰ����ʱ�䣺
* tm_now = Time_GetCalendarTime();
* Ȼ�����ֱ�Ӷ�tm_now.tm_wday��ȡ������
* 
* ����ʱ�䣺
* Step1. tm_now.xxx = xxxxxxxxx;
* Step2. Time_SetCalendarTime(tm_now);
* 
* ��������ʱ��Ĳ�
* struct tm t1,t2;
* t1_t = Time_ConvCalendarToUnix(t1);
* t2_t = Time_ConvCalendarToUnix(t2);
* dt = t1_t - t2_t;
* dt��������ʱ��������
* dt_tm = mktime(dt);	//ע��dt�����ƥ�䣬ansi���к���Ϊ�����ݣ�ע�ⳬ��
* ����Բο�������ϣ�����ansi-c��ĸ�ʽ������ȹ��ܣ�ctime��strftime��
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


/*��UNIXʱ���ת��Ϊ����ʱ��*/
struct tm Time_ConvUnixToCalendar(time_t t)
{
	struct tm *t_tm;
	t_tm = localtime(&t);
	t_tm->tm_year += 1900;	/* localtimeת�������tm_year�����ֵ����Ҫת�ɾ���ֵ */
	return *t_tm;
}

/*������ʱ��ת����UNIXʱ���*/
time_t Time_ConvCalendarToUnix(struct tm t)
{
	t.tm_year -= 1900;  /* �ⲿtm�ṹ��洢�����Ϊ2008��ʽ	*/
						/* ��time.h�ж������ݸ�ʽΪ1900�꿪ʼ����� */
						/* ���ԣ�������ת��ʱҪ���ǵ�������ء�*/
	return mktime(&t);
}

/*��RTCȡ��ǰʱ���Unixʱ���ֵ*/
time_t Time_GetUnixTime(void)
{
	return (time_t)RTC_GetCounter();
}

/*��RTCȡ��ǰʱ�������ʱ��*/
struct tm Time_GetCalendarTime(void)
{
	time_t t_t;
	struct tm t_tm;

	t_t = (time_t)RTC_GetCounter();
	t_tm = Time_ConvUnixToCalendar(t_t);
	return t_tm;
}

/*��������Unixʱ���д��RTC*/
void Time_SetUnixTime(time_t t)
{
	RTC_WaitForLastTask();
	RTC_SetCounter((u32)t);
	RTC_WaitForLastTask();
	return;
}

/*��������Calendar��ʽʱ��ת����UNIXʱ���д��RTC*/
void Time_SetCalendarTime(struct tm t)
{
	Time_SetUnixTime(Time_ConvCalendarToUnix(t));
	return;
}

/*RTC����*/
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


/*����ʱ��*/
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

	again:SendResponse(cmd,0,0,NULL);//����λ������ָ�Ҫ�����ʱ��
	while(!UpdateTimeFlag);
	UpdateTimeFlag=0;
	if(time.tm_year>2050||time.tm_year<2010
		||time.tm_mon>11	||time.tm_mon<0
		||time.tm_mday>31 	||time.tm_mday<1
		||time.tm_hour>23	||time.tm_min>59	||time.tm_sec>59)
	{
		SendResponse(cmd,0xff,0,NULL);//ʱ�����Ҫ����λ���ٴη��͸���ʱ��ָ��
		goto again;	
	}
	else
	{
		SendResponse(cmd,0x00,0,NULL);//ʱ�������ȷ����λ���յ���ָ����ٷ��͸���ʱ��ָ��
		Time_SetCalendarTime(time); 
	}
}

/* RTC��ʼ��*/
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
		time = Time_ConvUnixToCalendar(Time_GetUnixTime());//��ȡʱ��
		if(time.tm_year>2050||time.tm_year<2010
			||time.tm_mon>12	||time.tm_mon<1
			||time.tm_mday>31 	||time.tm_mday<1
			||time.tm_hour>23	||time.tm_min>59	||time.tm_sec>59) //�����ж�����ʱ���Ƿ����
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
   /*LCD��ʾ��ǰʱ��*/
   DispOutString(6,0,16,lcd_display_time,NORMALDISP);
}
/*RTC�жϷ������*/
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
