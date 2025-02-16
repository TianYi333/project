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
uint32_t IC_DOWN_TIMES;  		// 捕获低电平次数，单位1ms
uint8_t IC_END_FLAG;	// 捕获结束标志位
uint8_t IC_START_FLAG;  // 捕获开始标志，1：已捕获到高电平；0：还没有捕获到高电平
uint8_t IC_DONE_FLAG;  	// 捕获完成标志，1：已完成一次高电平捕获
uint8_t IC_DOWN_DONE_FLAG;// 低电压捕获完成标志，1：已完成一次低电平捕获
uint16_t IC_VALUE;  		// 输入捕获的捕获值
uint16_t IC_DOWN_VALUE;  		// 下降沿输入捕获的捕获值
uint16_t IC_DOWN_START_FLAG;  		// 下降沿输入捕获开始标志，1：开始；0：还没有开始

uint32_t Pulse_count_total=0;	// 输入捕获的总上升沿数量
uint32_t Pulse_count=0;	// 输入捕获的一段上升沿数量
uint16_t Pulse_Cycle_Unqualified=0;  //脉冲周期不合格数
uint16_t Starting_angle=0;//开始角度
uint16_t End_angle=0;//结束角度
uint16_t Angle=0;//旋转角度
uint8_t Angular_velocity=0;//角速度
uint8_t Pulse_frame_rate=0;//脉冲帧数
uint8_t Pulse_standard_frame_rate=0;//脉冲标准帧数
uint8_t Unqualified_pulse_frame_rate=0;//不合格脉冲帧数
uint8_t START_FLAG=0;//开始捕获标志位
uint32_t Standard_pulse_count=0;//标准上升沿数量
uint32_t One_cycle_angle=0;//一周期内旋转角度
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define FRAMELENGTH 9							//接受屏幕数据字节数
#define Pulse_Filtering 1000   		//脉冲滤波us
#define Interval 500 							//一帧脉冲间隔ms
#define Standard_Cycle_angle 0.2*1000000	//标准周期间隔角度°
#define Error_Criteria_angle 0.01*1000000	//标准误差角度
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
/* Definitions for Screen_TransferTask */
osThreadId_t Screen_TransferTaskHandle;
const osThreadAttr_t Screen_TransferTask_attributes = {
  .name = "Screen_TransferTask",
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
void Screen_Transfer(void *argument);

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
  Pulse_Reception_QueueHandle = osMessageQueueNew (5, 7*sizeof(uint8_t), &Pulse_Reception_Queue_attributes);

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

  /* creation of Screen_TransferTask */
  Screen_TransferTaskHandle = osThreadNew(Screen_Transfer, NULL, &Screen_TransferTask_attributes);

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
		
    osDelay(500);
		HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
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
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);//配置DIR引脚低电平，电平转换器B->A
	uint32_t Pulse_Cycle= 0;//脉冲周期
	uint32_t Pulse_Up_Cycle= 0;//脉冲高电平时间
	uint32_t Pulse_Down_Cycle= 0;//脉冲低电平时间
	uint8_t Pulse_Cycle_array[7];	//脉冲周期数组
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
		if(IC_DONE_FLAG == 1)	// 如果完成一次高电平捕获
		{
			IC_DONE_FLAG = 0;	//标志清零
			Pulse_Up_Cycle = IC_TIMES * 1000;	//脉冲时间为捕获次数 * 1000us
			Pulse_Up_Cycle += IC_VALUE;	 // 加上捕获时间（小于1ms的部分)			
			if(Pulse_Up_Cycle>=Pulse_Filtering)	//滤波
			{
				Pulse_Cycle_array[0]=0x55;
				Pulse_Cycle_array[1]=0xB1;
				Pulse_Cycle_array[2]=0x00;
				Pulse_Cycle_array[3]=0x00;
				Pulse_Cycle_array[4]=0x00;
				Pulse_Cycle_array[5]=0x00;
				Pulse_Cycle_array[6]=0xff;
				osMessageQueuePut(Pulse_Reception_QueueHandle,Pulse_Cycle_array,1,0);
			}					
		}	
		if(IC_DOWN_DONE_FLAG == 1)	// 在完成一次高电平后，完成低电平捕获
		{
			IC_DOWN_DONE_FLAG = 0;	//标志清零
			if(IC_END_FLAG==1)
			{
				IC_END_FLAG=0;
				continue;	
			}
			Pulse_Down_Cycle = IC_DOWN_TIMES * 1000;	//脉冲时间为捕获次数 * 1000us
			Pulse_Down_Cycle += IC_DOWN_VALUE;	 // 加上捕获时间（小于1ms的部分)
			Pulse_Cycle=Pulse_Up_Cycle+Pulse_Down_Cycle;//脉冲周期
			if(Pulse_Cycle>=Pulse_Filtering)	//滤波
			{
				Pulse_Cycle_array[0]=0x55;
				Pulse_Cycle_array[1]=0xB2;
				Pulse_Cycle_array[2]=Pulse_Cycle&0xff;
				Pulse_Cycle_array[3]=(Pulse_Cycle>>8)&0xff;
				Pulse_Cycle_array[4]=(Pulse_Cycle>>16)&0xff;
				Pulse_Cycle_array[5]=(Pulse_Cycle>>24)&0xff;
				Pulse_Cycle_array[6]=0xff;
				osMessageQueuePut(Pulse_Reception_QueueHandle,Pulse_Cycle_array,1,0);
			}
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
	uint32_t Pulse_Cycle_Send=0;//脉冲周期发送数
	uint8_t status=0;
	uint8_t Pulse_Cycle_Send_array[7];	//脉冲周期发送数组
	TJCPrintf("gather.t7.txt=\"%d\"",0);	//显示脉冲周期清0
	TJCPrintf("gather.t8.txt=\"%d\"",0);	//显示结束角度清0
	TJCPrintf("gather.t9.txt=\"%d\"",0);	//显示一段脉冲上升沿数量清0
	TJCPrintf("gather.t11.txt=\"%d\"",0);	//显示脉冲上升沿总数量清0
	TJCPrintf("gather.t12.txt=\"%d\"",0);	//显示脉冲帧数不合格数清0
	TJCPrintf("gather.t13.txt=\"%d\"",0);	//显示脉冲帧数清0
	TJCPrintf("set.n1.val=%d",0);	//显示开始角度清0
	TJCPrintf("set.n2.val=%d",0);	//显示结束角度清0
	TJCPrintf("set.n3.val=%d",0);	//显示角速度清0
	TJCPrintf("set.n4.val=%d",0);	//显示脉冲帧数清0
	TJCPrintf("page set");	//显示返回主页面
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
		if(START_FLAG==1)//判断是否接受到开始捕获命令
		{
			status=osMessageQueueGet(Pulse_Reception_QueueHandle,Pulse_Cycle_Send_array,NULL,Interval);
			if(status!=osOK)	//一段时间（500ms）未收到脉冲
			{	
				if(Pulse_count!=0)
				{
					if(Pulse_frame_rate<Pulse_standard_frame_rate)//判断帧数
					{
						Pulse_frame_rate++;	//脉冲帧数计数
						if(Pulse_count!=Standard_pulse_count)//一帧脉冲波未到达标准脉冲波数
						{
							Unqualified_pulse_frame_rate++;
						}
						Pulse_count=0;	//当前段脉冲上升沿数量清零
						Pulse_Cycle_Unqualified=0;	//脉冲周期不合格数清零
						TJCPrintf("gather.t7.txt=\"%d\"",0);	//显示脉冲周期清0
						TJCPrintf("gather.t8.txt=\"%d\"",0);	//显示结束角度清0
						TJCPrintf("gather.t9.txt=\"%d\"",0);	//显示一段脉冲上升沿数量清0
						TJCPrintf("gather.t12.txt=\"%d\"",Unqualified_pulse_frame_rate);	//显示脉冲帧数不合格数
						TJCPrintf("gather.t13.txt=\"%d\"",Pulse_frame_rate);	//显示脉冲帧数
					}
					else
					{
						Pulse_frame_rate=0;
						START_FLAG=0;	//脉冲捕获关闭
					}
				}				
			}
			else
			{
				if(Pulse_Cycle_Send_array[0]==0x55&&Pulse_Cycle_Send_array[6]==0xff)
				{
					if(Pulse_Cycle_Send_array[1]==0xB2)
					{
						Pulse_Cycle_Send=Pulse_Cycle_Send_array[2]|(Pulse_Cycle_Send_array[3]<<8)|(Pulse_Cycle_Send_array[4]<<16)|(Pulse_Cycle_Send_array[5]<<24);
						One_cycle_angle=Pulse_Cycle_Send*Angular_velocity;	//一脉冲周期内旋转的角度
						if((One_cycle_angle-Standard_Cycle_angle>Error_Criteria_angle)||(Standard_Cycle_angle-One_cycle_angle>Error_Criteria_angle)) //周期误差大于误差标准
						{	
							Pulse_Cycle_Unqualified++;
						}
						TJCPrintf("gather.t7.txt=\"%d\"",Pulse_Cycle_Send);	//显示周期	
					}
					Pulse_count_total++;	//脉冲上升沿总数
					Pulse_count++;	//一帧脉冲上升沿数量					
					TJCPrintf("gather.t9.txt=\"%d\"",Pulse_count);	//显示一段脉冲上升沿数量
					TJCPrintf("gather.t11.txt=\"%d\"",Pulse_count_total);	//显示脉冲上升沿总数量	
					TJCPrintf("gather.t8.txt=\"%d\"",Pulse_Cycle_Unqualified);	//显示脉冲周期不合格数	
				}
			}	
		}
  }
  /* USER CODE END Screen */
}

