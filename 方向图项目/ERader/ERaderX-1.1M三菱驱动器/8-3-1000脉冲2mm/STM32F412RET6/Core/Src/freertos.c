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
wiz_NetInfo gWIZNETINFO = {
  .mac = { 0x80, 0x80, 0x80,0x80,0x80,0x80 },//MSB - LSB
  .ip  = { 192, 168, 1, 11 },
  .sn  = { 255, 255, 255, 0 },
  .gw  = { 192, 168, 1, 1 },//网关参数
  .dns  = { 8, 8, 8, 8 },
  .dhcp = NETINFO_STATIC };

uint8_t receive_buff[RECEIVE_BUFF_SIZE];

	
//Motor motor1;
	
float Position=0;//距0点位置,单位mm
uint32_t Position_upload=0;//距0点距离上传值（单位0.001mm）
uint32_t zero_return_speed_value=20000;//回零速度（单位0.001mm/s）
uint16_t pwmVal_TIM8 = 0;  //占空比大小(CCRx的大小，高电平时间)
uint8_t Position_flag=0;//原点位置标志位
uint8_t Sports_status_flag=0;//运动状态标志位
uint8_t Extreme_position_flag=0;//极限位置标志位
uint8_t Position_upload_flag=1;//位置上传标志位
uint8_t Encoder_Processing_flag=0;//编码器数据处理标志位
uint8_t Zero_regression_flag=0;//是否回零过标志位
uint8_t Absolute_position_flag=0;//绝对位置标志位
uint8_t Servo_reset_flag=0;//伺服复位标志位
float Track_Length=0;//轨道长，单位毫米
uint32_t Target_Position=0;//目标位置
uint32_t Zero_Position=0;//零点位置
uint8_t Sports_mode=0;//运动模式
uint16_t Hard_Trigger_Number=0;//硬触发个数
uint16_t Hard_Trigger_Current_Number=0;//硬触发当前个数
uint32_t Hard_Trigger_Interval=0;//硬触发时间间隔
uint32_t Hard_Trigger_Position=0;//硬触发开始位置
uint8_t Hard_Trigger_start_flag=0;//硬触发开始标志位
uint8_t Hard_Trigger_flag=0;//硬触发标志位
uint32_t Trigger_Interval;//触发间隔
float Position_lastpulsec;//上次硬触发位置
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

osThreadId_t Servo_resetTaskHandle;
const osThreadAttr_t Servo_resetTask_attributes = {
  .name = "Servo_resetTask",
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
void Motor_drive(void *argument);//电动驱动
void Pulse_Emission(void *argument);//脉冲发射
void Extreme_position_Drive(void *argument);//极限位置控制
void Uart3_Communicate(void *argument);//极限位置控制
void Servo_reset(void *argument);//伺服复位

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
	Servo_resetTaskHandle = osThreadNew(Servo_reset, NULL, &Servo_resetTask_attributes);
	
	Ethernet_ReceiveHandle = osMessageQueueNew (5, 20*sizeof(uint8_t), &Ethernet_Receive_attributes);
	Ethernet_SendHandle = osMessageQueueNew (5, 20*sizeof(uint8_t), &Ethernet_Send_attributes);
	Motor_driveQueueHandle = osMessageQueueNew (5, 3*sizeof(uint8_t), &Motor_driveQueue_attributes);
	
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
	if(HAL_GPIO_ReadPin(IN1_GPIO_Port,IN1_Pin)==GPIO_PIN_RESET)//在原点
	{
		Position=0;
		Position_flag=1;
		Zero_regression_flag=1;
		Motor_pulsec_ount=0;
		HAL_GPIO_WritePin(TIM8_CH2_GPIO_Port,TIM8_CH2_Pin,GPIO_PIN_RESET);
	}
	else 
	{
		Motor_pulsec_ount=0xffffff;
		Position=1499;
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
		osDelay(20);
  }
}


