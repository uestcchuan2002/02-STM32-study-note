#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "pwm.h"

char lcd_buffer[32];

int main(void)
{
	int dir = 1;
	u16	led0_pwm_val = 0;

    HAL_Init();                      // 初始化HAL
    Stm32_Clock_Init(336, 8, 2, 7);  // 设置时钟,168Mhz
    delay_init(168);                 // 初始化延时函
    uart_init(115200);               // 初始化USART
    LED_Init();                      // 初始化LED
    KEY_Init();                      // 初始化KEY
    LCD_Init();                      // 初始化LCD
    TIM14_PWM_Init(500 - 1, 84 -1);   // PWM初始化
	
    POINT_COLOR = RED;
    LCD_ShowString(30, 10, 200, 16, 32, (u8*)"Apollo STM32F4/F7");
    LCD_ShowString(30, 50, 300, 16, 32, (u8*)"PWM OUTPUT");
    LCD_ShowString(30, 90, 200, 16, 32, (u8*)"ATOM@ALIENTEK");
    LCD_ShowString(30, 130, 200, 16, 32, (u8*)"2026/6/15");
    LCD_ShowString(30, 170, 200, 16, 32, (u8*)"author: chuan"); 
	
	
    POINT_COLOR = BLUE; // 设置字体为蓝
   
    while (1)
    {
		delay_ms(5);
		if (dir == 1) {
			led0_pwm_val++;
			if (led0_pwm_val == 500 - 1) {
				dir = 0;
			}
		} else {
			led0_pwm_val--;
			if (led0_pwm_val == 0) {
				dir = 1;
			}
		}
		TIM_SetTIM14Compare1(led0_pwm_val);
    }
}

