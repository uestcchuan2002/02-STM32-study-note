#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "FreeRTOS.h"
#include "task.h"
#include "freertos_demo.h"
#include "exti.h"

int main(void)
{
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);//4 位抢占优先级，0 位响应(子)优先级
    delay_init(168);		//初始化延时函数
	uart_init(115200);     	//初始化串口
	LED_Init();		        //初始化LED端口
	KEY_Init();
	EXTI_Init();
	freertos_demo();

}

