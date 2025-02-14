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
uint32_t IC_TIMES;  		// 捕获次数，单位1ms
uint8_t IC_START_FLAG;  // 捕获开始标志，1：已捕获到高电平；0：还没有捕获到高电平
uint8_t IC_DONE_FLAG;  	// 捕获完成标志，1：已完成一次高电平捕获
uint16_t IC_VALUE;  		// 输入捕获的捕获值
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
	uint32_t Pulse_Cycle= 0;//脉冲周期
	uint8_t Pulse_Cycle_array[6];	//脉冲周期数组
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
		if(IC_DONE_FLAG == 1)	// 如果完成一次高电平捕获
		{
			IC_DONE_FLAG = 0;	//标志清零
			Pulse_Cycle = IC_TIMES * 1000;	//脉冲时间为捕获次数 * 1000us
			Pulse_Cycle += IC_VALUE;	 // 加上捕获时间（小于1ms的部分)
			Pulse_Cycle_array[0]=0x55;
			Pulse_Cycle_array[1]=Pulse_Cycle&0xff;
			Pulse_Cycle_array[2]=(Pulse_Cycle>>8)&0xff;
			Pulse_Cycle_array[3]=(Pulse_Cycle>>16)&0xff;
			Pulse_Cycle_array[4]=(Pulse_Cycle>>24)&0xff;
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
	uint32_t Pulse_Cycle_Send=0;
	uint8_t status=0;
	uint8_t Pulse_Cycle_Send_array[6];	//脉冲周期发送数组
	int32_t Pulse_Cycle_difference=0;  //脉冲周期差
	uint32_t TIME=0;
	uint16_t Pulse_count=0;	// 输入捕获的一段上升沿数量
	uint32_t Pulse_count_total=0;// 输入捕获的总上升沿数量
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
		status=osMessageQueueGet(Pulse_Reception_QueueHandle,Pulse_Cycle_Send_array,NULL,5000);
		if(status!=osOK)	//一段时间未收到脉冲
		{		
			Pulse_count=0;	//当前段脉冲上升沿数量清零
			TJCPrintf("tm0.en=\"%d\"",1);	//打开屏幕数据复位定时器
		}
		else
		{
			if(Pulse_Cycle_Send_array[0]==0x55&&Pulse_Cycle_Send_array[5]==0xff)
			{
				Pulse_count_total++;	//脉冲上升沿总数
				Pulse_count++;	//一段数脉冲上升沿数量
				Pulse_Cycle_Send=Pulse_Cycle_Send_array[1]|(Pulse_Cycle_Send_array[2]<<8)|(Pulse_Cycle_Send_array[3]<<16)|(Pulse_Cycle_Send_array[4]<<24);
				Pulse_Cycle_difference=Pulse_Cycle_Send-TIME;	//周期差
				TJCPrintf("t8.txt=\"%d\"",Pulse_Cycle_difference);	//显示周期差
				TJCPrintf("t4.txt=\"%d\"",Pulse_Cycle_Send);	//显示周期	
				TJCPrintf("t7.txt=\"%d\"",Pulse_count);	//显示一段脉冲上升沿数量
				TJCPrintf("t10.txt=\"%d\"",Pulse_count_total);	//显示脉冲上升沿总数量				
				TJCPrintf("tm0.en=\"%d\"",0);	//关闭屏幕数据复位定时器
				TIME=Pulse_Cycle_Send;	//记录当前周期				
			}
		}	
  }
  /* USER CODE END Screen */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

