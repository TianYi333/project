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
  .gw  = { 192, 168, 1, 1 },//���ز���
  .dns  = { 8, 8, 8, 8 },
  .dhcp = NETINFO_STATIC };

uint8_t receive_buff[RECEIVE_BUFF_SIZE];

	
Motor motor1;
float Position;//��0��λ��,��λmm
uint16_t pwmVal_TIM8 = 0;  //ռ�ձȴ�С(CCRx�Ĵ�С���ߵ�ƽʱ��)
uint8_t Position_flag=0;//λ�ñ�־λ
uint8_t Encoder_Processing_flag=0;//���������ݴ����־λ
float  Track_Length;//���������λ����
uint8_t Sports_mode=0;//�˶�ģʽ
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

void Ethernet_Server(void *argument);//����ͨ��-�����
void Encoder(void *argument);//�綯��������ͨѶ
void Drive(void *argument);//����������
void Pulse_Emission(void *argument);//���巢��
void Screen(void *argument);//��Ļͨ��
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
  W5500Init();//w5500��ʼ��
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

void Ethernet_Server(void *argument)//����ͨ��-�����
{
  ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);  //�����������
  wiz_PhyConf phyconf;
  phyconf.by = PHY_CONFBY_SW;
  phyconf.duplex = PHY_DUPLEX_FULL;
  phyconf.speed = PHY_SPEED_10;
  phyconf.mode = PHY_MODE_AUTONEGO;
  ctlwizchip(CW_SET_PHYCONF, (void*)&phyconf); //����PHY����
  PHYStatusCheck();//����������Ӳ���
  PrintPHYConf();//��ӡPHY������Ϣ
  printf("Simple Ping Application\r\n");
  for(;;)
  {
		osDelay(1);
    printf("\r\nInitializing server socket\r\n");
	  //Parameters in order socket_id, protocol TCP or UDP, Port number, Flags=0
	  //Return value is socket ID on success
	  if(socket(1,Sn_MR_TCP,LISTEN_PORT,0)!=1)//����һ��socket��Socket�ţ�TCP/UDP���ͣ��˿ں�
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
	  if(listen(1)!=SOCK_OK)//�����˿�
	  {//our socket id is 1 (w5500 have 8 sockets from 0-7)
		  //error
		  printf("Cannot listen on port %d",LISTEN_PORT);
		  while(1);
	  }
	  uint8_t sr=0x00;//socket status register
	  do
	  {
		  sr=getSn_SR(1);//��ȡSn_SR�Ĵ���������0~7 
	  }while (sr!=SOCK_ESTABLISHED && sr!=SOCK_CLOSED);

	  if(sr==SOCK_CLOSED)
	  {
		  printf("Some error occurred on server socket. Please restart.\r\n");
		  while(1);
	  }
	  if(sr==SOCK_ESTABLISHED)//�ɹ�����
	  {
		  //we come here only when a client has connected.
		  //Now we can read data from the socket
		  printf("A client connected!\r\n");
		  printf("Waiting for Client Data ...!\r\n");
		  while(1)
		  {
			  int len=recv(1, receive_buff, RECEIVE_BUFF_SIZE);//�������豸��ȡ���ݵ�receive_buff
			  if(len==SOCKERR_SOCKSTATUS)	// ��ȡW5500���н��ջ���Ĵ�����ֵ������len
			  {
				  //client has disconnected
				  printf("Client has disconnected\r\n");
				  printf("*** SESSION OVER ***\r\n\r\n");
				  break;
			  }
				//Look for quit message and quit if received
			  if(strcmp((char*)receive_buff,"QUIT")==0)
			  {//�յ�"QUIT"���˿ڿͻ�������
				  printf("Received QUIT command from client\r\n");
				  printf("Disconnecting ... \r\n");
				  printf("*** SESSION OVER ***\r\n\r\n");
				  disconnect(1);//disconnect from the clinet �Ͽ��ͻ�������
				  break;//come out of while loop �˳����ص�131�У����µȴ��ͻ�������
			  }											
				osMessageQueuePut(Ethernet_ReceiveHandle,receive_buff,1,0);
				memset(receive_buff,0,RECEIVE_BUFF_SIZE);					
		  }//While loop (as long as client is connected)
//			send(1,(uint8_t*)Position,2);// the data          ��ͻ��˷��ͽ��յ������ݻ����ض��Ļش�	
	  }//if block, client connect success	
  }
}