void Drive(void *argument)//控制驱动器
{
	uint8_t	send_buff[13];
	uint8_t status=0;	
	uint32_t running_speed_value=2000;//运行速度
	uint32_t zero_return_creep_speed_value=0;//回零蠕变速度
	uint8_t Drive_control_array[20];	//驱动器控制数组
	uint16_t Data_Bytes=0;//数据字节
	uint8_t Motor_drive_send[3];//电机驱动数组
  for(;;)
  {
		osDelay(1);	
		status=osMessageQueueGet(Ethernet_ReceiveHandle,Drive_control_array,NULL,10);
		if(status==osOK)
		{					
			send_buff[0]=Drive_control_array[0];//0X19;事务处理标识
			send_buff[1]=Drive_control_array[1];//0XB2;事务处理标识
			send_buff[2]=0X00;//协议标识符，00 00为modbus协议
			send_buff[3]=0X00;//协议标识符，00 00为modbus协议
			send_buff[6]=Drive_control_array[6];//0XDB;设备地址
			send_buff[7]=Drive_control_array[7];//功能码
			switch(Drive_control_array[7])//判断功能码
			{
				case 0x01://读线圈(MBAP 功能码 起始地址H 起始地址L 线圈数量H 线圈数量L)
					if(Drive_control_array[8]==0x20)//判断起始地址
					{
						switch(Drive_control_array[9])//判断起始地址
						{
							case 0x0A://原点回归完成标志

								break;
							
							case 0x64://伺服运动状态
								send_buff[4]=0X00;//数据长度
								send_buff[5]=0X05;//数据长度
								send_buff[8]=0X01;//接下来数据的字节长度
								if(Sports_status_flag==1)//运动
								{
									send_buff[9]=0X01;//数据						
								}
								else if(Sports_status_flag==0)//停止
								{
									send_buff[9]=0X00;//数据
								}
								send(1,send_buff,10);//向客户端发送	
								memset(send_buff,0,13);
								break;

							case 0x01://原点回归/停止
								send_buff[4]=0X00;//数据长度
								send_buff[5]=0X05;//数据长度
								send_buff[8]=0X01;//接下来数据的字节长度
								if(Sports_status_flag==1)//在原点
								{
									send_buff[9]=0X01;
								}
								else if(Sports_status_flag==0)//不在原点
								{
									send_buff[9]=0X00;
								}													
								send(1,send_buff,10);//向客户端发送	
								memset(send_buff,0,13);	
								break;
							
							case 0x02://正转点动/停止
								
								break;
							
							case 0x03://反转点动/停止
							
								break;							
							
							case 0x04://相对运动/停止							
									
								break;
							
							case 0x05://绝对运动/停止	
								send_buff[4]=0X00;//数据长度
								send_buff[5]=0X05;//数据长度
								send_buff[8]=0X01;//接下来数据的字节长度
								if(Sports_status_flag==1)//运动
								{
									send_buff[9]=0X01;//数据						
								}
								else if(Sports_status_flag==0)//停止
								{
									send_buff[9]=0X00;//数据
								}
								send(1,send_buff,10);//向客户端发送	
								memset(send_buff,0,13);								
								break;
							
							case 0x00://硬触发启动/停止
								send_buff[4]=0X00;//数据长度
								send_buff[5]=0X05;//数据长度
								send_buff[8]=0X01;//接下来数据的字节长度						
								if(Hard_Trigger_start_flag==1)//触发
								{
									send_buff[9]=0X01;//数据						
								}
								else//停止
								{
									send_buff[9]=0X00;//数据
								}
								send(1,send_buff,10);//向客户端发送	
								memset(send_buff,0,13);								
								break;
							
							case 0x06://伺服复位
								send_buff[4]=0X00;//数据长度
								send_buff[5]=0X05;//数据长度
								send_buff[8]=0X01;//接下来数据的字节长度
								if(Position_flag==1)//在原点
								{
									send_buff[9]=0X01;
								}
								else if(Position_flag==0)//不在原点
								{
									send_buff[9]=0X00;
								}													
								send(1,send_buff,10);//向客户端发送	
								memset(send_buff,0,13);	
								break;

							case 0x09://雷达上电/断电
								send_buff[4]=0X00;//数据长度
								send_buff[5]=0X06;//数据长度
								send_buff[8]=0X01;//接下来数据的字节长度					
//								if()//上电
//								{
//									send_buff[9]=0X01;//数据						
//								}
//								else//
//								{
//									send_buff[9]=0X00;//数据
//								}
								send(1,send_buff,10);//向客户端发送	
								memset(send_buff,0,13);								
								break;
							
							default:
								break;
						}
					}
					break;
				
				case 0x02://读离散量输入
					send_buff[4]=0X00;//数据长度
					send_buff[5]=0X04;//数据长度
					send_buff[8]=0X01;//接下来数据的字节长度
					if(Drive_control_array[8]==0x00&&Drive_control_array[9]==0x01)//判断起始地址——正转极限光电状态
					{					
						if(HAL_GPIO_ReadPin(IN2_GPIO_Port,IN2_Pin)==GPIO_PIN_RESET)//到达限位
						{
							send_buff[9]=0X01;//数据
							send(1,send_buff,10);//向客户端发送	
							memset(send_buff,0,13);	
						}
						else
						{
							send_buff[9]=0X00;//数据
							send(1,send_buff,10);//向客户端发送	
							memset(send_buff,0,13);	
						}
					}						
					else if(Drive_control_array[8]==0x00&&Drive_control_array[9]==0x03)//判断起始地址——反转极限光电状态
					{	
						if(HAL_GPIO_ReadPin(IN0_GPIO_Port,IN0_Pin)==GPIO_PIN_RESET)//到达限位
						{
							send_buff[9]=0X01;//数据
							send(1,send_buff,10);//向客户端发送	
							memset(send_buff,0,13);	
						}
						else
						{
							send_buff[9]=0X00;//数据
							send(1,send_buff,10);//向客户端发送	
							memset(send_buff,0,13);	
						}						
					}
					else if(Drive_control_array[8]==0x00&&Drive_control_array[9]==0x04)//判断起始地址——零点限位触发状态	
					{
						if(HAL_GPIO_ReadPin(IN1_GPIO_Port,IN1_Pin)==GPIO_PIN_RESET)//到达限位
						{
							send_buff[9]=0X01;//数据
							send(1,send_buff,10);//向客户端发送	
							memset(send_buff,0,13);	
						}
						else
						{
							send_buff[9]=0X00;//数据
							send(1,send_buff,10);//向客户端发送	
							memset(send_buff,0,13);	
						}
					}
					else if(Drive_control_array[8]==0x00&&Drive_control_array[9]==0x05)//判断起始地址——伺服报警状态	
					{	
	
					}
					break;

				case 0x03://读保持寄存器(MBAP 功能码 起始地址H 起始地址L 寄存器数量H 寄存器数量L)
					Data_Bytes=(Drive_control_array[10]<<8|Drive_control_array[11])*2;//大端传输数据处理——读取数据字节
					send_buff[4]=0X00;//数据长度
					send_buff[5]=0x03+Data_Bytes;//数据长度
					if(Drive_control_array[8]==0x01)//判断起始地址
					{
						if(Data_Bytes==0x02)
						{
							if(Drive_control_array[9]==0x4A)//判断起始地址——加速时间
							{
								send_buff[8]=Data_Bytes;//接下来数据的字节长度
								send_buff[9]=0X00;//数据
								send_buff[10]=0X00;//数据
								send(1,send_buff,11);//向客户端发送	
								memset(send_buff,0,13);							
							}
							else if(Drive_control_array[9]==0x4E)//判断起始地址——减速时间
							{
								send_buff[8]=Data_Bytes;//接下来数据的字节长度
								send_buff[9]=0X00;//数据
								send_buff[10]=0X00;//数据
								send(1,send_buff,11);//向客户端发送	
								memset(send_buff,0,13);														
							}
							else if(Drive_control_array[9]==0x00)//判断起始地址——实时位置上传控制
							{
								send_buff[8]=Data_Bytes;//接下来数据的字节长度
								send_buff[9]=(Position_upload>>8)&0xFF;//数据
								send_buff[10]=Position_upload&0xFF;//数据
								send(1,send_buff,11);//向客户端发送	
								memset(send_buff,0,13);															
							}
							else if(Drive_control_array[9]==0x30)//判断起始地址——硬触发个数
							{
								send_buff[8]=Data_Bytes;//接下来数据的字节长度
								send_buff[9]=(Hard_Trigger_Number>>8)&0xFF;//数据
								send_buff[10]=Hard_Trigger_Number&0xFF;//数据
								send(1,send_buff,11);//向客户端发送	
								memset(send_buff,0,13);								
							}
							else if(Drive_control_array[9]==0x32)//判断起始地址——当前硬触发个数
							{
								send_buff[8]=Data_Bytes;//接下来数据的字节长度
								send_buff[9]=0X00;//数据
								send_buff[10]=0X00;//数据
								send(1,send_buff,11);//向客户端发送	
								memset(send_buff,0,13);															
							}
							else if(Drive_control_array[9]==0x77)//判断起始地址——上电后是否执行过归零
							{
								send_buff[8]=Data_Bytes;//接下来数据的字节长度
								if(Zero_regression_flag==1)
								{
									send_buff[9]=0X00;//数据
									send_buff[10]=0X01;//数据
									send(1,send_buff,11);//向客户端发送	
									memset(send_buff,0,13);
								}
								else if(Zero_regression_flag==0)
								{
									send_buff[9]=0X00;//数据
									send_buff[10]=0X00;//数据
									send(1,send_buff,11);//向客户端发送	
									memset(send_buff,0,13);	
								}									
							}
						}
						else if(Data_Bytes==0x04)
						{
							if(Drive_control_array[9]==0x2C)//判断起始地址——起始硬触发位置
							{
								send_buff[8]=Data_Bytes;//接下来数据的字节长度
								send_buff[11]=(Hard_Trigger_Position>>24)&0xFF;//数据
								send_buff[12]=(Hard_Trigger_Position>>16)&0xFF;//数据
								send_buff[9]=(Hard_Trigger_Position>>8)&0xFF;//数据
								send_buff[10]=Hard_Trigger_Position&0xFF;//数据
								send(1,send_buff,13);//向客户端发送	
								memset(send_buff,0,13);														
							}
							else if(Drive_control_array[9]==0x2E)//判断起始地址——硬触发间隔
							{
								send_buff[8]=Data_Bytes;//接下来数据的字节长度
								send_buff[11]=(Hard_Trigger_Interval>>24)&0xFF;//寄存器1H-数据
								send_buff[12]=(Hard_Trigger_Interval>>16)&0xFF;//寄存器1L数据
								send_buff[9]=(Hard_Trigger_Interval>>8)&0xFF;//寄存器2H数据
								send_buff[10]=Hard_Trigger_Interval&0xFF;//寄存器2L数据
								send(1,send_buff,13);//向客户端发送	
								memset(send_buff,0,13);														
							}
						}
					}
					
					else if(Drive_control_array[8]==0x00)//判断起始地址
					{
						if(Data_Bytes==0x02)
						{
							if(Drive_control_array[9]==0x20)//判断起始地址——伺服错误代码
							{
								send_buff[8]=Data_Bytes;//接下来数据的字节长度
								send_buff[9]=0X00;//数据
								send_buff[10]=0X00;//数据
								send(1,send_buff,11);//向客户端发送	
								memset(send_buff,0,13);							
							}
							else if(Drive_control_array[9]==0x96)//判断起始地址——PLC错误代码
							{
								send_buff[8]=Data_Bytes;//接下来数据的字节长度
								send_buff[9]=0X00;//数据
								send_buff[10]=0X00;//数据
								send(1,send_buff,11);//向客户端发送	
								memset(send_buff,0,13);														
							}
						}
						else if(Data_Bytes==0x04)
						{
							if(Drive_control_array[9]==0x64)//判断起始地址——回零速度
							{
								send_buff[8]=Data_Bytes;//接下来数据的字节长度
								send_buff[11]=(zero_return_speed_value>>24)&0xFF;//数据
								send_buff[12]=(zero_return_speed_value>>16)&0xFF;//数据
								send_buff[9]=(zero_return_speed_value>>8)&0xFF;//数据
								send_buff[10]=zero_return_speed_value&0xFF;//数据
								send(1,send_buff,13);//向客户端发送	
								memset(send_buff,0,13);																
							}
							else if(Drive_control_array[9]==0x68)//判断起始地址——回零蠕变速度
							{
								send_buff[8]=Data_Bytes;//接下来数据的字节长度
								send_buff[11]=(zero_return_creep_speed_value>>24)&0xFF;//寄存器1H-数据
								send_buff[12]=(zero_return_creep_speed_value>>16)&0xFF;//寄存器1L-数据
								send_buff[9]=(zero_return_creep_speed_value>>8)&0xFF;//寄存器2H-数据
								send_buff[10]=zero_return_creep_speed_value&0xFF;//寄存器2L-数据
								send(1,send_buff,13);//向客户端发送	
								memset(send_buff,0,13);									
							}
							else if(Drive_control_array[9]==0x78)//判断起始地址——运行速度
							{		
								send_buff[8]=Data_Bytes;//接下来数据的字节长度
								send_buff[11]=(running_speed_value>>24)&0xFF;//寄存器1H-数据
								send_buff[12]=(running_speed_value>>16)&0xFF;//寄存器1L-数据
								send_buff[9]=(running_speed_value>>8)&0xFF;//寄存器2H-数据
								send_buff[10]=running_speed_value&0xFF;//寄存器2L-数据
								send(1,send_buff,13);//向客户端发送	
								memset(send_buff,0,13);															
							}
							else if(Drive_control_array[9]==0x7A)//判断起始地址——相对定位距离
							{
								send_buff[8]=Data_Bytes;//接下来数据的字节长度
								send_buff[11]=0X00;//寄存器1H-数据
								send_buff[12]=0X00;//寄存器1L-数据
								send_buff[9]=0X00;//寄存器2H-数据
								send_buff[10]=0X00;//寄存器2L-数据
								send(1,send_buff,13);//向客户端发送	
								memset(send_buff,0,13);														
							}
							else if(Drive_control_array[9]==0x7C)//判断起始地址——绝对定位位置
							{
								send_buff[8]=Data_Bytes;//接下来数据的字节长度
								send_buff[11]=(Target_Position>>24)&0xFF;//寄存器1H-数据
								send_buff[12]=(Target_Position>>16)&0xFF;//寄存器1L-数据
								send_buff[9]=(Target_Position>>8)&0xFF;//寄存器2H-数据
								send_buff[10]=Target_Position&0xFF;//寄存器2L-数据
								send(1,send_buff,13);//向客户端发送	
								memset(send_buff,0,13);														
							}		
							else if(Drive_control_array[9]==0x7E)//判断起始地址——设备最大运行行程
							{
								send_buff[8]=Data_Bytes;//接下来数据的字节长度
								send_buff[9]=0X00;//寄存器1H-数据
								send_buff[10]=0X00;//寄存器1L-数据
								send_buff[11]=0X00;//寄存器2H-数据
								send_buff[12]=0X00;//寄存器2L-数据
								send(1,send_buff,13);//向客户端发送	
								memset(send_buff,0,13);														
							}	
							else if(Drive_control_array[9]==0x80)//判断起始地址——零点位置坐标
							{
								send_buff[8]=Data_Bytes;//接下来数据的字节长度
								send_buff[11]=(Zero_Position>>24)&0xFF;//寄存器1H-数据
								send_buff[12]=(Zero_Position>>16)&0xFF;//寄存器1L-数据
								send_buff[9]=(Zero_Position>>8)&0xFF;//寄存器2H-数据
								send_buff[10]=Zero_Position&0xFF;//寄存器2L-数据
								send(1,send_buff,13);//向客户端发送	
								memset(send_buff,0,13);														
							}	
							else if(Drive_control_array[9]==0x87)//判断起始地址——实时位置主动读取
							{
								send_buff[8]=Data_Bytes;//接下来数据的字节长度
								send_buff[11]=(Position_upload>>24)&0xFF;//寄存器1H-数据
								send_buff[12]=(Position_upload>>16)&0xFF;//寄存器1L-数据
								send_buff[9]=(Position_upload>>8)&0xFF;//寄存器2H-数据
								send_buff[10]=Position_upload&0xFF;//寄存器2L-数据
								send(1,send_buff,13);//向客户端发送	
								memset(send_buff,0,13);														
							}	
						}							
					}
					break;
				
				case 0x05://写单个线圈(请求：MBAP 功能码 寄存器地址H 寄存器地址L 数据H(FF/00) 数据L(00))
					send_buff[4]=0X00;//数据长度
					send_buff[5]=0X06;//数据长度
					send_buff[8]=Drive_control_array[8];//寄存器地址H
					send_buff[9]=Drive_control_array[9];//寄存器地址H
					if(Drive_control_array[8]==0x20)//判断起始地址
					{
						switch(Drive_control_array[9])//判断起始地址
						{
							case 0x01://原点回归/停止
								if(Drive_control_array[10]==0xFF)//回归
								{
									if(Position_flag==0)//不在原点
									{
										if(Sports_status_flag==0)//判断运动状态
										{
											send_buff[10]=0XFF;
											send_buff[11]=0X00;
											send(1,send_buff,12);//向客户端发送	
											memset(send_buff,0,13);	
											Sports_status_flag=1;
											Motor_drive_send[0]=GPIO_PIN_SET;
											Motor_drive_send[1]=((One_Circle_Distance*1000000*10)/(zero_return_speed_value*Subtraction/100))>>8&0xff;
											Motor_drive_send[2]=(One_Circle_Distance*1000000*10)/(zero_return_speed_value*Subtraction/100)&0xff;
											osMessageQueuePut(Motor_driveQueueHandle,Motor_drive_send,1,0);	
										}											
									}	
								}
								else if(Drive_control_array[10]==0x00)//停止
								{
									send_buff[10]=0X00;
									send_buff[11]=0X00;
									send(1,send_buff,12);//向客户端发送	
									memset(send_buff,0,13);	
									HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//关闭PWM输出
									HAL_GPIO_WritePin(TIM8_CH1N_GPIO_Port,TIM8_CH1N_Pin,GPIO_PIN_SET);//PA7-电机使能正输入(GPIO_PIN_RESET:开始，GPIO_PIN_SET:停止）								
									Sports_status_flag=0;									
								}
								break;
							
							case 0x02://正转点动/停止
								if(Drive_control_array[10]==0xFF)//开始
								{
									send_buff[10]=0XFF;
									send_buff[11]=0X00;
									send(1,send_buff,12);//向客户端发送	
									memset(send_buff,0,13);	
									Sports_status_flag=1;
									Position_flag=0;
									Motor_drive_send[0]=GPIO_PIN_RESET;
									Motor_drive_send[1]=((One_Circle_Distance*1000000*10)/(running_speed_value*Subtraction/100))>>8&0xff;
									Motor_drive_send[2]=(One_Circle_Distance*1000000*10)/(running_speed_value*Subtraction/100)&0xff;
									osMessageQueuePut(Motor_driveQueueHandle,Motor_drive_send,1,0);
								}
								else//停止
								{
									send_buff[10]=0X00;
									send_buff[11]=0X00;
									send(1,send_buff,12);//向客户端发送	
									memset(send_buff,0,13);	
									HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//关闭PWM输出
									HAL_GPIO_WritePin(TIM8_CH1N_GPIO_Port,TIM8_CH1N_Pin,GPIO_PIN_SET);//PA7-电机使能正输入(GPIO_PIN_RESET:开始，GPIO_PIN_SET:停止）
									Sports_status_flag=0;									
								}
								break;
							
							case 0x03://反转点动/停止
								if(Drive_control_array[10]==0xFF)//开始
								{
									send_buff[10]=0XFF;
									send_buff[11]=0X00;
									send(1,send_buff,12);//向客户端发送	
									memset(send_buff,0,13);
									Sports_status_flag=1;
									Position_flag=0;
									Motor_drive_send[0]=GPIO_PIN_SET;
									Motor_drive_send[1]=((One_Circle_Distance*1000000*10)/(running_speed_value*Subtraction/100))>>8&0xff;
									Motor_drive_send[2]=(One_Circle_Distance*1000000*10)/(running_speed_value*Subtraction/100)&0xff;
									osMessageQueuePut(Motor_driveQueueHandle,Motor_drive_send,1,0);								
								}
								else//停止
								{
									send_buff[10]=0X00;
									send_buff[11]=0X00;
									send(1,send_buff,12);//向客户端发送	
									memset(send_buff,0,13);	
									HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//关闭PWM输出
									HAL_GPIO_WritePin(TIM8_CH1N_GPIO_Port,TIM8_CH1N_Pin,GPIO_PIN_SET);//PA7-电机使能正输入(GPIO_PIN_RESET:开始，GPIO_PIN_SET:停止）
									Sports_status_flag=0;								
								}							
								break;							
							
							case 0x04://相对运动/停止
								if(Drive_control_array[10]==0xFF)//开始
								{
									send_buff[10]=0XFF;
									send_buff[11]=0X00;
									send(1,send_buff,12);//向客户端发送	
									memset(send_buff,0,13);	
									if(Sports_status_flag==0)
									{
										Position_flag=0;
										Sports_status_flag=1;
										Motor_drive_send[0]=GPIO_PIN_RESET;
										Motor_drive_send[1]=((One_Circle_Distance*1000000*10)/(running_speed_value*Subtraction/100))>>8&0xff;
										Motor_drive_send[2]=(One_Circle_Distance*1000000*10)/(running_speed_value*Subtraction/100)&0xff;
										osMessageQueuePut(Motor_driveQueueHandle,Motor_drive_send,1,0);
									}
								}
								else//停止
								{
									send_buff[10]=0X00;
									send_buff[11]=0X00;
									send(1,send_buff,12);//向客户端发送	
									memset(send_buff,0,13);	
									HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//关闭PWM输出
									HAL_GPIO_WritePin(TIM8_CH1N_GPIO_Port,TIM8_CH1N_Pin,GPIO_PIN_SET);//PA7-电机使能正输入(GPIO_PIN_RESET:开始，GPIO_PIN_SET:停止）
									Sports_status_flag=0;
								}								
								break;
							
							case 0x05://绝对运动/停止
								if(Drive_control_array[10]==0xFF)//开始
								{
									send_buff[10]=0XFF;
									send_buff[11]=0X00;
									send(1,send_buff,12);//向客户端发送	
									memset(send_buff,0,13);
									if(Sports_status_flag==0&&Extreme_position_flag==0)
									{
										if(Position_upload*10<Target_Position)//当前位置小于目标位置-正向移动
										{
											Motor_drive_send[0]=GPIO_PIN_RESET;
											Motor_drive_send[1]=((One_Circle_Distance*1000000*10)/(running_speed_value*Subtraction/100))>>8&0xff;
											Motor_drive_send[2]=(One_Circle_Distance*1000000*10)/(running_speed_value*Subtraction/100)&0xff;
											osMessageQueuePut(Motor_driveQueueHandle,Motor_drive_send,1,0);
										}
										else if(Position_upload*10>Target_Position&&Position_flag==0)//当前位置大于目标位置-反向移动
										{
											Motor_drive_send[0]=GPIO_PIN_SET;
											Motor_drive_send[1]=((One_Circle_Distance*1000000*10)/(running_speed_value*Subtraction/100))>>8&0xff;
											Motor_drive_send[2]=(One_Circle_Distance*1000000*10)/(running_speed_value*Subtraction/100)&0xff;
											osMessageQueuePut(Motor_driveQueueHandle,Motor_drive_send,1,0);
										}
										Position_flag=0;
										Sports_status_flag=1;
									}
								}
								else//停止
								{
									send_buff[10]=0X00;
									send_buff[11]=0X00;
									send(1,send_buff,12);//向客户端发送	
									memset(send_buff,0,13);	
									HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//关闭PWM输出
									HAL_GPIO_WritePin(TIM8_CH1N_GPIO_Port,TIM8_CH1N_Pin,GPIO_PIN_SET);//PA7-电机使能正输入(GPIO_PIN_RESET:开始，GPIO_PIN_SET:停止）
									Sports_status_flag=0;
								}							
								break;
							
							case 0x14://停止运动状态
								if(Drive_control_array[10]==0xFF)//停止
								{
									HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//关闭PWM输出
									HAL_GPIO_WritePin(TIM8_CH1N_GPIO_Port,TIM8_CH1N_Pin,GPIO_PIN_SET);//PA7-电机使能正输入(GPIO_PIN_RESET:开始，GPIO_PIN_SET:停止）
									Sports_status_flag=0;
									send_buff[10]=0XFF;
									send_buff[11]=0X00;
								}
								else
								{
									send_buff[10]=0X00;
									send_buff[11]=0X00;
								}
								send(1,send_buff,12);//向客户端发送	
								memset(send_buff,0,13);								
								break;
							
							case 0x00://硬触发启动/停止
								if(Drive_control_array[10]==0xFF)//开始
								{
									Hard_Trigger_start_flag=1;
									send_buff[10]=0XFF;
									send_buff[11]=0X00;							
								}
								else//停止
								{
									Hard_Trigger_start_flag=0;
									send_buff[10]=0X00;
									send_buff[11]=0X00;
								}
								send(1,send_buff,12);//向客户端发送	
								memset(send_buff,0,13);								
								break;
							
							case 0x06://伺服复位
								if(Drive_control_array[10]==0xFF)
								{
									send_buff[10]=0XFF;
									send_buff[11]=0X00;
									send(1,send_buff,12);//向客户端发送	
									memset(send_buff,0,13);	
									Servo_reset_flag=1;
								}
								else if(Drive_control_array[10]==0x00)
								{
									send_buff[10]=0X00;
									send_buff[11]=0X00;
									send(1,send_buff,12);//向客户端发送	
									memset(send_buff,0,13);	
									Servo_reset_flag=0;									
								}
								break;
							
							case 0x07://伺服急停
								if(Drive_control_array[10]==0xFF)
								{
									send_buff[10]=0XFF;
									send_buff[11]=0X00;
								}
								else
								{
									send_buff[10]=0X00;
									send_buff[11]=0X00;
								}
								send(1,send_buff,12);//向客户端发送	
								memset(send_buff,0,13);	
								break;

							case 0x09://雷达上电/断电
								if(Drive_control_array[10]==0xFF)
								{
									send_buff[10]=0XFF;
									send_buff[11]=0X00;
								}
								else
								{
									send_buff[10]=0X00;
									send_buff[11]=0X00;
								}
								send(1,send_buff,12);//向客户端发送	
								memset(send_buff,0,13);								
								break;
							
							case 0x0F://PLC错误清除							
								if(Drive_control_array[10]==0xFF)
								{
									send_buff[10]=0XFF;
									send_buff[11]=0X00;
								}
								else
								{
									send_buff[10]=0X00;
									send_buff[11]=0X00;
								}
								send(1,send_buff,12);//向客户端发送	
								memset(send_buff,0,13);	
								break;
							
							default:
								break;													
						}
					}
					break;
				
				case 0x06://写单个保持寄存器(请求：MBAP 功能码 寄存器地址H 寄存器地址L 寄存器值H 寄存器值L)
//					if(Sports_status_flag==0)//未开始运动
//					{
						send_buff[4]=0X00;//数据长度
						send_buff[5]=0X06;//数据长度
						send_buff[8]=Drive_control_array[8];//寄存器地址
						send_buff[9]=Drive_control_array[9];//寄存器地址
						send_buff[10]=Drive_control_array[10];//寄存器值H
						send_buff[11]=Drive_control_array[11];//寄存器值L
						if(Drive_control_array[8]==0x01)//判断起始地址
						{
							if(Drive_control_array[9]==0x4A)//判断起始地址——加速时间
							{						
								send(1,send_buff,12);//向客户端发送	
								memset(send_buff,0,13);													
							}
							else if(Drive_control_array[9]==0x4E)//判断起始地址——减速时间
							{
								send(1,send_buff,12);//向客户端发送	
								memset(send_buff,0,13);												
							}
							else if(Drive_control_array[9]==0x00)//判断起始地址——实时位置上传控制
							{
								if(Drive_control_array[10]||Drive_control_array[11])
								{
									Position_upload_flag=1;//开始实时位置上传		
								}
								else
								{
									Position_upload_flag=0;//停止实时位置上传
								}
								send(1,send_buff,12);//向客户端发送	
								memset(send_buff,0,13);												
							}
							else if(Drive_control_array[9]==0x30)//判断起始地址——硬触发个数
							{
								Hard_Trigger_Number=(Drive_control_array[10]<<8|Drive_control_array[11]);//大端传输数据处理——硬触发个数
								send(1,send_buff,12);//向客户端发送	
								memset(send_buff,0,13);												
							}					
						}
//					}										
					break;
					
				case 0x10://写多个保持寄存器(请求：MBAP(7位) 功能码 起始地址H 起始地址L 寄存器数量H 寄存器数量L 字节长度 寄存器值)
//				if(Sports_status_flag==0)//未开始运动
//				{
						send_buff[4]=0X00;//数据长度
						send_buff[5]=0X06;//数据长度
						send_buff[8]=Drive_control_array[8];//寄存器起始地址H
						send_buff[9]=Drive_control_array[9];//寄存器起始地址L
						send_buff[10]=Drive_control_array[10];//寄存器数量H
						send_buff[11]=Drive_control_array[11];//寄存器数量L
						if(Drive_control_array[8]==0x00)//判断起始地址
						{
							if(Drive_control_array[9]==0x64)//判断起始地址——回零速度
							{
								zero_return_speed_value=(Drive_control_array[13]<<8|Drive_control_array[14]|Drive_control_array[15]<<24|Drive_control_array[16]<<16);//大端传输数据处理——回零速度0.001mm/s
								send(1,send_buff,12);//向客户端发送	
								memset(send_buff,0,13);																
							}
							else if(Drive_control_array[9]==0x68)//判断起始地址——回零蠕变速度
							{
								zero_return_creep_speed_value=(Drive_control_array[13]<<8|Drive_control_array[14]|Drive_control_array[15]<<24|Drive_control_array[16]<<16);//大端传输数据处理——回零蠕变速度0.001mm/s
								send(1,send_buff,12);//向客户端发送	
								memset(send_buff,0,13);									
							}
							else if(Drive_control_array[9]==0x78)//判断起始地址——运行速度
							{
//								running_speed_value=(Drive_control_array[13]<<24|Drive_control_array[14]<<16|Drive_control_array[15]<<8|Drive_control_array[16]);//大端传输数据处理——运行速度0.001mm/s
								running_speed_value=(Drive_control_array[13]<<8|Drive_control_array[14]|Drive_control_array[15]<<24|Drive_control_array[16]<<16);//大端传输数据处理——运行速度0.001mm/s
								send(1,send_buff,12);//向客户端发送	
								memset(send_buff,0,13);		
							}							
							else if(Drive_control_array[9]==0x7A)//判断起始地址——相对定位距离
							{
								send(1,send_buff,12);//向客户端发送	
								memset(send_buff,0,13);														
							}
							else if(Drive_control_array[9]==0x7C)//判断起始地址——绝对定位位置
							{
								Absolute_position_flag=1;
								Target_Position=(Drive_control_array[13]<<8|Drive_control_array[14]|Drive_control_array[15]<<24|Drive_control_array[16]<<16);//大端传输数据处理——绝对定位位置0.001mm
								if(Target_Position==0x00)
								{
									Extreme_position_flag=0;
								}
								send(1,send_buff,12);//向客户端发送	
								memset(send_buff,0,13);														
							}		
							else if(Drive_control_array[9]==0x7E)//判断起始地址——设备最大运行行程
							{
								send(1,send_buff,12);//向客户端发送	
								memset(send_buff,0,13);														
							}	
							else if(Drive_control_array[9]==0x80)//判断起始地址——零点位置坐标
							{
								Zero_Position=(Drive_control_array[13]<<8|Drive_control_array[14]|Drive_control_array[15]<<24|Drive_control_array[16]<<16);//大端传输数据处理——零点位置0.001mm
								send(1,send_buff,12);//向客户端发送	
								memset(send_buff,0,13);														
							}	
						}
						else if(Drive_control_array[8]==0x01)//判断起始地址
						{
							if(Drive_control_array[9]==0x2C)//判断起始地址——起始硬触发位置
							{
								Hard_Trigger_Position=(Drive_control_array[13]<<8|Drive_control_array[14]|Drive_control_array[15]<<24|Drive_control_array[16]<<16);//大端传输数据处理——起始硬触发位置0.001mm
								Position_lastpulsec=Hard_Trigger_Position;
								send(1,send_buff,12);//向客户端发送	
								memset(send_buff,0,13);														
							}
							else if(Drive_control_array[9]==0x2E)//判断起始地址——硬触发间隔
							{
								Hard_Trigger_Interval=(Drive_control_array[13]<<8|Drive_control_array[14]|Drive_control_array[15]<<24|Drive_control_array[16]<<16);//大端传输数据处理——硬触发间隔0.001mm
								send(1,send_buff,12);//向客户端发送	
								memset(send_buff,0,13);														
							}				
						}	
//					}						
					break;
					
				default:
					break;						
			}				
			memset(Drive_control_array,0,20);		
		}
  }
}


