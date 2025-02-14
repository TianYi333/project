#include "time.h"
#include "main.h"
#include "stm32f4xx_hal.h"


TIM_HandleTypeDef TIM3_Handler;


//通用定时器 3 中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器 3
void TIME3_Init(uint16_t arr,uint16_t psc)
{
	TIM3_Handler.Instance = TIM3;
	TIM3_Handler.Init.Prescaler = psc;//分频数
	TIM3_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;//计数模式向上计数
	TIM3_Handler.Init.Period = arr;//自动装载值
	TIM3_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;//时钟分频因子
	TIM3_Handler.Init.RepetitionCounter = 0;//用来设置重复计数器寄存器的值
	HAL_TIM_Base_Init(&TIM3_Handler);//中断初始化
	HAL_TIM_Base_Start_IT(&TIM3_Handler);	//中断使能
}


void TIM3_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&TIM3_Handler);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	HAL_IWDG_Refresh(&hiwdg);
}