uint16_t a=100;
void Drive(void *argument)//����������
{
	uint8_t	send_buff[12];
	uint8_t status=0;	
	uint8_t Drive_control_array[12];	//��������������
	HAL_GPIO_WritePin(CH2_GPIO_Port,CH2_Pin,GPIO_PIN_RESET);//PC7-���ʹ��������(GPIO_PIN_RESET:��ʼ��GPIO_PIN_SET:ֹͣ��
	osDelay(1);
	HAL_GPIO_WritePin(CH2N_GPIO_Port,CH2N_Pin,GPIO_PIN_SET);//PB0-����������(GPIO_PIN_RESET:˳ʱ�룬GPIO_PIN_SET:��ʱ�룩
	osDelay(1);
	HAL_TIM_Base_Start_IT(&htim8);//����TIM8��ʱ���жϣ�20KHZ������50us��
//	__HAL_TIM_SET_AUTORELOAD(&htim8,20);//�޸��Զ���װ��ֵ
	HAL_TIM_PWM_Start_IT(&htim8,TIM_CHANNEL_1);//����TIM8_CH1��PWM��������жϣ�
  for(;;)
  {
		osDelay(1);
		
//		if(a<3000)
//		{
//			a++;
//			__HAL_TIM_SET_AUTORELOAD(&htim8,a);//�޸��Զ���װ��ֵ
//			__HAL_TIM_PRESCALER(&htim8,a);//�޸�Ԥ��Ƶֵ
//			TIM8->CCR1 = a;//�޸�ռ�ձ�
//			__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, a);//�޸�ռ�ձ�
//			HAL_TIM_PWM_Start_IT(&htim8,TIM_CHANNEL_1);//����TIM8_CH1��PWM��������жϣ�
//			osDelay(2000);			
//		}		
		status=osMessageQueueGet(Ethernet_ReceiveHandle,Drive_control_array,NULL,10);
		if(status==osOK)
		{					
			send_buff[0]=Drive_control_array[0];//0X19;�������ʶ
			send_buff[1]=Drive_control_array[1];//0XB2;�������ʶ
			send_buff[2]=0X00;//Э���ʶ����00 00ΪmodbusЭ��
			send_buff[3]=0X00;//Э���ʶ����00 00ΪmodbusЭ��
			send_buff[6]=Drive_control_array[6];//0XDB;�豸��ַ
			send_buff[7]=Drive_control_array[7];//������
			switch(Drive_control_array[7])//�жϹ�����
			{
				case 0x01:
					if(Drive_control_array[8]==0x20)//�ж���ʼ��ַ
					{
						switch(Drive_control_array[9])//�ж���ʼ��ַ
						{
							case 0x0A://ԭ��ع���ɱ�־
								send_buff[4]=0X00;//���ݳ���
								send_buff[5]=0X04;//���ݳ���	
								send_buff[8]=0X01;//���������ݵ��ֽڳ���							
//								if()//���
//								{
//									send_buff[9]=0X01;						
//								}
//								else//δ���
//								{
//									send_buff[9]=0X00;
//								}
								send(1,send_buff,10);//��ͻ��˷���
								memset(send_buff,0,12);	
								break;
							
							case 0x64://�ŷ��˶�״̬
								send_buff[4]=0X00;//���ݳ���
								send_buff[5]=0X04;//���ݳ���
								send_buff[8]=0X01;//���������ݵ��ֽڳ���
//								if()//�˶�
//								{
//									send_buff[9]=0X01;						
//								}
//								else//��ֹ
//								{
//									send_buff[9]=0X00;
//								}
								send(1,send_buff,10);//��ͻ��˷���
								memset(send_buff,0,12);	
								break;

							case 0x01://ԭ��ع�/ֹͣ
								send_buff[4]=0X00;//���ݳ���
								send_buff[5]=0X06;//���ݳ���
								send_buff[8]=0X20;//���������ݵ��ֽڳ���
								send_buff[9]=0X02;//���������ݵ��ֽڳ���
								send_buff[11]=0X00;//���������ݵ��ֽڳ���
//								if()//
//								{
//									send_buff[10]=0XFF;						
//								}
//								else//
//								{
//									send_buff[10]=0X00;
//								}
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,12);	
								break;
							
							case 0x02://��ת�㶯/ֹͣ
								send_buff[4]=0X00;//���ݳ���
								send_buff[5]=0X06;//���ݳ���
								send_buff[8]=0X20;//���������ݵ��ֽڳ���
								send_buff[9]=0X02;//���������ݵ��ֽڳ���
								send_buff[11]=0X00;//���������ݵ��ֽڳ���
//								if()//
//								{
//									send_buff[10]=0XFF;						
//								}
//								else//
//								{
//									send_buff[10]=0X00;
//								}
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,12);								
								break;
							
							case 0x03://��ת�㶯/ֹͣ
								send_buff[4]=0X00;//���ݳ���
								send_buff[5]=0X06;//���ݳ���
								send_buff[8]=0X20;//���������ݵ��ֽڳ���
								send_buff[9]=0X02;//���������ݵ��ֽڳ���
								send_buff[11]=0X00;//���������ݵ��ֽڳ���								
//								if()//
//								{
//									send_buff[10]=0XFF;						
//								}
//								else//
//								{
//									send_buff[10]=0X00;
//								}
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,12);								
								break;							
							
							case 0x04://����˶�/ֹͣ
								send_buff[4]=0X00;//���ݳ���
								send_buff[5]=0X06;//���ݳ���
								send_buff[8]=0X20;//���������ݵ��ֽڳ���
								send_buff[9]=0X02;//���������ݵ��ֽڳ���
								send_buff[11]=0X00;//���������ݵ��ֽڳ���								
//								if()//
//								{
//									send_buff[10]=0XFF;						
//								}
//								else//
//								{
//									send_buff[10]=0X00;
//								}
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,12);								
								break;
							
							case 0x05://�����˶�/ֹͣ
								send_buff[4]=0X00;//���ݳ���
								send_buff[5]=0X06;//���ݳ���
								send_buff[8]=0X20;//���������ݵ��ֽڳ���
								send_buff[9]=0X02;//���������ݵ��ֽڳ���
								send_buff[11]=0X00;//���������ݵ��ֽڳ���								
//								if()//
//								{
//									send_buff[10]=0XFF;						
//								}
//								else//
//								{
//									send_buff[10]=0X00;
//								}
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,12);								
								break;
							
							case 0x00://Ӳ��������/ֹͣ
								send_buff[4]=0X00;//���ݳ���
								send_buff[5]=0X06;//���ݳ���
								send_buff[8]=0X20;//���������ݵ��ֽڳ���
								send_buff[9]=0X02;//���������ݵ��ֽڳ���
								send_buff[11]=0X00;//���������ݵ��ֽڳ���								
//								if()//
//								{
//									send_buff[10]=0XFF;						
//								}
//								else//
//								{
//									send_buff[10]=0X00;
//								}
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,12);								
								break;
							
							case 0x06://�ŷ���λ
								send_buff[4]=0X00;//���ݳ���
								send_buff[5]=0X06;//���ݳ���
								send_buff[8]=0X20;//���������ݵ��ֽڳ���
								send_buff[9]=0X02;//���������ݵ��ֽڳ���
								send_buff[11]=0X00;//���������ݵ��ֽڳ���								