void Motor_drive(void *argument)//电动驱动
{
	uint8_t status=0;	
//	uint16_t number_i=10000;
	uint16_t number;
	uint8_t Motor_drive_receive[3];//电机驱动数组
	for(;;)
	{    
		osDelay(1);
		status=osMessageQueueGet(Motor_driveQueueHandle,Motor_drive_receive,NULL,10);
		if(status==osOK)
		{	
			if(Sports_status_flag==1)
			{
				number=Motor_drive_receive[1]<<8|Motor_drive_receive[2];
				HAL_GPIO_WritePin(TIM8_CH1N_GPIO_Port,TIM8_CH1N_Pin,GPIO_PIN_RESET);//PA7-电机使能正输入(GPIO_PIN_RESET:开始，GPIO_PIN_SET:停止）
				osDelay(1);	
				HAL_GPIO_WritePin(TIM8_CH2_GPIO_Port,TIM8_CH2_Pin,(GPIO_PinState)Motor_drive_receive[0]);//PC7-电机方向控制(GPIO_PIN_RESET:顺时针，GPIO_PIN_SET:逆时针）
				osDelay(1);	
				HAL_TIM_Base_Start_IT(&htim8);//开启TIM8定时器中断（20KHZ，周期50us）
				__HAL_TIM_SET_AUTORELOAD(&htim8,number-1);//修改自动重装载值
				HAL_TIM_PWM_Start_IT(&htim8,TIM_CHANNEL_1);//启动TIM8_CH1的PWM波输出（中断）
//				if(number>=100)
//				{
//					while(number_i>number&&HAL_GPIO_ReadPin(TIM8_CH1N_GPIO_Port,TIM8_CH1N_Pin)==GPIO_PIN_RESET)
//					{
//						__HAL_TIM_SET_AUTORELOAD(&htim8,number_i-1);//修改自动重装载值
//						HAL_TIM_PWM_Start_IT(&htim8,TIM_CHANNEL_1);//启动TIM8_CH1的PWM波输出（中断）
//						if(number_i>=5000)
//						{
//							number_i=number_i-100;
//							osDelay(1);
//						}
//						else if(5000>number_i&&number_i>=500)
//						{
//							number_i=number_i-10;
//							osDelay(5);	
//						}	
//						else
//						{
//							number_i=number_i-5;
//							osDelay(6);
//						}	
//					}
//					number_i=10000;
//				}
			}
		}
		memset(Motor_drive_receive,0,3);			
	}
}


