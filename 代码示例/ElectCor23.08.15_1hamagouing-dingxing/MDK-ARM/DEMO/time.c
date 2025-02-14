#include "time.h"
#include "main.h"
#include "stm32f4xx_hal.h"


TIM_HandleTypeDef TIM3_Handler;


//ͨ�ö�ʱ�� 3 �жϳ�ʼ��
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
//����ʹ�õ��Ƕ�ʱ�� 3
void TIME3_Init(uint16_t arr,uint16_t psc)
{
	TIM3_Handler.Instance = TIM3;
	TIM3_Handler.Init.Prescaler = psc;//��Ƶ��
	TIM3_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;//����ģʽ���ϼ���
	TIM3_Handler.Init.Period = arr;//�Զ�װ��ֵ
	TIM3_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;//ʱ�ӷ�Ƶ����
	TIM3_Handler.Init.RepetitionCounter = 0;//���������ظ��������Ĵ�����ֵ
	HAL_TIM_Base_Init(&TIM3_Handler);//�жϳ�ʼ��
	HAL_TIM_Base_Start_IT(&TIM3_Handler);	//�ж�ʹ��
}


void TIM3_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&TIM3_Handler);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	HAL_IWDG_Refresh(&hiwdg);
}

