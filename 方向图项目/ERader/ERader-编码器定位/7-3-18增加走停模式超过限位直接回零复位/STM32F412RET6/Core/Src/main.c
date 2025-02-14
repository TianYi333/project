/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
float speed_Record[SPEED_RECORD_NUM]={0};
int32_t Motor_pulsec_ount;//输入电机脉冲数
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
float Speed_Low_Filter(float new_Spe,float *speed_Record);
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  MX_SPI2_Init();
  MX_TIM3_Init();
  MX_TIM8_Init();
  MX_TIM13_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
	
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 12;
  RCC_OscInitStruct.PLL.PLLN = 96;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)//GPIO中断回调函数
{
	if(GPIO_Pin==IN0_Pin)
	{
		__HAL_GPIO_EXTI_CLEAR_IT(IN0_Pin);//清除中断标志位
		Zero_regression_flag=0;
		HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//关闭PWM输出
		HAL_GPIO_WritePin(TIM8_CH1N_GPIO_Port,TIM8_CH1N_Pin,GPIO_PIN_SET);//PA7-电机使能正输入(GPIO_PIN_RESET:开始，GPIO_PIN_SET:停止）
		Sports_status_flag=0;	
		Extreme_position_flag=1;
	}
	
	else if(GPIO_Pin==IN1_Pin)//原点
	{
		Position_flag=1;
		Sports_status_flag=0;	
		Zero_regression_flag=1;	
		Hard_Trigger_Current_Number=0;
		HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//关闭PWM输出
		HAL_GPIO_WritePin(TIM8_CH1N_GPIO_Port,TIM8_CH1N_Pin,GPIO_PIN_SET);//PA7-电机使能正输入(GPIO_PIN_RESET:开始，GPIO_PIN_SET:停止）
		Motor_pulsec_ount=0;	
		Encoder_Processing_flag=1;
		__HAL_GPIO_EXTI_CLEAR_IT(IN1_Pin);//清除中断标志位
	}	
	
	else if(GPIO_Pin==IN2_Pin)
	{
		__HAL_GPIO_EXTI_CLEAR_IT(IN2_Pin);//清除中断标志位
		Zero_regression_flag=0;
		HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//关闭PWM输出
		HAL_GPIO_WritePin(TIM8_CH1N_GPIO_Port,TIM8_CH1N_Pin,GPIO_PIN_SET);//PA7-电机使能正输入(GPIO_PIN_RESET:开始，GPIO_PIN_SET:停止）
		Sports_status_flag=0;	
		Extreme_position_flag=1;
	}
}



/*
 * 进行速度的平均滤波
 * 输入新采样到的速度，存放速度的数组，
 * 返回滤波后的速度
 */
float Speed_Low_Filter(float new_Spe,float *speed_Record)
{
    float sum = 0.0f;
    float test_Speed;
    for(uint8_t i=SPEED_RECORD_NUM-1;i>0;i--)//将现有数据后移一位
    {
        speed_Record[i] = speed_Record[i-1];
        sum += speed_Record[i-1];
    }
    speed_Record[0] = new_Spe;//第一位是新的数据
    sum += new_Spe;
    test_Speed = sum/SPEED_RECORD_NUM;
    return test_Speed;//返回均值
}

//void Motor_Rive(GPIO_PinState enable,GPIO_PinState direction,uint16_t number)//电机驱动函数
//{
//	uint8_t number_i=0xff;
//	if(number>0x05)
//	{
//		HAL_GPIO_WritePin(TIM8_CH1N_GPIO_Port,TIM8_CH1N_Pin,enable);//PA7-电机使能正输入(GPIO_PIN_RESET:开始，GPIO_PIN_SET:停止）
//		osDelay(1);	
//		HAL_GPIO_WritePin(TIM8_CH2_GPIO_Port,TIM8_CH2_Pin,direction);//PC7-电机方向控制(GPIO_PIN_RESET:顺时针，GPIO_PIN_SET:逆时针）
//		osDelay(1);	
//		HAL_TIM_Base_Start_IT(&htim8);//开启TIM8定时器中断（20KHZ，周期50us）
//		while(number_i>number)
//		{
//			number_i--;
//			__HAL_TIM_SET_AUTORELOAD(&htim8,number_i);//修改自动重装载值
//	//		__HAL_TIM_PRESCALER(&htim8,number);//修改预分频值
//	//		TIM8->CCR1 = running_speed_value;//修改占空比
//	//		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, number);//修改占空比
//			HAL_TIM_PWM_Start_IT(&htim8,TIM_CHANNEL_1);//启动TIM8_CH1的PWM波输出（中断）
//			if(number_i>=0x96)
//			{
//				HAL_Delay(2);
//			}
//			else if(0x96>number_i&&number_i>=0x28)
//			{
//				HAL_Delay(15);	
//			}	
//			else
//			{
//				HAL_Delay(20);
//			}	
//		}
//		number_i=0xff;
//	}
//}

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */
	else if(htim->Instance==TIM8)//PWM输出
	{			
		Encoder_Processing_flag=1;
		if(HAL_GPIO_ReadPin(TIM8_CH2_GPIO_Port,TIM8_CH2_Pin)==GPIO_PIN_RESET&&HAL_GPIO_ReadPin(TIM8_CH1N_GPIO_Port,TIM8_CH1N_Pin)==GPIO_PIN_RESET)//正转
		{
			Motor_pulsec_ount++;
		}
		else if(HAL_GPIO_ReadPin(TIM8_CH2_GPIO_Port,TIM8_CH2_Pin)==GPIO_PIN_SET&&HAL_GPIO_ReadPin(TIM8_CH1N_GPIO_Port,TIM8_CH1N_Pin)==GPIO_PIN_RESET)//反转 
		{
			Motor_pulsec_ount--;
		}
	}	
	else if(htim->Instance==TIM13)//硬触发定时50us一次
	{
		if(Hard_Trigger_flag==1)
		{
			HAL_GPIO_WritePin(IO_OUT_GPIO_Port, IO_OUT_Pin, GPIO_PIN_RESET);//硬触发关闭
			HAL_TIM_Base_Stop_IT(&htim13);	
		}
	}	
  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
