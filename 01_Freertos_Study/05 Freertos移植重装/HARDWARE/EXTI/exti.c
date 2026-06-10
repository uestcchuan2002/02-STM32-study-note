#include "exti.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "FreeRTOS.h"
#include "task.h"
#include "freertos_demo.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//外部中断驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2017/4/7
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//外部中断初始化
void EXTI_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
   
    __HAL_RCC_GPIOE_CLK_ENABLE();               //开启GPIOE时钟
    
   
    
    GPIO_Initure.Pin=GPIO_PIN_2; 	//PE2,3,4
    GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;     //下降沿触发
    GPIO_Initure.Pull=GPIO_PULLUP;
    HAL_GPIO_Init(GPIOE,&GPIO_Initure);
 
    
    //中断线2-PE2
    HAL_NVIC_SetPriority(EXTI2_IRQn,6,0);       //抢占优先级为2，子优先级为1
    HAL_NVIC_EnableIRQ(EXTI2_IRQn);             //使能中断线2
    
  
}

void EXTI2_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);		//调用中断处理公用函数
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    BaseType_t YieldRequired;
	
	delay_xms(10);	//消抖
    switch(GPIO_Pin)
    {
        
        case GPIO_PIN_2:
            if(KEY2==0)  //LED1翻转
            {
       YieldRequired=xTaskResumeFromISR(task1_handler);//恢复任务2
		printf("在中断恢复task1!!!\r\n");
		if(YieldRequired==pdTRUE)
		{
		/*如果函数xTaskResumeFromISR()返回值为pdTRUE，那么说明要恢复的这个
		任务的任务优先级等于或者高于正在运行的任务(被中断打断的任务),所以在
		退出中断的时候一定要进行上下文切换！*/
		portYIELD_FROM_ISR(YieldRequired);
		} 
	}
            break;
      
    }
}
