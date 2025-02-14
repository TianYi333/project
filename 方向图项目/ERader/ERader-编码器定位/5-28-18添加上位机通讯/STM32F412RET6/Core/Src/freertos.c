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

uint8_t receive_buff[RECEIVE_BUFF_SIZE];

	
Motor motor1;
float Position;//距0点位置,单位mm
uint16_t pwmVal_TIM8 = 0;  //占空比大小(CCRx的大小，高电平时间)
uint8_t Position_flag=0;//位置标志位
uint8_t Encoder_Processing_flag=0;//编码器数据处理标志位
float  Track_Length;//轨道长，单位毫米
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

osMessageQueueId_t Ethernet_SendHandle;
const osMessageQueueAttr_t Ethernet_Send_attributes = {
  .name = "Ethernet_Send"
};

void Ethernet_Server(void *argument);//网络通信-服务端
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
	Pulse_EmissionTaskHandle = osThreadNew(Pulse_Emission, NULL, &Pulse_EmissionTask_attributes);
	ScreenTaskHandle = osThreadNew(Screen, NULL, &ScreenTask_attributes);
	
	Ethernet_ReceiveHandle = osMessageQueueNew (5, 12*sizeof(uint8_t), &Ethernet_Receive_attributes);
	Ethernet_SendHandle = osMessageQueueNew (5, 20*sizeof(uint8_t), &Ethernet_Send_attributes);
	
	
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
			  if(len==SOCKERR_SOCKSTATUS)	// 读取W5500空闲接收缓存寄存器的值并赋给len
			  {
				  //client has disconnected
				  printf("Client has disconnected\r\n");
				  printf("*** SESSION OVER ***\r\n\r\n");
				  break;
			  }
				//Look for quit message and quit if received
			  if(strcmp((char*)receive_buff,"QUIT")==0)
			  {//收到"QUIT"，端口客户端连接
				  printf("Received QUIT command from client\r\n");
				  printf("Disconnecting ... \r\n");
				  printf("*** SESSION OVER ***\r\n\r\n");
				  disconnect(1);//disconnect from the clinet 断开客户端连接
				  break;//come out of while loop 退出，回到131行，重新等待客户端连接
			  }											
				osMessageQueuePut(Ethernet_ReceiveHandle,receive_buff,1,0);
				memset(receive_buff,0,RECEIVE_BUFF_SIZE);					
		  }//While loop (as long as client is connected)
//			send(1,(uint8_t*)Position,2);// the data          向客户端发送接收到的内容或者特定的回答	
	  }//if block, client connect success	
  }
}

