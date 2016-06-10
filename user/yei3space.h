#ifndef YEI3SPACE
#define YEI3SPACE

#include "stm32f37x_spi.h"
#include "usart.h"

#define BODYSENSOR 0x00
#define UPPERSENSOR 0x01
#define FORESENSOR 0x02

void Sensor_Init(void);			 //��ʼ��SPI��
void SPI3_SetSpeed(u8 SpeedSet); //����SPI�ٶ�   
uint16_t Sensor_SendByte(uint16_t TxData);//SPI���߶�дһ���ֽ�
int Sensor_Sel(int index);
int Sensor_GetOriQuaternion(int index);
int Sensor_BeginCommunicate(void);
int Sensor_WaitDataReady(void);
u8 Sensor_ReadResponse(int len);
int reverseArray(char * src, char *des, int len);


#endif