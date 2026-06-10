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
/*FreeRTOS*********************************************************************************************/
#include "FreeRTOS.h"
#include "task.h"

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
#define TASK2_STK_SIZE  130                 /* 任务堆栈大小 */
TaskHandle_t            Task2Task_Handler;  /* 任务句柄 */
void task2(void *pvParameters);             /* 任务函数 */

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

    while(1)
    {
		LED0_TOGGLE();
		vTaskDelay(500);
		
    }
}

char WriteBuffer[500];
/**
 * @brief       task2 
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void task2(void *pvParameters)
{
	UBaseType_t task2_num = 0;
	UBaseType_t task2_NewPriority = 5;
	UBaseType_t All_TaskNum = 0;
	UBaseType_t All_TaskNum2 = 0;
	UBaseType_t WaterMark = 0;
	TaskStatus_t * Task_StatusArray;
	TaskStatus_t * Task_StatusArray2;
	uint8_t i =0;
	
	task2_num = uxTaskPriorityGet( Task2Task_Handler );/* 获取指定任务的任务优先级 */
	printf("Task2的任务优先级为:%ld\r\n",task2_num);
	
	vTaskPrioritySet( Task2Task_Handler , task2_NewPriority );/* 修改任务的优先级 */
	task2_num = uxTaskPriorityGet( Task2Task_Handler );/* 获取指定任务的任务优先级 */
	printf("Task2的任务优先级为:%ld\r\n",task2_num);
	
	All_TaskNum = uxTaskGetNumberOfTasks( );/* 获取系统任务数量函数 */
	printf("总的系统任务数量:%ld\r\n",All_TaskNum);
	
	/* 获取系统中所有任务状态信息函数 */
	Task_StatusArray = mymalloc( SRAMIN,sizeof(TaskStatus_t)*All_TaskNum );
	All_TaskNum2 = uxTaskGetSystemState( Task_StatusArray,All_TaskNum,NULL );
	printf("总的系统任务数量:%ld\r\n",All_TaskNum2);
	printf("任务名称\t任务优先级\t任务序号\t\r\n");
	for(i=0;i<5;i++)
	{
		printf("%s\t%ld\t%ld\t\r\n",Task_StatusArray[i].pcTaskName,
		                            Task_StatusArray[i].uxCurrentPriority,
								    Task_StatusArray[i].xTaskNumber);
	}
	
	/* 获取系统中单个任务状态信息函数 */
	Task_StatusArray2 = mymalloc( SRAMIN,sizeof(TaskStatus_t) );
	vTaskGetInfo( Task2Task_Handler,Task_StatusArray2,pdFALSE,eInvalid);  
	printf("任务名称:%s\t任务优先级:%ld\t任务序号%ld\t\r\n",Task_StatusArray2[0].pcTaskName,
		                            Task_StatusArray2[0].uxCurrentPriority,
								    Task_StatusArray2[0].xTaskNumber);
	
	/* 获取当前任务的任务句柄函数 */
	TaskHandle_t   Temp_Handler;
	Temp_Handler = xTaskGetCurrentTaskHandle();
	printf("获取的任务句柄为:0x%x\r\n", (int)Temp_Handler);
	
	/* 通过任务名获取任务句柄函数 */
	Temp_Handler = xTaskGetHandle( "task2" ); 
	printf("通过任务名获取任务句柄为:0x%x\r\n", (int)Temp_Handler);
	
	/* 获取指定任务的任务堆栈历史最小剩余函数 */
	WaterMark = uxTaskGetStackHighWaterMark( Task2Task_Handler );
	printf("堆栈历史最小剩余为：%ld\r\n",WaterMark);
	
	/* 查询指定任务运行状态函数 */
	eTaskState   Temp_State;
	Temp_State = eTaskGetState(Task1Task_Handler);
	printf("任务运行状态为:%d\r\n",(int)Temp_State);
	
	/* 以“表格”的形式获取系统中任务信息函数 */
	
	vTaskList(WriteBuffer);
	printf("%s\r\n",WriteBuffer);
	
	
    while(1)
    {
		vTaskDelay(1000);
    }
}
