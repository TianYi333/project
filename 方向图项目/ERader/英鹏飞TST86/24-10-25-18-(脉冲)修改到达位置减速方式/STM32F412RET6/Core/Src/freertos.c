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

/* TIM8_CH1(PWM输出)-OUT1；TIM8_CH1N(电机使能)-OUT2；TIM8_CH2(电机方向控制)-OUT3；TIM8_CH2N(伺服复位)-OUT4；TIM8_CH3(雷达上电)-OUT5；TIM8_CH4(清除滞留脉冲)-OUT6 */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
wiz_NetInfo gWIZNETINFO = {
  .mac = { 0x80, 0x80, 0x80,0x80,0x80,0x80 },//MSB - LSB
  .ip  = { 192, 168, 1, 11 },
  .sn  = { 255, 255, 255, 0 },
  .gw  = { 192, 168, 1, 1 },//网关参数
  .dns  = { 8, 8, 8, 8 },
  .dhcp = NETINFO_STATIC };

uint8_t receive_buff[RECEIVE_BUFF_SIZE];

	
/***********************************************variable***********************************/
float    variable_Position=0;//距0点位置,单位mm
uint32_t variable_Position_upload=0;//距0点距离上传值（单位0.001mm）
uint32_t variable_Zero_return_speed_value=10000;//回零速度（单位0.001mm/s）
uint32_t variable_Running_speed_value=5000;//运行速度（单位0.001mm/s）
uint32_t variable_Target_position=0;//目标位置
uint32_t variable_Zero_position=0;//零点位置	
uint32_t variable_Hard_trigger_interval=0;//硬触发时间间隔
uint32_t variable_Hard_trigger_position=0;//硬触发开始位置
uint32_t variable_Trigger_interval;//触发间隔
float    variable_Position_lastpulsec;//上次硬触发位置
	
/***********************************************flag***************************************/
uint8_t flag_Extreme_position=0;//极限位置标志位
uint8_t flag_Position_upload=1;//位置上传标志位
uint8_t flag_Encoder_processing=0;//编码器数据处理标志位
uint8_t flag_Zero_regression=0;//是否回零过标志位
uint8_t flag_Absolute_position=0;//绝对位置标志位
uint8_t flag_Hard_trigger_start=0;//硬触发开始标志位
uint8_t flag_Hard_trigger=0;//硬触发标志位	
uint8_t flag_Motor_deceleration_stops=0;//点击减速停止标志位

