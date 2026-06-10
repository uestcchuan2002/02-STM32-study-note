#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "freertos_demo.h"


int main(void)
{ 
	HAL_NVIC_SetPriorityGrouping(4);//设置系统中断优先级分组4
	delay_init(168);		//初始化延时函数
	uart_init(115200);     	//初始化串口
	LED_Init();		        //初始化LED端口
	KEY_Init();
	freertos_demo();
}

 


