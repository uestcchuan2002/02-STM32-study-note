#ifndef __INPUT_CAPTURE_H
#define __INPUT_CAPTURE_H

#include "main.h"
#include "sys.h"

extern u8  TIM5CH1_CAPTURE_STA;
extern u32 TIM5CH1_CAPTURE_VAL;

void TIM5_CH1_Cap_Init(u32 arr,u16 psc);

#endif
