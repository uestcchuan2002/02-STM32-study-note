#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "input_capture.h"

char lcd_buffer[32];

int main(void)
{
	long long temp = 0;

    HAL_Init();                      	// 初始化HAL
    Stm32_Clock_Init(336, 8, 2, 7);  	// 设置时钟,168Mhz
    delay_init(168);                 	// 初始化延时函
    uart_init(115200);               	// 初始化USART
    LED_Init();                      	// 初始化LED
    KEY_Init();                      	// 初始化KEY
    LCD_Init();                      	// 初始化LCD
   	
    TIM5_CH1_Cap_Init(0xFFFFFFFF, 84 - 1);	// 以1MHZ的频率计数 
	
    POINT_COLOR = RED;
    LCD_ShowString(30, 10, 200, 16, 32, (u8*)"Apollo STM32F4/F7");
    LCD_ShowString(30, 50, 300, 16, 32, (u8*)"INPUT CAPTURE");
    LCD_ShowString(30, 90, 200, 16, 32, (u8*)"ATOM@ALIENTEK");
    LCD_ShowString(30, 130, 200, 16, 32, (u8*)"2026/6/15");
    LCD_ShowString(30, 170, 200, 16, 32, (u8*)"author: chuan"); 
	
	
    POINT_COLOR = BLUE; // 设置字体为蓝
   
    while (1)
    {
		delay_ms(10);
		if (TIM5CH1_CAPTURE_STA & 0x80) {
			temp = TIM5CH1_CAPTURE_STA & 0x3F;
			temp = temp * 0xFFFFFFFF;
			temp += TIM5CH1_CAPTURE_VAL;
			printf("HIGH:%lld us\r\n", temp);
			TIM5CH1_CAPTURE_STA = 0;
		}
    }
}



