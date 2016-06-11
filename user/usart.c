#include "usart.h"
#include "ads1298.h"
#include "schedule.h"
#include "motor.h"

State state=HEAD;

void USART2_Init(int baudrate)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
     
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);	//使能USART2，GPIOA时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; //PA.15 KEY
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	//复用
    GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;//推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.15
    GPIO_ResetBits(GPIOA,GPIO_Pin_15);
    
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_7);
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_7);
    
    //USART2_TX   GPIOA.2
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.2

    //USART2_RX	  GPIOA.3初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.3 


    //USART 初始化设置

    USART_InitStructure.USART_BaudRate = baudrate;//串口波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(USART2, &USART_InitStructure); //初始化串口2
    
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
        

    USART_Cmd(USART2, ENABLE);                    //使能串口2 
    
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启串口接受中断
    //USART_ITConfig(USART2, USART_IT_TXE, ENABLE);//开启串口接受中断


}



void parse(u8 c)
{
    static u8 index=0,length=0,command,addr,value,sum;
    u8 deviceID;
    int retval;
    if (state == HEAD && c==0xff)
    {
        state=LENGTH;
        return ;
    }
	if (state == LENGTH)
	{
        state=COMMAND;
        length = c;
        index=0;
        sum=0;
        return;
    }
    if (state == COMMAND)
    {
        command=c;
        sum+=c;
        index++;
        if (index<length)
            state=ADDR;
        else
            state=CHECK;
        return;
    }
    if (state==ADDR)
    {
        addr=c;
        sum+=c;
        index++;
        if (index<length)
            state=VALUE;
        else
            state=CHECK;
        return ;
    }
    if (state==VALUE)
    {
        value=c;
        sum+=c;
        index++;
        state=CHECK;
        return;
    }
    if (state==CHECK)
    {
        if (sum==c)
        {  
            if (command==0x00)
            {
                addr=shakeHands();
                if (addr==0x92)
                    printf("ADS1298 started successfully\n"); 
                else
                    printf("ADS1298 started failed, %x\n",addr); 
            }
            if (command==0x01)
            {
                GPIO_SetBits(GPIOA,GPIO_Pin_15);
            }
            if (command == 0x02)
            {
                motorshake(addr,80,20,value);
            }
            if (command ==0x03)
            {
                writeRegister(0x0d,addr);
                delayMs(20);
                writeRegister(0x0e,value);
                
            }
            if (command ==0x04)
            {
                retval=configForNoiseTest();
                USART2_SendByte(0xff);
                USART2_SendByte(0xff);
                USART2_SendByte(0x03);
                if (retval==0)
                {
                    USART2_SendByte(0xff);
                    USART2_SendByte(0xff);
                }else
                {
                    USART2_SendByte(0xee);
                    USART2_SendByte(0xee);
                }
            }
            if (command ==0x05)
            {
                retval=configForSquarewaveTest();
                USART2_SendByte(0xff);
                USART2_SendByte(0xff);
                USART2_SendByte(0x03);
                if (retval==0)
                {
                    USART2_SendByte(0xff);
                    USART2_SendByte(0xff);
                }else
                {
                    USART2_SendByte(0xee);
                    USART2_SendByte(0xee);
                }
            }
            if ( command == 0x06)
            {
                retval=configForNormalMeasurement();
                USART2_SendByte(0xff);
                USART2_SendByte(0xff);
                USART2_SendByte(0x03);
                if (retval==0)
                {
                    USART2_SendByte(0xff);
                    USART2_SendByte(0xff);
                }else
                {
                    USART2_SendByte(0xee);
                    USART2_SendByte(0xee);
                }
            }
            if (command==0x10)
                beginReadDataC();   //begin continuously conversion
            if (command==0x11)
                stopReadDataC();    //stop ..
            if (command==0x20)
            {
                // read register and return
                deviceID=readRegister(addr,0);
                USART2_SendByte(0xff);
                USART2_SendByte(0xff);
                USART2_SendByte(0x03);
                USART2_SendByte(deviceID);
                USART2_SendByte(deviceID);
            }
                
            if (command==0x40)
            {
                // write register and return the new value
                writeRegister(addr,value);
                delayMs(10);
                USART2_SendByte(0xff);
                USART2_SendByte(0xff);
                USART2_SendByte(0x03);
                deviceID=readRegister(addr,0);
                USART2_SendByte(deviceID);
                USART2_SendByte(deviceID);
            }
            if (command==0x80)
            {
                // restart
                resetADS1298();
                USART2_SendByte(0xff);
                USART2_SendByte(0xff);
                USART2_SendByte(0x03);
                USART2_SendByte(0xff);
                USART2_SendByte(0xff);                
            }
        }
        state=HEAD;
        return;
    }
}

void USART2_SendByte(u8 c)
{
    USART_SendData(USART2,c);
    while( USART_GetFlagStatus(USART2,USART_FLAG_TC)!= SET);
}

int fputc(int ch, FILE *f)
{
    USART2_SendByte(ch);
}