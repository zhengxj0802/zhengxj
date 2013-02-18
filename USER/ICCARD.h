#ifndef _ICCARD_h_
#define _ICCARD_h_

#include "stm32f10x.h"

void ICCARD_Configuration(void);
void I2C1_Configuration(void);
u8 ICCheckCardInsert(void);
void POWERONICCARD(void);
void POWEROFFICCARD(void);

#endif
