/**
 ****************************************************************************************************
 * @file        freertos.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.4
 * @date        2022-01-04
 * @brief       FreeRTOS 移植实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 探索者F407开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "freertos_demo.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/TIMER/btim.h"
#include "./SYSTEM/delay/delay.h"
#include "./MALLOC/malloc.h"
#include "./BSP/KEY/key.h"
/*FreeRTOS*********************************************************************************************/
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/******************************************************************************************************/
/*FreeRTOS配置*/

/* START_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define START_TASK_PRIO 1                   /* 任务优先级 */
#define START_STK_SIZE  128                 /* 任务堆栈大小 */
TaskHandle_t            StartTask_Handler;  /* 任务句柄 */
void start_task(void *pvParameters);        /* 任务函数 */

/* TASK1 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK1_PRIO      2                   /* 任务优先级 */
#define TASK1_STK_SIZE  128                 /* 任务堆栈大小 */
TaskHandle_t            low_task_Handler;  /* 任务句柄 */
void low_task(void *pvParameters);             /* 任务函数 */

/* TASK2 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK2_PRIO      3                   /* 任务优先级 */
#define TASK2_STK_SIZE  128                 /* 任务堆栈大小 */
TaskHandle_t         middle_task_Handler;  /* 任务句柄 */
void middle_task(void *pvParameters);             /* 任务函数 */

/* TASK3 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK3_PRIO      4                   /* 任务优先级 */
#define TASK3_STK_SIZE  128                 /* 任务堆栈大小 */
TaskHandle_t      high_task_Handler;  /* 任务句柄 */
void high_task(void *pvParameters);             /* 任务函数 */


SemaphoreHandle_t   m_Handle_t;


/******************************************************************************************************/

/* LCD刷屏时使用的颜色 */
uint16_t lcd_discolor[11] = {WHITE, BLACK, BLUE, RED,
                             MAGENTA, GREEN, CYAN, YELLOW,
                             BROWN, BRRED, GRAY};

									 					 
/**
 * @brief       FreeRTOS例程入口函数
 * @param       无
 * @retval      无
 */
void freertos_demo(void)
{
    lcd_show_string(10, 10, 220, 32, 32, "STM32", RED);
    lcd_show_string(10, 47, 220, 24, 24, "FreeRTOS Porting", RED);
    lcd_show_string(10, 76, 220, 16, 16, "ATOM@ALIENTEK", RED);
    

	m_Handle_t = xSemaphoreCreateBinary();
	if(m_Handle_t != NULL)
	{
		printf("二值信号量创建成功！！\r\n");
	}
	else
	{
		printf("二值信号量创建失败！！\r\n");
	}
	
	xSemaphoreGive( m_Handle_t ); // 释放二值信号量
	
    xTaskCreate((TaskFunction_t )start_task,            /* 任务函数 */
                (const char*    )"start_task",          /* 任务名称 */
                (uint16_t       )START_STK_SIZE,        /* 任务堆栈大小 */
                (void*          )NULL,                  /* 传入给任务函数的参数 */
                (UBaseType_t    )START_TASK_PRIO,       /* 任务优先级 */
                (TaskHandle_t*  )&StartTask_Handler);   /* 任务句柄 */
    vTaskStartScheduler();
}

/**
 * @brief       start_task
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           /* 进入临界区 */

    /* 创建任务1 */
    xTaskCreate((TaskFunction_t )low_task,
                (const char*    )"low_task",
                (uint16_t       )TASK1_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK1_PRIO,
                (TaskHandle_t*  )&low_task_Handler);
				
    xTaskCreate((TaskFunction_t )middle_task,
                (const char*    )"middle_task",
                (uint16_t       )TASK2_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK2_PRIO,
                (TaskHandle_t*  )&middle_task_Handler);		

    xTaskCreate((TaskFunction_t )high_task,
                (const char*    )"high_task",
                (uint16_t       )TASK3_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK3_PRIO,
                (TaskHandle_t*  )&high_task_Handler);				
				
    vTaskDelete(StartTask_Handler); /* 删除开始任务 */
    taskEXIT_CRITICAL();            /* 退出临界区 */
}

/**
 * @brief       task1 低优先级任务
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void low_task(void *pvParameters)
{

    while(1)
    {
		printf("low_task获取信号量！！！\r\n");
		xSemaphoreTake( m_Handle_t, portMAX_DELAY );
		printf("low_task正在运行！！！\r\n");
		delay_ms(3000);
		printf("low_task释放信号量！！！\r\n");
		xSemaphoreGive( m_Handle_t ); // 释放二值信号量
		vTaskDelay(1000);
    }
}


/**
 * @brief       task2 中优先级任务
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void middle_task(void *pvParameters)
{
	
    while(1)
    {
		printf("中优先级任务正在运行！！！\r\n");
		vTaskDelay(1000);
		
    }
}

/**
 * @brief       task3 高优先级任务
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void high_task(void *pvParameters)
{

    while(1)
    {
		printf("high_task获取信号量！！！\r\n");
		xSemaphoreTake( m_Handle_t, portMAX_DELAY );
		printf("high_task正在运行！！！\r\n");
		delay_ms(1000);
		printf("high_task释放信号量！！！\r\n");
		xSemaphoreGive( m_Handle_t ); // 释放二值信号量
		vTaskDelay(1000);
    }
}



