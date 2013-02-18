#ifndef _RTC_TIME_H_
#define _RTC_TIME_H_

#include "stm32f10x.h"
#include <time.h>

void RTC_Init(void);
void Time_Display(void);
void Time_Regulate(void);

#endif
