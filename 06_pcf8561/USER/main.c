#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "usmart.h"
#include "24cxx.h"
#include "pcf8591.h"


//要写入到24c02的字符串数组
const u8 TEXT_Buffer[]={"Explorer STM32F4 IIC TEST"};
#define SIZE sizeof(TEXT_Buffer)


int main(void)
{
    HAL_Init();                   	//初始化HAL库    
    Stm32_Clock_Init(336,8,2,7);  	//设置时钟,168Mhz
	delay_init(168);               	//初始化延时函数
	uart_init(115200);             	//初始化USART
	usmart_dev.init(84); 		    //初始化USMART
	LED_Init();						//初始化LED	
	KEY_Init();						//初始化KEY
 	LCD_Init();           			//初始化LCD
	
	AT24CXX_Init();				    //初始化IIC
	
	PCF8563Init();
	
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,(u8 *)"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,(u8 *)"IIC TEST");	
	LCD_ShowString(30,90,200,16,16,(u8 *)"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,(u8 *)"2016/1/13");	 
	LCD_ShowString(30,130,200,16,16,(u8 *)"KEY1:Write  KEY0:Read");	//显示提示信息	  
	
	LCD_ShowString(30,150,200,16,16,(u8 *)"PCF8563 Ready!");    
	POINT_COLOR=BLUE;//设置字体为蓝色	  
	while(1)
	{
		GetRealTime(&CurTime);//读取时间
		printf("年:%d 月:%d 日:%d 时:%d 分:%d 秒:%d 星期%d\r\n", CurTime.year, (int)CurTime.mon, (int)CurTime.day, (int)CurTime.hour, (int)CurTime.min, (int)CurTime.sec, (int)CurTime.week);
		delay_ms(2000);

		LED0=!LED0;//提示系统正在运行		   
	} 
}

