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
  .ip  = { 192, 168, 1, 10 },
  .sn  = { 255, 255, 255, 0 },
  .gw  = { 192, 168, 1, 1 },//网关参数
  .dns  = { 8, 8, 8, 8 },
  .dhcp = NETINFO_STATIC };

Motor motor1;
float Position;//距0点位置,单位mm
uint16_t pwmVal_TIM8 = 0;  //占空比大小(CCRx的大小，高电平时间)
uint8_t Position_flag=0;//位置标志位
uint8_t Encoder_Processing_flag=0;//编码器数据处理标志位
float  Track_Length;//轨道长，单位毫米
uint16_t pwmVal_TIM13 = 0;  //占空比大小(CCRx的大小，高电平时间)
uint8_t Sports_mode=0;//运动模式
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

osThreadId_t Ethernet_ClinetTaskHandle;
const osThreadAttr_t Ethernet_ClinetTask_attributes = {
  .name = "Ethernet_ClinetTask",
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

osThreadId_t ScreenTaskHandle;
const osThreadAttr_t ScreenTask_attributes = {
  .name = "ScreenTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

osMessageQueueId_t Ethernet_ReceiveHandle;
const osMessageQueueAttr_t Ethernet_Receive_attributes = {
  .name = "Ethernet_Receive"
};

void Ethernet_Server(void *argument);//网络通信-服务端
void Ethernet_Clinet(void *argument);//网络通信-服务端
void Encoder(void *argument);//电动机编码器通讯
void Drive(void *argument);//控制驱动器
void Pulse_Emission(void *argument);//脉冲发射
void Screen(void *argument);//屏幕通信
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
	Ethernet_ClinetTaskHandle = osThreadNew(Ethernet_Clinet, NULL, &Ethernet_ClinetTask_attributes);
	EncoderTaskHandle = osThreadNew(Encoder, NULL, &EncoderTask_attributes);
	DriveTaskHandle = osThreadNew(Drive, NULL, &DriveTask_attributes);
	Pulse_EmissionTaskHandle = osThreadNew(Pulse_Emission, NULL, &Pulse_EmissionTask_attributes);
	ScreenTaskHandle = osThreadNew(Screen, NULL, &ScreenTask_attributes);
	
	Ethernet_ReceiveHandle = osMessageQueueNew (5, 5*sizeof(uint8_t), &Ethernet_Receive_attributes);
	
	
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
void Ethernet_Server(void *argument)//网口通信-服务端
{
	uint8_t Ethernet_Receive_array[5];	//上位机控制数组
  for(;;)
  {
		osDelay(1);
    printf("\r\nInitializing server socket\r\n");
	  //Parameters in order socket_id, protocol TCP or UDP, Port number, Flags=0
	  //Return value is socket ID on success
	  if(socket(1,Sn_MR_TCP,LISTEN_PORT,0)!=1)//创建一个socket：Socket号，TCP/UDP类型，端口号
	  {
		  //error
		  printf("Cannot create Socket!\r\n");
		  while(1);//halt here
	  }
	  //success
	  printf("Socket Created Successfully ! \r\n");
	  uint8_t socket_io_mode=SOCK_IO_BLOCK;
	  ctlsocket(1, CS_SET_IOMODE , &socket_io_mode);//set blocking IO mode
    printf("IP Address is %d.%d.%d.%d\r\n",gWIZNETINFO.ip[0],gWIZNETINFO.ip[1],gWIZNETINFO.ip[2],gWIZNETINFO.ip[3]);
	  printf("Start listening on port %d ! \r\n",LISTEN_PORT);
	  printf("Waiting for a client connection. \r\n");
	  //Make it a passive socket (i.e. listen for connection)
	  if(listen(1)!=SOCK_OK)//监听端口
	  {//our socket id is 1 (w5500 have 8 sockets from 0-7)
		  //error
		  printf("Cannot listen on port %d",LISTEN_PORT);
		  while(1);
	  }
	  uint8_t sr=0x00;//socket status register
	  do
	  {
		  sr=getSn_SR(1);//获取Sn_SR寄存器，参数0~7 
	  }while (sr!=SOCK_ESTABLISHED && sr!=SOCK_CLOSED);

	  if(sr==SOCK_CLOSED)
	  {
		  printf("Some error occurred on server socket. Please restart.\r\n");
		  while(1);
	  }
	  if(sr==SOCK_ESTABLISHED)//成功连接
	  {
		  //we come here only when a client has connected.
		  //Now we can read data from the socket
		  printf("A client connected!\r\n");
		  printf("Waiting for Client Data ...!\r\n");
		  while(1)
		  {
			  int len=recv(1, receive_buff, RECEIVE_BUFF_SIZE);//从连接设备读取数据到receive_buff
			  if(len==SOCKERR_SOCKSTATUS)
			  {
				  //client has disconnected
				  printf("Client has disconnected\r\n");
				  printf("*** SESSION OVER ***\r\n\r\n");
				  break;
			  }
			  receive_buff[len]='\0';
			  printf("Received %d bytes from client\r\n",len);
			  printf("Data Received: %s", receive_buff);
        if(strcmp((char*)receive_buff,"Who are u")==0)
			  {//判断接收到"Who are u"
				  memcpy(receive_buff,"I am role_2099!",15);//修改应答内容
          len = 15;
			  }
			  //Echo the data back encloused in a [] pair
			  send(1,(uint8_t*)"[",1);//starting sq bracket 向客户端发送[
			  send(1,receive_buff,len);// the data          向客户端发送接收到的内容或者特定的回答
			  send(1,(uint8_t*)"]",1);//closing sq bracket  向客户端发送]

			  printf("\r\nECHO sent back to client\r\n");

			  //Look for quit message and quit if received
			  if(strcmp((char*)receive_buff,"QUIT")==0)
			  {//收到"QUIT"，端口客户端连接
				  printf("Received QUIT command from client\r\n");
				  printf("Disconnecting ... \r\n");
				  printf("*** SESSION OVER ***\r\n\r\n");
				  disconnect(1);//disconnect from the clinet 断开客户端连接
				  break;//come out of while loop 退出，回到131行，重新等待客户端连接
			  }

		  }//While loop (as long as client is connected)

	  }//if block, client connect success
		osMessageQueuePut(Ethernet_ReceiveHandle,Ethernet_Receive_array,1,0);	
  }
}

void Ethernet_Clinet(void *argument)//网口通信-客户端
{	
  for(;;)
  {
    osDelay(1);
  }
}

void Encoder(void *argument)//电动机编码器通讯
{
	if(Position_flag==1)//复位到原点后开始采集编码器
	{
		HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);      //开启编码器定时器
		__HAL_TIM_ENABLE_IT(&htim2,TIM_IT_UPDATE);          //开启编码器定时器更新中断,防溢出处理
		HAL_TIM_Base_Start_IT(&htim3);                    //开启100ms定时器中断
		__HAL_TIM_SET_COUNTER(&htim2, 10000);               //编码器定时器初始值设定为10000
		motor1.lastCount = 0;                               //结构体内容初始化
		motor1.totalCount = 0;
		motor1.overflowNum = 0;                                  
		motor1.speed = 0;
		motor1.direct = 0;
		motor1.distance = 0;
		Position=0.0f;
	}
  for(;;)
  {
		osDelay(1);
		if(Encoder_Processing_flag==1)
		{
			Encoder_Processing_flag=0;
			motor1.direct = __HAL_TIM_IS_TIM_COUNTING_DOWN(&htim2);//如果向上计数（正转），返回值为0，否则返回值为1
			int16_t pluse = COUNTERNUM - RELOADVALUE/2;
			motor1.totalCount = pluse+ motor1.overflowNum * RELOADVALUE/2;//从开始到现在当前100ms的总脉冲数	
			motor1.speed = (float)(motor1.totalCount - motor1.lastCount) / (4 * MOTOR_SPEED_RERATIO * PULSE_PRE_ROUND);//算得100ms转了多少圈,除以4是因为4倍频
			motor1.speed = Speed_Low_Filter(motor1.speed,speed_Record);//滤波函数
			motor1.lastCount = motor1.totalCount; //记录这一次的计数值 
			motor1.distance = One_Circle_Distance * motor1.speed;//计算移动距离
			if(Position_flag==1)
			{					
				if(motor1.direct==1)//反转
				{
					Position = Position - motor1.distance;	//距零点距离
				}
				else//正转
				{
					Position = Position + motor1.distance;	//距零点距离		
				}
			}					
			else if(Position_flag==2)
			{	
				Position=0.0f;;
				motor1.lastCount=0;
				Position_flag=1;
			}			
		}	
  }

}


void Drive(void *argument)//控制驱动器
{
	uint8_t status=0;	
	uint8_t Drive_control_array[5];	//驱动器控制数组
	HAL_TIM_Base_Start_IT(&htim8);//开启TIM8定时器中断（200KHZ，周期5us）
	HAL_TIM_PWM_Start_IT(&htim8,TIM_CHANNEL_1);//启动TIM8_CH1的PWM波输出（中断）
  for(;;)
  {
		osDelay(1);
		status=osMessageQueueGet(Ethernet_ReceiveHandle,Drive_control_array,NULL,10);
		if(status!=osOK)	//一段时间（500ms）未收到脉冲
		{		
//			HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//关闭PWM输出	
//			__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwmVal_TIM8);//修改占空比大小
//			HAL_TIM_PWM_Start_IT(&htim8,TIM_CHANNEL_1);//启动TIM8_CH1的PWM波输出（中断）	
			
//		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);//控制电机驱动控制电机正反装
//		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,GPIO_PIN_SET);//控制电机驱动控制电机启停
		}
  }
}

void Pulse_Emission(void *argument)//脉冲发射（TIM13_CH1通道）
{	
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);//配置DIR引脚高电平，电平转换器A->B
	HAL_TIM_Base_Start_IT(&htim13);//开启TIM13定时器中断
	HAL_TIM_PWM_Start_IT(&htim13,TIM_CHANNEL_1);//启动TIM13_CH1的PWM波输出（中断）_一周期模式（One Pulse Mode）
  for(;;)
  {
    osDelay(1);
		osDelay(500);
		HAL_GPIO_TogglePin(GPIOC, LED2_Pin);
  }
}


