/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "stdarg.h"		  //包含需要的头文件 
#include "string.h"        //包含需要的头文件

#define Rx_Max    1024  	 
extern uint8_t		  Rx_Flag;
extern uint16_t		Rx_Len;
extern uint8_t		  Rx_Buf[Rx_Max];	
#define USART1_RX_ENABLE     0                 //是否开启接收功能  1：开启  0：关闭
#define USART1_TXBUFF_SIZE   256               //定义串口1 发送缓冲区大小 256字节
#define USART1_RXBUFF_SIZE   256               //定义串口1 接收缓冲区大小 256字节

/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);

/* USER CODE BEGIN Prototypes */
void u1_printf(char* fmt,...); 
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

