
# README

## 1. 输入捕获简介

输入捕获是 STM32 定时器的一种常用功能，主要用于测量外部输入信号的时间参数，例如：

- 脉冲宽度
- 信号周期
- 高电平持续时间
- PWM 输入信号占空比

本实验使用 `STM32F407` 的 `TIM5_CH1` 输入捕获功能，对外部输入到 `PA0` 引脚的脉冲信号进行捕获，并计算其高电平持续时间。

由于 `TIM5` 是 32 位定时器，因此它的计数范围较大，适合测量较长时间的脉冲宽度。

---

## 2. 核心原理

输入捕获的基本思想是：

1. 配置定时器通道为输入捕获模式。
2. 首先捕获输入信号的上升沿。
3. 捕获到上升沿后，将定时器计数值清零。
4. 修改捕获极性，改为捕获下降沿。
5. 捕获到下降沿时，读取当前定时器计数值。
6. 该计数值即为高电平持续的时间。

本程序中使用 `TIM5_CH1` 进行输入捕获：

- 第一次捕获上升沿，表示高电平开始。
- 第二次捕获下降沿，表示高电平结束。
- 两次边沿之间的计数值即为高电平脉宽。

捕获状态由变量 `TIM5CH1_CAPTURE_STA` 表示：

```c
u8 TIM5CH1_CAPTURE_STA;
```

其各位含义如下：

| 位 | 含义 |
|---|---|
| bit7 | 捕获完成标志，`1` 表示成功捕获一次 |
| bit6 | 捕获到上升沿标志，`1` 表示已经捕获到高电平开始 |
| bit5 ~ bit0 | 定时器溢出次数 |

捕获值由变量 `TIM5CH1_CAPTURE_VAL` 保存：

```c
u32 TIM5CH1_CAPTURE_VAL;
```

由于 `TIM5` 是 32 位定时器，所以最大计数值为：

```c
0xFFFFFFFF
```

如果定时器计数频率为 `1MHz`，即 `1us` 计数一次，则最大可测时间约为：

```text
4294 秒
```

---

## 3. STM32 编程

### 3.1 环境

- 设备：`STM32F407`
- 定时器：`TIM5`
- 捕获通道：`TIM5_CH1`
- 输入引脚：`PA0`
- 引脚复用功能：`GPIO_AF2_TIM5`
- 使用库：STM32 官方 `HAL` 库

需要添加的官方 HAL 库文件为：

1. `stm32f4xx_hal_tim.h`
2. `stm32f4xx_hal_tim.c`

---

### 3.2 关键步骤

#### 1. 定义定时器句柄

```c
TIM_HandleTypeDef TIM5_Handler;
```

该句柄用于保存 `TIM5` 的配置信息，并在 HAL 库函数中使用。

---

#### 2. 初始化 TIM5 输入捕获

```c
void TIM5_CH1_Cap_Init(u32 arr, u16 psc)
```

该函数用于初始化 `TIM5_CH1` 输入捕获功能。

主要配置内容包括：

```c
TIM5_Handler.Instance = TIM5;
TIM5_Handler.Init.Prescaler = psc;
TIM5_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
TIM5_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;
TIM5_Handler.Init.Period = arr;
HAL_TIM_IC_Init(&TIM5_Handler);
```

参数说明：

| 参数 | 说明 |
|---|---|
| `arr` | 自动重装载值 |
| `psc` | 预分频系数 |

如果希望定时器以 `1MHz` 频率计数，可以根据定时器时钟频率设置 `psc`，使计数周期为 `1us`。

---

#### 3. 配置输入捕获通道

```c
TIM5_CH1Config.ICPolarity = TIM_ICPOLARITY_RISING;
TIM5_CH1Config.ICPrescaler = TIM_ICPSC_DIV1;
TIM5_CH1Config.ICSelection = TIM_ICSELECTION_DIRECTTI;
TIM5_CH1Config.ICFilter = 0;
HAL_TIM_IC_ConfigChannel(&TIM5_Handler, &TIM5_CH1Config, TIM_CHANNEL_1);
```

配置说明：

| 配置项 | 作用 |
|---|---|
| `TIM_ICPOLARITY_RISING` | 初始捕获上升沿 |
| `TIM_ICPSC_DIV1` | 每次有效边沿都捕获 |
| `TIM_ICSELECTION_DIRECTTI` | 通道直接连接到输入引脚 |
| `ICFilter = 0` | 不使用滤波 |

---

#### 4. 开启输入捕获中断和更新中断

```c
HAL_TIM_IC_Start_IT(&TIM5_Handler, TIM_CHANNEL_1);
__HAL_TIM_ENABLE_IT(&TIM5_Handler, TIM_IT_UPDATE);
```

其中：

- 输入捕获中断用于检测上升沿和下降沿。
- 更新中断用于处理定时器溢出。

---

#### 5. 配置 GPIO 和 NVIC

```c
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
```

该函数会被 `HAL_TIM_IC_Init()` 自动调用。

主要完成以下工作：

1. 使能 `TIM5` 时钟。
2. 使能 `GPIOA` 时钟。
3. 配置 `PA0` 为复用功能。
4. 设置 `PA0` 复用为 `TIM5_CH1`。
5. 配置并开启 `TIM5` 中断。

关键代码如下：

