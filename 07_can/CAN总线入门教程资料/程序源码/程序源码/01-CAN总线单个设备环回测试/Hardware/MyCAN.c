#include "stm32f10x.h"                  // Device header

/**
 * can外设初始化
 */
void MyCAN_Init(void)
{
    /* 1.开启GPIO的时钟和CAN外设时钟，CAN位于APB1总线上 */
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
    }
	
	/* 2.配置GPIO引脚 */
    {
        /* 采用的复用推挽输出 */
	    GPIO_InitTypeDef GPIO_InitStructure;
	    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	    GPIO_Init(GPIOA, &GPIO_InitStructure);
        /* 上拉输入，因为默认状态下位高电平 */
	    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	    GPIO_Init(GPIOA, &GPIO_InitStructure);
    }

    /* 3.配置CAN寄存器 */
    {
        CAN_InitTypeDef CAN_InitStructure;
        /* can的模式：回环模式 */
	    CAN_InitStructure.CAN_Mode = CAN_Mode_LoopBack;
        /* 预分频->波特率，F103的APB1设置为36MHz */
	    CAN_InitStructure.CAN_Prescaler = 48;		//波特率 = 36M / 48 / (1 + 2 + 3) = 125K
        /* 位特性的设置 */
	    CAN_InitStructure.CAN_BS1 = CAN_BS1_2tq;
	    CAN_InitStructure.CAN_BS2 = CAN_BS2_3tq;
	    CAN_InitStructure.CAN_SJW = CAN_SJW_2tq;
        /* 不自动重传：允许自动重传 Not AutoMatic Retrasmission */
	    CAN_InitStructure.CAN_NART = DISABLE;
        /* 发送FIFO优先级：ID小的先发送 Transmit FIFI priority */
	    CAN_InitStructure.CAN_TXFP = DISABLE;
        /* Recive FIFO Lock Mode */
	    CAN_InitStructure.CAN_RFLM = DISABLE;
        /* Automatic Wake Up Mode */
	    CAN_InitStructure.CAN_AWUM = DISABLE;
        /* Time Triger Conter Mode */
	    CAN_InitStructure.CAN_TTCM = DISABLE;
        /* Automatic Bus Off Mode */
	    CAN_InitStructure.CAN_ABOM = DISABLE;
	    CAN_Init(CAN1, &CAN_InitStructure);
    }
	
	/* can外设过滤器配置 */
    {
        CAN_FilterInitTypeDef CAN_FilterInitStructure;
        /* 决定哪个过滤器 */
	    CAN_FilterInitStructure.CAN_FilterNumber = 0;
        /* 过滤器id寄存器写入 */
	    CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
	    CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
        /* 过滤器mask寄存器写入 */
	    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
	    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
        /* 过滤器为32位模式 */
	    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
        /* 过滤器为屏蔽模式 */
	    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
        /* 过滤结果到FIFO0 */
	    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
        /* 开启此过滤器 */
	    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	    CAN_FilterInit(&CAN_FilterInitStructure);
    }
	
}

/**
 * can外设发送数据
 */
void MyCAN_Transmit(uint32_t ID, uint8_t Length, uint8_t *Data)
{
	CanTxMsg TxMessage;
	TxMessage.StdId = ID;
	TxMessage.ExtId = ID;
	TxMessage.IDE = CAN_Id_Standard;		//CAN_ID_STD
	TxMessage.RTR = CAN_RTR_Data;
	TxMessage.DLC = Length;
	for (uint8_t i = 0; i < Length; i ++)
	{
		TxMessage.Data[i] = Data[i];
	}
	
    /* 发送数据，返回发送的邮箱 */
	uint8_t TransmitMailbox = CAN_Transmit(CAN1, &TxMessage);
	
	uint32_t Timeout = 0;
    /* 等待发送完成 */
	while (CAN_TransmitStatus(CAN1, TransmitMailbox) != CAN_TxStatus_Ok)
	{
		Timeout ++;
		if (Timeout > 100000)
		{
			break;
		}
	}
}

/**
 * 检查是否有接收信号
 */
uint8_t MyCAN_ReceiveFlag(void)
{
	if (CAN_MessagePending(CAN1, CAN_FIFO0) > 0)
	{
		return 1;
	}
	return 0;
}

/**
 * can外设接收数据
 */
void MyCAN_Receive(uint32_t *ID, uint8_t *Length, uint8_t *Data)
{
	CanRxMsg RxMessage;
	CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
	
	if (RxMessage.IDE == CAN_Id_Standard)
	{
		*ID = RxMessage.StdId;
	}
	else
	{
		*ID = RxMessage.ExtId;
	}
	
	if (RxMessage.RTR == CAN_RTR_Data)
	{
		*Length = RxMessage.DLC;
		for (uint8_t i = 0; i < *Length; i ++)
		{
			Data[i] = RxMessage.Data[i];
		}
	}
	else
	{
		//...
	}
}
