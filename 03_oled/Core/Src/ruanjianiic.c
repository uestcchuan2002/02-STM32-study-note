#include "ruanjianiic.h"
#include "gpio.h"
#include "OLED_Font.h"

/*引脚配置*/
#define SCL_Set	   HAL_GPIO_WritePin(SCL_GPIO_Port, SCL_Pin, GPIO_PIN_SET)
#define SCL_ReSet	 HAL_GPIO_WritePin(SCL_GPIO_Port, SCL_Pin, GPIO_PIN_RESET)
#define SDA_Set	   HAL_GPIO_WritePin(SDA_GPIO_Port, SDA_Pin, GPIO_PIN_SET)
#define SDA_ReSet	 HAL_GPIO_WritePin(SDA_GPIO_Port, SDA_Pin, GPIO_PIN_RESET)


/**
  * @brief  I2C开始
  * @param  无
  * @retval 无
  */
void IIC_Start(void)
{
		SDA_Set;
	  SCL_Set;	
		SDA_ReSet;
		SCL_ReSet;
}

/**
  * @brief  I2C停止
  * @param  无
  * @retval 无
  */
void IIC_End(void)
{
		SDA_ReSet;
		SCL_Set;
		SDA_Set;
}

/**
  * @brief  I2C发送一个字节
  * @param  Byte 要发送的一个字节
  * @retval 无
  */
void	IIC_SendByte(uint8_t Byte)
{
	uint8_t i=0;
	for(i=0;i<8;i++)
	{
		if((Byte & (0x80>>i))!=0)
		{
			SDA_Set;
		}
		else
		{
			SDA_ReSet;
		}
		SCL_Set;
		SCL_ReSet;
	}
	//在多给一个时钟，不处理应答信号
	SCL_Set;
	SCL_ReSet;
}

/**
  * @brief  OLED写命令
  * @param  Command 要写入的命令
  * @retval 无
  */
void	OLED_WriteCommend(uint8_t commend)
{
	IIC_Start();
	IIC_SendByte(0x78);
	IIC_SendByte(0x00);
	IIC_SendByte(commend);
	IIC_End();
}

/**
	* @brief	OLED写数据
	* @param	Data 要写入的数据
	* @retval 无
	*/
void	OLED_WriteData(uint8_t Data)
{
	IIC_Start();
	IIC_SendByte(0x78);
	IIC_SendByte(0x40);
	IIC_SendByte(Data);
	IIC_End();
}

/**
  * @brief  OLED设置光标位置
  * @param  Y 以左上角为原点，向下方向的坐标，范围：0~7
  * @param  X 以左上角为原点，向右方向的坐标，范围：0~127
  * @retval 无
  */
void	OLED_SetCursor(uint8_t Y , uint8_t X)
{
	OLED_WriteCommend(0xB0 | Y);//设置y的地址
	OLED_WriteCommend(0x10 | (0xF0 & X) >> 4);//设置x地址的高八位
	OLED_WriteCommend(0x00 | (0x0F & X));//设置x地址的低八位
}

/**
  * @brief  OLED清屏
  * @param  无
  * @retval 无
  */
void OLED_Clear(void)
{
	for(uint8_t i = 0 ; i < 8;i++)
	{
		OLED_SetCursor(i, 0);
		for(uint8_t j=0;j<128;j++)
		{
			OLED_WriteData(0x00);
		}
	}
}

/**
  * @brief  OLED显示一个字符
  * @param  Line 行位置，范围：1~4
  * @param  Column 列位置，范围：1~16
  * @param  Char 要显示的一个字符，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowChar(uint8_t Line,uint8_t Column,char Char)
{
	uint8_t i;
	OLED_SetCursor((Line-1)*2, (Column-1)*8);//将光标移到上半部分
	for(i = 0;i < 8 ;i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i]);//显示上半部分内容
	}
	OLED_SetCursor((Line-1)*2+1, (Column-1)*8);//将光标移到下半部分
	for(i = 0;i < 8 ;i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i+8]);//显示下半部分内容
	}
}

/**
  * @brief  OLED显示字符串
  * @param  Line 行位置，范围：1~4
  * @param  Column 列位置，范围：1~16
  * @param  String 要显示的字符串，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowString(uint8_t Line,uint8_t Column,const char *String)
{
	for(uint8_t i = 0;String[i] != '\0';i++)
	{
		OLED_ShowChar(Line,Column+i,String[i]);
	}
}

/**
  * @brief  OLED显示图
  * @retval 无
  */
