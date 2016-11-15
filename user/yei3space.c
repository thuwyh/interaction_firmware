#include "yei3space.h"

char sensorBuffer[255];
u8 sensorPointer;

void Sensor_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);//PORTBʱ��ʹ�� 
    RCC_APB1PeriphClockCmd(	RCC_APB1Periph_SPI3,  ENABLE );//SPI3ʱ��ʹ�� 	

    
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_6);
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_6);
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_6);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3  | GPIO_Pin_5 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	//����
    GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;//�������  //PB13/14/15����������� 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOB
    
    SPI_I2S_DeInit(SPI3);
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//����SPI����ģʽ:����Ϊ��SPI
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		//����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	//����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
    SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ����Ķ���ʽ
    SPI_Init(SPI3, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���

    SPI_NSSInternalSoftwareConfig(SPI3,SPI_NSSInternalSoft_Set);
    SPI_RxFIFOThresholdConfig(SPI3, SPI_RxFIFOThreshold_QF);
    SPI_Cmd(SPI3, ENABLE); //ʹ��SPI����
    
    //Ƭѡ�ź�
    GPIO_InitStructure.GPIO_Mode= GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6| GPIO_Pin_7 |GPIO_Pin_8;
    GPIO_Init(GPIOB,&GPIO_InitStructure);
    GPIO_SetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8);
}

uint16_t Sensor_SendByte(uint8_t TxData)
{
    /* Loop while DR register in not emplty */
    while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET);

    /* Send byte through the SPI1 peripheral */
    SPI_SendData8(SPI3, TxData);

    /* Wait to receive a byte */
    while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET);

    /* Return the byte read from the SPI bus */
    return SPI_ReceiveData8(SPI3);
}

int Sensor_BeginCommunicate(void)
{
    u8 res;
    res=Sensor_SendByte(0xf6);
    if (res!=0x00 && res!=0x04)
        return -1;
    else
        return 0;
}

int Sensor_WaitDataReady(void)
{
    u8 res;
    int counter=0;
    res=Sensor_SendByte(0xff);
    while (res!=0x01)
    {
        res=Sensor_SendByte(0xff);
        counter++;
        if (counter>100000)
            return -1;
    }
    return 0;
}

u8 Sensor_ReadResponse(int len)
{
    for (sensorPointer=0;sensorPointer<len;sensorPointer++)
    {
        sensorBuffer[sensorPointer]=Sensor_SendByte(0xff);
        //USART1_SendByte(sensorBuffer[sensorPointer]);
    }
    return 0;
}

int Sensor_Sel(int index)
{
//    GPIO_SetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8);
    if (index==BODYSENSOR)
        GPIO_ResetBits(GPIOB,GPIO_Pin_6);
    if (index==UPPERSENSOR)
        GPIO_ResetBits(GPIOB,GPIO_Pin_7);
    if (index==FORESENSOR)
        GPIO_ResetBits(GPIOB,GPIO_Pin_8);
    return 0;
}

int Sensor_Desel(int index)
{
//    GPIO_SetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8);
    if (index==BODYSENSOR)
        GPIO_SetBits(GPIOB,GPIO_Pin_6);
    if (index==UPPERSENSOR)
        GPIO_SetBits(GPIOB,GPIO_Pin_7);
    if (index==FORESENSOR)
        GPIO_SetBits(GPIOB,GPIO_Pin_8);
    return 0;
}

int Sensor_GetOriQuaternion(int index)
{
    u8 res;
    int ret,i;
    float quat[4];
    Sensor_Sel(index);
    ret=Sensor_BeginCommunicate();
    if (ret<0)
        return -1;
    Sensor_SendByte(0x06);
    ret=Sensor_WaitDataReady();
    if (ret<0)
        return -1;
    ret=Sensor_ReadResponse(16);
    if (ret<0)
        return -1;
    reverseArray(sensorBuffer,(char*)quat,4);
    //send package head
    //send data type
    //send sensor ID
    //printf("quat: %f, %f, %f, %f\n",quat[0],quat[1],quat[2],quat[3]);//replace this
    res=0;
    for (i=0;i<16;i++)
    {
        USART2_SendByte(*((u8*)quat+i));
        res+=*((u8*)quat+i);
    }
    Sensor_Desel(index);
    //send checksum
    return res;
}

int reverseArray(char * src, char *des, int len)
{
    int i,j;
    for (i=0;i<len;i++)
        for (j=0;j<4;j++)
            des[(i<<2)+j]=src[(i<<2)+3-j];
    return 0;
}

