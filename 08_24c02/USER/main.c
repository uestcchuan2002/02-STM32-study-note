#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "24cxx.h"
#include "stddef.h"
#include "stdio.h"

// EEPROM存储OTA升级信息
#define OTA_INFO_MAGIC 0x4F544149u

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


static ota_eeprom_info_t ota_eeprom_info;
char lcd_buffer[40];

uint32_t ota_info_calc_crc(const ota_eeprom_info_t *info);
uint32_t crc32_calc(const uint8_t *data, uint32_t len);

int main(void)
{
    u16 i = 0;
    u8 key = 0;
    u8 first_check = 0;

    HAL_Init();                     // 初始化HAL库
    Stm32_Clock_Init(336, 8, 2, 7); // 设置时钟,168Mhz
    delay_init(168);                // 初始化延时函数
    uart_init(115200);              // 初始化USART
    LED_Init();                     // 初始化LED
    KEY_Init();                     // 初始化KEY
    LCD_Init();                     // 初始化LCD

    AT24CXX_Init();                 // 初始化IIC
    POINT_COLOR = RED;
    LCD_ShowString(30, 50, 200, 16, 16, (u8*)"Apollo STM32F4/F7");
    LCD_ShowString(30, 70, 300, 16, 16, (u8*)"EEPROM Storage Ota");
    LCD_ShowString(30, 90, 200, 16, 16, (u8*)"ATOM@ALIENTEK");
    LCD_ShowString(30, 110, 200, 16, 16, (u8*)"2026/6/10");
    LCD_ShowString(30, 130, 200, 16, 16, (u8*)"author: chuan"); 

    while (AT24CXX_Check())         // 检测不到24c02
    {
        LCD_ShowString(30, 150, 200, 16, 16, (u8*)"24C02 Check Failed!");
        delay_ms(500);
        LCD_ShowString(30, 150, 200, 16, 16, (u8*)"Please Check!      ");
        delay_ms(500);
        LED0 = !LED0;               // DS0闪烁
    }
    LCD_ShowString(30, 150, 200, 16, 16, (u8*)"24C02 Ready!");

    first_check = AT24CXX_ReadOneByte(254);
    if (first_check == 0xAB) {
        // ota信息已经写入
    } else {
        // 第一次写入
        AT24CXX_WriteOneByte(254, 0XAB);
        delay_ms(10);
        ota_eeprom_info.magic = OTA_INFO_MAGIC;
        ota_eeprom_info.struct_ver = 1;
        ota_eeprom_info.length = sizeof(ota_eeprom_info_t);
        ota_eeprom_info.fw_version = 0x00000001;
        ota_eeprom_info.active_slot = 0x0a;
        ota_eeprom_info.target_slot = 0x0b;
        ota_eeprom_info.ota_state = BOOT_NORMAL;
        ota_eeprom_info.boot_count = 0;
        ota_eeprom_info.upgrade_counter = 0;
        ota_eeprom_info.crc32 = ota_info_calc_crc(&ota_eeprom_info);
        AT24CXX_Write(0, (u8*)(&ota_eeprom_info), sizeof(ota_eeprom_info_t));
        delay_ms(100);
    }

    POINT_COLOR = BLUE; // 设置字体为蓝色
    AT24CXX_Read(0, (u8*)&ota_eeprom_info, sizeof(ota_eeprom_info_t));
    // 根据 ota_state 来判断是否需要升级
    switch (ota_eeprom_info.ota_state) 
    {
        case BOOT_NORMAL:
            // 正常跳转至活跃app
            printf("正常状态, 活跃分区\r\n");
        break;
        case BOOT_SWITCH_REQUEST:
            printf("跳转至目标分区\r\n");
        break;
        case BOOT_TESTING:
            printf("新版测试阶段\r\n");
        break;
    }

    sprintf(lcd_buffer, "magic:%x", ota_eeprom_info.magic);
    LCD_ShowString(30, 170, 200, 16, 16, (u8*)lcd_buffer);
    sprintf(lcd_buffer, "struct_ver:%x", ota_eeprom_info.struct_ver);
    LCD_ShowString(30, 190, 200, 16, 16, (u8*)lcd_buffer);
    sprintf(lcd_buffer, "length:%d", ota_eeprom_info.length);
    LCD_ShowString(30, 210, 200, 16, 16, (u8*)lcd_buffer);
    sprintf(lcd_buffer, "fw_version:%d", ota_eeprom_info.fw_version);
    LCD_ShowString(30, 230, 200, 16, 16, (u8*)lcd_buffer);
    sprintf(lcd_buffer, "active_slot:%x", ota_eeprom_info.active_slot);
    LCD_ShowString(30, 250, 200, 16, 16, (u8*)lcd_buffer);
    sprintf(lcd_buffer, "target_slot:%x", ota_eeprom_info.target_slot);
    LCD_ShowString(30, 270, 200, 16, 16, (u8*)lcd_buffer);
    sprintf(lcd_buffer, "ota_state:%d", ota_eeprom_info.ota_state);
    LCD_ShowString(30, 290, 200, 16, 16, (u8*)lcd_buffer);
    sprintf(lcd_buffer, "boot_count:%d", ota_eeprom_info.boot_count);
    LCD_ShowString(30, 310, 200, 16, 16, (u8*)lcd_buffer);
    sprintf(lcd_buffer, "current boot_count:%d", ota_eeprom_info.boot_count);
    LCD_ShowString(30, 350, 200, 16, 16, (u8*)lcd_buffer);

    while (1)
    {
        key = KEY_Scan(0);
        if (key == KEY0_PRES) {
            ota_eeprom_info.boot_count++;
            sprintf(lcd_buffer, "current boot_count:%d", ota_eeprom_info.boot_count);
            LCD_ShowString(30, 350, 200, 16, 16, (u8*)lcd_buffer);
            ota_eeprom_info.crc32 = ota_info_calc_crc(&ota_eeprom_info);
            AT24CXX_Write(0, (u8*)(&ota_eeprom_info), sizeof(ota_eeprom_info_t));
            delay_ms(100);
        } else if (key == KEY1_PRES) {
            ota_eeprom_info.boot_count--;
            sprintf(lcd_buffer, "current boot_count:%d", ota_eeprom_info.boot_count);
            LCD_ShowString(30, 350, 200, 16, 16, (u8*)lcd_buffer);
            ota_eeprom_info.crc32 = ota_info_calc_crc(&ota_eeprom_info);
            AT24CXX_Write(0, (u8*)(&ota_eeprom_info), sizeof(ota_eeprom_info_t));
            delay_ms(100);
        }

        i++;
        delay_ms(10);
        if (i == 20)
        {
            LED0 = !LED0; // 提示系统正在运行
            i = 0;
        }
    }
}

uint32_t crc32_calc(const uint8_t *data, uint32_t len)
{
    uint32_t crc = 0xFFFFFFFFu;
    uint8_t i;
    while (len--) {
        crc ^= *data++;

        for (i = 0; i < 8; i++) {
            if (crc & 1u) {
                crc = (crc >> 1) ^ 0xEDB88320u;
            } else {
                crc = crc >> 1;
            }
        }
    }

    return crc ^ 0xFFFFFFFFu;
}

uint32_t ota_info_calc_crc(const ota_eeprom_info_t *info)
{
    return crc32_calc((const uint8_t *)info,
                      offsetof(ota_eeprom_info_t, crc32));
}
