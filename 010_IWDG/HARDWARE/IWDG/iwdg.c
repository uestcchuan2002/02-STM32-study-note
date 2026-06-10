#include "iwdg.h"

IWDG_HandleTypeDef iwdg_hander;

/*
	初始化独立看门狗
	始终有内部低速时钟（LSL）提供，32kHz左右
	prer: 分配系数： IWDG_PRESCALER_4~IWDG_PRESCALER_256
	rlr:  自动重装值 0 ~ 0xFFF
	时间计算： time = (4 * 2 ^ prer) * rlr / 32 ms
*/
void IWDG_Init(uint8_t prer, uint16_t rlr)
{
	iwdg_hander.Instance = IWDG;
	iwdg_hander.Init.Prescaler = prer;
	iwdg_hander.Init.Reload = rlr;
	HAL_IWDG_Init(&iwdg_hander);
}

/* 喂狗函数 */
void IWDG_Feed_Dog(void) 
{
	HAL_IWDG_Refresh(&iwdg_hander);
}









