#ifndef YEI3SPACE
#define YEI3SPACE

#include "stm32f37x_spi.h"
#include "usart.h"

#define BODYSENSOR 0x00
#define UPPERSENSOR 0x01
#define FORESENSOR 0x02

void Sensor_Init(void);			 //初始化SPI口
void SPI3_SetSpeed(u8 SpeedSet); //设置SPI速度   
uint16_t Sensor_SendByte(uint16_t TxData);//SPI总线读写一个字节
int Sensor_Sel(int index);
int Sensor_GetOriQuaternion(int index);
int Sensor_BeginCommunicate(void);
int Sensor_WaitDataReady(void);
u8 Sensor_ReadResponse(int len);
int reverseArray(char * src, char *des, int len);


#endif