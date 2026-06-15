#include "input_capture.h"

TIM_HandleTypeDef TIM5_Handler;  

/*
	定时器5通道1输入捕获配置 
	arr：自动重装值(TIM2,TIM5是32位的!!) 
	psc：时钟预分频数 
*/
void TIM5_CH1_Cap_Init(u32 arr, u16 psc) 
{
	TIM_IC_InitTypeDef TIM5_CH1Config; 
	
	TIM5_Handler.Instance = TIM5;
	TIM5_Handler.Init.Prescaler = psc;
	TIM5_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	TIM5_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;
	TIM5_Handler.Init.Period = arr;
	HAL_TIM_IC_Init(&TIM5_Handler);

	TIM5_CH1Config.ICPolarity = TIM_ICPOLARITY_RISING;
	TIM5_CH1Config.ICPrescaler = TIM_ICPSC_DIV1;
	TIM5_CH1Config.ICSelection = TIM_ICSELECTION_DIRECTTI;
	TIM5_CH1Config.ICFilter = 0;
	HAL_TIM_IC_ConfigChannel(&TIM5_Handler, &TIM5_CH1Config, TIM_CHANNEL_1);

	HAL_TIM_IC_Start_IT(&TIM5_Handler, TIM_CHANNEL_1);
	__HAL_TIM_ENABLE_IT(&TIM5_Handler, TIM_IT_UPDATE);
}


/*
	定时器5底层驱动，时钟使能，引脚配置 
	此函数会被HAL_TIM_IC_Init()调用 
	htim:定时器5句柄
*/
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim) 
{ 
    GPIO_InitTypeDef GPIO_Initure; 
    __HAL_RCC_TIM5_CLK_ENABLE();  			//使能TIM5时钟 
    __HAL_RCC_GPIOA_CLK_ENABLE();  			//开启GPIOA时钟 
    
	GPIO_Initure.Pin=GPIO_PIN_0;    		//PA0 
    GPIO_Initure.Mode=GPIO_MODE_AF_PP; 		//复用推挽输出 
    GPIO_Initure.Pull=GPIO_PULLDOWN;  		//下拉 
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;  	//高速 
    GPIO_Initure.Alternate=GPIO_AF2_TIM5;  	//PA0复用为TIM5通道1 
    
    HAL_GPIO_Init(GPIOA,&GPIO_Initure); 

	//设置中断优先级，抢占优先级2，子优先级0 
    HAL_NVIC_SetPriority(TIM5_IRQn,2,0); 	
    HAL_NVIC_EnableIRQ(TIM5_IRQn);   	//开启ITM5中断通道   
}

/*
	捕获状态 
	[7]:0,没有的捕获;1,成功捕获到一次. 
	[6]:0,还没捕获到低电平;1,已经捕获到低电平了. 
	[5:0]:捕获低电平后溢出的次数(对于32位定时器来说,1us计数器加1,溢出时间:4294秒)成功	
*/
u8  TIM5CH1_CAPTURE_STA = 0; 	// 输入捕获状态           
u32 TIM5CH1_CAPTURE_VAL; 		// 输入捕获值(TIM2/TIM5是32位)

/* 定时器5中断服务函数 */
void TIM5_IRQHandler(void) 
{ 
	HAL_TIM_IRQHandler(&TIM5_Handler);	//定时器共用处理函数 
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{	
	/* 还未捕获成功 */
	if ((TIM5CH1_CAPTURE_STA & 0x80) == 0) {
		/* 已经捕获到了高电平 */
		if ((TIM5CH1_CAPTURE_STA & 0x40)) {
			if ((TIM5CH1_CAPTURE_STA & 0x3F) == 0x3F) {
				/* 标记捕获成功 */
				TIM5CH1_CAPTURE_STA |= 0x80;
				TIM5CH1_CAPTURE_VAL = 0xFFFFFFFF;
			} else {
				TIM5CH1_CAPTURE_STA++;
			}
		}
	}
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	/* 还未捕获成功 */
	if ((TIM5CH1_CAPTURE_STA & 0x80) == 0) {
		/* 捕获到一个下降沿 */
		if ((TIM5CH1_CAPTURE_STA & 0x40)) {
			/* 标记成功捕获到一次高电平脉宽 */
			TIM5CH1_CAPTURE_STA |= 0x80;
			/* 获取当前的捕获值 */
			TIM5CH1_CAPTURE_VAL = HAL_TIM_ReadCapturedValue(&TIM5_Handler, TIM_CHANNEL_1);
			/* 一定要先清除原来的设置！！    */
			TIM_RESET_CAPTUREPOLARITY(&TIM5_Handler, TIM_CHANNEL_1);
			/* 配置TIM5通道1上升沿捕获        */
			TIM_SET_CAPTUREPOLARITY(&TIM5_Handler, TIM_CHANNEL_1, TIM_ICPOLARITY_RISING);
		} else {
			/* 清空 */
			TIM5CH1_CAPTURE_STA = 0;
			TIM5CH1_CAPTURE_VAL = 0;
			/* 标记捕获到了上升沿 */
			TIM5CH1_CAPTURE_STA |= 0x40;
			/* 关闭定时器5 */
			__HAL_TIM_DISABLE(&TIM5_Handler);
			__HAL_TIM_SET_COUNTER(&TIM5_Handler, 0); 
			/* 一定要先清除原来的设置！！    */
			TIM_RESET_CAPTUREPOLARITY(&TIM5_Handler, TIM_CHANNEL_1);
			/* 配置TIM5通道1下降沿捕获        */
			TIM_SET_CAPTUREPOLARITY(&TIM5_Handler, TIM_CHANNEL_1, TIM_ICPOLARITY_FALLING);
			/* 配置TIM5通道1上升沿捕获          */
			__HAL_TIM_ENABLE(&TIM5_Handler);
		}
	}
}








