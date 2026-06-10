#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "MyI2C.h"
#include "MPU6050.h"
#include "Timer.h"

//串口1初始化
void usart1_init(u32 bound) {
	
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);	// GPIOA时钟,串口1时钟使能
	
	USART_DeInit(USART1);  //复位串口1
	//USART1_TX   PA9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA9
	
	//USART1_RX	  PA10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA10
	
	USART_InitStructure.USART_BaudRate = bound;//波特率一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	
	USART_Init(USART1, &USART_InitStructure); //初始化串口	1
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断 
	
	//设置中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
	USART_Cmd(USART1, ENABLE);                    //使能串口 
}

void usart_send_bytes(USART_TypeDef *USARTx, char *c,uint32_t cnt)
{
    while(cnt--)
    {
        USART_SendData(USARTx, *c++);
        while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
    }
}

uint8_t Serial_RxPacket[8];				//定义接收数据包数组
uint8_t Serial_RxFlag;					//定义接收数据包标志位

void USART1_IRQHandler(void)
{
	static uint8_t RxState = 0;		//定义表示当前状态机状态的静态变量	
	static uint8_t pRxPacket = 0;	//定义表示当前接收数据位置的静态变量
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)//接收到数据
	{	 
		uint8_t RxData = USART_ReceiveData(USART1);				//读取数据寄存器，存放在接收的数据变量
		
		/*使用状态机的思路，依次处理数据包的不同部分*/
		if (RxState == 0) {
			if (RxData == 0x55) {
				RxState = 1;	//进入到下一个状态
				pRxPacket = 0;  //数据包的位置归零
			}
		} else if (RxState == 1) {
			if (RxData == 0x53) {
				RxState = 2;	//进入到下一个状态
				pRxPacket = 0;  //数据包的位置归零
			}
		} else if (RxState == 2) {
			Serial_RxPacket[pRxPacket] = RxData;	//将数据存入数据包数组的指定位置
			pRxPacket++;	//数据包的位置自增
			if (pRxPacket >= 8) {
				RxState = 3;
			} 
		} else if (RxState == 3) {
			//if ((0x55 + 0x53 + Serial_RxPacket[pRxPacket - 1] + Serial_RxPacket[pRxPacket - 2] + Serial_RxPacket[pRxPacket - 3] + Serial_RxPacket[pRxPacket - 4]) % 256 == RxData) {
				RxState = 0;
				Serial_RxFlag = 1;
			//}
		}
		
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);		//清除标志位
	}  	
}

double read_Yaw(uint8_t YawL, uint8_t YawH) {
	double Yaw = 0;
	Yaw =  ((YawH << 8) | YawL) * 0.0055;
	return Yaw;
}

uint8_t Serial_GetRxFlag(void)
{
	if (Serial_RxFlag == 1)			//如果标志位为1
	{
		Serial_RxFlag = 0;
		return 1;					//则返回1，并自动清零标志位
	}
	return 0;						//如果标志位为0，则返回0
}

float Pitch, Roll, Yaw;


int main(void)
{
	OLED_Init();			/*OLED初始化*/
	MPU6050_Init();			//MPU6050初始化
	usart1_init(115200);	/*串口1初始化*/
	Timer_Init();
	
	OLED_ShowString(0, 0, "MPU6050", OLED_8X16);
	

	while(1)
	{
//		if (Serial_GetRxFlag() == 1)	//如果接收到数据包
//		{
//			OLED_ShowFloatNum(8, 16, (double)read_Yaw(Serial_RxPacket[4], Serial_RxPacket[5]), 3, 3, OLED_8X16);
//			OLED_Update();
//		}
	
		OLED_Printf(0, 16, OLED_8X16, "Pitch:%+06.lf", Pitch);
		OLED_Printf(0, 32, OLED_8X16, "Pitch:%+06.lf", Roll);
		OLED_Printf(0, 48, OLED_8X16, "Pitch:%+06.lf", Yaw);
		OLED_Update();
	}
}

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		MPU6050_ReadDMP(&Pitch, &Roll, &Yaw);
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}