void Screen(void *argument)//屏幕通信
{
//	initRingBuff();																//初始化环形缓存区
//	HAL_UART_Receive_IT(&huart2,RxBuff,1);				//打开串口接收中断
	TJCPrintf("0x00");         							  		//为确保串口HMI正常通信
  for(;;)
  {
    osDelay(1);
//		while(usize >= FRAMELENGTH)//串口缓存区数量大于6
//	  {
//		  //校验帧头帧尾是否匹配
//		  if(u(0) != 0x55 || u(6) != 0xff || u(7) != 0xff || u(8) != 0xff)
//		  {
//			  //不匹配删除1字节
//			  udelete(1);
//		  }else
//		  {
//			  //匹配，跳出循环
//			  break;
//		  }
//	  }	
//	  //进行解析
//	  if(usize >= FRAMELENGTH && u(0) == 0x55 && u(6) == 0xff && u(7) == 0xff && u(8) == 0xff)
//	  {
//		  udelete(FRAMELENGTH);//删除串口缓冲区中相应长度的数据
//	  }		
//		TJCPrintf("set.t3.txt=\"%d\"",Position);	//显示位移距离
//		TJCPrintf("set.j0.val=%d",Position/Track_Length);	//显示进度条距离
//		TJCPrintf("set.t1.txt=\"%d\"",motor1.direct);	//显示运动方向
//		TJCPrintf("set.t4.txt=\"%d\"",motor1.speed*10);	//显示速度
//		TJCPrintf("set.t4.txt=\"%d\"",Sports_mode);	//显示运动模式
	
  }
}
/* USER CODE END Application */

