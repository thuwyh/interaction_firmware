#include "ads1298.h"
#include "schedule.h"
#include "stm32f37x_exti.h"
#include "yei3space.h"

char emgBuffer[255];
u8 emgPointer;
int counter=0;

void ads1298_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;

    RCC_AHBPeriphClockCmd(	RCC_AHBPeriph_GPIOA|RCC_AHBPeriph_GPIOB|RCC_AHBPeriph_GPIOE, ENABLE );//PORTB时钟使能 
    
    RCC_APB2PeriphClockCmd(	RCC_APB2Periph_SPI1,  ENABLE );//SPI1时钟使能 
    
    //DRDY
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//输入
    GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;//上拉
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_Init(GPIOA,&GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_5);
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_5);
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource0,GPIO_AF_5);
    
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	//复用SCK,MISO   
    GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
    
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;  
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA,SCK
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA,MISO
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB,MOSI

    SPI_I2S_DeInit(SPI1);
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//设置SPI工作模式:设置为主SPI
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//设置SPI的数据大小:SPI发送接收8位帧结构
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		//串行同步时钟的空闲状态为高电平
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//串行同步时钟的第二个跳变沿（上升或下降）数据被采样
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		//定义波特率预分频的值:波特率预分频值为256
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
    SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式
    SPI_Init(SPI1, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
    
    SPI_NSSInternalSoftwareConfig(SPI1,SPI_NSSInternalSoft_Set);
    SPI_RxFIFOThresholdConfig(SPI1, SPI_RxFIFOThreshold_QF);
    SPI_Cmd(SPI1, ENABLE); //使能SPI外设
    //SPI_I2S_ClearFlag(SPI1, SPI_I2S_IT_RXNE);
    
    //RESET & pwdn
    GPIO_InitStructure.GPIO_Mode= GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;
    GPIO_Init(GPIOE,&GPIO_InitStructure);
    
    //片选信号 PB1 & START PB2
    GPIO_InitStructure.GPIO_Mode= GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2;
    GPIO_Init(GPIOB,&GPIO_InitStructure);
    
    GPIO_SetBits(GPIOE,GPIO_Pin_8|GPIO_Pin_9);
    GPIO_ResetBits(GPIOB,GPIO_Pin_1);
    GPIO_ResetBits(GPIOB,GPIO_Pin_2);
    
}

uint8_t EMG_SendByte(uint8_t TxData)
{
    /* Loop while DR register in not emplty */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

    /* Send byte through the SPI1 peripheral */
    //SPI_I2S_SendData16(SPI1, TxData);
    SPI_SendData8(SPI1,TxData);
    
    /* Wait to receive a byte */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

    /* Return the byte read from the SPI bus */
    //return SPI_I2S_ReceiveData16(SPI1);
    
    return SPI_ReceiveData8(SPI1);
}

u8 readRegister(char start,u8 count)
{
    char r;
    enableADS1298();
    EMG_SendByte(RREG|start);
    EMG_SendByte(0x00);
    emgPointer=0;
    r=EMG_SendByte(0xff);
    disableADS1298();
    return r;
}



void enableADS1298(void)
{
    GPIO_ResetBits(GPIOB,GPIO_Pin_1);
}

void disableADS1298(void)
{
    GPIO_SetBits(GPIOB,GPIO_Pin_1);
}

int writeRegister(u8 addr,u8 value)
{
    enableADS1298();
    EMG_SendByte(WREG|addr);
    EMG_SendByte(0x00);
    EMG_SendByte(value);
    disableADS1298();
    //delayMs(1);
    if (readRegister(addr,1)==value)
        return 0;
    else
        return -1;
}

float readData(void){
    
    
}

void beginReadDataC(void){
    
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable SYSCFG clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
 
    //EXTI2 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=4 ;//抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
    
    /* Connect EXTI0 Line to PA0 pin */
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource4);
    EXTI_InitStructure.EXTI_Line = EXTI_Line4;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿中断
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    //Start RDATAC
    enableADS1298();
    delayMs(5);

    EMG_SendByte(START);
    delayMs(1);
    EMG_SendByte(RDATAC);
    disableADS1298();
    
    //START HIGH
    GPIO_ResetBits(GPIOB,GPIO_Pin_2);
    
        enableADS1298();
}


void stopReadDataC(void)
{
    //Stop RDATAC
    enableADS1298();

    delayMs(5);
    EMG_SendByte(STOP);
    delayMs(5);
    EMG_SendByte(SDATAC);
    
    disableADS1298();
    
    //START Low
    //GPIO_ResetBits(GPIOB,GPIO_Pin_2);
}

