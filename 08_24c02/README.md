# README

## 1. 简介

这是一个通过`EEPROM`来存储`OTA`升级信息的小案例，设计专门的`OTA`升级数据结构，为`BootLoader`的分区跳转提供必要信息

## 2. OAT升级信息数据结构

```c
typedef enum				/* 启动状态机 */
{
	BOOT_NORMAL = 0,		/* 正常启动 */
	BOOT_SWITCH_REQUEST,    /* 请求切换分区 */
    BOOT_TESTING,           /* 新版本试运行 */
} OTA_STATE;

typedef struct {
    uint32_t magic;         // 魔术字
    uint16_t struct_ver;    // EEPROM数据结构的版本
    uint16_t length;        // 数据结构的长度

    uint32_t fw_version;    // 当前固件版本
    uint8_t  active_slot;   // 活跃分区
    uint8_t  target_slot;   // 目标分区
    OTA_STATE  ota_state;   // OTA升级状态
    uint16_t boot_count;    // 启动次数

    uint32_t upgrade_counter;   // 升级次数

    uint32_t crc32;         // crc校验         
} ota_eeprom_info_t;
```
## 3. 快速移植

- 硬件平台：STM32系列MCU--cortex-M
- 存储外设：EEPROM(AT24CXX)
- 驱动程序软件`IIC`:
    - `myiic.h`和`myiic.c`驱动文件
    - `24cxx.h`和`24cxx.c`芯片驱动文件
    - 修改对应引脚与时钟
```c
// 用户引脚定义
#define IIC_SCL_Pin GPIO_PIN_8
#define IIC_SCL_Port GPIOB
#define IIC_SDA_Pin GPIO_PIN_9
#define IIC_SDA_Port GPIOB
#define IIC_GPIO_RCC_ENABLE __HAL_RCC_GPIOB_CLK_ENABLE() 
```
## 4. 使用方法

源码例程
> 更新`ota_eeprom_info_t`结构体数据后，都需要重新计算`crc`,并且重新写入`EEPRON`
