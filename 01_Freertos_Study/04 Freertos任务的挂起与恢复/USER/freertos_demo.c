#include "freertos_demo.h"

/*******************************************************************************************************/
/* freeRTOS任务配置 */

/***
* 任务创建任务配置
* 包括: 任务优先级 堆栈大小 任务句柄 创建任务
*/
#define START_TASK_STACK_SIZE		128   //偏大的去定义堆栈的大小
#define START_TASK_PRIO					1			//开始任务的任务优先级
TaskHandle_t	start_task_handler;			//开始任务的任务句柄
void start_task( void * pvParameters );

/***
* TASK1任务配置
* 包括: 任务优先级 堆栈大小 任务句柄 创建任务
*/
#define TASK1_STACK_SIZE		128   //偏大的去定义堆栈的大小
#define TASK1_PRIO					2			//task1的任务优先级
TaskHandle_t	task1_handler;			//任务句柄
void task1( void * pvParameters );

/***
* TASK2任务配置
* 包括: 任务优先级 堆栈大小 任务句柄 创建任务
*/
#define TASK2_STACK_SIZE		128   //偏大的去定义堆栈的大小
#define TASK2_PRIO					3			//task2的任务优先级
TaskHandle_t	task2_handler;			//任务句柄
void task2( void * pvParameters );

/***
* TASK3任务配置
* 包括: 任务优先级 堆栈大小 任务句柄 创建任务
*/
#define TASK3_STACK_SIZE		128   //偏大的去定义堆栈的大小
#define TASK3_PRIO					4			//task3的任务优先级
TaskHandle_t	task3_handler;			//任务句柄
void task3( void * pvParameters );

void freertos_demo(void)
{        
	  xTaskCreate((TaskFunction_t        ) 	start_task,
                (char *                ) 	"start_task",
                (uint16_t              ) 	START_TASK_STACK_SIZE,
                (void *                ) 	NULL,
                (UBaseType_t           ) 	START_TASK_PRIO,
                (TaskHandle_t *        ) 	&start_task_handler);
    vTaskStartScheduler();          //开启任务调度
}

void start_task( void * pvParameters )
{
		 taskENTER_CRITICAL();        /* 进入临界区，创建任务过程我们必须保证在临界区 */
		 xTaskCreate((TaskFunction_t        ) task1,
                (char *                ) 	"task1",
                (uint16_t              ) 	TASK1_STACK_SIZE,
                (void *                ) 	NULL,
                (UBaseType_t           ) 	TASK1_PRIO,
                (TaskHandle_t *        ) 	&task1_handler);
								
		 xTaskCreate((TaskFunction_t        ) task2,
                (char *                ) 	"task2",
                (uint16_t              ) 	TASK2_STACK_SIZE,
                (void *                ) 	NULL,
                (UBaseType_t           ) 	TASK2_PRIO,
                (TaskHandle_t *        ) 	&task2_handler);

		 xTaskCreate((TaskFunction_t        ) task3,
                (char *                ) 	"task3",
                (uint16_t              ) 	TASK3_STACK_SIZE,
                (void *                ) 	NULL,
                (UBaseType_t           ) 	TASK3_PRIO,
                (TaskHandle_t *        ) 	&task3_handler);		
		 
		 vTaskDelete(NULL);
		 taskEXIT_CRITICAL();            /* 退出临界区 */
}

/* 任务一，实现LED0每500ms翻转一次*/
void task1( void * pvParameters )
{
		while(1)
		{
			printf("task1正在运行！！！\r\n");
			LED0 = ~LED0;
			vTaskDelay(1000);
		}
}

/* 任务二，实现LED1每500ms翻转一次*/
void task2( void * pvParameters )
{
		while(1)
		{
			printf("task2正在运行！！！\r\n");
			LED1 = ~LED1;
			vTaskDelay(1000);			
		}	
}

/* 任务三，判断按键KEY0，按下KEY0挂起task1，按下KEY1恢复task1 */
void task3( void * pvParameters )
{
		uint8_t key = 0;
		while(1)
		{
			//printf("task3正在运行！！！\r\n");
			key = KEY_Scan(0);
			if(key == KEY0_PRES)
			{
				printf("task1挂起！！！\r\n");
				vTaskSuspend(task1_handler);
			}
			else if(key == KEY1_PRES)
			{
				printf("在任务中恢复task1!!!\r\n");
				vTaskResume(task1_handler);
			}
			vTaskDelay(10);	
		}	
}








