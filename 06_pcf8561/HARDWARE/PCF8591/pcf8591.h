#ifndef _PCF8591_H
#define _PCF8591_H

#include <stdint.h>
#include "myiic.h"

/*********************************************/
//数据类型定义部分
//定义结构体变量等
/********************************************/

typedef struct {
	uint16_t year;
	uint8_t mon;
	uint8_t day;
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
	uint8_t week;
}sTime;

/*********************************************/
//预编译部分
/********************************************/

/*
此处定义全局变量及函数
*/
extern sTime CurTime;

/*********************************************/
//子函数引用声明部分
/********************************************/

#define PCF8563_Check_Data                       (uint8_t)0x55  //检测用，可用其他数值

#define PCF8563_ADDR 							 (uint8_t)0x51 	//7位地址
#define PCF8563_Write                            (uint8_t)0xa2  //写命令
#define PCF8563_Read                             (uint8_t)0xa3  //读命令，或者用（PCF8563_Write + 1）

//
//电源复位功能
//
#define PCF8563_PowerResetEnable                 (uint8_t)0x08
#define PCF8563_PowerResetDisable                (uint8_t)0x09

//
//世纪位操作定义
//
#define PCF_Century_SetBitC                      (uint8_t)0x80

#define PCF_Century_19xx                         (uint8_t)0x03
#define PCF_Century_20xx                         (uint8_t)0x04

//
//数据格式
//
#define PCF_Format_BIN                           (uint8_t)0x01
#define PCF_Format_BCD                           (uint8_t)0x02

//
//设置PCF8563模式用
//
#define PCF_Mode_Normal                          (uint8_t)0x05

/******************************************************************************
                             参数寄存器地址宏定义
******************************************************************************/

#define PCF8563_Address_Control_Status_1         (uint8_t)0x00  //控制/状态寄存器1
#define PCF8563_Address_Control_Status_2         (uint8_t)0x01  //控制/状态寄存器2


/******************************************************************************
                               参数屏蔽宏定义
******************************************************************************/

#define PCF8563_Shield_Control_Status_1          (uint8_t)0xa8
#define PCF8563_Shield_Control_Status_2          (uint8_t)0x1f

#define PCF8563_Shield_Seconds                   (uint8_t)0x7f
#define PCF8563_Shield_Minutes                   (uint8_t)0x7f
#define PCF8563_Shield_Hours                     (uint8_t)0x3f

#define PCF8563_Shield_Days                      (uint8_t)0x3f
#define PCF8563_Shield_WeekDays                  (uint8_t)0x07
#define PCF8563_Shield_Months_Century            (uint8_t)0x1f
#define PCF8563_Shield_Years                     (uint8_t)0xff

/******************************************************************************
                                 参数宏定义
******************************************************************************/
#define PCF8563_Address_Years                    (uint8_t)0x08  //年
#define PCF8563_Address_Months                   (uint8_t)0x07  //月
#define PCF8563_Address_Days                     (uint8_t)0x05  //日
#define PCF8563_Address_WeekDays                 (uint8_t)0x06  //星期
#define PCF8563_Address_Hours                    (uint8_t)0x04  //小时
#define PCF8563_Address_Minutes                  (uint8_t)0x03  //分钟
#define PCF8563_Address_Seconds                  (uint8_t)0x02  //秒
//控制/状态寄存器1 --> 0x00
//
#define PCF_Control_Status_NormalMode            (uint8_t)(~(1<<7))  //普通模式
#define PCF_Control_Status_EXT_CLKMode           (uint8_t)(1<<7)     //EXT_CLK测试模式
#define PCF_Control_ChipRuns                     (uint8_t)(~(1<<5))  //芯片运行
#define PCF_Control_ChipStop                     (uint8_t)(1<<5)     //芯片停止运行，所有芯片分频器异步置逻辑0
#define PCF_Control_TestcClose                   (uint8_t)(~(1<<3))  //电源复位功能失效（普通模式时置逻辑0）
#define PCF_Control_TestcOpen                    (uint8_t)(1<<3)     //电源复位功能有效
//秒寄存器 --> 0x02
//
#define PCF_Accuracy_ClockNo                     (uint8_t)(1<<7)     //不保证准确的时钟/日历数据
#define PCF_Accuracy_ClockYes                    (uint8_t)(~(1<<7))  //保证准确的时钟/日历数据


/******************************************************************************
                                  外部功能函数                      
******************************************************************************/

void	PCF8563_Write_Byte                 (uint8_t REG_ADD, uint8_t dat);  //PCF8563某寄存器写入一个字节数据
uint8_t	PCF8563_Read_Byte                  (uint8_t REG_ADD);  //PCF8563某寄存器读取一个字节数据
void	PCF8563_Write_nByte                (uint8_t REG_ADD, uint8_t num, uint8_t *pBuff);  //PCF8563写入多组数据
void	PCF8563_Read_nByte                 (uint8_t REG_ADD, uint8_t num, uint8_t *pBuff);  //PCF8563读取多组数据

void SetRealTime(sTime *time);  //PCF8563写入时间信息
void GetRealTime(sTime *time); //PCF8563读取时间信息
void PCF8563Init(void);



#endif


