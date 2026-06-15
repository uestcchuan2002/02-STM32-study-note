#include "myiic.h"
#include "delay.h"

//  软件IIC初始化 —— 已改为【开漏输出】
void IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    IIC_GPIO_RCC_ENABLE;

    GPIO_Initure.Pin = IIC_SCL_Pin;
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_OD;  //    漏输出（关键修改）
    GPIO_Initure.Pull = GPIO_NOPULL;          //    外部已有上拉，内部不上拉
    GPIO_Initure.Speed = GPIO_SPEED_FAST;
    HAL_GPIO_Init(IIC_SCL_Port, &GPIO_Initure);

    GPIO_Initure.Pin = IIC_SDA_Pin;
    HAL_GPIO_Init(IIC_SDA_Port, &GPIO_Initure);

    IIC_SDA = 1;
    IIC_SCL = 1;
}

/*
    起始信号：通知开始通信了
    本质是在SCL为高电平时，SDA产生一个下降沿信号
*/
void IIC_Start(void)
{
    IIC_SDA = 1;
    IIC_SCL = 1;
    delay_us(4);
    IIC_SDA = 0;
    delay_us(4);
    IIC_SCL = 0;
}

/*
    停止信号：通知通信结束
    本质是SCL处于高电平时，SDA产生一个上升沿信号
*/
void IIC_Stop(void)
{
    IIC_SDA = 0;
    delay_us(4);
    IIC_SCL = 1;
    IIC_SDA = 1;
    delay_us(4);
}

/*
    IIC 每传输 8 bit 后，第 9 个时钟用于应答
    ACK：SDA 被接收方拉低
    return 0:应答成功   1:应答失败
*/
u8 IIC_Wait_Ack(void)
{
    u8 ucErrTime = 0;
    IIC_SDA = 1;  // 开漏模式下，写1=释放总线，无需切换输入模式
    delay_us(1);
    IIC_SCL = 1;
    delay_us(1);

    while(READ_SDA)
    {
        ucErrTime++;
        if(ucErrTime > 250)
        {
            IIC_Stop();
            return 1;
        }
    }
    IIC_SCL = 0;
    return 0;
}

// ACK应答
void IIC_Ack(void)
{
    IIC_SCL = 0;
    IIC_SDA = 0;
    delay_us(2);
    IIC_SCL = 1;
    delay_us(2);
    IIC_SCL = 0;
}

// NACK应答
void IIC_NAck(void)
{
    IIC_SCL = 0;
    IIC_SDA = 1;
    delay_us(2);
    IIC_SCL = 1;
    delay_us(2);
    IIC_SCL = 0;
}

// 发送一个字节
void IIC_Send_Byte(u8 txd)
{
    int i;
    IIC_SCL = 0;
    for (i = 0; i < 8; i++) 
    {
        // 1. 取出当前最高位 bit7，放到 SDA 上
        IIC_SDA = (txd & 0x80) >> 7;

        // 2. 数据左移一位，下次发下一位
        txd = txd << 1;
        // 延时稳定
        delay_us(2);

        // 3. 时钟拉高 → 从机【读取】SDA数据
        IIC_SCL = 1;
        delay_us(2);

        // 4. 时钟拉低 → 准备下一位数据
        IIC_SCL = 0;
        delay_us(2);
    }
}

// 读取一个字节
u8 IIC_Read_Byte(unsigned char ack) 
{
    int i = 0;
    u8 recevive = 0;

    // 1. 释放SDA总线
    IIC_SDA = 1;

    // 2. 循环8次接收一个字节
    for (; i < 8; i++) {
        // 2. 拉低时钟，让从机修改数据位
        IIC_SCL = 0;
        delay_us(2);

        // 3. 拉高时钟，读取SDA数据
        IIC_SCL = 1;

        // 4. 左移一位recevive,准备接收新的bit
        recevive <<= 1;

        // 5.读取数据
        if (READ_SDA == 1) {
            recevive++;
        } else {
            // 不变
        }
        delay_us(1);
    }

    // 6. 读完8位后，主机发送应答位
    if (ack == 0) {
        IIC_NAck();
    } else {
        IIC_Ack();
    }

    return recevive;
}
