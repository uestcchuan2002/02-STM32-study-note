#ifndef __PWM_H
#define __PWM_H

#include "main.h"
#include "sys.h"

void TIM14_PWM_Init(u16 arr, u16 psc);
void TIM_SetTIM14Compare1(u32 compare);



#endif