void OLED_ShowPicture(uint8_t picture)
{
	for(uint8_t i=0;i<8;i++)
	{
		OLED_SetCursor(i, 0);//将光标移到下半部分
		for(uint8_t j=0;j<128;j++)
		{
			OLED_WriteData(OLED_Picture[picture-1][j+128*i]);//显示上半部分内容
		}
	}
}


/**
  * @brief  OLED显示汉字
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~8
  * @param  HanZinum 汉字所对应的序号
  * @retval 无
  */
void OLED_ShowChinese(uint8_t Line, uint8_t Column, uint8_t HanZinum )
{
	uint8_t i;	
	OLED_SetCursor((Line-1)*2, (Column-1)*16);//将光标移到上半部分
	for(i = 0;i < 16 ;i++)
	{
		OLED_WriteData(OLED_Chinese[HanZinum-1][i]);//显示上半部分内容
	}
	OLED_SetCursor((Line-1)*2+1, (Column-1)*16);//将光标移到下半部分
	for(i = 0;i < 16 ;i++)
	{
		OLED_WriteData(OLED_Chinese[HanZinum-1][i+16]);//显示上半部分内容
	}	
}


/**
  * @brief  OLED次方函数
  * @retval 返回值等于X的Y次方
  */
uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y--)
	{
		Result *= X;
	}
	return Result;
}

/**
  * @brief  OLED显示数字（十进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~4294967295
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
  */
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
  * @brief  OLED显示数字（十进制，带符号数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：-2147483648~2147483647
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
  */
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length)
{
	uint8_t i;
	uint32_t Number1;
	if (Number >= 0)
	{
		OLED_ShowChar(Line, Column, '+');
		Number1 = Number;
	}
	else
	{
		OLED_ShowChar(Line, Column, '-');
		Number1 = -Number;
	}
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i + 1, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
  * @brief  OLED显示数字（十六进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~0xFFFFFFFF
  * @param  Length 要显示数字的长度，范围：1~8
  * @retval 无
  */
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i, SingleNumber;
	for (i = 0; i < Length; i++)							
	{
		SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
		if (SingleNumber < 10)
		{
			OLED_ShowChar(Line, Column + i, SingleNumber + '0');
		}
		else
		{
			OLED_ShowChar(Line, Column + i, SingleNumber - 10 + 'A');
		}
	}
}

/**
  * @brief  OLED显示数字（二进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~1111 1111 1111 1111
  * @param  Length 要显示数字的长度，范围：1~16
  * @retval 无
  */
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(2, Length - i - 1) % 2 + '0');
	}
}


void OLED_Init(void)
{
	uint32_t i, j;
	for (i = 0; i < 1000; i++)			//上电延时
	{
		for (j = 0; j < 1000; j++);
	}
	
	OLED_WriteCommend(0xAE);	//关闭显示
	
	OLED_WriteCommend(0xD5);	//设置显示时钟分频比/振荡器频率
	OLED_WriteCommend(0x80);
	
	OLED_WriteCommend(0xA8);	//设置多路复用率
	OLED_WriteCommend(0x3F);
	
	OLED_WriteCommend(0xD3);	//设置显示偏移
	OLED_WriteCommend(0x00);
	
	OLED_WriteCommend(0x40);	//设置显示开始行
	
	OLED_WriteCommend(0xA1);	//设置左右方向，0xA1正常 0xA0左右反置
	
	OLED_WriteCommend(0xC8);	//设置上下方向，0xC8正常 0xC0上下反置

	OLED_WriteCommend(0xDA);	//设置COM引脚硬件配置
	OLED_WriteCommend(0x12);
	
	OLED_WriteCommend(0x81);	//设置对比度控制
	OLED_WriteCommend(0xCF);

	OLED_WriteCommend(0xD9);	//设置预充电周期
	OLED_WriteCommend(0xF1);

	OLED_WriteCommend(0xDB);	//设置VCOMH取消选择级别
	OLED_WriteCommend(0x30);

	OLED_WriteCommend(0xA4);	//设置整个显示打开/关闭

	OLED_WriteCommend(0xA6);	//设置正常/倒转显示

	OLED_WriteCommend(0x8D);	//设置充电泵
	OLED_WriteCommend(0x14);

	OLED_WriteCommend(0xAF);	//开启显示
		
	OLED_Clear();				//OLED清屏
}









