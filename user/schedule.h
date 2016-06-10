#ifndef SCHEDULE_H
#define SCHEDULE_H

#include "stm32f37x.h"

void systick_init(void);
void doschedule(void);
void delayMs(int time);
void delay100us(int time);
void counterde();
int getSystime(void);

#endif
