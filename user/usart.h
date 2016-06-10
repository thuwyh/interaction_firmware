#ifndef USARTDEBUGGER_H
#define USARTDEBUGGER_H
#include "stm32f37x.h"
#include "stm32f37x_usart.h"
#include <stdio.h>

typedef enum {
    HEAD,LENGTH,COMMAND,ADDR,VALUE,CHECK
}State;

void USART2_Init(int baudrate);
void USART2_SendByte(u8 c);
void parse(u8 c);



#endif
