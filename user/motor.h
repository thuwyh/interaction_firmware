#ifndef MOTOR_H
#define MOTOR_H

#include "stm32f37x.h"

#define MOTOR1 0x01
#define MOTOR2 0x02
#define MOTOR3 0x03

void motor_init(void);
void motorshake(u8 motors,int shanketime, int sleeptime, int times);

#endif