```c
__HAL_RCC_TIM5_CLK_ENABLE();
__HAL_RCC_GPIOA_CLK_ENABLE();

GPIO_Initure.Pin = GPIO_PIN_0;
GPIO_Initure.Mode = GPIO_MODE_AF_PP;
GPIO_Initure.Pull = GPIO_PULLDOWN;
GPIO_Initure.Speed = GPIO_SPEED_HIGH;
GPIO_Initure.Alternate = GPIO_AF2_TIM5;

HAL_GPIO_Init(GPIOA, &GPIO_Initure);

HAL_NVIC_SetPriority(TIM5_IRQn, 2, 0);
HAL_NVIC_EnableIRQ(TIM5_IRQn);
```

---

#### 6. 编写 TIM5 中断服务函数

```c
void TIM5_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM5_Handler);
}
```

在中断服务函数中调用 HAL 库的统一处理函数，实际的捕获回调和溢出回调会由 HAL 库自动调用。

---

#### 7. 处理定时器溢出

```c
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
```

当定时器发生溢出时，会进入该回调函数。

如果已经捕获到上升沿，但还没有捕获到下降沿，则需要记录溢出次数。

```c
if ((TIM5CH1_CAPTURE_STA & 0x40)) {
    if ((TIM5CH1_CAPTURE_STA & 0x3F) == 0x3F) {
        TIM5CH1_CAPTURE_STA |= 0x80;
        TIM5CH1_CAPTURE_VAL = 0xFFFFFFFF;
    } else {
        TIM5CH1_CAPTURE_STA++;
    }
}
```

当溢出次数达到最大值时，认为捕获完成，并将捕获值设置为最大值。

---

#### 8. 处理输入捕获回调

```c
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
```

该函数是输入捕获的核心逻辑。

第一次进入时，捕获到上升沿：

```c
TIM5CH1_CAPTURE_STA = 0;
TIM5CH1_CAPTURE_VAL = 0;
TIM5CH1_CAPTURE_STA |= 0x40;

__HAL_TIM_DISABLE(&TIM5_Handler);
__HAL_TIM_SET_COUNTER(&TIM5_Handler, 0);

TIM_RESET_CAPTUREPOLARITY(&TIM5_Handler, TIM_CHANNEL_1);
TIM_SET_CAPTUREPOLARITY(&TIM5_Handler, TIM_CHANNEL_1, TIM_ICPOLARITY_FALLING);

__HAL_TIM_ENABLE(&TIM5_Handler);
```

此时程序完成：

1. 清空捕获状态。
2. 清空捕获值。
3. 标记已经捕获到上升沿。
4. 关闭定时器。
5. 将计数器清零。
6. 将捕获极性改为下降沿。
7. 重新开启定时器。

第二次进入时，捕获到下降沿：

```c
TIM5CH1_CAPTURE_STA |= 0x80;
TIM5CH1_CAPTURE_VAL = HAL_TIM_ReadCapturedValue(&TIM5_Handler, TIM_CHANNEL_1);

TIM_RESET_CAPTUREPOLARITY(&TIM5_Handler, TIM_CHANNEL_1);
TIM_SET_CAPTUREPOLARITY(&TIM5_Handler, TIM_CHANNEL_1, TIM_ICPOLARITY_RISING);
```

此时程序完成：

1. 标记捕获完成。
2. 读取捕获值。
3. 将捕获极性重新设置为上升沿，等待下一次捕获。

---

## 4. 出现的问题

### 4.1 必须先清除原来的捕获极性

在切换上升沿捕获和下降沿捕获时，需要先清除原来的捕获极性配置：

```c
TIM_RESET_CAPTUREPOLARITY(&TIM5_Handler, TIM_CHANNEL_1);
```

然后再重新设置新的捕获极性：

```c
TIM_SET_CAPTUREPOLARITY(&TIM5_Handler, TIM_CHANNEL_1, TIM_ICPOLARITY_FALLING);
```

或：

```c
TIM_SET_CAPTUREPOLARITY(&TIM5_Handler, TIM_CHANNEL_1, TIM_ICPOLARITY_RISING);
```

如果不先清除原来的配置，可能会导致捕获边沿切换失败。

---

### 4.2 需要开启更新中断

如果只开启输入捕获中断，而没有开启更新中断，则定时器溢出时无法进入溢出回调函数：

```c
__HAL_TIM_ENABLE_IT(&TIM5_Handler, TIM_IT_UPDATE);
```

这样在测量较长脉冲时，可能无法正确统计溢出次数。

---

### 4.3 GPIO 复用功能必须配置正确

`TIM5_CH1` 对应的输入引脚为 `PA0`，需要将 `PA0` 配置为复用功能：

```c
GPIO_Initure.Alternate = GPIO_AF2_TIM5;
```

如果复用功能配置错误，定时器无法捕获外部输入信号。

---

### 4.4 中断函数名称必须正确

`TIM5` 的中断服务函数名称必须为：

```c
void TIM5_IRQHandler(void)
```

如果函数名写错，中断将无法正常进入。

---

### 4.5 输入信号需要接到 PA0

本实验使用的是 `TIM5_CH1`，因此外部输入信号应接到：

```text
PA0
```

如果接到其他引脚，需要重新确认该引脚是否支持 `TIM5_CH1` 复用功能。

---

## 5. 总结

本程序通过 `TIM5_CH1` 实现了输入捕获功能，可以测量输入信号的高电平持续时间。

整体流程如下：

1. 初始化 `TIM5` 为输入捕获模式。
2. 配置 `PA0` 为 `TIM5_CH1` 复用功能。
3. 初始捕获上升沿。
4. 捕获到上升沿后清零计数器。
5. 切换为下降沿捕获。
6. 捕获到下降沿后读取计数值。
7. 得到高电平持续时间。

如果定时器计数频率配置为 `1MHz`，则：

```text
高电平时间 = TIM5CH1_CAPTURE_VAL us
```
```