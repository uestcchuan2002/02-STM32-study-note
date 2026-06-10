#include "pcf8591.h"

uint8_t buffer[7];//数据存储缓存
sTime CurTime;

static uint8_t RTC_BinToBcd2(uint8_t BINValue)
{
    uint8_t bcdhigh = 0;

    while (BINValue >= 10)
    {
        bcdhigh++;
        BINValue -= 10;
    }

    return ((uint8_t)(bcdhigh << 4) | BINValue);
}

static uint8_t RTC_Bcd2ToBin(uint8_t BCDValue)
{
    uint8_t tmp = 0;

    tmp = ((uint8_t)(BCDValue & (uint8_t)0xF0) >> (uint8_t)0x04) * 10;
    return (tmp + (BCDValue & (uint8_t)0x0F));
}

void PCF8563_Write_Byte(uint8_t REG_ADD, uint8_t dat)
{
    IIC_Start();  
	IIC_Send_Byte(PCF8563_Write);
	IIC_Wait_Ack();
	IIC_Send_Byte(REG_ADD);
	IIC_Wait_Ack();
	IIC_Send_Byte(dat);
	IIC_Wait_Ack();
	IIC_Stop();
}

void PCF8563_Write_nByte(uint8_t REG_ADD, uint8_t num, uint8_t *pBuff)
{
	int i;
	IIC_Start();
    IIC_Send_Byte(PCF8563_Write);           // 写地址
    IIC_Wait_Ack();
    IIC_Send_Byte(REG_ADD);        			// 起始寄存器地址
    IIC_Wait_Ack();
    
    for (i = 0; i < num; i++) {
        IIC_Send_Byte(pBuff[i]);   			// 连续写数据（PCF8563 自动地址递增）
        IIC_Wait_Ack();
    }
    
    IIC_Stop();
}


uint8_t PCF8563_Read_Byte(uint8_t REG_ADD)
{
	uint8_t ReData;
	IIC_Start();
	IIC_Send_Byte(PCF8563_Write);
	IIC_Wait_Ack();
	IIC_Send_Byte(REG_ADD);
	IIC_Wait_Ack();	    
	IIC_Start(); 
	IIC_Send_Byte(PCF8563_Read);           //进入接收模式	
	IIC_Wait_Ack();
	ReData = IIC_Read_Byte(0);
	IIC_Stop();		//产生一个停止条件	    
	return ReData;
}

void PCF8563_Read_nByte(uint8_t REG_ADD, uint8_t num, uint8_t *pBuff)
{
	int i;
    IIC_Start();
    IIC_Send_Byte(PCF8563_Write);
    IIC_Wait_Ack();
    IIC_Send_Byte(REG_ADD);
    IIC_Wait_Ack();	    
    IIC_Start();
    IIC_Send_Byte(PCF8563_Read);
    IIC_Wait_Ack();

    for (i = 0; i < num; i++) {
        // 如果是最后一个字节，发送 NACK；否则发送 ACK
        pBuff[i] = IIC_Read_Byte((i == num - 1) ? 0 : 1);
    }

    IIC_Stop();
}

void SetRealTime(sTime* time)
{
    buffer[0] = RTC_BinToBcd2(time->sec);
    buffer[1] = RTC_BinToBcd2(time->min);
    buffer[2] = RTC_BinToBcd2(time->hour);
    buffer[3] = RTC_BinToBcd2(time->day);
    buffer[4] = time->week; // Weekday is binary (0-6)
    buffer[5] = RTC_BinToBcd2(time->mon);

    buffer[6] = RTC_BinToBcd2(time->year); // 假设 year 是 0-99

    PCF8563_Write_nByte(PCF8563_Address_Seconds, 7, buffer);
}

void GetRealTime(sTime* time)
{
    PCF8563_Read_nByte(PCF8563_Address_Seconds, 7, buffer);

    time->sec  = RTC_Bcd2ToBin(buffer[0] & PCF8563_Shield_Seconds);
    time->min  = RTC_Bcd2ToBin(buffer[1] & PCF8563_Shield_Minutes);
    time->hour = RTC_Bcd2ToBin(buffer[2] & PCF8563_Shield_Hours);
    time->day  = RTC_Bcd2ToBin(buffer[3] & PCF8563_Shield_Days);
    time->week = buffer[4] & PCF8563_Shield_WeekDays; // already binary
    time->mon  = RTC_Bcd2ToBin(buffer[5] & PCF8563_Shield_Months_Century);
    time->year = RTC_Bcd2ToBin(buffer[6] & PCF8563_Shield_Years);
}

void PCF8563Init(void)
{
	sTime time;
	sTime InitTime = {25, 12, 25, 20, 1, 0, 1}; //22年1月10日12时00分00秒星期一
	PCF8563_Write_Byte(PCF8563_Address_Control_Status_1, 0x00);  
	PCF8563_Write_Byte(PCF8563_Address_Control_Status_2, 0x00);  
    
	GetRealTime(&time);
	
	if (time.year < 10 || time.year > 2039) {
        // 时间无效（可能是首次上电、电池耗尽、寄存器乱码）
        
        SetRealTime(&InitTime); // 只在此时写入
    }
	
}