uint16_t a=100;
void Drive(void *argument)//控制驱动器
{
	uint8_t	send_buff[12];
	uint8_t status=0;	
	uint8_t Drive_control_array[12];	//驱动器控制数组
	HAL_GPIO_WritePin(CH2_GPIO_Port,CH2_Pin,GPIO_PIN_RESET);//PC7-电机使能正输入(GPIO_PIN_RESET:开始，GPIO_PIN_SET:停止）
	osDelay(1);
	HAL_GPIO_WritePin(CH2N_GPIO_Port,CH2N_Pin,GPIO_PIN_SET);//PB0-电机方向控制(GPIO_PIN_RESET:顺时针，GPIO_PIN_SET:逆时针）
	osDelay(1);
	HAL_TIM_Base_Start_IT(&htim8);//开启TIM8定时器中断（20KHZ，周期50us）
//	__HAL_TIM_SET_AUTORELOAD(&htim8,20);//修改自动重装载值
	HAL_TIM_PWM_Start_IT(&htim8,TIM_CHANNEL_1);//启动TIM8_CH1的PWM波输出（中断）
  for(;;)
  {
		osDelay(1);
		
//		if(a<3000)
//		{
//			a++;
//			__HAL_TIM_SET_AUTORELOAD(&htim8,a);//修改自动重装载值
//			__HAL_TIM_PRESCALER(&htim8,a);//修改预分频值
//			TIM8->CCR1 = a;//修改占空比
//			__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, a);//修改占空比
//			HAL_TIM_PWM_Start_IT(&htim8,TIM_CHANNEL_1);//启动TIM8_CH1的PWM波输出（中断）
//			osDelay(2000);			
//		}		
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
				case 0x01:
					if(Drive_control_array[8]==0x20)//判断起始地址
					{
						switch(Drive_control_array[9])//判断起始地址
						{
							case 0x0A://原点回归完成标志
								send_buff[4]=0X00;//数据长度
								send_buff[5]=0X04;//数据长度	
								send_buff[8]=0X01;//接下来数据的字节长度							
//								if()//完成
//								{
//									send_buff[9]=0X01;						
//								}
//								else//未完成
//								{
//									send_buff[9]=0X00;
//								}
								send(1,send_buff,10);//向客户端发送
								memset(send_buff,0,12);	
								break;
							
							case 0x64://伺服运动状态
								send_buff[4]=0X00;//数据长度
								send_buff[5]=0X04;//数据长度
								send_buff[8]=0X01;//接下来数据的字节长度
//								if()//运动
//								{
//									send_buff[9]=0X01;						
//								}
//								else//静止
//								{
//									send_buff[9]=0X00;
//								}
								send(1,send_buff,10);//向客户端发送
								memset(send_buff,0,12);	
								break;

							case 0x01://原点回归/停止
								send_buff[4]=0X00;//数据长度
								send_buff[5]=0X06;//数据长度
								send_buff[8]=0X20;//接下来数据的字节长度
								send_buff[9]=0X02;//接下来数据的字节长度
								send_buff[11]=0X00;//接下来数据的字节长度
//								if()//
//								{
//									send_buff[10]=0XFF;						
//								}
//								else//
//								{
//									send_buff[10]=0X00;
//								}
								send(1,send_buff,12);//向客户端发送	
								memset(send_buff,0,12);	
								break;
							
							case 0x02://正转点动/停止
								send_buff[4]=0X00;//数据长度
								send_buff[5]=0X06;//数据长度
								send_buff[8]=0X20;//接下来数据的字节长度
								send_buff[9]=0X02;//接下来数据的字节长度
								send_buff[11]=0X00;//接下来数据的字节长度
//								if()//
//								{
//									send_buff[10]=0XFF;						
//								}
//								else//
//								{
//									send_buff[10]=0X00;
//								}
								send(1,send_buff,12);//向客户端发送	
								memset(send_buff,0,12);								
								break;
							
							case 0x03://反转点动/停止
								send_buff[4]=0X00;//数据长度
								send_buff[5]=0X06;//数据长度
								send_buff[8]=0X20;//接下来数据的字节长度
								send_buff[9]=0X02;//接下来数据的字节长度
								send_buff[11]=0X00;//接下来数据的字节长度								
//								if()//
//								{
//									send_buff[10]=0XFF;						
//								}
//								else//
//								{
//									send_buff[10]=0X00;
//								}
								send(1,send_buff,12);//向客户端发送	
								memset(send_buff,0,12);								
								break;							
							
							case 0x04://相对运动/停止
								send_buff[4]=0X00;//数据长度
								send_buff[5]=0X06;//数据长度
								send_buff[8]=0X20;//接下来数据的字节长度
								send_buff[9]=0X02;//接下来数据的字节长度
								send_buff[11]=0X00;//接下来数据的字节长度								
//								if()//
//								{
//									send_buff[10]=0XFF;						
//								}
//								else//
//								{
//									send_buff[10]=0X00;
//								}
								send(1,send_buff,12);//向客户端发送	
								memset(send_buff,0,12);								
								break;
							
							case 0x05://绝对运动/停止
								send_buff[4]=0X00;//数据长度
								send_buff[5]=0X06;//数据长度
								send_buff[8]=0X20;//接下来数据的字节长度
								send_buff[9]=0X02;//接下来数据的字节长度
								send_buff[11]=0X00;//接下来数据的字节长度								
//								if()//
//								{
//									send_buff[10]=0XFF;						
//								}
//								else//
//								{
//									send_buff[10]=0X00;
//								}
								send(1,send_buff,12);//向客户端发送	
								memset(send_buff,0,12);								
								break;
							
							case 0x00://硬触发启动/停止
								send_buff[4]=0X00;//数据长度
								send_buff[5]=0X06;//数据长度
								send_buff[8]=0X20;//接下来数据的字节长度
								send_buff[9]=0X02;//接下来数据的字节长度
								send_buff[11]=0X00;//接下来数据的字节长度								
//								if()//
//								{
//									send_buff[10]=0XFF;						
//								}
//								else//
//								{
//									send_buff[10]=0X00;
//								}
								send(1,send_buff,12);//向客户端发送	
								memset(send_buff,0,12);								
								break;
							
							case 0x06://伺服复位
								send_buff[4]=0X00;//数据长度
								send_buff[5]=0X06;//数据长度
								send_buff[8]=0X20;//接下来数据的字节长度
								send_buff[9]=0X02;//接下来数据的字节长度
								send_buff[11]=0X00;//接下来数据的字节长度								
//								if()//
//								{
//									send_buff[10]=0XFF;						
//								}
//								else//
//								{
//									send_buff[10]=0X00;
//								}
								send(1,send_buff,12);//向客户端发送	
								memset(send_buff,0,12);								
								break;

							case 0x09://雷达上电/断电
								send_buff[4]=0X00;//数据长度
								send_buff[5]=0X06;//数据长度
								send_buff[8]=0X20;//接下来数据的字节长度
								send_buff[9]=0X02;//接下来数据的字节长度
								send_buff[11]=0X00;//接下来数据的字节长度								
//								if()//
//								{
//									send_buff[10]=0XFF;						
//								}
//								else//
//								{
//									send_buff[10]=0X00;
//								}
								send(1,send_buff,12);//向客户端发送	
								memset(send_buff,0,12);								
								break;
							
							default:
								break;
						}
					}
					break;
				
				case 0x02:
					send_buff[4]=0X00;//数据长度
					send_buff[5]=0X04;//数据长度
					send_buff[8]=0X01;//接下来数据的字节长度
					if(Drive_control_array[8]==0x00&&Drive_control_array[9]==0x01)//判断起始地址——正转极限光电状态
					{						
//						if()//
//						{
//							send_buff[9]=0X01;						
//						}
//						else//
//						{
//							send_buff[9]=0X00;
//						}
						send(1,send_buff,10);//向客户端发送	
						memset(send_buff,0,12);		
					}						
					else if(Drive_control_array[8]==0x00&&Drive_control_array[9]==0x03)//判断起始地址——反转极限光电状态
					{						
//						if()//
//						{
//							send_buff[9]=0X01;						
//						}
//						else//
//						{
//							send_buff[9]=0X00;
//						}
						send(1,send_buff,10);//向客户端发送	
						memset(send_buff,0,12);	
					}
					else if(Drive_control_array[8]==0x00&&Drive_control_array[9]==0x04)//判断起始地址——零点限位触发状态	
					{
//						if()//
//						{
//							send_buff[9]=0X01;						
//						}
//						else//
//						{
//							send_buff[9]=0X00;
//						}
						send(1,send_buff,10);//向客户端发送	
						memset(send_buff,0,12);	
					}
					else if(Drive_control_array[8]==0x00&&Drive_control_array[9]==0x05)//判断起始地址——伺服报警状态	
					{	
//						if()//
//						{
//							send_buff[9]=0X01;						
//						}
//						else//
//						{
//							send_buff[9]=0X00;
//						}
						send(1,send_buff,10);//向客户端发送	
						memset(send_buff,0,12);	
					}
					break;

				case 0x03:
					send_buff[4]=0X00;//数据长度
					send_buff[5]=0X06;//数据长度
					if(Drive_control_array[8]==0x01)//判断起始地址
					{
						if(Drive_control_array[9]==0x4A)//判断起始地址——加速时间
						{
							send_buff[8]=0X02;//接下来数据的字节长度
							send_buff[9]=0X01;
							send(1,send_buff,10);//向客户端发送	
							memset(send_buff,0,12);							
						}
						else if(Drive_control_array[9]==0x4E)//判断起始地址——减速时间
						{
							send_buff[8]=0X02;//接下来数据的字节长度
							send_buff[9]=0X01;
							send(1,send_buff,10);//向客户端发送	
							memset(send_buff,0,12);														
						}
						else if(Drive_control_array[9]==0x00)//判断起始地址——实时位置上传控制
						{
							send_buff[8]=0X02;//接下来数据的字节长度
							send_buff[9]=0X01;
							send(1,send_buff,10);//向客户端发送	
							memset(send_buff,0,12);															
						}
						else if(Drive_control_array[9]==0x30)//判断起始地址——硬触发个数
						{
							send_buff[8]=0X02;//接下来数据的字节长度
							send_buff[9]=0X01;
							send(1,send_buff,10);//向客户端发送	
							memset(send_buff,0,12);								
						}
						else if(Drive_control_array[9]==0x32)//判断起始地址——当前硬触发个数
						{
							send_buff[8]=0X02;//接下来数据的字节长度
							send_buff[9]=0X01;
							send(1,send_buff,10);//向客户端发送	
							memset(send_buff,0,12);															
						}
						else if(Drive_control_array[9]==0x2C)//判断起始地址——起始硬触发位置
						{
							send_buff[8]=0X02;//接下来数据的字节长度
							send_buff[9]=0X01;
							send_buff[10]=0X04;
							send_buff[11]=0X03;
							send(1,send_buff,12);//向客户端发送	
							memset(send_buff,0,12);														
						}
						else if(Drive_control_array[9]==0x2E)//判断起始地址——硬触发间隔
						{
							send_buff[8]=0X02;//接下来数据的字节长度
							send_buff[9]=0X01;
							send_buff[10]=0X04;
							send_buff[11]=0X03;
							send(1,send_buff,12);//向客户端发送	
							memset(send_buff,0,12);														
						}
					}
					
					else if(Drive_control_array[8]==0x00)//判断起始地址
					{
						if(Drive_control_array[9]==0x20)//判断起始地址——伺服错误代码
						{
							send_buff[8]=0X02;//接下来数据的字节长度
							send_buff[9]=0X01;
							send(1,send_buff,10);//向客户端发送	
							memset(send_buff,0,12);							
						}
						else if(Drive_control_array[9]==0x96)//判断起始地址——PLC错误代码
						{
							send_buff[8]=0X02;//接下来数据的字节长度
							send_buff[9]=0X01;
							send(1,send_buff,10);//向客户端发送	
							memset(send_buff,0,12);														
						}
						else if(Drive_control_array[9]==0x64)//判断起始地址——回零速度
						{
							send_buff[8]=0X02;//接下来数据的字节长度
							send_buff[9]=0X01;
							send_buff[10]=0X04;
							send_buff[11]=0X03;
							send(1,send_buff,12);//向客户端发送	
							memset(send_buff,0,12);																
						}
						else if(Drive_control_array[9]==0x68)//判断起始地址——回零蠕变速度
						{
							send_buff[8]=0X02;//接下来数据的字节长度
							send_buff[9]=0X01;
							send_buff[10]=0X04;
							send_buff[11]=0X03;
							send(1,send_buff,12);//向客户端发送	
							memset(send_buff,0,12);									
						}
						else if(Drive_control_array[9]==0x78)//判断起始地址——运行速度
						{
							send_buff[8]=0X02;//接下来数据的字节长度
							send_buff[9]=0X01;
							send_buff[10]=0X04;
							send_buff[11]=0X03;
							send(1,send_buff,12);//向客户端发送	
							memset(send_buff,0,12);															
						}
						else if(Drive_control_array[9]==0x7A)//判断起始地址——相对定位距离
						{
							send_buff[8]=0X02;//接下来数据的字节长度
							send_buff[9]=0X01;
							send_buff[10]=0X04;
							send_buff[11]=0X03;
							send(1,send_buff,12);//向客户端发送	
							memset(send_buff,0,12);														
						}
						else if(Drive_control_array[9]==0x7C)//判断起始地址——绝对定位位置
						{
							send_buff[8]=0X02;//接下来数据的字节长度
							send_buff[9]=0X01;
							send_buff[10]=0X04;
							send_buff[11]=0X03;
							send(1,send_buff,12);//向客户端发送	
							memset(send_buff,0,12);														
						}		
						else if(Drive_control_array[9]==0x7E)//判断起始地址——设备最大运行行程
						{
							send_buff[8]=0X02;//接下来数据的字节长度
							send_buff[9]=0X01;
							send_buff[10]=0X04;
							send_buff[11]=0X03;
							send(1,send_buff,12);//向客户端发送	
							memset(send_buff,0,12);														
						}	
						else if(Drive_control_array[9]==0x80)//判断起始地址——零点位置坐标
						{
							send_buff[8]=0X02;//接下来数据的字节长度
							send_buff[9]=0X01;
							send_buff[10]=0X04;
							send_buff[11]=0X03;
							send(1,send_buff,12);//向客户端发送	
							memset(send_buff,0,12);														
						}							
					}
					break;
				
				case 0x05:
					send_buff[4]=0X00;//数据长度
					send_buff[5]=0X06;//数据长度
					send_buff[8]=0X20;//接下来数据的字节长度
					send_buff[9]=0X02;				
					send_buff[11]=0X00;
					if(Drive_control_array[8]==0x20)//判断起始地址
					{
						switch(Drive_control_array[9])//判断起始地址
						{
							case 0x01://原点回归/停止
//								if()//
//								{
//									send_buff[10]=0XFF;						
//								}
//								else//
//								{
//									send_buff[10]=0X00;
//								}
								send(1,send_buff,12);//向客户端发送	
								memset(send_buff,0,12);	
								break;
							
							case 0x02://正转点动/停止
//								if()//
//								{
//									send_buff[10]=0XFF;						
//								}
//								else//
//								{
//									send_buff[10]=0X00;
//								}
								send(1,send_buff,12);//向客户端发送	
								memset(send_buff,0,12);								
								break;
							
							case 0x03://反转点动/停止
//								if()//
//								{
//									send_buff[10]=0XFF;						
//								}
//								else//
//								{
//									send_buff[10]=0X00;
//								}
								send(1,send_buff,12);//向客户端发送	
								memset(send_buff,0,12);								
								break;							
							
							case 0x04://相对运动/停止
//								if()//
//								{
//									send_buff[10]=0XFF;						
//								}
//								else//
//								{
//									send_buff[10]=0X00;
//								}
								send(1,send_buff,12);//向客户端发送	
								memset(send_buff,0,12);								
								break;
							
							case 0x05://绝对运动/停止
//								if()//
//								{
//									send_buff[10]=0XFF;						
//								}
//								else//
//								{
//									send_buff[10]=0X00;
//								}
								send(1,send_buff,12);//向客户端发送	
								memset(send_buff,0,12);								
								break;
							
							case 0x14://停止运动状态
//								if()//
//								{
//									send_buff[10]=0XFF;						
//								}
//								else//
//								{
//									send_buff[10]=0X00;
//								}
								send(1,send_buff,12);//向客户端发送	
								memset(send_buff,0,12);								
								break;
							
							case 0x00://硬触发启动/停止
//								if()//
//								{
//									send_buff[10]=0XFF;						
//								}
//								else//
//								{
//									send_buff[10]=0X00;
//								}
								send(1,send_buff,12);//向客户端发送	
								memset(send_buff,0,12);								
								break;
							
							case 0x06://伺服复位
//								if()//
//								{
//									send_buff[10]=0XFF;						
//								}
//								else//
//								{
//									send_buff[10]=0X00;
//								}
								send(1,send_buff,12);//向客户端发送	
								memset(send_buff,0,12);								
								break;
							
							case 0x07://伺服急停
//								if()//
//								{
//									send_buff[10]=0XFF;						
//								}
//								else//
//								{
//									send_buff[10]=0X00;
//								}
								send(1,send_buff,12);//向客户端发送	
								memset(send_buff,0,12);								
								break;

							case 0x09://雷达上电/断电
//								if()//
//								{
//									send_buff[10]=0XFF;						
//								}
//								else//
//								{
//									send_buff[10]=0X00;
//								}
								send(1,send_buff,12);//向客户端发送	
								memset(send_buff,0,12);								
								break;
							
							case 0x0F://PLC错误清除
//								if()//
//								{
//									send_buff[10]=0XFF;						
//								}
//								else//
//								{
//									send_buff[10]=0X00;
//								}
								send(1,send_buff,12);//向客户端发送	
								memset(send_buff,0,12);								
								break;
							
							default:
								break;													
						}
					}
					break;
				
				case 0x06:
					send_buff[4]=0X00;//数据长度
					send_buff[5]=0X06;//数据长度
					send_buff[8]=0X02;//接下来数据的字节长度
					if(Drive_control_array[8]==0x01)//判断起始地址
					{
						if(Drive_control_array[9]==0x4A)//判断起始地址——加速时间
						{
							send_buff[9]=0X01;
							send(1,send_buff,10);//向客户端发送	
							memset(send_buff,0,12);													
						}
						else if(Drive_control_array[9]==0x4E)//判断起始地址——减速时间
						{
							send_buff[9]=0X01;
							send(1,send_buff,10);//向客户端发送	
							memset(send_buff,0,12);												
						}
						else if(Drive_control_array[9]==0x00)//判断起始地址——实时位置上传控制
						{
							send_buff[9]=0X01;
							send(1,send_buff,10);//向客户端发送	
							memset(send_buff,0,12);												
						}
						else if(Drive_control_array[9]==0x30)//判断起始地址——硬触发个数
						{
							send_buff[9]=0X01;
							send(1,send_buff,10);//向客户端发送	
							memset(send_buff,0,12);												
						}					
					}			
					break;
					
				case 0x10:
					send_buff[4]=0X00;//数据长度
					send_buff[5]=0X06;//数据长度
					send_buff[8]=0X02;//接下来数据的字节长度
					if(Drive_control_array[8]==0x00)//判断起始地址
					{
						if(Drive_control_array[9]==0x64)//判断起始地址——回零速度
						{
							send_buff[9]=0X01;
							send_buff[10]=0X04;
							send_buff[11]=0X03;
							send(1,send_buff,12);//向客户端发送	
							memset(send_buff,0,12);																
						}
						else if(Drive_control_array[9]==0x68)//判断起始地址——回零蠕变速度
						{
							send_buff[9]=0X01;
							send_buff[10]=0X04;
							send_buff[11]=0X03;
							send(1,send_buff,12);//向客户端发送	
							memset(send_buff,0,12);									
						}
						else if(Drive_control_array[9]==0x78)//判断起始地址——运行速度
						{
							send_buff[9]=0X01;
							send_buff[10]=0X04;
							send_buff[11]=0X03;
							send(1,send_buff,12);//向客户端发送	
							memset(send_buff,0,12);		
						}							
						else if(Drive_control_array[9]==0x7A)//判断起始地址——相对定位距离
						{
							send_buff[9]=0X01;
							send_buff[10]=0X04;
							send_buff[11]=0X03;
							send(1,send_buff,12);//向客户端发送	
							memset(send_buff,0,12);														
						}
						else if(Drive_control_array[9]==0x7C)//判断起始地址——绝对定位位置
						{
							send_buff[9]=0X01;
							send_buff[10]=0X04;
							send_buff[11]=0X03;
							send(1,send_buff,12);//向客户端发送	
							memset(send_buff,0,12);														
						}		
						else if(Drive_control_array[9]==0x7E)//判断起始地址——设备最大运行行程
						{
							send_buff[9]=0X01;
							send_buff[10]=0X04;
							send_buff[11]=0X03;
							send(1,send_buff,12);//向客户端发送	
							memset(send_buff,0,12);														
						}	
						else if(Drive_control_array[9]==0x80)//判断起始地址——零点位置坐标
						{
							send_buff[9]=0X01;
							send_buff[10]=0X04;
							send_buff[11]=0X03;
							send(1,send_buff,12);//向客户端发送	
							memset(send_buff,0,12);														
						}	
					}
					else if(Drive_control_array[8]==0x01)//判断起始地址
					{
						if(Drive_control_array[9]==0x2C)//判断起始地址——起始硬触发位置
						{
							send_buff[9]=0X01;
							send_buff[10]=0X04;
							send_buff[11]=0X03;
							send(1,send_buff,12);//向客户端发送	
							memset(send_buff,0,12);														
						}
						else if(Drive_control_array[9]==0x2E)//判断起始地址——硬触发间隔
						{
							send_buff[9]=0X01;
							send_buff[10]=0X04;
							send_buff[11]=0X03;
							send(1,send_buff,12);//向客户端发送	
							memset(send_buff,0,12);														
						}				
					}														
					break;
					
				default:
					break;						
			}				
			memset(Drive_control_array,0,12);		
		}
  }
}

