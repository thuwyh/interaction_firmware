#ifndef ADS1298_H
#define ADS1298_H

#include "stm32f37x_spi.h"
#include "usart.h"

// ADS1298 commands
#define WREG 0x40
#define RREG 0x20
#define START 0x08
#define STOP 0x0a
#define RDATAC 0x10
#define SDATAC 0x11


uint8_t EMG_SendByte(uint8_t TxData);
void ads1298_init(void);
u8 readRegister(char start,u8 count);
void beginReadDataC(void);
void stopReadDataC(void);
int writeRegister(u8 addr,u8 value);
float readData(void);
void resetADS1298(void);
void enableADS1298(void);
void disableADS1298(void);
void fakeHandler(void);
u8 shakeHands(void);


#endif