void EXTI4_IRQHandler(void)
{       
    int i;
    
    uint8_t tmp,sum;

    counter++;
    //send package head
    //send data type
    sum=0;
    if (counter&0x01)
    {
        USART2_SendByte(0xff);
        USART2_SendByte(0xff);
        USART2_SendByte(0x01);
    }



    for (i=0;i<27;i++)
    {
        tmp=EMG_SendByte(0x00);
        if (counter&0x01)
            USART2_SendByte(tmp);
        sum+=tmp;
    }
    
    if (counter&0x01)
    {
        USART2_SendByte(sum);
    }
    

//    if (counter&0x01)
//        USART2_SendByte(sum);//send checksum
//    if (counter%25==0)
//    {
//        //Sensor_GetOriQuaternion(BODYSENSOR);
//        //Sensor_GetOriQuaternion(UPPERSENSOR);
//        Sensor_GetOriQuaternion(FORESENSOR);
//        Sensor_GetOriQuaternion(FORESENSOR);
//        Sensor_GetOriQuaternion(FORESENSOR);
//    }
//    if(counter%500==0)
//        printf("%d\n",counter);
    EXTI_ClearITPendingBit(EXTI_Line4);
}

void fakeHandler(void)
{
    int i;
    u8 sum=0;
    counter++;
    if (counter&0x01)
    {
    //send package head
    //send data type
        USART2_SendByte(0xff);
        USART2_SendByte(0xff);
        USART2_SendByte(0x01);
        for (i=0;i<27;i++)
            USART2_SendByte(0x00);
        USART2_SendByte(0x00);//send checksum
    }
    if (counter%25==0)
    {
        //Sensor_GetOriQuaternion(BODYSENSOR);
        //Sensor_GetOriQuaternion(UPPERSENSOR);
        Sensor_GetOriQuaternion(FORESENSOR);
        Sensor_GetOriQuaternion(FORESENSOR);
        Sensor_GetOriQuaternion(FORESENSOR);
    }
}

u8 shakeHands(void)
{
    u8 deviceID;
    deviceID=readRegister(0x00,1);
    return deviceID;
}

int resetADS1298(void)
{
    int trytime;
    GPIO_SetBits(GPIOE,GPIO_Pin_8|GPIO_Pin_9);
    GPIO_ResetBits(GPIOB,GPIO_Pin_1);
    GPIO_ResetBits(GPIOB,GPIO_Pin_2);
    delayMs(10);

    for (trytime=0;trytime<5;trytime++)
    {      
        GPIO_ResetBits(GPIOE,GPIO_Pin_8);
        delayMs(50);
        GPIO_SetBits(GPIOE,GPIO_Pin_8);
        delayMs(10);
        
        enableADS1298();
        EMG_SendByte(SDATAC);
        disableADS1298();
        if (shakeHands()==0x92)
            break;
        delayMs(10);
    }
    if (trytime==5)
        return -1;
    
    for (trytime=0;trytime<5;trytime++)
    {
        writeRegister(0x01,0x86);
        delayMs(10);
        if (readRegister(0x01,1)==0x86)
            break;
        delayMs(10);
    }
    if (trytime==5)
        return -1;
    
    for (trytime=0;trytime<5;trytime++)
    {
        writeRegister(0x03,0xdc);
        delayMs(10);
        if (readRegister(0x03,1)==0xdc)
            break;
        delayMs(10);
    }
    if (trytime==5)
        return -1;
    
    return 0;
}

int configForNoiseTest(void)
{
    u8 addr,trytime;
    //配置通道
    for (addr=0x05;addr<0x09;addr++)
    {
        for (trytime=0;trytime<5;trytime++)
        {
            // input short
            writeRegister(addr,0x01);
            delayMs(10);
            if (readRegister(addr,1)==0x01)
                break;
            delayMs(10);
        }
        if (trytime==5)
            return -1;
    }
    return 0;
}

int configForSquarewaveTest(void)
{
    u8 addr,trytime;
    //配置测试信号发生器
    for (trytime=0;trytime<5;trytime++)
    {
        writeRegister(0x02,0x10);
        delayMs(10);
        if (readRegister(0x02,1)==0x10)
            break;
        delayMs(10);
    }
    if (trytime==5)
        return -1;
    
    //配置通道
    for (addr=0x05;addr<0x09;addr++)
    {
        for (trytime=0;trytime<5;trytime++)
        {
            writeRegister(addr,0x05);
            delayMs(10);
            if (readRegister(addr,1)==0x05)
                break;
            delayMs(10);
        }
        if (trytime==5)
            return -1;
    }    
    return 0;
}

int configForNormalMeasurement(void)
{
    u8 addr,trytime;
    
    //配置通道
    for (addr=0x05;addr<0x09;addr++)
    {
        for (trytime=0;trytime<5;trytime++)
        {
            writeRegister(addr,0x00);
            delayMs(10);
            if (readRegister(addr,1)==0x00)
                break;
            delayMs(10);
        }
        if (trytime==5)
            return -1;
    }    
    return 0;    
}