//								if()//
//								{
//									send_buff[10]=0XFF;						
//								}
//								else//
//								{
//									send_buff[10]=0X00;
//								}
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,12);								
								break;

							case 0x09://�״��ϵ�/�ϵ�
								send_buff[4]=0X00;//���ݳ���
								send_buff[5]=0X06;//���ݳ���
								send_buff[8]=0X20;//���������ݵ��ֽڳ���
								send_buff[9]=0X02;//���������ݵ��ֽڳ���
								send_buff[11]=0X00;//���������ݵ��ֽڳ���								
//								if()//
//								{
//									send_buff[10]=0XFF;						
//								}
//								else//
//								{
//									send_buff[10]=0X00;
//								}
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,12);								
								break;
							
							default:
								break;
						}
					}
					break;
				
				case 0x02:
					send_buff[4]=0X00;//���ݳ���
					send_buff[5]=0X04;//���ݳ���
					send_buff[8]=0X01;//���������ݵ��ֽڳ���
					if(Drive_control_array[8]==0x00&&Drive_control_array[9]==0x01)//�ж���ʼ��ַ������ת���޹��״̬
					{						
//						if()//
//						{
//							send_buff[9]=0X01;						
//						}
//						else//
//						{
//							send_buff[9]=0X00;
//						}
						send(1,send_buff,10);//��ͻ��˷���	
						memset(send_buff,0,12);		
					}						
					else if(Drive_control_array[8]==0x00&&Drive_control_array[9]==0x03)//�ж���ʼ��ַ������ת���޹��״̬
					{						
//						if()//
//						{
//							send_buff[9]=0X01;						
//						}
//						else//
//						{
//							send_buff[9]=0X00;
//						}
						send(1,send_buff,10);//��ͻ��˷���	
						memset(send_buff,0,12);	
					}
					else if(Drive_control_array[8]==0x00&&Drive_control_array[9]==0x04)//�ж���ʼ��ַ���������λ����״̬	
					{
//						if()//
//						{
//							send_buff[9]=0X01;						
//						}
//						else//
//						{
//							send_buff[9]=0X00;
//						}
						send(1,send_buff,10);//��ͻ��˷���	
						memset(send_buff,0,12);	
					}
					else if(Drive_control_array[8]==0x00&&Drive_control_array[9]==0x05)//�ж���ʼ��ַ�����ŷ�����״̬	
					{	
//						if()//
//						{
//							send_buff[9]=0X01;						
//						}
//						else//
//						{
//							send_buff[9]=0X00;
//						}
						send(1,send_buff,10);//��ͻ��˷���	
						memset(send_buff,0,12);	
					}
					break;

				case 0x03:
					send_buff[4]=0X00;//���ݳ���
					send_buff[5]=0X06;//���ݳ���
					if(Drive_control_array[8]==0x01)//�ж���ʼ��ַ
					{
						if(Drive_control_array[9]==0x4A)//�ж���ʼ��ַ��������ʱ��
						{
							send_buff[8]=0X02;//���������ݵ��ֽڳ���
							send_buff[9]=0X01;
							send(1,send_buff,10);//��ͻ��˷���	
							memset(send_buff,0,12);							
						}
						else if(Drive_control_array[9]==0x4E)//�ж���ʼ��ַ��������ʱ��
						{
							send_buff[8]=0X02;//���������ݵ��ֽڳ���
							send_buff[9]=0X01;
							send(1,send_buff,10);//��ͻ��˷���	
							memset(send_buff,0,12);														
						}
						else if(Drive_control_array[9]==0x00)//�ж���ʼ��ַ����ʵʱλ���ϴ�����
						{
							send_buff[8]=0X02;//���������ݵ��ֽڳ���
							send_buff[9]=0X01;
							send(1,send_buff,10);//��ͻ��˷���	
							memset(send_buff,0,12);															
						}
						else if(Drive_control_array[9]==0x30)//�ж���ʼ��ַ����Ӳ��������
						{
							send_buff[8]=0X02;//���������ݵ��ֽڳ���
							send_buff[9]=0X01;
							send(1,send_buff,10);//��ͻ��˷���	
							memset(send_buff,0,12);								
						}
						else if(Drive_control_array[9]==0x32)//�ж���ʼ��ַ������ǰӲ��������
						{
							send_buff[8]=0X02;//���������ݵ��ֽڳ���
							send_buff[9]=0X01;
							send(1,send_buff,10);//��ͻ��˷���	
							memset(send_buff,0,12);															
						}
						else if(Drive_control_array[9]==0x2C)//�ж���ʼ��ַ������ʼӲ����λ��
						{
							send_buff[8]=0X02;//���������ݵ��ֽڳ���
							send_buff[9]=0X01;
							send_buff[10]=0X04;
							send_buff[11]=0X03;
							send(1,send_buff,12);//��ͻ��˷���	
							memset(send_buff,0,12);														
						}
						else if(Drive_control_array[9]==0x2E)//�ж���ʼ��ַ����Ӳ�������
						{
							send_buff[8]=0X02;//���������ݵ��ֽڳ���
							send_buff[9]=0X01;
							send_buff[10]=0X04;
							send_buff[11]=0X03;
							send(1,send_buff,12);//��ͻ��˷���	
							memset(send_buff,0,12);														
						}
					}
					
					else if(Drive_control_array[8]==0x00)//�ж���ʼ��ַ
					{
						if(Drive_control_array[9]==0x20)//�ж���ʼ��ַ�����ŷ��������
						{
							send_buff[8]=0X02;//���������ݵ��ֽڳ���
							send_buff[9]=0X01;
							send(1,send_buff,10);//��ͻ��˷���	
							memset(send_buff,0,12);							
						}
						else if(Drive_control_array[9]==0x96)//�ж���ʼ��ַ����PLC�������
						{
							send_buff[8]=0X02;//���������ݵ��ֽڳ���
							send_buff[9]=0X01;
							send(1,send_buff,10);//��ͻ��˷���	
							memset(send_buff,0,12);														
						}
						else if(Drive_control_array[9]==0x64)//�ж���ʼ��ַ���������ٶ�
						{
							send_buff[8]=0X02;//���������ݵ��ֽڳ���
							send_buff[9]=0X01;
							send_buff[10]=0X04;
							send_buff[11]=0X03;
							send(1,send_buff,12);//��ͻ��˷���	
							memset(send_buff,0,12);																
						}
						else if(Drive_control_array[9]==0x68)//�ж���ʼ��ַ������������ٶ�
						{
							send_buff[8]=0X02;//���������ݵ��ֽڳ���
							send_buff[9]=0X01;
							send_buff[10]=0X04;
							send_buff[11]=0X03;
							send(1,send_buff,12);//��ͻ��˷���	
							memset(send_buff,0,12);									
						}
						else if(Drive_control_array[9]==0x78)//�ж���ʼ��ַ���������ٶ�
						{
							send_buff[8]=0X02;//���������ݵ��ֽڳ���
							send_buff[9]=0X01;
							send_buff[10]=0X04;
							send_buff[11]=0X03;
							send(1,send_buff,12);//��ͻ��˷���	
							memset(send_buff,0,12);															
						}
						else if(Drive_control_array[9]==0x7A)//�ж���ʼ��ַ������Զ�λ����
						{
							send_buff[8]=0X02;//���������ݵ��ֽڳ���
							send_buff[9]=0X01;
							send_buff[10]=0X04;
							send_buff[11]=0X03;
							send(1,send_buff,12);//��ͻ��˷���	
							memset(send_buff,0,12);														
						}
						else if(Drive_control_array[9]==0x7C)//�ж���ʼ��ַ�������Զ�λλ��
						{
							send_buff[8]=0X02;//���������ݵ��ֽڳ���
							send_buff[9]=0X01;
							send_buff[10]=0X04;
							send_buff[11]=0X03;
							send(1,send_buff,12);//��ͻ��˷���	
							memset(send_buff,0,12);														
						}		
						else if(Drive_control_array[9]==0x7E)//�ж���ʼ��ַ�����豸��������г�
						{
							send_buff[8]=0X02;//���������ݵ��ֽڳ���
							send_buff[9]=0X01;
							send_buff[10]=0X04;
							send_buff[11]=0X03;
							send(1,send_buff,12);//��ͻ��˷���	
							memset(send_buff,0,12);														
						}	
						else if(Drive_control_array[9]==0x80)//�ж���ʼ��ַ�������λ������
						{
							send_buff[8]=0X02;//���������ݵ��ֽڳ���
							send_buff[9]=0X01;
							send_buff[10]=0X04;
							send_buff[11]=0X03;
							send(1,send_buff,12);//��ͻ��˷���	
							memset(send_buff,0,12);														
						}							
					}
					break;
				
				case 0x05:
					send_buff[4]=0X00;//���ݳ���
					send_buff[5]=0X06;//���ݳ���
					send_buff[8]=0X20;//���������ݵ��ֽڳ���
					send_buff[9]=0X02;				
					send_buff[11]=0X00;
					if(Drive_control_array[8]==0x20)//�ж���ʼ��ַ
					{
						switch(Drive_control_array[9])//�ж���ʼ��ַ
						{
							case 0x01://ԭ��ع�/ֹͣ
//								if()//
//								{
//									send_buff[10]=0XFF;						
//								}
//								else//
//								{
//									send_buff[10]=0X00;
//								}
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,12);	
								break;
							
							case 0x02://��ת�㶯/ֹͣ
//								if()//
//								{
//									send_buff[10]=0XFF;						
//								}
//								else//
//								{
//									send_buff[10]=0X00;
//								}
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,12);								
								break;
							
							case 0x03://��ת�㶯/ֹͣ
