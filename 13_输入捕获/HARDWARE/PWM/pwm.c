#include "pwm.h"

TIM_HandleTypeDef TIM14_Handler;
TIM_OC_InitTypeDef TIM14_CH1Handler;

/*
	TIM14 PWM初始化
	arr: 自动重装值
	psc: 时钟预分频器
	定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
	Ft=定时器工作频率,单位:Mhz 
*/
void TIM14_PWM_Init(u16 arr, u16 psc)
{
	/* 1.初始化TIM14定时器：实例、预分频系数、计数模式、分频系数、自动重装值*/
	TIM14_Handler.Instance = TIM14;
	TIM14_Handler.Init.Prescaler = psc;
	TIM14_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;
	TIM14_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	TIM14_Handler.Init.Period = arr;
	/* PWM定时器采用专门的初始化函数 */
	HAL_TIM_PWM_Init(&TIM14_Handler);

	/* 2.配置PWM参数：模式、比较值、极性 */
	TIM14_CH1Handler.OCMode = TIM_OCMODE_PWM1;
	TIM14_CH1Handler.Pulse = arr / 2;
	TIM14_CH1Handler.OCPolarity = TIM_OCPOLARITY_LOW;
	HAL_TIM_PWM_ConfigChannel(&TIM14_Handler, &TIM14_CH1Handler, TIM_CHANNEL_1);

	/* 3.输出PWM*/
	HAL_TIM_PWM_Start(&TIM14_Handler, TIM_CHANNEL_1);
}


/*
	定时器底层驱动，时钟使能，引脚配置 
	此函数会被HAL_TIM_PWM_Init()调用 
	htim:定时器句柄 
*/
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim) 
{
	GPIO_InitTypeDef GPIO_Init;
	
	__HAL_RCC_TIM14_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	
	GPIO_Init.Pin = GPIO_PIN_9;
	GPIO_Init.Mode = GPIO_MODE_AF_PP;
	GPIO_Init.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_Init.Alternate = GPIO_AF9_TIM14;
	HAL_GPIO_Init(GPIOF, &GPIO_Init);
}


/*
	设置TIM14通道1的占空比 
	compare:比较值
*/
void TIM_SetTIM14Compare1(u32 compare)
{
	TIM14->CCR1 = compare;
}










