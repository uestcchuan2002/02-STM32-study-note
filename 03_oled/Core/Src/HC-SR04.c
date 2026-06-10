#include "HC-SR04.h"

#define CPU_FREQUENCY_MHZ    72		// STM32Ê±ÖÓÖ÷Æµ
uint32_t Distance = 2;
uint32_t num = 0;

void Delay_us(__IO uint32_t delay)
{
    int last, curr, val;
    int temp;

    while (delay != 0)
    {
        temp = delay > 900 ? 900 : delay;
        last = SysTick->VAL;
        curr = last - CPU_FREQUENCY_MHZ * temp;
        if (curr >= 0)
        {
            do
            {
                val = SysTick->VAL;
            }
            while ((val < last) && (val >= curr));
        }
        else
        {
            curr += CPU_FREQUENCY_MHZ * 1000;
            do
            {
                val = SysTick->VAL;
            }
            while ((val <= last) || (val > curr));
        }
        delay -= temp;
    }
}

void HC_SR04_Computer(void)
{
	uint32_t Average_Distance=0;
	for(uint8_t i=0;i<8;i++)
	{
		HAL_GPIO_WritePin(HC_Trig_GPIO_Port,HC_Trig_Pin,GPIO_PIN_SET);
		Delay_us(20);
		HAL_GPIO_WritePin(HC_Trig_GPIO_Port,HC_Trig_Pin,GPIO_PIN_RESET);
		HAL_Delay(60);
		Average_Distance = Average_Distance +Distance;
	}
	Average_Distance = Average_Distance /8;
	u1_printf("the distance = %d\r\n",Average_Distance);
}