//								if()//
//								{
//									send_buff[10]=0XFF;						
//								}
//								else//
//								{
//									send_buff[10]=0X00;
//								}
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,12);								
								break;							
							
							case 0x04://����˶�/ֹͣ
//								if()//
//								{
//									send_buff[10]=0XFF;						
//								}
//								else//
//								{
//									send_buff[10]=0X00;
//								}
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,12);								
								break;
							
							case 0x05://�����˶�/ֹͣ
//								if()//
//								{
//									send_buff[10]=0XFF;						
//								}
//								else//
//								{
//									send_buff[10]=0X00;
//								}
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,12);								
								break;
							
							case 0x14://ֹͣ�˶�״̬
//								if()//
//								{
//									send_buff[10]=0XFF;						
//								}
//								else//
//								{
//									send_buff[10]=0X00;
//								}
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,12);								
								break;
							
							case 0x00://Ӳ��������/ֹͣ
//								if()//
//								{
//									send_buff[10]=0XFF;						
//								}
//								else//
//								{
//									send_buff[10]=0X00;
//								}
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,12);								
								break;
							
							case 0x06://�ŷ���λ
//								if()//
//								{
//									send_buff[10]=0XFF;						
//								}
//								else//
//								{
//									send_buff[10]=0X00;
//								}
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,12);								
								break;
							
							case 0x07://�ŷ���ͣ
