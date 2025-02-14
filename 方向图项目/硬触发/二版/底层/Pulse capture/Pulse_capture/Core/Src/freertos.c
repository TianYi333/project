/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "tjc_usart_hmi.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
uint32_t IC_TIMES;  		// �����������λ1ms
uint8_t IC_START_FLAG;  // ����ʼ��־��1���Ѳ��񵽸ߵ�ƽ��0����û�в��񵽸ߵ�ƽ
uint8_t IC_DONE_FLAG;  	// ������ɱ�־��1�������һ�θߵ�ƽ����
uint16_t IC_VALUE;  		// ���벶��Ĳ���ֵ
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Pulse_acquisitionTask */
osThreadId_t Pulse_acquisitionTaskHandle;
const osThreadAttr_t Pulse_acquisitionTask_attributes = {
  .name = "Pulse_acquisitionTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for ScreenTask */
osThreadId_t ScreenTaskHandle;
const osThreadAttr_t ScreenTask_attributes = {
  .name = "ScreenTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Pulse_Reception_Queue */
osMessageQueueId_t Pulse_Reception_QueueHandle;
const osMessageQueueAttr_t Pulse_Reception_Queue_attributes = {
  .name = "Pulse_Reception_Queue"
};
/* Definitions for Pulse_Sending_Queue */
osMessageQueueId_t Pulse_Sending_QueueHandle;
const osMessageQueueAttr_t Pulse_Sending_Queue_attributes = {
  .name = "Pulse_Sending_Queue"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void Pulse_acquisition(void *argument);
void Screen(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of Pulse_Reception_Queue */
  Pulse_Reception_QueueHandle = osMessageQueueNew (5, 6*sizeof(uint8_t), &Pulse_Reception_Queue_attributes);

  /* creation of Pulse_Sending_Queue */
  Pulse_Sending_QueueHandle = osMessageQueueNew (5, 6*sizeof(uint8_t), &Pulse_Sending_Queue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of Pulse_acquisitionTask */
  Pulse_acquisitionTaskHandle = osThreadNew(Pulse_acquisition, NULL, &Pulse_acquisitionTask_attributes);

  /* creation of ScreenTask */
  ScreenTaskHandle = osThreadNew(Screen, NULL, &ScreenTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_Pulse_acquisition */
/**
* @brief Function implementing the Pulse_acquisitionTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Pulse_acquisition */
void Pulse_acquisition(void *argument)
{
  /* USER CODE BEGIN Pulse_acquisition */
	uint32_t time = 0;
	uint8_t Pulse_Cycle_array[6];
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
		if(IC_DONE_FLAG == 1)  // ������һ�θߵ�ƽ����
		{
			IC_DONE_FLAG = 0;  // ��־����
			time = IC_TIMES * 1000;  //����ʱ��Ϊ������� * 1000us
			time += IC_VALUE;  // ���ϲ���ʱ�䣨С��1ms�Ĳ���)
			Pulse_Cycle_array[0]=0x55;
			Pulse_Cycle_array[1]=time&0xff;
			Pulse_Cycle_array[2]=(time>>8)&0xff;
			Pulse_Cycle_array[3]=(time>>16)&0xff;
			Pulse_Cycle_array[4]=(time>>24)&0xff;
			Pulse_Cycle_array[5]=0xff;
			osMessageQueuePut(Pulse_Reception_QueueHandle,Pulse_Cycle_array,1,0);
		}
  }
  /* USER CODE END Pulse_acquisition */
}

/* USER CODE BEGIN Header_Screen */
/**
* @brief Function implementing the ScreenTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Screen */
void Screen(void *argument)
{
  /* USER CODE BEGIN Screen */
	uint32_t time=0;
	uint8_t status=0;
	uint8_t Cycle_Send_array[6];
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
		status=osMessageQueueGet(Pulse_Reception_QueueHandle,Cycle_Send_array,NULL,5000);
		if(status!=osOK)
		{		
			TJCPrintf("tm0.en=\"%d\"",1);//����Ļ���ݸ�λ��ʱ��
		}
		else
		{
			if(Cycle_Send_array[0]==0x55&&Cycle_Send_array[5]==0xff)
			{
				time=Cycle_Send_array[1]|(Cycle_Send_array[2]<<8)|(Cycle_Send_array[3]<<16)|(Cycle_Send_array[4]<<24);
				TJCPrintf("t4.txt=\"%d\"",time);
				TJCPrintf("tm0.en=\"%d\"",0);//�ر���Ļ���ݸ�λ��ʱ��				
			}
		}	
  }
  /* USER CODE END Screen */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

