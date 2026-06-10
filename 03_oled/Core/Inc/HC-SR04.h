#ifndef __HCSR04_H
#define __HCSR04_H

#include "main.h"
#include "gpio.h"
#include "usart.h"

extern uint32_t num;
extern uint32_t Distance;

void Delay_us(__IO uint32_t delay);
void HC_SR04_Computer(void);

#endif