void Encoder(void *argument)//电动机编码器通讯
{
	uint8_t Real_time_location_array[6]={0xCF,0xAE,0x00,0x00,0x00,0x00};//实时位置数组
  for(;;)
  {
		if(Encoder_Processing_flag==1)
		{
			Encoder_Processing_flag=0;
			if(Position_flag==1||HAL_GPIO_ReadPin(IN1_GPIO_Port,IN1_Pin)==GPIO_PIN_RESET)//原点校准
			{	
				Position=0;
				Position_upload=0;
			}	
			else if(Position_flag==0)
			{
				Position = ((float)Motor_pulsec_ount/Subtraction)*One_Circle_Distance;	//距原点距离
				Position_upload = Position*100;//位置上传数据（单位0.01mm）
				if(Zero_regression_flag==1&&Absolute_position_flag==1)//之前回过零且接收到绝对位置
				{
					if(HAL_GPIO_ReadPin(TIM8_CH2_GPIO_Port,TIM8_CH2_Pin)==GPIO_PIN_RESET)//正向移动
					{
						if(Position_upload*10>Target_Position)
						{
							HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//关闭PWM输出
							HAL_GPIO_WritePin(TIM8_CH1N_GPIO_Port,TIM8_CH1N_Pin,GPIO_PIN_SET);//PA7-电机使能正输入(GPIO_PIN_RESET:开始，GPIO_PIN_SET:停止）
							Sports_status_flag=0;	//运动状态	
							Absolute_position_flag=0;	
						}		
					}
					else if(HAL_GPIO_ReadPin(TIM8_CH2_GPIO_Port,TIM8_CH2_Pin)==GPIO_PIN_SET)//反向移动
					{
						if(Position_upload*10<Target_Position)
						{
							HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//关闭PWM输出
							HAL_GPIO_WritePin(TIM8_CH1N_GPIO_Port,TIM8_CH1N_Pin,GPIO_PIN_SET);//PA7-电机使能正输入(GPIO_PIN_RESET:开始，GPIO_PIN_SET:停止）
							Sports_status_flag=0;	//运动状态	
							Absolute_position_flag=0;	
						}
					}					
				}	
			}
			if(Position_upload_flag==1)//实时位置上传
			{				
				Real_time_location_array[2]=Position_upload&0xFF;
				Real_time_location_array[3]=(Position_upload>>8)&0xFF;
				Real_time_location_array[4]=(Position_upload>>16)&0xFF;
				Real_time_location_array[5]=(Position_upload>>24)&0xFF;
				send(2,Real_time_location_array,6);//向客户端发送	
			}	
			if(Hard_Trigger_flag==1)
			{
				Trigger_Interval=Position*1000-Position_lastpulsec;
				if(Trigger_Interval>=Hard_Trigger_Interval*10)//.。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。
				{
					Position_lastpulsec=Position*1000;
					Hard_Trigger_flag=0;
					Trigger_Interval=0;					
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
		if(Hard_Trigger_start_flag==1&&Position_upload>=Hard_Trigger_Position&&Sports_status_flag==1)//硬触发条件(硬触发启动，到达硬触发位置，运动状态)
		{
			if(Hard_Trigger_flag==0)
			{
				if(Hard_Trigger_Current_Number<Hard_Trigger_Number)//硬触发次数判定
				{		
					Hard_Trigger_Current_Number++;
					HAL_GPIO_WritePin(IO_OUT_GPIO_Port, IO_OUT_Pin, GPIO_PIN_SET);//PA2-硬触发
					HAL_TIM_Base_Start_IT(&htim13);//开启TIM13定时器中断（1MHZ，计数50次，50us）
					Hard_Trigger_flag=1;
				}
				else
				{
					Hard_Trigger_Current_Number=0;
					Hard_Trigger_start_flag=0;
				}
			}
		}
		else if(Hard_Trigger_start_flag==0)
		{
			Hard_Trigger_Current_Number=0;
		}
  }
}


void Extreme_position_Drive(void *argument)//极限位置控制
{
  for(;;)
  {
		osDelay(1);
		if(Extreme_position_flag==1)
		{
				HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//关闭PWM输出
				HAL_GPIO_WritePin(TIM8_CH1N_GPIO_Port,TIM8_CH1N_Pin,GPIO_PIN_SET);//PA7-电机使能正输入(GPIO_PIN_RESET:开始，GPIO_PIN_SET:停止）
				Sports_status_flag=0;
		}
  }
}


void Uart3_Communicate(void *argument)//串口通讯
{
	const uint8_t str_date[12] = __DATE__; //示例 "Mar 23 2022"  
	const uint8_t str_time[10] = __TIME__;
	uint8_t uart_receive_buff[3];
	uint8_t	VERSION[6];
	uint8_t y1,y2,d1,d2,t1,t2;
  for(;;)
  {
		osDelay(1);
		HAL_UART_Receive_IT(&huart3, uart_receive_buff, 3);
		VERSION[0] = 0XAA;
		VERSION[1] = 0X0A;//轨道号
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

		if(uart_receive_buff[0]==0XAA && uart_receive_buff[1]==0X01 && uart_receive_buff[2]==0XBB)
		{
			HAL_UART_Transmit(&huart3, VERSION, 6, 10);
		}
		memset(uart_receive_buff,0,3);	
  }
}

void Servo_reset(void *argument)//伺服复位
{
  for(;;)
  {
		if(Servo_reset_flag==1)
		{
			HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//关闭PWM输出
			HAL_GPIO_WritePin(TIM8_CH1N_GPIO_Port,TIM8_CH1N_Pin,GPIO_PIN_SET);//PA7-电机使能正输入(GPIO_PIN_RESET:开始，GPIO_PIN_SET:停止）
			Sports_status_flag=0;
			HAL_GPIO_WritePin(TIM8_CH2N_GPIO_Port,TIM8_CH2N_Pin,GPIO_PIN_RESET);	
			osDelay(500);
			HAL_GPIO_WritePin(TIM8_CH2N_GPIO_Port,TIM8_CH2N_Pin,GPIO_PIN_SET);
			Servo_reset_flag=0;
		}	
	}
}
/* USER CODE END Application */