//								if()//
//								{
//									send_buff[10]=0XFF;						
//								}
//								else//
//								{
//									send_buff[10]=0X00;
//								}
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,12);								
								break;

							case 0x09://�״��ϵ�/�ϵ�
//								if()//
//								{
//									send_buff[10]=0XFF;						
//								}
//								else//
//								{
//									send_buff[10]=0X00;
//								}
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,12);								
								break;
							
							case 0x0F://PLC�������
//								if()//
//								{
//									send_buff[10]=0XFF;						
//								}
//								else//
//								{
//									send_buff[10]=0X00;
//								}
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,12);								
								break;
							
							default:
								break;													
						}
					}
					break;
				
				case 0x06:
					send_buff[4]=0X00;//���ݳ���
					send_buff[5]=0X06;//���ݳ���
					send_buff[8]=0X02;//���������ݵ��ֽڳ���
					if(Drive_control_array[8]==0x01)//�ж���ʼ��ַ
					{
						if(Drive_control_array[9]==0x4A)//�ж���ʼ��ַ��������ʱ��
						{
							send_buff[9]=0X01;
							send(1,send_buff,10);//��ͻ��˷���	
							memset(send_buff,0,12);													
						}
						else if(Drive_control_array[9]==0x4E)//�ж���ʼ��ַ��������ʱ��
						{
							send_buff[9]=0X01;
							send(1,send_buff,10);//��ͻ��˷���	
							memset(send_buff,0,12);												
						}
						else if(Drive_control_array[9]==0x00)//�ж���ʼ��ַ����ʵʱλ���ϴ�����
						{
							send_buff[9]=0X01;
							send(1,send_buff,10);//��ͻ��˷���	
							memset(send_buff,0,12);												
						}
						else if(Drive_control_array[9]==0x30)//�ж���ʼ��ַ����Ӳ��������
						{
							send_buff[9]=0X01;
							send(1,send_buff,10);//��ͻ��˷���	
							memset(send_buff,0,12);												
						}					
					}			
					break;
					
				case 0x10:
					send_buff[4]=0X00;//���ݳ���
					send_buff[5]=0X06;//���ݳ���
					send_buff[8]=0X02;//���������ݵ��ֽڳ���
					if(Drive_control_array[8]==0x00)//�ж���ʼ��ַ
					{
						if(Drive_control_array[9]==0x64)//�ж���ʼ��ַ���������ٶ�
						{
							send_buff[9]=0X01;
							send_buff[10]=0X04;
							send_buff[11]=0X03;
							send(1,send_buff,12);//��ͻ��˷���	
							memset(send_buff,0,12);																
						}
						else if(Drive_control_array[9]==0x68)//�ж���ʼ��ַ������������ٶ�
						{
							send_buff[9]=0X01;
							send_buff[10]=0X04;
							send_buff[11]=0X03;
							send(1,send_buff,12);//��ͻ��˷���	
							memset(send_buff,0,12);									
						}
						else if(Drive_control_array[9]==0x78)//�ж���ʼ��ַ���������ٶ�
						{
							send_buff[9]=0X01;
							send_buff[10]=0X04;
							send_buff[11]=0X03;
							send(1,send_buff,12);//��ͻ��˷���	
							memset(send_buff,0,12);		
						}							
						else if(Drive_control_array[9]==0x7A)//�ж���ʼ��ַ������Զ�λ����
						{
							send_buff[9]=0X01;
							send_buff[10]=0X04;
							send_buff[11]=0X03;
							send(1,send_buff,12);//��ͻ��˷���	
							memset(send_buff,0,12);														
						}
						else if(Drive_control_array[9]==0x7C)//�ж���ʼ��ַ�������Զ�λλ��
						{
							send_buff[9]=0X01;
							send_buff[10]=0X04;
							send_buff[11]=0X03;
							send(1,send_buff,12);//��ͻ��˷���	
							memset(send_buff,0,12);														
						}		
						else if(Drive_control_array[9]==0x7E)//�ж���ʼ��ַ�����豸��������г�
						{
							send_buff[9]=0X01;
							send_buff[10]=0X04;
							send_buff[11]=0X03;
							send(1,send_buff,12);//��ͻ��˷���	
							memset(send_buff,0,12);														
						}	
						else if(Drive_control_array[9]==0x80)//�ж���ʼ��ַ�������λ������
						{
							send_buff[9]=0X01;
							send_buff[10]=0X04;
							send_buff[11]=0X03;
							send(1,send_buff,12);//��ͻ��˷���	
							memset(send_buff,0,12);														
						}	
					}
					else if(Drive_control_array[8]==0x01)//�ж���ʼ��ַ
					{
						if(Drive_control_array[9]==0x2C)//�ж���ʼ��ַ������ʼӲ����λ��
						{
							send_buff[9]=0X01;
							send_buff[10]=0X04;
							send_buff[11]=0X03;
							send(1,send_buff,12);//��ͻ��˷���	
							memset(send_buff,0,12);														
						}
						else if(Drive_control_array[9]==0x2E)//�ж���ʼ��ַ����Ӳ�������
						{
							send_buff[9]=0X01;
							send_buff[10]=0X04;
							send_buff[11]=0X03;
							send(1,send_buff,12);//��ͻ��˷���	
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

void Encoder(void *argument)//�綯��������ͨѶ
{
//	if(Position_flag==1)//��λ��ԭ���ʼ�ɼ�������
//	{
		HAL_GPIO_WritePin(TIM2_DIR_GPIO_Port, TIM2_DIR_Pin, GPIO_PIN_RESET);//PC3-����DIR���ŵ͵�ƽ����ƽת����B->A
		HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);      //������������ʱ��
		__HAL_TIM_ENABLE_IT(&htim2,TIM_IT_UPDATE);          //������������ʱ�������ж�,���������
		HAL_TIM_Base_Start_IT(&htim3);                    //����100ms��ʱ���ж�
		__HAL_TIM_SetCounter(&htim2, 10000);               //��������ʱ����ʼֵ�趨Ϊ10000
		motor1.lastCount = 0;                               //�ṹ�����ݳ�ʼ��
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
			motor1.direct = __HAL_TIM_IS_TIM_COUNTING_DOWN(&htim2);//������ϼ�������ת��������ֵΪ0�����򷵻�ֵΪ1
			int16_t pluse = COUNTERNUM - (RELOADVALUE)/2;
			motor1.totalCount = pluse+ motor1.overflowNum * (int16_t)RELOADVALUE/2;//�ӿ�ʼ�����ڵ�ǰ100ms����������	
			motor1.speed = (float)(motor1.totalCount - motor1.lastCount) / (4 * MOTOR_SPEED_RERATIO * PULSE_PRE_ROUND);//���1msת�˶���Ȧ,����4����Ϊ4��Ƶ
//			motor1.speed = Speed_Low_Filter(motor1.speed,speed_Record);//�˲�����
			motor1.lastCount = motor1.totalCount; //��¼��һ�εļ���ֵ 
			motor1.distance = One_Circle_Distance * motor1.speed;//�����ƶ�����
//			if(Position_flag==1)
//			{					
				Position = Position + motor1.distance;	//��������
//			}					
//			else if(Position_flag==2)//ԭ��У׼
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

void Pulse_Emission(void *argument)//���崥����TIM13ͨ����
{	
	HAL_GPIO_WritePin(IO_DIR_GPIO_Port, IO_DIR_Pin, GPIO_PIN_SET);//PA4-����DIR���Ÿߵ�ƽ����ƽת����A->B
  for(;;)
  {
		osDelay(1);
//		if()//Ӳ��������
//		{
//			HAL_GPIO_WritePin(IO_OUT_GPIO_Port, IO_OUT_Pin, GPIO_PIN_SET);//PA6-Ӳ����
//			HAL_TIM_Base_Start_IT(&htim13);//����TIM13��ʱ���жϣ�10MHZ������10�Σ�1us��
//		}
		osDelay(100);
		HAL_GPIO_TogglePin(GPIOC, LED2_Pin);
  }
}


void Screen(void *argument)//��Ļͨ��
{
//	initRingBuff();																//��ʼ�����λ�����
//	HAL_UART_Receive_IT(&huart2,RxBuff,1);				//�򿪴��ڽ����ж�
	TJCPrintf("0x00");         							  		//Ϊȷ������HMI����ͨ��
  for(;;)
  {
    osDelay(1);
//		while(usize >= FRAMELENGTH)//���ڻ�������������6
//	  {
//		  //У��֡ͷ֡β�Ƿ�ƥ��
//		  if(u(0) != 0x55 || u(6) != 0xff || u(7) != 0xff || u(8) != 0xff)
//		  {
//			  //��ƥ��ɾ��1�ֽ�
//			  udelete(1);
//		  }else
//		  {
//			  //ƥ�䣬����ѭ��
//			  break;
//		  }
//	  }	
//	  //���н���
//	  if(usize >= FRAMELENGTH && u(0) == 0x55 && u(6) == 0xff && u(7) == 0xff && u(8) == 0xff)
//	  {
//		  udelete(FRAMELENGTH);//ɾ�����ڻ���������Ӧ���ȵ�����
//	  }		
//		TJCPrintf("set.t3.txt=\"%d\"",Position);	//��ʾλ�ƾ���
//		TJCPrintf("set.j0.val=%d",Position/Track_Length);	//��ʾ����������
//		TJCPrintf("set.t1.txt=\"%d\"",motor1.direct);	//��ʾ�˶�����
//		TJCPrintf("set.t4.txt=\"%d\"",motor1.speed*10);	//��ʾ�ٶ�
//		TJCPrintf("set.t4.txt=\"%d\"",Sports_mode);	//��ʾ�˶�ģʽ
	
  }
}
/* USER CODE END Application */