/* USER CODE BEGIN Header_Screen_Transfer */
/**
* @brief Function implementing the Screen_TransferTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Screen_Transfer */
void Screen_Transfer(void *argument)
{
  /* USER CODE BEGIN Screen_Transfer */
  /* Infinite loop */
  for(;;)
  {
		osDelay(1);
		while(usize >= FRAMELENGTH)//串口缓存区数量大于6
	  {
		  //校验帧头帧尾是否匹配
		  if(u(0) != 0x55 || u(6) != 0xff || u(7) != 0xff || u(8) != 0xff)
		  {
			  //不匹配删除1字节
			  udelete(1);
		  }else
		  {
			  //匹配，跳出循环
			  break;
		  }
	  }	
	  //进行解析
	  if(usize >= FRAMELENGTH && u(0) == 0x55 && u(6) == 0xff && u(7) == 0xff && u(8) == 0xff)
	  {
			switch(u(1))
			{
				case 0XEE://退出本次捕获
					Pulse_Cycle_Unqualified=0;	//脉冲周期不合格数清0
					Pulse_count=0;	//当前段一帧脉冲上升沿数量清0
					Standard_pulse_count=0;//标准一帧上升沿数量清0
					Pulse_count_total=0;	//脉冲上升沿总数清0					
					Unqualified_pulse_frame_rate=0;	//脉冲帧数不合格数清0
					Pulse_frame_rate=0;//总脉冲帧数清0					
					Starting_angle=0;//上次开始角度参数清0
					End_angle=0;//上次结束角度参数清0
					Angular_velocity=0;//上次角速度参数清0
					Pulse_standard_frame_rate=0;//上次脉冲标准帧数参数清0
					START_FLAG=0;	//脉冲捕获关闭	
					IC_END_FLAG=0;				
					break;
				
				case 0XA1://开始角度
					Starting_angle=(u(5)<<24)|(u(4)<<16)|(u(3)<<8)|(u(2)<<0);//因为屏幕常量为小端传输，因此需此移位计算
					break;
				
				case 0XA2://结束角度
					End_angle=(u(5)<<24)|(u(4)<<16)|(u(3)<<8)|(u(2)<<0);
					break;
				
				case 0XA3://角速度
					Angular_velocity=(u(5)<<24)|(u(4)<<16)|(u(3)<<8)|(u(2)<<0);
					break;
				
				case 0XA4://脉冲标准帧数
					Pulse_standard_frame_rate=(u(5)<<24)|(u(4)<<16)|(u(3)<<8)|(u(2)<<0);
					break;
				
				case 0XAA://开始捕获
					START_FLAG=1;
					Angle=End_angle-Starting_angle;//旋转角度计算
					Standard_pulse_count=Angle/0.2+1;//标准一帧上升沿数量
					TJCPrintf("gather.t10.txt=\"%d\"",Standard_pulse_count);	//显示标准一帧上升沿数量	
					break;
				
				default:
					break;
			}
		  udelete(FRAMELENGTH);//删除串口缓冲区中相应长度的数据
	  }		

  }
  /* USER CODE END Screen_Transfer */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

