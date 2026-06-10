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
#include "queue.h"

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
TaskHandle_t            Task1Task_Handler;  /* 任务句柄 */
void task1(void *pvParameters);             /* 任务函数 */

/* TASK2 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK2_PRIO      3                   /* 任务优先级 */
#define TASK2_STK_SIZE  128                 /* 任务堆栈大小 */
TaskHandle_t            Task2Task_Handler;  /* 任务句柄 */
void task2(void *pvParameters);             /* 任务函数 */

/* TASK3 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK3_PRIO      4                   /* 任务优先级 */
#define TASK3_STK_SIZE  128                 /* 任务堆栈大小 */
TaskHandle_t            Task3Task_Handler;  /* 任务句柄 */
void task3(void *pvParameters);             /* 任务函数 */

QueueHandle_t key_Handle_t;				/* 按键句柄 */
QueueHandle_t bigDate_Handle_t;			/* 大数据句柄 */
char buf[100] = {"我是一个大数据数组：123456qwerty"};
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
    
	key_Handle_t = xQueueCreate(2,sizeof(uint8_t));
	if(key_Handle_t != NULL)
	{
		printf("key_queue队列创建成功！！\r\n");
	}
	else
	{
		printf("key_queue队列创建失败！！\r\n");
	}
	
	bigDate_Handle_t = xQueueCreate(1,sizeof(char *));
	if(bigDate_Handle_t != NULL)
	{
		printf("bigDate_queue队列创建成功！！\r\n");
	}
	else
	{
		printf("bigDate_queue队列创建失败！！\r\n");
	}
	
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
    xTaskCreate((TaskFunction_t )task1,
                (const char*    )"task1",
                (uint16_t       )TASK1_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK1_PRIO,
                (TaskHandle_t*  )&Task1Task_Handler);
				
    xTaskCreate((TaskFunction_t )task2,
                (const char*    )"task2",
                (uint16_t       )TASK2_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK2_PRIO,
                (TaskHandle_t*  )&Task2Task_Handler);				

    xTaskCreate((TaskFunction_t )task3,
                (const char*    )"task3",
                (uint16_t       )TASK3_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK3_PRIO,
                (TaskHandle_t*  )&Task3Task_Handler);
				
    vTaskDelete(StartTask_Handler); /* 删除开始任务 */
    taskEXIT_CRITICAL();            /* 退出临界区 */
}

/**
 * @brief       task1 
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void task1(void *pvParameters)
{
	uint8_t key_num = 0;
	BaseType_t err = 0;
	char *m_buf =buf;
    while(1)
    {
		key_num = key_scan(0);
		if(key_num == KEY0_PRES || key_num == KEY1_PRES)
		{
			taskENTER_CRITICAL();           /* 进入临界区 */
			err = xQueueSend( key_Handle_t, &key_num, portMAX_DELAY );
			if(err == pdTRUE)
			{
				printf(" 队列上传成功！！\r\n");
			}
			else
			{
				printf(" 队列上传失败！！\r\n");
			}
			taskEXIT_CRITICAL();            /* 退出临界区 */
		}
		else if(key_num == WKUP_PRES)
		{
			taskENTER_CRITICAL();           /* 进入临界区 */
			err = xQueueSend( bigDate_Handle_t, &m_buf, portMAX_DELAY );
			if(err == pdTRUE)
			{
				printf(" 队列上传成功！！\r\n");
			}
			else
			{
				printf(" 队列上传失败！！\r\n");
			}
			taskEXIT_CRITICAL();            /* 退出临界区 */
		}
		
		vTaskDelay(10);
    }
}


/**
 * @brief       task2 
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void task2(void *pvParameters)
{
	uint8_t num = 0;
    while(1)
    {
		if(xQueueReceive( key_Handle_t, &num, portMAX_DELAY ) == pdPASS)
		{
			printf("队列读取成功，数据:%d\r\n",num);
		}
		else
		{
			printf("队列读取失败\r\n");
		}                
    }
}

/**
 * @brief       task3 
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void task3(void *pvParameters)
{
	char *m_buf ;
    while(1)
    {
		if(xQueueReceive( bigDate_Handle_t, &m_buf, portMAX_DELAY ) == pdTRUE)
		{
			printf("数据:%s\r\n", m_buf);
		}
		else
		{
			printf("队列读取失败\r\n");
		}
    }
}

