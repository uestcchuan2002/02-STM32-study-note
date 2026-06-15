# README

## 1. PWM简介

`PWM`全称脉冲宽度调整，是一种利用微处理器的数字输出来对模拟电路进行控制的有效技术，广泛应用与电机控制，灯光控制等需要调速，调量的场所。

有2个重要的参数对于`PWM`信号：其频率、占空比
- 频率决定：多久时间一个脉冲周期，很多设备的控制对一个特定的要求
- 占空比：即一个周期内高电平的所占比例，往往决定调速的大小

## 2. 核心原理

对于STM32产生`PWM`信号的核心原理如下：
1. 通过定时器产生一个计数，实现自动重装
2. 设定一个`PWM比较值`，与定时器计数值进行比较
3. 计数值比比较值低时，控制`IO`口输出低电平（或者高电平，与设置极性有关）
4. 计数值比比较值高时，控制`IO`口输出高电平
5. 同时上述比较，生成在一个计数周期内的高低电平翻转，生成波形


## 3. STM32编程

### 3.1 环境

- 设备为 `STM32F407`
- 需要添加的官方 `HAL`库为：
    1. `stm32f4xx_hal_tim.h`
    2. `stm32f4xx_hal_tim.c`

### 3.2 关键步骤
1. 定义定时器句柄和PWM参数句柄
```c
TIM_HandleTypeDef TIM14_Handler;
TIM_OC_InitTypeDef TIM14_CH1Handler;
```

2. 定时器、PWM通道、GPIO口初始化
```c
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
```

3. 设置占空比函数->设置比较值
```c
/*
	设置TIM14通道1的占空比 
	compare:比较值
*/
void TIM_SetTIM14Compare1(u32 compare)
{
	TIM14->CCR1 = compare;
}
```

## 4. 出现的问题

无