void Encoder(void *argument)//电动机编码器通讯
{
//	if(Position_flag==1)//复位到原点后开始采集编码器
//	{
		HAL_GPIO_WritePin(TIM2_DIR_GPIO_Port, TIM2_DIR_Pin, GPIO_PIN_RESET);//PC3-配置DIR引脚低电平，电平转换器B->A
		HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);      //开启编码器定时器
		__HAL_TIM_ENABLE_IT(&htim2,TIM_IT_UPDATE);          //开启编码器定时器更新中断,防溢出处理
		HAL_TIM_Base_Start_IT(&htim3);                    //开启100ms定时器中断
		__HAL_TIM_SetCounter(&htim2, 10000);               //编码器定时器初始值设定为10000
		motor1.lastCount = 0;                               //结构体内容初始化
		motor1.totalCount = 0;
		motor1.overflowNum = 0;                                  
		motor1.speed = 0;
		motor1.direct = 0;
		motor1.distance = 0;
		Position=0.0f;
//	}
  for(;;)
  {
		osDelay(1);
		if(Encoder_Processing_flag==1)
		{
			Encoder_Processing_flag=0;
			motor1.direct = __HAL_TIM_IS_TIM_COUNTING_DOWN(&htim2);//如果向上计数（正转），返回值为0，否则返回值为1
			int16_t pluse = COUNTERNUM - (RELOADVALUE)/2;
			motor1.totalCount = pluse+ motor1.overflowNum * (int16_t)RELOADVALUE/2;//从开始到现在当前100ms的总脉冲数	
			motor1.speed = (float)(motor1.totalCount - motor1.lastCount) / (4 * MOTOR_SPEED_RERATIO * PULSE_PRE_ROUND);//算得1ms转了多少圈,除以4是因为4倍频
//			motor1.speed = Speed_Low_Filter(motor1.speed,speed_Record);//滤波函数
			motor1.lastCount = motor1.totalCount; //记录这一次的计数值 
			motor1.distance = One_Circle_Distance * motor1.speed;//计算移动距离
//			if(Position_flag==1)
//			{					
				Position = Position + motor1.distance;	//距零点距离
//			}					
//			else if(Position_flag==2)//原点校准
//			{	
//				Position=0.0f;;
//				motor1.lastCount=0;
//				motor1.overflowNum=0;
//				motor1.totalCount=0;
//				motor1.speed=0;
//				pluse=0;
//				Position_flag=1;
//			}			
		}	
  }

}

void Pulse_Emission(void *argument)//脉冲触发（TIM13通道）
{	
	HAL_GPIO_WritePin(IO_DIR_GPIO_Port, IO_DIR_Pin, GPIO_PIN_SET);//PA4-配置DIR引脚高电平，电平转换器A->B
  for(;;)
  {
		osDelay(1);
//		if()//硬触发条件
//		{
//			HAL_GPIO_WritePin(IO_OUT_GPIO_Port, IO_OUT_Pin, GPIO_PIN_SET);//PA6-硬触发
//			HAL_TIM_Base_Start_IT(&htim13);//开启TIM13定时器中断（10MHZ，计数10次，1us）
//		}
		osDelay(100);
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

