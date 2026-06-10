#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "iwdg.h"

#define IWDG_PRER IWDG_PRESCALER_128
#define IWDG_RLR 1500
#define IWDG_RCC 32000

char lcd_buffer[32];

int main(void)
{
	int i = 0;
    int sec = 0;
    int cur_sec;
    HAL_Init();                     // 初始化HAL库
    Stm32_Clock_Init(336, 8, 2, 7); // 设置时钟,168Mhz
    delay_init(168);                // 初始化延时函数
    uart_init(115200);              // 初始化USART
    LED_Init();                     // 初始化LED
    KEY_Init();                     // 初始化KEY
    LCD_Init();                     // 初始化LCD

	IWDG_Init(IWDG_PRER, IWDG_RLR);	// 初始化IWDG
    sec = 4 * (1 << IWDG_PRER) * IWDG_RLR / IWDG_RCC;
    cur_sec = sec;

    POINT_COLOR = RED;
    LCD_ShowString(30, 10, 200, 16, 32, (u8*)"Apollo STM32F4/F7");
    LCD_ShowString(30, 50, 300, 16, 32, (u8*)"IWDG Testing");
    LCD_ShowString(30, 90, 200, 16, 32, (u8*)"ATOM@ALIENTEK");
    LCD_ShowString(30, 130, 200, 16, 32, (u8*)"2026/6/10");
    LCD_ShowString(30, 170, 200, 16, 32, (u8*)"author: chuan"); 
	
	LED0 = 1;

    POINT_COLOR = BLUE; // 设置字体为蓝色
    
	
   
    while (1)
    {
		if (KEY_Scan(0) == WKUP_PRES)
		{
			IWDG_Feed_Dog();
            cur_sec = sec;
		}
		i++;
		if (i == 100) 
		{
			i = 0;
            cur_sec--;
            sprintf(lcd_buffer, "iwdg reset time %d\r\n", cur_sec);
			LCD_ShowString(30, 210, 300, 16, 24, (u8*)lcd_buffer);
		}
		
		delay_ms(10);
    }
}

