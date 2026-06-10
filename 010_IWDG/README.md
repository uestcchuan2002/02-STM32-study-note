# README

## 1. 简介

这个`demo`实现的是`STM32`的`IWDG`独立看门狗的功能，通过按键来实现对看门狗进行喂食，若在规定时间内没有喂食，则`STM32`会自动重启。

## 2. 简单的理论知识

1. 设置开门狗的时钟分频与复位重装值是需要解除写保护的->往key寄存器中写入0xcccc
2. 看门狗的时钟有内部低速时钟提供，这是一个RC时钟，所以说可能不是很准确，一般使用`32kHz`计算
3. 喂狗操作实际上也是通过往key寄存器中写0xaaaa，来实现喂狗


## 3. 快速移植

- 硬件平台：STM32系列MCU--cortex-M
- 存储外设：IWDG
- 驱动程序软件`IWDG`:
    - `IWDG.h`和`IWDG.c`驱动文件
    - 修改对应引脚
```c
// 用户引脚定义
#define IWDG_PRER IWDG_PRESCALER_128
#define IWDG_RLR 1500
#define IWDG_RCC 32000
```
