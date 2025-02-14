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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
Motor motor1;
float Position;//距0点位置,单位mm
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
/* Definitions for myTask01 */
osThreadId_t myTask01Handle;
const osThreadAttr_t myTask01_attributes = {
  .name = "myTask01",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for myQueue01 */
osMessageQueueId_t myQueue01Handle;
const osMessageQueueAttr_t myQueue01_attributes = {
  .name = "myQueue01"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
osThreadId_t EthernetTaskHandle;
const osThreadAttr_t EthernetTask_attributes = {
  .name = "EthernetTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

osThreadId_t EncoderTaskHandle;
const osThreadAttr_t EncoderTask_attributes = {
  .name = "EncoderTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

osThreadId_t DriveTaskHandle;
const osThreadAttr_t DriveTask_attributes = {
  .name = "DriveTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

osThreadId_t Pulse_EmissionTaskHandle;
const osThreadAttr_t Pulse_EmissionTask_attributes = {
  .name = "Pulse_EmissionTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

osThreadId_t Screen_TransferTaskHandle;
const osThreadAttr_t Screen_TransferTask_attributes = {
  .name = "Screen_TransferTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};


void Ethernet(void *argument);//网络通信
void Encoder(void *argument);//电动机编码器通讯
void Drive(void *argument);//控制驱动器
void Pulse_Emission(void *argument);//脉冲发射
void Screen_Transfer(void *argument);//屏幕通信
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartTask01(void *argument);

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
  /* creation of myQueue01 */
  myQueue01Handle = osMessageQueueNew (5, sizeof(uint8_t), &myQueue01_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of myTask01 */
  myTask01Handle = osThreadNew(StartTask01, NULL, &myTask01_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
	EthernetTaskHandle = osThreadNew(Ethernet, NULL, &EthernetTask_attributes);
	EncoderTaskHandle = osThreadNew(Encoder, NULL, &EncoderTask_attributes);
	DriveTaskHandle = osThreadNew(Drive, NULL, &DriveTask_attributes);
	Pulse_EmissionTaskHandle = osThreadNew(Pulse_Emission, NULL, &Pulse_EmissionTask_attributes);
	Screen_TransferTaskHandle = osThreadNew(Screen_Transfer, NULL, &Screen_TransferTask_attributes);
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

/* USER CODE BEGIN Header_StartTask01 */
/**
* @brief Function implementing the myTask01 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask01 */
void StartTask01(void *argument)
{
  /* USER CODE BEGIN StartTask01 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartTask01 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
wiz_NetInfo gWIZNETINFO = {
  .mac = { 0x80, 0x80, 0x80,0x80,0x80,0x80 },//MSB - LSB
  .ip  = { 192, 168, 1, 10 },
  .sn  = { 255, 255, 255, 0 },
  .gw  = { 192, 168, 1, 1 },//网关参数
  .dns  = { 8, 8, 8, 8 },
  .dhcp = NETINFO_STATIC };

void Ethernet(void *argument)//W5500通讯
{
	printf("A simple Ping Application using W5500!\r\n");
  W5500Init();//w5500初始化
  ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);  //配置网络参数
  wiz_PhyConf phyconf;
  phyconf.by = PHY_CONFBY_SW;
  phyconf.duplex = PHY_DUPLEX_FULL;
  phyconf.speed = PHY_SPEED_10;
  phyconf.mode = PHY_MODE_AUTONEGO;
  ctlwizchip(CW_SET_PHYCONF, (void*)&phyconf); //配置PHY参数
  PHYStatusCheck();//检查网络连接参数
  PrintPHYConf();//打印PHY配置信息
  printf("Simple Ping Application\r\n");
  for(;;)
  {
    osDelay(500);
		HAL_GPIO_TogglePin(GPIOC, LED2_Pin);
  }

}


void Encoder(void *argument)//电动机编码器通讯
{
	HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);      //开启编码器定时器
	__HAL_TIM_ENABLE_IT(&htim2,TIM_IT_UPDATE);          //开启编码器定时器更新中断,防溢出处理
	HAL_TIM_Base_Start_IT(&htim3);                    //开启100ms定时器中断
	__HAL_TIM_SET_COUNTER(&htim2, 10000);               //编码器定时器初始值设定为0
	motor1.lastCount = 0;                               //结构体内容初始化
	motor1.totalCount = 0;
	motor1.overflowNum = 0;                                  
	motor1.speed = 0;
	motor1.direct = 0;
	motor1.distance = 0;
  for(;;)
  {
    osDelay(1);
  }

}


uint16_t pwmVal_TIM8 = 0;  //占空比大小(CCRx的大小，高电平时间)
void Drive(void *argument)//控制驱动器
{
	HAL_TIM_Base_Start_IT(&htim8);//开启TIM8定时器中断
	HAL_TIM_PWM_Start_IT(&htim8,TIM_CHANNEL_1);//启动TIM8_CH1的PWM波输出（中断）
  for(;;)
  {
//		__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwmVal_TIM8);//修改占空比大小
//		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);//控制电机驱动控制电机正反装
//		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,GPIO_PIN_SET);//控制电机驱动控制电机启停
    osDelay(1);
  }
}


uint16_t pwmVal_TIM13 = 0;  //占空比大小(CCRx的大小，高电平时间)
void Pulse_Emission(void *argument)//脉冲发射（TIM13_CH1通道）
{	
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);//配置DIR引脚高电平，电平转换器A->B
	HAL_TIM_Base_Start_IT(&htim13);//开启TIM13定时器中断
	HAL_TIM_PWM_Start_IT(&htim13,TIM_CHANNEL_1);//启动TIM13_CH1的PWM波输出（中断）
  for(;;)
  {
//		__HAL_TIM_SetCompare(&htim13,TIM_CHANNEL_1,pwmVal_TIM13);//修改占空比大小
    osDelay(1);
  }
}


void Screen_Transfer(void *argument)//屏幕通信
{
	initRingBuff();																//初始化环形缓存区
	HAL_UART_Receive_IT(&huart2,RxBuff,1);				//打开串口接收中断
	TJCPrintf("0x00");         							  		//为确保串口HMI正常通信
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
//			switch(u(1))
//			{
//				case 0XEE://退出本次捕获
//					Pulse_count=0;	//当前段一帧脉冲上升沿数量清0
//					Standard_pulse_count=0;//标准一帧上升沿数量清0
//					Pulse_count_total=0;	//脉冲上升沿总数清0					
//					Unqualified_pulse_frame_rate=0;	//脉冲帧数不合格数清0
//					Pulse_frame_rate=0;//总脉冲帧数清0					
//					Starting_angle=0;//上次开始角度参数清0
//					End_angle=0;//上次结束角度参数清0
//					Angular_velocity=0;//上次角速度参数清0
//					Pulse_standard_frame_rate=0;//上次脉冲标准帧数参数清0
//					START_FLAG=0;	//脉冲捕获关闭				
//					break;
//				
//				case 0XA1://开始角度
//					Starting_angle=(u(5)<<24)|(u(4)<<16)|(u(3)<<8)|(u(2)<<0);//因为屏幕常量为小端传输，因此需此移位计算
//					break;
//				
//				case 0XA2://结束角度
//					End_angle=(u(5)<<24)|(u(4)<<16)|(u(3)<<8)|(u(2)<<0);
//					break;
//				
//				case 0XA3://角速度
//					Angular_velocity=(u(5)<<24)|(u(4)<<16)|(u(3)<<8)|(u(2)<<0);
//					break;
//				
//				case 0XA4://脉冲标准帧数
//					Pulse_standard_frame_rate=(u(5)<<24)|(u(4)<<16)|(u(3)<<8)|(u(2)<<0);
//					break;
//				
//				case 0XAA://开始捕获
//					START_FLAG=1;
//					Angle=End_angle-Starting_angle;//旋转角度计算
//					Standard_pulse_count=Angle/0.2+1;//标准一帧上升沿数量
//					TJCPrintf("gather.t10.txt=\"%d\"",Standard_pulse_count);	//显示标准一帧上升沿数量	
//					break;
//				
//				default:
//					break;
//			}
		  udelete(FRAMELENGTH);//删除串口缓冲区中相应长度的数据
	  }		
  }
}
/* USER CODE END Application */

