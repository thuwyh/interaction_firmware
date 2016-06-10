#include "motor.h"
#include "schedule.h"

void motor_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_AHBPeriphClockCmd(	RCC_AHBPeriph_GPIOA, ENABLE );//PORTA时钟使能 
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   //输入
    GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;      //上拉
    GPIO_InitStructure.GPIO_OType=GPIO_OType_OD;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10;
    
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    
    //Iout=0 if GPIO is set
    GPIO_ResetBits(GPIOA, GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10);
    
    GPIO_SetBits(GPIOA, GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10);

}

void motorshake(u8 motors,int shaketime, int sleeptime, int times)
{
    uint16_t GPIO_Pins = 0;
    int timenow,t;
    if (motors & 0x01)
        GPIO_Pins |= GPIO_Pin_8;
    if (motors & 0x02)
        GPIO_Pins |= GPIO_Pin_9;
    if (motors & 0x04)
        GPIO_Pins |= GPIO_Pin_10;
    
    for (t=0;t<times;t++)
    {
        GPIO_ResetBits(GPIOA,GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10);
        
        timenow=getSystime();
        while (getSystime()<timenow+shaketime);
        
        GPIO_SetBits(GPIOA,GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10);

        timenow=getSystime();
        while (getSystime()<timenow+shaketime);
    }
}
