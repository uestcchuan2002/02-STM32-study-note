#include "time.h"
#include "led.h"

/* 定时器3句柄 */
TIM_HandleTypeDef TIM3_Handler;

/*
	定时器3初始化
	arr: 自动重装值
	pse: 定时器分频系数
	- 在SystemInit()函数中设置的APB1的分频系数为4，即APB1 = 168MHz / 4 = 42MHz
	- 但是定时器3时钟（挂载在APB1上）的时钟频率有下面特性：当APB1分频系数为1时，定时器3的时钟频率与APB1相同
	- 大于1时，都为APB1的两倍，因此：tim3_clock_freq = APB1 * 2 = 84MHz

	因此，定时器3的定时周期可以如下计算:
	计数周期为分频后的结果：(tim3_clock_freq / psc + 1)
	计数值为：arr + 1
	周期为：(arr + 1) * 1 / (tim3_clock_freq / psc + 1) = (arr + 1) * (psc + 1) / tim3_clock_freq
	举例: arr: 8399 , psc = 9999 --> period = 1 s
*/
void TIM3_Init(uint16_t arr, uint16_t psc) 
{
	TIM3_Handler.Instance = TIM3;
	
	TIM3_Handler.Init.Prescaler = psc;
	TIM3_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;
	TIM3_Handler.Init.Period = arr;
	TIM3_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_Base_Init(&TIM3_Handler);
	HAL_TIM_Base_Start_IT(&TIM3_Handler);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim)
{
	if (htim->Instance == TIM3) {
		
		__HAL_RCC_TIM3_CLK_ENABLE();
		HAL_NVIC_SetPriority(TIM3_IRQn, 1, 3);
		HAL_NVIC_EnableIRQ(TIM3_IRQn);
	}
}


void TIM3_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&TIM3_Handler);
}

//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef * htim)
//{
//	if (htim==(&TIM3_Handler)) {
//		LED1 = !LED1;
//	}
//}