/***********************************************number*************************************/	
uint16_t number_Hard_trigger=0;//硬触发个数
uint16_t number_Hard_trigger_curren=0;//硬触发当前个数
uint8_t  number_Iwdg=0;//看门狗计数
int32_t  number_Motor_pulsec;//输入电机脉冲数

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
osThreadId_t Ethernet_ServerTaskHandle;
const osThreadAttr_t Ethernet_ServerTask_attributes = {
  .name = "Ethernet_ServerTask",
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

osThreadId_t Motor_driveTaskHandle;
const osThreadAttr_t Motor_driveTask_attributes = {
  .name = "Motor_driveTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

osThreadId_t Pulse_EmissionTaskHandle;
const osThreadAttr_t Pulse_EmissionTask_attributes = {
  .name = "Pulse_EmissionTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

osThreadId_t Extreme_position_DriveTaskHandle;
const osThreadAttr_t Extreme_position_DriveTask_attributes = {
  .name = "Extreme_position_DriveTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

osThreadId_t Uart3_CommunicateTaskHandle;
const osThreadAttr_t Uart3_CommunicateTask_attributes = {
  .name = "Uart3_CommunicateTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

osThreadId_t Iwdg_functionTaskHandle;
const osThreadAttr_t Iwdg_functionTask_attributes = {
  .name = "Iwdg_functionTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

osThreadId_t Motor_deceleration_stopsTaskHandle;
const osThreadAttr_t Motor_deceleration_stopsTask_attributes = {
  .name = "Motor_deceleration_stopsTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

osMessageQueueId_t Ethernet_ReceiveHandle;
const osMessageQueueAttr_t Ethernet_Receive_attributes = {
  .name = "Ethernet_Receive"
};

osMessageQueueId_t Ethernet_SendHandle;
const osMessageQueueAttr_t Ethernet_Send_attributes = {
  .name = "Ethernet_Send"
};

osMessageQueueId_t Motor_driveQueueHandle;
const osMessageQueueAttr_t Motor_driveQueue_attributes = {
  .name = "Motor_driveQueue"
};


void Ethernet_Server(void *argument);//网络通信-服务端
void Encoder(void *argument);//电动机编码器通讯
void Drive(void *argument);//控制驱动器
void Motor_drive(void *argument);//电机加速启动
void Pulse_Emission(void *argument);//脉冲发射
void Extreme_position_Drive(void *argument);//极限位置控制
void Uart3_Communicate(void *argument);//极限位置控制
void Iwdg_function(void *argument);//看门狗函数
void Motor_deceleration_stops(void *argument);//电机减速停止
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
	Ethernet_ServerTaskHandle = osThreadNew(Ethernet_Server, NULL, &Ethernet_ServerTask_attributes);
	EncoderTaskHandle = osThreadNew(Encoder, NULL, &EncoderTask_attributes);
	DriveTaskHandle = osThreadNew(Drive, NULL, &DriveTask_attributes);
	Motor_driveTaskHandle = osThreadNew(Motor_drive, NULL, &Motor_driveTask_attributes);
	Pulse_EmissionTaskHandle = osThreadNew(Pulse_Emission, NULL, &Pulse_EmissionTask_attributes);
	Extreme_position_DriveTaskHandle = osThreadNew(Extreme_position_Drive, NULL, &Extreme_position_DriveTask_attributes);
	Uart3_CommunicateTaskHandle = osThreadNew(Uart3_Communicate, NULL, &Uart3_CommunicateTask_attributes);
	Iwdg_functionTaskHandle = osThreadNew(Iwdg_function, NULL, &Iwdg_functionTask_attributes);
	Motor_deceleration_stopsTaskHandle = osThreadNew(Motor_deceleration_stops, NULL, &Motor_deceleration_stopsTask_attributes);
	
	
	Ethernet_ReceiveHandle = osMessageQueueNew (10, 20*sizeof(uint8_t), &Ethernet_Receive_attributes);
	Ethernet_SendHandle = osMessageQueueNew (5, 20*sizeof(uint8_t), &Ethernet_Send_attributes);
	Motor_driveQueueHandle = osMessageQueueNew (5, 3*sizeof(uint8_t), &Motor_driveQueue_attributes);
	
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
	if(REACH)//原点判断
	{
		variable_Position=0;
		flag_Zero_regression=1;
		number_Motor_pulsec=0;
	}
	else 
	{
		number_Motor_pulsec=0xffffff;
		variable_Position=2000;
	}
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
		osDelay(100);
		HAL_GPIO_TogglePin(GPIOC, LED1_Pin);
  }
  /* USER CODE END StartTask01 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

void Ethernet_Server(void *argument)//网口通信-服务端
{
  W5500Init();//w5500初始化
  ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);  //配置网络参数
  wiz_PhyConf phyconf;
  phyconf.by = PHY_CONFBY_SW;
  phyconf.duplex = PHY_DUPLEX_FULL;
  phyconf.speed = PHY_SPEED_10;
  phyconf.mode = PHY_MODE_AUTONEGO;
  ctlwizchip(CW_SET_PHYCONF, (void*)&phyconf); //配置PHY参数
  for(;;)
  {
		net_process_socket(1, LISTEN_PORT_1);
		net_process_socket(2, LISTEN_PORT_2);
		osDelay(10);
  }
}

void Drive(void *argument)//控制驱动器
{
	uint8_t	 array_Send_buff[13];
	uint8_t  array_Drive_control[20];	//驱动器控制数组
	uint8_t  array_Motor_drive_send[3];//电机驱动数组
	uint8_t  status=0;	
	uint32_t variable_Zero_return_creep_speed_value=0;//回零蠕变速度
	uint16_t number_Data_bytes=0;//数据字节
  for(;;)
  {
		osDelay(1);	
		status=osMessageQueueGet(Ethernet_ReceiveHandle,array_Drive_control,NULL,10);
		if(status==osOK)
		{		
			number_Iwdg=0;	
			array_Send_buff[0]=array_Drive_control[0];//0X19;事务处理标识
			array_Send_buff[1]=array_Drive_control[1];//0XB2;事务处理标识
			array_Send_buff[2]=0X00;//协议标识符，00 00为modbus协议
			array_Send_buff[3]=0X00;//协议标识符，00 00为modbus协议
			array_Send_buff[6]=array_Drive_control[6];//0XDB;设备地址
			array_Send_buff[7]=array_Drive_control[7];//功能码
			switch(array_Drive_control[7])//判断功能码
			{
				case 0x01://读线圈(MBAP 功能码 起始地址H 起始地址L 线圈数量H 线圈数量L)
					if(array_Drive_control[8]==0x20)//判断起始地址
					{
						switch(array_Drive_control[9])//判断起始地址
						{
							case 0x0A://原点回归完成标志

								break;
							
							case 0x64://伺服运动状态
								array_Send_buff[4]=0X00;//数据长度
								array_Send_buff[5]=0X05;//数据长度
								array_Send_buff[8]=0X01;//接下来数据的字节长度
								if(MOTION)//运动
								{
									array_Send_buff[9]=0X01;//数据						
								}
								else if(CEASE)//停止
								{
									array_Send_buff[9]=0X00;//数据
								}
								send(1,array_Send_buff,10);//向客户端发送	
								break;

							case 0x01://原点回归/停止
								array_Send_buff[4]=0X00;//数据长度
								array_Send_buff[5]=0X05;//数据长度
								array_Send_buff[8]=0X01;//接下来数据的字节长度
								if(CEASE&&REACH)//回零
								{
									array_Send_buff[9]=0X00;
								}
								else//回零中
								{
									array_Send_buff[9]=0X01;
								}													
								send(1,array_Send_buff,10);//向客户端发送		
								break;
							
							case 0x02://正转点动/停止
								
								break;
							
							case 0x03://反转点动/停止
							
								break;							
							
							case 0x04://相对运动/停止							
									
								break;
							
							case 0x05://绝对运动/停止	
								array_Send_buff[4]=0X00;//数据长度
								array_Send_buff[5]=0X05;//数据长度
								array_Send_buff[8]=0X01;//接下来数据的字节长度
								if(MOTION)//运动
								{
									array_Send_buff[9]=0X01;//数据								
								}
								else if(CEASE)//停止
								{
									array_Send_buff[9]=0X00;//数据
								}
								send(1,array_Send_buff,10);//向客户端发送								
								break;
							
							case 0x00://硬触发启动/停止
								array_Send_buff[4]=0X00;//数据长度
								array_Send_buff[5]=0X05;//数据长度
								array_Send_buff[8]=0X01;//接下来数据的字节长度						
								if(flag_Hard_trigger_start==1)//触发
								{
									array_Send_buff[9]=0X01;//数据						
								}
								else//停止
								{
									array_Send_buff[9]=0X00;//数据
								}
								send(1,array_Send_buff,10);//向客户端发送									
								break;
							
							case 0x06://伺服复位
								array_Send_buff[4]=0X00;//数据长度
								array_Send_buff[5]=0X05;//数据长度
								array_Send_buff[8]=0X01;//接下来数据的字节长度
								array_Send_buff[9]=0X01;												
								send(1,array_Send_buff,10);//向客户端发送		
								break;

							case 0x09://雷达上电/断电
								array_Send_buff[4]=0X00;//数据长度
								array_Send_buff[5]=0X06;//数据长度
								array_Send_buff[8]=0X01;//接下来数据的字节长度					
								if(RADAR_ON)//上电
								{
									array_Send_buff[9]=0X01;//数据						
								}
								else if(RADAR_OFF)//断电
								{
									array_Send_buff[9]=0X00;//数据
								}
								send(1,array_Send_buff,10);//向客户端发送									
								break;
							
							default:
								break;
						}
					}
					break;
				
				case 0x02://读离散量输入
					array_Send_buff[4]=0X00;//数据长度
					array_Send_buff[5]=0X04;//数据长度
					array_Send_buff[8]=0X01;//接下来数据的字节长度
					if(array_Drive_control[8]==0x00&&array_Drive_control[9]==0x01)//判断起始地址——正转极限光电状态
					{					
						if(HAL_GPIO_ReadPin(IN2_GPIO_Port,IN2_Pin)==GPIO_PIN_RESET)//到达限位
						{
							array_Send_buff[9]=0X01;//数据
							send(1,array_Send_buff,10);//向客户端发送	
						}
						else
						{
							array_Send_buff[9]=0X00;//数据
							send(1,array_Send_buff,10);//向客户端发送	
						}
					}						
					else if(array_Drive_control[8]==0x00&&array_Drive_control[9]==0x03)//判断起始地址——反转极限光电状态
					{	
						if(HAL_GPIO_ReadPin(IN0_GPIO_Port,IN0_Pin)==GPIO_PIN_RESET)//到达限位
						{
							array_Send_buff[9]=0X01;//数据
							send(1,array_Send_buff,10);//向客户端发送		
						}
						else
						{
							array_Send_buff[9]=0X00;//数据
							send(1,array_Send_buff,10);//向客户端发送	
						}						
					}
					else if(array_Drive_control[8]==0x00&&array_Drive_control[9]==0x04)//判断起始地址——零点限位触发状态	
					{
						if(HAL_GPIO_ReadPin(IN1_GPIO_Port,IN1_Pin)==GPIO_PIN_RESET)//到达限位
						{
							array_Send_buff[9]=0X01;//数据
							send(1,array_Send_buff,10);//向客户端发送		
						}
						else
						{
							array_Send_buff[9]=0X00;//数据
							send(1,array_Send_buff,10);//向客户端发送	
						}
					}
					else if(array_Drive_control[8]==0x00&&array_Drive_control[9]==0x05)//判断起始地址——伺服报警状态	
					{	
	
					}
					break;

				case 0x03://读保持寄存器(MBAP 功能码 起始地址H 起始地址L 寄存器数量H 寄存器数量L)
					number_Data_bytes=(array_Drive_control[10]<<8|array_Drive_control[11])*2;//大端传输数据处理——读取数据字节
					array_Send_buff[4]=0X00;//数据长度
					array_Send_buff[5]=0x03+number_Data_bytes;//数据长度
					if(array_Drive_control[8]==0x01)//判断起始地址
					{
						if(number_Data_bytes==0x02)
						{
							if(array_Drive_control[9]==0x4A)//判断起始地址——加速时间
							{
								array_Send_buff[8]=number_Data_bytes;//接下来数据的字节长度
								array_Send_buff[9]=0X00;//数据
								array_Send_buff[10]=0X00;//数据
								send(1,array_Send_buff,11);//向客户端发送							
							}
							else if(array_Drive_control[9]==0x4E)//判断起始地址——减速时间
							{
								array_Send_buff[8]=number_Data_bytes;//接下来数据的字节长度
								array_Send_buff[9]=0X00;//数据
								array_Send_buff[10]=0X00;//数据
								send(1,array_Send_buff,11);//向客户端发送														
							}
							else if(array_Drive_control[9]==0x00)//判断起始地址——实时位置上传控制
							{
								array_Send_buff[8]=number_Data_bytes;//接下来数据的字节长度
								array_Send_buff[9]=(variable_Position_upload>>8)&0xFF;//数据
								array_Send_buff[10]=variable_Position_upload&0xFF;//数据
								send(1,array_Send_buff,11);//向客户端发送																
							}
							else if(array_Drive_control[9]==0x30)//判断起始地址——硬触发个数
							{
								array_Send_buff[8]=number_Data_bytes;//接下来数据的字节长度
								array_Send_buff[9]=(number_Hard_trigger>>8)&0xFF;//数据
								array_Send_buff[10]=number_Hard_trigger&0xFF;//数据
								send(1,array_Send_buff,11);//向客户端发送								
							}
							else if(array_Drive_control[9]==0x32)//判断起始地址——当前硬触发个数
							{
								array_Send_buff[8]=number_Data_bytes;//接下来数据的字节长度
								array_Send_buff[9]=0X00;//数据
								array_Send_buff[10]=0X00;//数据
								send(1,array_Send_buff,11);//向客户端发送															
							}
							else if(array_Drive_control[9]==0x77)//判断起始地址——上电后是否执行过归零
							{
								array_Send_buff[8]=number_Data_bytes;//接下来数据的字节长度
								if(flag_Zero_regression==1)
								{
									array_Send_buff[9]=0X00;//数据
									array_Send_buff[10]=0X01;//数据
									send(1,array_Send_buff,11);//向客户端发送	
								}
								else if(flag_Zero_regression==0)
								{
									array_Send_buff[9]=0X00;//数据
									array_Send_buff[10]=0X00;//数据
									send(1,array_Send_buff,11);//向客户端发送	
								}									
							}
						}
						else if(number_Data_bytes==0x04)
						{
							if(array_Drive_control[9]==0x2C)//判断起始地址——起始硬触发位置
							{
								array_Send_buff[8]=number_Data_bytes;//接下来数据的字节长度
								array_Send_buff[11]=(variable_Hard_trigger_position>>24)&0xFF;//数据
								array_Send_buff[12]=(variable_Hard_trigger_position>>16)&0xFF;//数据
								array_Send_buff[9]=(variable_Hard_trigger_position>>8)&0xFF;//数据
								array_Send_buff[10]=variable_Hard_trigger_position&0xFF;//数据
								send(1,array_Send_buff,13);//向客户端发送															
							}
							else if(array_Drive_control[9]==0x2E)//判断起始地址——硬触发间隔
							{
								array_Send_buff[8]=number_Data_bytes;//接下来数据的字节长度
								array_Send_buff[11]=(variable_Hard_trigger_interval>>24)&0xFF;//寄存器1H-数据
								array_Send_buff[12]=(variable_Hard_trigger_interval>>16)&0xFF;//寄存器1L数据
								array_Send_buff[9]=(variable_Hard_trigger_interval>>8)&0xFF;//寄存器2H数据
								array_Send_buff[10]=variable_Hard_trigger_interval&0xFF;//寄存器2L数据
								send(1,array_Send_buff,13);//向客户端发送														
							}
						}
					}
					
					else if(array_Drive_control[8]==0x00)//判断起始地址
					{
						if(number_Data_bytes==0x02)
						{
							if(array_Drive_control[9]==0x20)//判断起始地址——伺服错误代码
							{
								array_Send_buff[8]=number_Data_bytes;//接下来数据的字节长度
								array_Send_buff[9]=0X00;//数据
								array_Send_buff[10]=0X00;//数据
								send(1,array_Send_buff,11);//向客户端发送								
							}
							else if(array_Drive_control[9]==0x96)//判断起始地址——PLC错误代码
							{
								array_Send_buff[8]=number_Data_bytes;//接下来数据的字节长度
								array_Send_buff[9]=0X00;//数据
								array_Send_buff[10]=0X00;//数据
								send(1,array_Send_buff,11);//向客户端发送														
							}
						}
						else if(number_Data_bytes==0x04)
						{
							if(array_Drive_control[9]==0x64)//判断起始地址——回零速度
							{
								array_Send_buff[8]=number_Data_bytes;//接下来数据的字节长度
								array_Send_buff[11]=(variable_Zero_return_speed_value>>24)&0xFF;//数据
								array_Send_buff[12]=(variable_Zero_return_speed_value>>16)&0xFF;//数据
								array_Send_buff[9]=(variable_Zero_return_speed_value>>8)&0xFF;//数据
								array_Send_buff[10]=variable_Zero_return_speed_value&0xFF;//数据
								send(1,array_Send_buff,13);//向客户端发送															
							}
							else if(array_Drive_control[9]==0x68)//判断起始地址——回零蠕变速度
							{
								array_Send_buff[8]=number_Data_bytes;//接下来数据的字节长度
								array_Send_buff[11]=(variable_Zero_return_creep_speed_value>>24)&0xFF;//寄存器1H-数据
								array_Send_buff[12]=(variable_Zero_return_creep_speed_value>>16)&0xFF;//寄存器1L-数据
								array_Send_buff[9]=(variable_Zero_return_creep_speed_value>>8)&0xFF;//寄存器2H-数据
								array_Send_buff[10]=variable_Zero_return_creep_speed_value&0xFF;//寄存器2L-数据
								send(1,array_Send_buff,13);//向客户端发送									
							}
							else if(array_Drive_control[9]==0x78)//判断起始地址——运行速度
							{		
								array_Send_buff[8]=number_Data_bytes;//接下来数据的字节长度
								array_Send_buff[11]=(variable_Running_speed_value>>24)&0xFF;//寄存器1H-数据
								array_Send_buff[12]=(variable_Running_speed_value>>16)&0xFF;//寄存器1L-数据
								array_Send_buff[9]=(variable_Running_speed_value>>8)&0xFF;//寄存器2H-数据
								array_Send_buff[10]=variable_Running_speed_value&0xFF;//寄存器2L-数据
								send(1,array_Send_buff,13);//向客户端发送															
							}
							else if(array_Drive_control[9]==0x7A)//判断起始地址——相对定位距离
							{
								array_Send_buff[8]=number_Data_bytes;//接下来数据的字节长度
								array_Send_buff[11]=0X00;//寄存器1H-数据
								array_Send_buff[12]=0X00;//寄存器1L-数据
								array_Send_buff[9]=0X00;//寄存器2H-数据
								array_Send_buff[10]=0X00;//寄存器2L-数据
								send(1,array_Send_buff,13);//向客户端发送														
							}
							else if(array_Drive_control[9]==0x7C)//判断起始地址——绝对定位位置
							{
								array_Send_buff[8]=number_Data_bytes;//接下来数据的字节长度
								array_Send_buff[11]=(variable_Target_position>>24)&0xFF;//寄存器1H-数据
								array_Send_buff[12]=(variable_Target_position>>16)&0xFF;//寄存器1L-数据
								array_Send_buff[9]=(variable_Target_position>>8)&0xFF;//寄存器2H-数据
								array_Send_buff[10]=variable_Target_position&0xFF;//寄存器2L-数据
								send(1,array_Send_buff,13);//向客户端发送														
							}		
							else if(array_Drive_control[9]==0x7E)//判断起始地址——设备最大运行行程
							{
								array_Send_buff[8]=number_Data_bytes;//接下来数据的字节长度
								array_Send_buff[9]=0X00;//寄存器1H-数据
								array_Send_buff[10]=0X00;//寄存器1L-数据
								array_Send_buff[11]=0X00;//寄存器2H-数据
								array_Send_buff[12]=0X00;//寄存器2L-数据
								send(1,array_Send_buff,13);//向客户端发送															
							}	
							else if(array_Drive_control[9]==0x80)//判断起始地址——零点位置坐标
							{
								array_Send_buff[8]=number_Data_bytes;//接下来数据的字节长度
								array_Send_buff[11]=(variable_Zero_position>>24)&0xFF;//寄存器1H-数据
								array_Send_buff[12]=(variable_Zero_position>>16)&0xFF;//寄存器1L-数据
								array_Send_buff[9]=(variable_Zero_position>>8)&0xFF;//寄存器2H-数据
								array_Send_buff[10]=variable_Zero_position&0xFF;//寄存器2L-数据
								send(1,array_Send_buff,13);//向客户端发送															
							}	
							else if(array_Drive_control[9]==0x87)//判断起始地址——实时位置主动读取
							{
								array_Send_buff[8]=number_Data_bytes;//接下来数据的字节长度
								array_Send_buff[11]=(variable_Position_upload>>24)&0xFF;//寄存器1H-数据
								array_Send_buff[12]=(variable_Position_upload>>16)&0xFF;//寄存器1L-数据
								array_Send_buff[9]=(variable_Position_upload>>8)&0xFF;//寄存器2H-数据
								array_Send_buff[10]=variable_Position_upload&0xFF;//寄存器2L-数据
								send(1,array_Send_buff,13);//向客户端发送															
							}	
						}							
					}
					break;
				
				case 0x05://写单个线圈(请求：MBAP 功能码 寄存器地址H 寄存器地址L 数据H(FF/00) 数据L(00))
					array_Send_buff[4]=0X00;//数据长度
					array_Send_buff[5]=0X06;//数据长度
					array_Send_buff[8]=array_Drive_control[8];//寄存器地址H
					array_Send_buff[9]=array_Drive_control[9];//寄存器地址H
					if(array_Drive_control[8]==0x20)//判断起始地址
					{
						switch(array_Drive_control[9])//判断起始地址
						{
							case 0x01://原点回归/停止
								if(array_Drive_control[10]==0xFF)//回归
								{
									if(AWAY&&CEASE)//不在原点且停止运动
									{
										array_Send_buff[10]=0XFF;
										array_Send_buff[11]=0X00;
										send(1,array_Send_buff,12);//向客户端发送	
										flag_Extreme_position=0;
										flag_Zero_regression=0;
										variable_Target_position=0;//绝对定位位置设置为0
										if(HAL_GPIO_ReadPin(IN0_GPIO_Port,IN0_Pin)==GPIO_PIN_RESET)//在头部-正向移动
										{
											array_Motor_drive_send[0]=GPIO_PIN_RESET;
											array_Motor_drive_send[1]=(variable_Zero_return_speed_value/1000)>>8&0xff;
											array_Motor_drive_send[2]=variable_Zero_return_speed_value/1000&0xff;
											osMessageQueuePut(Motor_driveQueueHandle,array_Motor_drive_send,1,0);	
										}
										else
										{
											array_Motor_drive_send[0]=GPIO_PIN_SET;
											array_Motor_drive_send[1]=(variable_Zero_return_speed_value/1000)>>8&0xff;
											array_Motor_drive_send[2]=variable_Zero_return_speed_value/1000&0xff;
											osMessageQueuePut(Motor_driveQueueHandle,array_Motor_drive_send,1,0);	
										}
									}											
								}
								else if(array_Drive_control[10]==0x00)//停止
								{
									array_Send_buff[10]=0X00;
									array_Send_buff[11]=0X00;
									send(1,array_Send_buff,12);//向客户端发送		
									DO_CEASE;//PA7-电机使能
									HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//关闭PWM输出														
								}
								break;
							
							case 0x02://正转点动/停止
								if(array_Drive_control[10]==0xFF)//开始
								{
									array_Send_buff[10]=0XFF;
									array_Send_buff[11]=0X00;
									send(1,array_Send_buff,12);//向客户端发送		
									array_Motor_drive_send[0]=GPIO_PIN_RESET;
									array_Motor_drive_send[1]=(variable_Running_speed_value/1000)>>8&0xff;
									array_Motor_drive_send[2]=variable_Running_speed_value/1000&0xff;
									osMessageQueuePut(Motor_driveQueueHandle,array_Motor_drive_send,1,0);
								}
								else//停止
								{
									array_Send_buff[10]=0X00;
									array_Send_buff[11]=0X00;
									send(1,array_Send_buff,12);//向客户端发送		
//									DO_CEASE;//PA7-电机使能
//									HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//关闭PWM输出
									flag_Motor_deceleration_stops=3;									
								}
								break;
							
							case 0x03://反转点动/停止
								if(array_Drive_control[10]==0xFF)//开始
								{
									array_Send_buff[10]=0XFF;
									array_Send_buff[11]=0X00;
									send(1,array_Send_buff,12);//向客户端发送	
									array_Motor_drive_send[0]=GPIO_PIN_SET;
									array_Motor_drive_send[1]=(variable_Running_speed_value/1000)>>8&0xff;
									array_Motor_drive_send[2]=variable_Running_speed_value/1000&0xff;
									osMessageQueuePut(Motor_driveQueueHandle,array_Motor_drive_send,1,0);								
								}
								else//停止
								{
									array_Send_buff[10]=0X00;
									array_Send_buff[11]=0X00;
									send(1,array_Send_buff,12);//向客户端发送		
//									DO_CEASE;//PA7-电机使能
//									HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//关闭PWM输出	
									flag_Motor_deceleration_stops=3;									
								}							
								break;							
							
							case 0x04://相对运动/停止
								if(array_Drive_control[10]==0xFF)//开始
								{
									array_Send_buff[10]=0XFF;
									array_Send_buff[11]=0X00;
									send(1,array_Send_buff,12);//向客户端发送		
									if(CEASE)
									{
										array_Motor_drive_send[0]=GPIO_PIN_RESET;
										array_Motor_drive_send[1]=(variable_Running_speed_value/1000)>>8&0xff;
										array_Motor_drive_send[2]=variable_Running_speed_value/1000&0xff;
										osMessageQueuePut(Motor_driveQueueHandle,array_Motor_drive_send,1,0);
									}
								}
								else//停止
								{
									array_Send_buff[10]=0X00;
									array_Send_buff[11]=0X00;
									send(1,array_Send_buff,12);//向客户端发送	
//									DO_CEASE;//PA7-电机使能
//									HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//关闭PWM输出
									flag_Motor_deceleration_stops=3;
								}								
								break;
							
							case 0x05://绝对运动/停止
								if(array_Drive_control[10]==0xFF)//开始
								{
									array_Send_buff[10]=0XFF;
									array_Send_buff[11]=0X00;
									send(1,array_Send_buff,12);//向客户端发送	
									if(CEASE&&flag_Extreme_position==0)
									{
										if(variable_Position_upload*10<variable_Target_position||variable_Position<0)//当前位置小于目标位置或当前位置小于0-正向移动
										{
											array_Motor_drive_send[0]=GPIO_PIN_RESET;
											array_Motor_drive_send[1]=(variable_Running_speed_value/1000)>>8&0xff;
											array_Motor_drive_send[2]=variable_Running_speed_value/1000&0xff;
											osMessageQueuePut(Motor_driveQueueHandle,array_Motor_drive_send,1,0);
										}
										else if(variable_Position_upload*10>variable_Target_position&&AWAY)//当前位置大于目标位置且不在原点-反向移动
										{
											array_Motor_drive_send[0]=GPIO_PIN_SET;
											array_Motor_drive_send[1]=(variable_Running_speed_value/1000)>>8&0xff;
											array_Motor_drive_send[2]=variable_Running_speed_value/1000&0xff;
											osMessageQueuePut(Motor_driveQueueHandle,array_Motor_drive_send,1,0);
										}
									}
								}
								else//停止
								{
									array_Send_buff[10]=0X00;
									array_Send_buff[11]=0X00;
									send(1,array_Send_buff,12);//向客户端发送		
//									DO_CEASE;//PA7-电机使能
//									HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//关闭PWM输出
									flag_Motor_deceleration_stops=3;
								}							
								break;
							
							case 0x14://停止运动状态
								if(array_Drive_control[10]==0xFF)//停止
								{
//									DO_CEASE;//PA7-电机使能
//									HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//关闭PWM输出
									flag_Motor_deceleration_stops=3;
									array_Send_buff[10]=0XFF;
									array_Send_buff[11]=0X00;
								}
								else
								{
									array_Send_buff[10]=0X00;
									array_Send_buff[11]=0X00;
								}
								send(1,array_Send_buff,12);//向客户端发送								
								break;
							
							case 0x00://硬触发启动/停止
								if(array_Drive_control[10]==0xFF)//开始
								{
									flag_Hard_trigger_start=1;
									array_Send_buff[10]=0XFF;
									array_Send_buff[11]=0X00;							
								}
								else//停止
								{
									flag_Hard_trigger_start=0;
									array_Send_buff[10]=0X00;
									array_Send_buff[11]=0X00;
								}
								send(1,array_Send_buff,12);//向客户端发送								
								break;
							
							case 0x06://伺服复位
								if(array_Drive_control[10]==0xFF)
								{
									array_Send_buff[10]=0XFF;
									array_Send_buff[11]=0X00;
									send(1,array_Send_buff,12);//向客户端发送		
								}
								else if(array_Drive_control[10]==0x00)
								{
									array_Send_buff[10]=0X00;
									array_Send_buff[11]=0X00;
									send(1,array_Send_buff,12);//向客户端发送											
								}
								break;
							
							case 0x07://伺服急停
								if(array_Drive_control[10]==0xFF)
								{
									array_Send_buff[10]=0XFF;
									array_Send_buff[11]=0X00;
								}
								else
								{
									array_Send_buff[10]=0X00;
									array_Send_buff[11]=0X00;
								}
								send(1,array_Send_buff,12);//向客户端发送		
								break;

							case 0x09://雷达上电/断电
								if(array_Drive_control[10]==0xFF)
								{
									array_Send_buff[10]=0XFF;
									array_Send_buff[11]=0X00;
									DO_RADAR_ON;
								}
								else
								{
									array_Send_buff[10]=0X00;
									array_Send_buff[11]=0X00;
									DO_RADAR_OFF;
								}
								send(1,array_Send_buff,12);//向客户端发送									
								break;
							
							case 0x0F://PLC错误清除							
								if(array_Drive_control[10]==0xFF)
								{
									array_Send_buff[10]=0XFF;
									array_Send_buff[11]=0X00;
								}
								else
								{
									array_Send_buff[10]=0X00;
									array_Send_buff[11]=0X00;
								}
								send(1,array_Send_buff,12);//向客户端发送		
								break;
							
							default:
								break;													
						}
					}
					break;
				
				case 0x06://写单个保持寄存器(请求：MBAP 功能码 寄存器地址H 寄存器地址L 寄存器值H 寄存器值L)
//					if(CEASE)//未开始运动
//					{
						array_Send_buff[4]=0X00;//数据长度
						array_Send_buff[5]=0X06;//数据长度
						array_Send_buff[8]=array_Drive_control[8];//寄存器地址
						array_Send_buff[9]=array_Drive_control[9];//寄存器地址
						array_Send_buff[10]=array_Drive_control[10];//寄存器值H
						array_Send_buff[11]=array_Drive_control[11];//寄存器值L
						if(array_Drive_control[8]==0x01)//判断起始地址
						{
							if(array_Drive_control[9]==0x4A)//判断起始地址——加速时间
							{						
								send(1,array_Send_buff,12);//向客户端发送													
							}
							else if(array_Drive_control[9]==0x4E)//判断起始地址——减速时间
							{
								send(1,array_Send_buff,12);//向客户端发送													
							}
							else if(array_Drive_control[9]==0x00)//判断起始地址——实时位置上传控制
							{
								if(array_Drive_control[10]||array_Drive_control[11])
								{
									flag_Position_upload=1;//开始实时位置上传		
								}
								else
								{
									flag_Position_upload=0;//停止实时位置上传
								}
								send(1,array_Send_buff,12);//向客户端发送												
							}
							else if(array_Drive_control[9]==0x30)//判断起始地址——硬触发个数
							{
								number_Hard_trigger=(array_Drive_control[10]<<8|array_Drive_control[11]);//大端传输数据处理——硬触发个数
								send(1,array_Send_buff,12);//向客户端发送												
							}					
						}
//					}										
					break;
					
				case 0x10://写多个保持寄存器(请求：MBAP(7位) 功能码 起始地址H 起始地址L 寄存器数量H 寄存器数量L 字节长度 寄存器值)
//				if(CEASE)//未开始运动
//				{
						array_Send_buff[4]=0X00;//数据长度
						array_Send_buff[5]=0X06;//数据长度
						array_Send_buff[8]=array_Drive_control[8];//寄存器起始地址H
						array_Send_buff[9]=array_Drive_control[9];//寄存器起始地址L
						array_Send_buff[10]=array_Drive_control[10];//寄存器数量H
						array_Send_buff[11]=array_Drive_control[11];//寄存器数量L
						if(array_Drive_control[8]==0x00)//判断起始地址
						{
							if(array_Drive_control[9]==0x64)//判断起始地址——回零速度
							{
								variable_Zero_return_speed_value=(array_Drive_control[13]<<8|array_Drive_control[14]|array_Drive_control[15]<<24|array_Drive_control[16]<<16);//大端传输数据处理——回零速度0.001mm/s
								send(1,array_Send_buff,12);//向客户端发送																
							}
							else if(array_Drive_control[9]==0x68)//判断起始地址——回零蠕变速度
							{
								variable_Zero_return_creep_speed_value=(array_Drive_control[13]<<8|array_Drive_control[14]|array_Drive_control[15]<<24|array_Drive_control[16]<<16);//大端传输数据处理——回零蠕变速度0.001mm/s
								send(1,array_Send_buff,12);//向客户端发送									
							}
							else if(array_Drive_control[9]==0x78)//判断起始地址——运行速度
							{
//								variable_Running_speed_value=(array_Drive_control[13]<<24|array_Drive_control[14]<<16|array_Drive_control[15]<<8|array_Drive_control[16]);//大端传输数据处理——运行速度0.001mm/s
								variable_Running_speed_value=(array_Drive_control[13]<<8|array_Drive_control[14]|array_Drive_control[15]<<24|array_Drive_control[16]<<16);//大端传输数据处理——运行速度0.001mm/s
								send(1,array_Send_buff,12);//向客户端发送	
							}							
							else if(array_Drive_control[9]==0x7A)//判断起始地址——相对定位距离
							{
								send(1,array_Send_buff,12);//向客户端发送															
							}
							else if(array_Drive_control[9]==0x7C)//判断起始地址——绝对定位位置
							{
								flag_Absolute_position=1;
								variable_Target_position=(array_Drive_control[13]<<8|array_Drive_control[14]|array_Drive_control[15]<<24|array_Drive_control[16]<<16);//大端传输数据处理——绝对定位位置0.001mm
								if(variable_Target_position==0x00)
								{
									flag_Extreme_position=0;
									flag_Zero_regression=0;
								}
								send(1,array_Send_buff,12);//向客户端发送													
							}		
							else if(array_Drive_control[9]==0x7E)//判断起始地址——设备最大运行行程
							{
								send(1,array_Send_buff,12);//向客户端发送															
							}	
							else if(array_Drive_control[9]==0x80)//判断起始地址——零点位置坐标
							{
								variable_Zero_position=(array_Drive_control[13]<<8|array_Drive_control[14]|array_Drive_control[15]<<24|array_Drive_control[16]<<16);//大端传输数据处理——零点位置0.001mm
								send(1,array_Send_buff,12);//向客户端发送														
							}	
						}
						else if(array_Drive_control[8]==0x01)//判断起始地址
						{
							if(array_Drive_control[9]==0x2C)//判断起始地址——起始硬触发位置
							{
								variable_Hard_trigger_position=(array_Drive_control[13]<<8|array_Drive_control[14]|array_Drive_control[15]<<24|array_Drive_control[16]<<16);//大端传输数据处理——起始硬触发位置0.001mm
								variable_Position_lastpulsec=variable_Hard_trigger_position;
								send(1,array_Send_buff,12);//向客户端发送														
							}
							else if(array_Drive_control[9]==0x2E)//判断起始地址——硬触发间隔
							{
								variable_Hard_trigger_interval=(array_Drive_control[13]<<8|array_Drive_control[14]|array_Drive_control[15]<<24|array_Drive_control[16]<<16);//大端传输数据处理——硬触发间隔0.001mm
								send(1,array_Send_buff,12);//向客户端发送																					
							}				
						}	
//					}						
					break;
					
				default:
					break;						
			}	
			memset(array_Send_buff,0,13);			
			memset(array_Drive_control,0,20);		
		}
  }
}


void Motor_drive(void *argument)//电机加速启动
{
	uint8_t  status=0;	
	uint16_t number_v=0;
	uint16_t number_Reloading;
	uint16_t number_speed;
	uint8_t  array_Motor_drive_receive[3];//电机驱动数组
	for(;;)
	{    
		__HAL_TIM_CLEAR_IT(&htim8, TIM_IT_UPDATE);
		HAL_TIM_Base_Start_IT(&htim8);//开启TIM8定时器中断（20KHZ，周期50us）
		status=osMessageQueueGet(Motor_driveQueueHandle,array_Motor_drive_receive,NULL,0);
		if(status==osOK)
		{	
			number_speed=array_Motor_drive_receive[1]<<8|array_Motor_drive_receive[2];
			DO_MOTION;//PA7-电机使能
			osDelay(1);	
			HAL_GPIO_WritePin(TIM8_CH2_GPIO_Port,TIM8_CH2_Pin,(GPIO_PinState)array_Motor_drive_receive[0]);//PC7-电机方向控制(GPIO_PIN_RESET:顺时针，GPIO_PIN_SET:逆时针）
			osDelay(1);	
			while(number_speed>number_v&&MOTION)
			{
				number_v++;
				number_Reloading=(One_Circle_Distance*1000)/number_v;//自动重装载值n=(One_Circle_Distance*1000000*1000)/(variable_Running_speed_value*Subtraction)
				__HAL_TIM_SET_AUTORELOAD(&htim8,number_Reloading-1);//修改自动重装载值
				__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, (number_Reloading/2)-1);//修改占空比
				HAL_TIM_PWM_Start_IT(&htim8,TIM_CHANNEL_1);//启动TIM8_CH1的PWM波输出（中断）
				osDelay(1);
			}
			number_v=0;	
		}
		memset(array_Motor_drive_receive,0,3);			
	}
}


void Encoder(void *argument)//电动机编码器通讯
{
	uint8_t Real_time_location_array[6]={0xCF,0xAE,0x00,0x00,0x00,0x00};//实时位置数组
  for(;;)
  {
		if(flag_Encoder_processing==1)
		{
			flag_Encoder_processing=0;
			if(REACH)//原点校准
			{	
				variable_Position=0;
				variable_Position_upload=0;
			}	
			else if(AWAY)
			{
				variable_Position = ((float)number_Motor_pulsec/Subtraction)*One_Circle_Distance;	//距原点距离
				variable_Position_upload = variable_Position*100;//位置上传数据（单位0.01mm）
				if(flag_Zero_regression==1&&flag_Absolute_position==1)//之前回过零且接收到绝对位置
				{
					if(COROTATION)//正向移动
					{
						if(flag_Motor_deceleration_stops==0&&variable_Position_upload*10<variable_Target_position&&variable_Position_upload*10>=variable_Target_position-25000)
						{
							flag_Motor_deceleration_stops=1;
						}
						else if(flag_Motor_deceleration_stops==2&&variable_Position_upload*10>=variable_Target_position)
						{
							DO_CEASE;//PA7-电机使能
							HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//关闭PWM输出
							flag_Absolute_position=0;	
							flag_Motor_deceleration_stops=0;
						}	
					}
					else if(REVERSAL)//反向移动
					{
						if(flag_Motor_deceleration_stops==0&&variable_Position_upload*10>variable_Target_position&&variable_Position_upload*10<=variable_Target_position+25000)
						{
							flag_Motor_deceleration_stops=1;
						}
						else if(flag_Motor_deceleration_stops==2&&variable_Position_upload*10<=variable_Target_position)
						{
							DO_CEASE;//PA7-电机使能
							HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//关闭PWM输出
							flag_Absolute_position=0;	
							flag_Motor_deceleration_stops=0;
						}	
					}					
				}	
			}
			if(flag_Position_upload==1)//实时位置上传
			{				
				Real_time_location_array[2]=variable_Position_upload&0xFF;
				Real_time_location_array[3]=(variable_Position_upload>>8)&0xFF;
				Real_time_location_array[4]=(variable_Position_upload>>16)&0xFF;
				Real_time_location_array[5]=(variable_Position_upload>>24)&0xFF;
				send(2,Real_time_location_array,6);//向客户端发送	
			}	
			if(flag_Hard_trigger==1)
			{
				variable_Trigger_interval=variable_Position*1000-variable_Position_lastpulsec;
				if(variable_Trigger_interval>=variable_Hard_trigger_interval*10)//.。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。
				{
					variable_Position_lastpulsec=variable_Position_lastpulsec+variable_Hard_trigger_interval*10;//.。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。
					flag_Hard_trigger=0;
					variable_Trigger_interval=0;					
				}			
			}
		}	
  }
}

void Pulse_Emission(void *argument)//脉冲触发（TIM13通道）
{	
  for(;;)
  {
//		osDelay(1);
		if(flag_Hard_trigger_start==1&&variable_Position_upload>=variable_Hard_trigger_position&&MOTION)//硬触发条件(硬触发启动，到达硬触发位置，运动状态)
		{
			if(flag_Hard_trigger==0)
			{
				if(number_Hard_trigger_curren<number_Hard_trigger)//硬触发次数判定
				{		
					number_Hard_trigger_curren++;
					HAL_GPIO_WritePin(PULSE_GPIO_Port, PULSE_Pin, GPIO_PIN_SET);//PA2-硬触发
					__HAL_TIM_CLEAR_IT(&htim13, TIM_IT_UPDATE);//**********************************清除更新中断请求位,防止开启中断就进入中断*******************************
					HAL_TIM_Base_Start_IT(&htim13);//开启TIM13定时器中断（1MHZ，计数50次，50us）
					flag_Hard_trigger=1;
				}
				else
				{
					number_Hard_trigger_curren=0;
					flag_Hard_trigger_start=0;
				}
			}
		}
		else if(flag_Hard_trigger_start==0)
		{
			number_Hard_trigger_curren=0;
		}
  }
}


void Extreme_position_Drive(void *argument)//极限位置控制
{
  for(;;)
  {
		osDelay(1);
		if(flag_Extreme_position==1)
		{
			DO_CEASE;//PA7-电机使能
			HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//关闭PWM输出
		}
  }
}


void Uart3_Communicate(void *argument)//串口通讯
{
	const uint8_t str_date[12] = __DATE__; //示例 "Mar 23 2022"  
	const uint8_t str_time[10] = __TIME__;
	uint8_t uart_receive_buff[3];
	uint8_t	VERSION[7];
	uint8_t y1,y2,d1,d2,t1,t2;
  for(;;)
  {
		osDelay(1);
		HAL_UART_Receive_IT(&huart3, uart_receive_buff, 3);
		VERSION[0] = 0XAA;
		VERSION[1] = 0X0C;//轨道号
		y1 = str_date[9] - '0';
		y2 = str_date[10] - '0';
		VERSION[2] = y1 << 4 | y2;
		switch(str_date[2])
    {
        case 'n': //Jan和jun
           VERSION[3] = str_date[1] == 'a' ? 1 : 6; //判断1月还是6月
            break;

        case 'b': //Feb
            VERSION[3] = 2;
            break;

        case 'r': //Mart和Apr
            VERSION[3] = str_date[1] == 'a' ? 3 : 4; //判断3月还是4月
            break;

        case 'y': //May
            VERSION[3] = 5;
            break;

        case 'l': //Jul
            VERSION[3] = 7;
            break;

        case 'g': //Aug
            VERSION[3] = 8;
            break;

        case 'p': //Sep
            VERSION[3] = 9;
            break;

        case 't': //Oct
            VERSION[3] = 0X10; //10
            break;

        case 'v': //Nov
            VERSION[3] = 0X11; //11
            break;

        case 'c': //Dec
            VERSION[3] = 0X12; //12
            break;
    }
		d1 = str_date[4] - '0'; //日期 十位
    d2 = str_date[5] - '0'; //日期 个位
		VERSION[4] = d1 << 4 | d2;
		t1 = str_time[0] - '0'; //小时 十位
    t2 = str_time[1] - '0'; //小时 个位
		VERSION[5] = t1 << 4 | t2;
		VERSION[6] =0xBB;

		if(uart_receive_buff[0]==0XAA && uart_receive_buff[1]==0X01 && uart_receive_buff[2]==0XBB)
		{
			HAL_UART_Transmit(&huart3, VERSION, 7, 10);
		}
		memset(uart_receive_buff, 0, 3);	
  }
}


void Iwdg_function(void *argument)//看门狗函数
{
  for(;;)
  {
		osDelay(1);	
		if(number_Iwdg<40)//当等于40时，则说明20min没有上位机控制信号，则单片机重启
		{	
			number_Iwdg++;
			HAL_IWDG_Refresh(&hiwdg);//最大喂狗时间32s
			osDelay(30000);//30s
		}		
	}
}


void Motor_deceleration_stops(void *argument)//电机减速停止
{
  for(;;)
  {
		uint16_t number_Reloading;
		uint16_t number_v=0;
		osDelay(1);	
		if(flag_Motor_deceleration_stops==1)//电机开始减速停止
		{	
			flag_Motor_deceleration_stops=2;
			number_v=variable_Running_speed_value/1000;
			while(number_v>10&&MOTION)
			{
				number_v--;
				number_Reloading=5000/number_v;
				__HAL_TIM_SET_AUTORELOAD(&htim8,number_Reloading-1);//修改自动重装载值
				__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, (number_Reloading/2)-1);//修改占空比
				HAL_TIM_PWM_Start_IT(&htim8,TIM_CHANNEL_1);//启动TIM8_CH1的PWM波输出（中断）
				osDelay(1);
			}	
		}	
		else if(flag_Motor_deceleration_stops==3)
		{	
			number_v=variable_Running_speed_value/1000;
			while(number_v>1&&MOTION)
			{
				number_v--;
				number_Reloading=5000/number_v;
				__HAL_TIM_SET_AUTORELOAD(&htim8,number_Reloading-1);//修改自动重装载值
				__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, (number_Reloading/2)-1);//修改占空比
				HAL_TIM_PWM_Start_IT(&htim8,TIM_CHANNEL_1);//启动TIM8_CH1的PWM波输出（中断）
				osDelay(1);
			}	
			DO_CEASE;//PA7-电机使能
			HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//关闭PWM输出
			flag_Motor_deceleration_stops=0;
		}
	}
}

/* USER CODE END Application */

