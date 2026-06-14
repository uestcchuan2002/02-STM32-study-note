# README

## 1. 定时器简介

`stm32F4`拥有`14`个定时器资源，这些定时器根据功能的多少分为：*高级定时器*、*通用定时器*和*基本定时器* 3类
今天这个实验完成的基于通用定时器3的定时中断实验，实现了在预先设定的时间周期后，自动对应的时间周期产生中断的效果
> 用途：当我们需要进行周期性的工作，比如显示屏数据显示、数据上报云端等工作，但是又不想在主循环中阻塞计时时，就可以通过一个定时器定时来执行这项周期性的工作

*适用场景*：非阻塞计时的周期性工作场所

## 2. 核心原理

定时器的本质就是一个*计数器*，当我们知道了需要计数到的值和每个计数值的频率的时候，就能精确的指定从`0 ~ 计数设定值`的时间。到达了计数值时，产生一个中断，跳转至中断服务函执行。
因此往往需要以下准备：
- 一个精确的定时器时钟源，确保计数稳定进行
- 对时钟源进行分频，因为时钟源往往频率太高，导致能够计数任务很快就完成了
- 设置计数设定值
- 开启定时中断，到达计数值时，自动进行中断跳转

## 3. STM32编程

### 3.1 环境

- 设备为 `STM32F407`
- 需要添加的官方 `HAL`库为：
    1. `stm32f4xx_hal_tim.h`
    2. `stm32f4xx_hal_tim.c`

### 3.2 关键步骤

1. 定义定时器句柄
`TIM_HandleTypeDef TIM3_Handler;`
    作用：这个句柄包含了定时器的所有信息与配置，用于初始化与识别

2. 定时器初始化
```c
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
```

3. 底层驱动配置
```c
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim)
{
	if (htim->Instance == TIM3) {
		
		__HAL_RCC_TIM3_CLK_ENABLE();
		HAL_NVIC_SetPriority(TIM3_IRQn, 1, 3);
		HAL_NVIC_EnableIRQ(TIM3_IRQn);
	}
}
```

4. 定时器中断服务
```c
void TIM3_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&TIM3_Handler);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef * htim)
{
	if (htim==(&TIM3_Handler)) {
		LED1 = !LED1;
	}
}

```

## 4. 出现的问题

1. 定时器启动函数必须为 `HAL_TIM_Base_Start_IT(&TIM3_Handler);`
因为启动后需要使用到中断，因此启动函数的后缀必须包含 `IT`