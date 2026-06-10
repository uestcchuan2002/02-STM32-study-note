#include "buzzer.h"


//设置蜂鸣器的状态：
//0:蜂鸣器响
//其他：蜂鸣器停
void Set_Buzzer_State(uint8_t state)
{
	if(state==1)
	{
		HAL_GPIO_WritePin(buzzer_GPIO_Port,buzzer_Pin,GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(buzzer_GPIO_Port,buzzer_Pin,GPIO_PIN_RESET);		
	}
}
