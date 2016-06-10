#include "schedule.h"

int systime=0;
int delaycount;

void counterde()
{
    if (delaycount>0)
        delaycount--;
}

void delayMs(int time)
{
    delaycount=time;
    while (delaycount);

}

void delay100us(int time)
{
    delaycount=0;
    while(delaycount<time);
}

void systick_init(void)
{
    if (SysTick_Config(SystemCoreClock / 1000))
        while(1);
    //SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);      

}

void doschedule(void)
{
    //Sensor_GetOriQuaternion(BODYSENSOR);   
}

int getSystime(void)
{
    return systime;
}
