#include "24cxx.h"
#include "delay.h"

// 初始化IIC接口
void AT24CXX_Init(void)
{
	IIC_Init();//IIC初始化
}

/*
    在AT24CXX指定地址读出一个数据
    ReadAddr:开始读数的地址  
    返回值  :读到的数据
*/
u8 AT24CXX_ReadOneByte(u16 ReadAddr)
{
    u8 temp = 0;
    // 1. 起始信号
    IIC_Start();
    // 2. 从机地址 注意24c02容量地址规则
    if (EE_TYPE > AT24C16) {
        // 大容量
        IIC_Send_Byte(0xA0);
        IIC_Wait_Ack();
        IIC_Send_Byte(ReadAddr >> 8);
    } else {
        // 小容量
        IIC_Send_Byte(0xA0 + ((ReadAddr / 256) << 1));
    }
    IIC_Wait_Ack();
    // 3.地址第八位发送
    IIC_Send_Byte(ReadAddr % 256);
    IIC_Wait_Ack();
    // 4. 注意发送转为接收需要重新发送一个开始信号
    IIC_Start();
    // 5. 进入接收模式
    IIC_Send_Byte(0xA1);
    IIC_Wait_Ack();
    // 6. 接收数据
    temp = IIC_Read_Byte(0);
    // 7. 停止信号
    IIC_Stop();

    return temp;
}

/*
    在AT24CXX指定地址写入一个数据
    WriteAddr  :写入数据的目的地址    
    DataToWrite:要写入的数据
*/
void AT24CXX_WriteOneByte(u16 WriteAddr, u8 DataToWrite)
{				   	  	    																 
    IIC_Start();
    if (EE_TYPE > AT24C16) {
        // 大容量
        IIC_Send_Byte(0xA0);
        IIC_Wait_Ack();
        IIC_Send_Byte(WriteAddr >> 8);
    } else {
        // 小容量
        IIC_Send_Byte(0xA0 + ((WriteAddr / 256) << 1));
    }
    IIC_Wait_Ack();
    IIC_Send_Byte(WriteAddr % 256);
    IIC_Wait_Ack();
    IIC_Send_Byte(DataToWrite);
    IIC_Wait_Ack();
    IIC_Stop();

    delay_ms(10);
}

/*
    在AT24CXX里面的指定地址开始写入长度为Len的数据
    该函数用于写入16bit或者32bit的数据.
    WriteAddr  :开始写入的地址  
    DataToWrite:数据数组首地址
    Len        :要写入数据的长度2,4
*/
void AT24CXX_WriteLenByte(u16 WriteAddr, u32 DataToWrite, u8 Len)
{  	
	u8 t;
	for(t = 0; t < Len; t++)
	{
		AT24CXX_WriteOneByte(WriteAddr + t, (DataToWrite >> (8 * t)) & 0xff);
	}												    
}


/*
    在AT24CXX里面的指定地址开始读出长度为Len的数据
    该函数用于读出16bit或者32bit的数据.
    ReadAddr    :开始读出的地址 
    返回值      :数据
    Len         :要读出数据的长度2, 4
*/
u32 AT24CXX_ReadLenByte(u16 ReadAddr, u8 Len)
{
    u32 temp = 0;
    u8 i;
    for (i = 0; i < Len; i++) {
        temp = temp << 8;
        temp += AT24CXX_ReadOneByte(ReadAddr + Len - 1 - i);
    }

    return temp;
}


/*
    检查AT24CXX是否正常
    这里用了24XX的最后一个地址(255)来存储标志字.
    如果用其他24C系列,这个地址要修改
    返回1:检测失败
    返回0:检测成功
*/
u8 AT24CXX_Check(void)
{
    u8 temp;
    temp = AT24CXX_ReadOneByte(255);
    if (temp == 0x55) {
        return 0;
    } else {
        AT24CXX_WriteOneByte(255, 0x55);
        delay_ms(10);
        temp = AT24CXX_ReadOneByte(255);
        if (temp == 0x55) 
            return 0;
    }
    return 1;
}

/*
    在AT24CXX里面的指定地址开始读出指定个数的数据
    ReadAddr :开始读出的地址 对24c02为0~255
    pBuffer  :数据数组首地址
    NumToRead:要读出数据的个数
*/
void AT24CXX_Read(u16 ReadAddr, u8 *pBuffer, u16 NumToRead)
{
    while (NumToRead) {
        // 1. 读取单个字节数据
        *pBuffer = AT24CXX_ReadOneByte(ReadAddr);
        // 2. 更新读取地址和存储地址
        pBuffer++;
        ReadAddr++;
        NumToRead--;
    }
}  


/*
    在AT24CXX里面的指定地址开始写入指定个数的数据
    WriteAddr :开始写入的地址 对24c02为0~255
    pBuffer   :数据数组首地址
    NumToWrite:要写入数据的个数
*/
void AT24CXX_Write(u16 WriteAddr, u8 *pBuffer, u16 NumToWrite)
{
	while(NumToWrite--)
	{
		AT24CXX_WriteOneByte(WriteAddr, *pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}
