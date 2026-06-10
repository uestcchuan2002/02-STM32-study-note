#ifndef __IWDG_H
#define __IWDG_H

#include "main.h"
#include "sys.h"

void IWDG_Init(uint8_t prer, uint16_t rlr);
void IWDG_Feed_Dog(void);

#endif

