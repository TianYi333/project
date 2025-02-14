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
  .gw  = { 192, 168, 1, 1 },//���ز���
  .dns  = { 8, 8, 8, 8 },
  .dhcp = NETINFO_STATIC };

uint8_t receive_buff[RECEIVE_BUFF_SIZE];

	
Motor motor1;
float Position=0;//��0��λ��,��λmm
uint32_t Position_upload=0;//��0������ϴ�ֵ����λ0.001mm��
uint32_t zero_return_speed_value=0x12;//�����ٶȣ���λ0.001mm/s��
uint16_t pwmVal_TIM8 = 0;  //ռ�ձȴ�С(CCRx�Ĵ�С���ߵ�ƽʱ��)
uint8_t Position_flag=0;//λ�ñ�־λ
uint8_t Sports_status_flag=0;//�˶�״̬��־λ
uint8_t Extreme_position_flag=0;//����λ�ñ�־λ
uint8_t Position_upload_flag=0;//λ���ϴ���־λ
uint8_t Encoder_Processing_flag=0;//���������ݴ����־λ
uint8_t Target_Position_flag=0;//����λ�ñ�־λ
float Track_Length=0;//���������λ����
uint32_t Target_Position=0;//Ŀ��λ��
uint8_t Sports_mode=0;//�˶�ģʽ
uint16_t Hard_Trigger_Number=0;//Ӳ��������
uint16_t Hard_Trigger_Current_Number=0;//Ӳ������ǰ����
uint32_t Hard_Trigger_Interval=0;//Ӳ����ʱ����
uint32_t Hard_Trigger_Position=0;//Ӳ������ʼλ��
uint8_t Hard_Trigger_start_flag=0;//Ӳ������ʼ��־λ
uint8_t Hard_Trigger_flag=0;//Ӳ������־λ
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

osThreadId_t Extreme_position_DriveTaskHandle;
const osThreadAttr_t Extreme_position_DriveTask_attributes = {
  .name = "Extreme_position_DriveTask",
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
void Extreme_position_Drive(void *argument);//��Ļͨ��
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
	Pulse_EmissionTaskHandle = osThreadNew(Pulse_Emission, NULL, &Pulse_EmissionTask_attributes);
	Extreme_position_DriveTaskHandle = osThreadNew(Extreme_position_Drive, NULL, &Extreme_position_DriveTask_attributes);
	
	Ethernet_ReceiveHandle = osMessageQueueNew (5, 20*sizeof(uint8_t), &Ethernet_Receive_attributes);
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
		osDelay(100);
		HAL_GPIO_TogglePin(GPIOC, LED1_Pin);
  }
  /* USER CODE END StartTask01 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

void Ethernet_Server(void *argument)//����ͨ��-�����
{
	printf("A simple Ping Application using W5500!\r\n");
  W5500Init();//w5500��ʼ��
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


void Drive(void *argument)//����������
{
	uint8_t	send_buff[13];
	uint8_t status=0;	
	uint32_t running_speed_value=0x00;//�����ٶ�
	uint32_t zero_return_creep_speed_value=0;//��������ٶ�
	uint8_t Drive_control_array[20];	//��������������
	uint16_t Data_Bytes=0;//�����ֽ�
//	HAL_GPIO_WritePin(CH2_GPIO_Port,CH2_Pin,GPIO_PIN_RESET);//PC7-���ʹ��������(GPIO_PIN_RESET:��ʼ��GPIO_PIN_SET:ֹͣ��
//	osDelay(1);
//	HAL_GPIO_WritePin(CH2N_GPIO_Port,CH2N_Pin,GPIO_PIN_SET);//PB0-����������(GPIO_PIN_RESET:˳ʱ�룬GPIO_PIN_SET:��ʱ�룩
//	osDelay(1);
//	HAL_TIM_Base_Start_IT(&htim8);//����TIM8��ʱ���жϣ�20KHZ������50us��
  for(;;)
  {
		osDelay(1);	
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
				case 0x01://����Ȧ(MBAP ������ ��ʼ��ַH ��ʼ��ַL ��Ȧ����H ��Ȧ����L)
					if(Drive_control_array[8]==0x20)//�ж���ʼ��ַ
					{
						switch(Drive_control_array[9])//�ж���ʼ��ַ
						{
							case 0x0A://ԭ��ع���ɱ�־

								break;
							
							case 0x64://�ŷ��˶�״̬

								break;

							case 0x01://ԭ��ع�/ֹͣ
								send_buff[4]=0X00;//���ݳ���
								send_buff[5]=0X05;//���ݳ���
								send_buff[8]=0X01;//���������ݵ��ֽڳ���
								if(Position_flag==1)//��ԭ��
								{
									send_buff[9]=0X01;
								}
								else if(Position_flag==0)//����ԭ��
								{
									send_buff[9]=0X00;
								}													
								send(1,send_buff,10);//��ͻ��˷���	
								memset(send_buff,0,13);	
								break;
							
							case 0x02://��ת�㶯/ֹͣ
								
								break;
							
							case 0x03://��ת�㶯/ֹͣ
							
								break;							
							
							case 0x04://����˶�/ֹͣ							
									
								break;
							
							case 0x05://�����˶�/ֹͣ	
								send_buff[4]=0X00;//���ݳ���
								send_buff[5]=0X05;//���ݳ���
								send_buff[8]=0X01;//���������ݵ��ֽڳ���
								if(Sports_status_flag==1)//�˶�
								{
									send_buff[9]=0X01;//����						
								}
								else if(Sports_status_flag==0)//ֹͣ
								{
									send_buff[9]=0X00;//����
								}
								send(1,send_buff,10);//��ͻ��˷���	
								memset(send_buff,0,13);								
								break;
							
							case 0x00://Ӳ��������/ֹͣ
								send_buff[4]=0X00;//���ݳ���
								send_buff[5]=0X05;//���ݳ���
								send_buff[8]=0X01;//���������ݵ��ֽڳ���						
								if(Hard_Trigger_start_flag==1)//����
								{
									send_buff[9]=0X01;//����						
								}
								else//ֹͣ
								{
									send_buff[9]=0X00;//����
								}
								send(1,send_buff,10);//��ͻ��˷���	
								memset(send_buff,0,13);								
								break;
							
							case 0x06://�ŷ���λ
								
								break;

							case 0x09://�״��ϵ�/�ϵ�
								send_buff[4]=0X00;//���ݳ���
								send_buff[5]=0X06;//���ݳ���
								send_buff[8]=0X01;//���������ݵ��ֽڳ���					
//								if()//�ϵ�
//								{
//									send_buff[9]=0X01;//����						
//								}
//								else//
//								{
//									send_buff[9]=0X00;//����
//								}
								send(1,send_buff,10);//��ͻ��˷���	
								memset(send_buff,0,13);								
								break;
							
							default:
								break;
						}
					}
					break;
				
				case 0x02://����ɢ������
					send_buff[4]=0X00;//���ݳ���
					send_buff[5]=0X04;//���ݳ���
					send_buff[8]=0X01;//���������ݵ��ֽڳ���
					if(Drive_control_array[8]==0x00&&Drive_control_array[9]==0x01)//�ж���ʼ��ַ������ת���޹��״̬
					{						
	
					}						
					else if(Drive_control_array[8]==0x00&&Drive_control_array[9]==0x03)//�ж���ʼ��ַ������ת���޹��״̬
					{						
	
					}
					else if(Drive_control_array[8]==0x00&&Drive_control_array[9]==0x04)//�ж���ʼ��ַ���������λ����״̬	
					{
	
					}
					else if(Drive_control_array[8]==0x00&&Drive_control_array[9]==0x05)//�ж���ʼ��ַ�����ŷ�����״̬	
					{	
	
					}
					break;

				case 0x03://�����ּĴ���(MBAP ������ ��ʼ��ַH ��ʼ��ַL �Ĵ�������H �Ĵ�������L)
					Data_Bytes=(Drive_control_array[10]<<8|Drive_control_array[11])*2;//��˴������ݴ�������ȡ�����ֽ�
					send_buff[4]=0X00;//���ݳ���
					send_buff[5]=0x03+Data_Bytes;//���ݳ���
					if(Drive_control_array[8]==0x01)//�ж���ʼ��ַ
					{
						if(Data_Bytes==0x02)
						{
							if(Drive_control_array[9]==0x4A)//�ж���ʼ��ַ��������ʱ��
							{
								send_buff[8]=Data_Bytes;//���������ݵ��ֽڳ���
								send_buff[9]=0X00;//����
								send_buff[10]=0X00;//����
								send(1,send_buff,11);//��ͻ��˷���	
								memset(send_buff,0,13);							
							}
							else if(Drive_control_array[9]==0x4E)//�ж���ʼ��ַ��������ʱ��
							{
								send_buff[8]=Data_Bytes;//���������ݵ��ֽڳ���
								send_buff[9]=0X00;//����
								send_buff[10]=0X00;//����
								send(1,send_buff,11);//��ͻ��˷���	
								memset(send_buff,0,13);														
							}
							else if(Drive_control_array[9]==0x00)//�ж���ʼ��ַ����ʵʱλ���ϴ�����
							{
								send_buff[8]=Data_Bytes;//���������ݵ��ֽڳ���
								send_buff[9]=(Position_upload>>8)&0xFF;//����
								send_buff[10]=Position_upload&0xFF;//����
								send(1,send_buff,11);//��ͻ��˷���	
								memset(send_buff,0,13);															
							}
							else if(Drive_control_array[9]==0x30)//�ж���ʼ��ַ����Ӳ��������
							{
								send_buff[8]=Data_Bytes;//���������ݵ��ֽڳ���
								send_buff[9]=(Hard_Trigger_Number>>8)&0xFF;//����
								send_buff[10]=Hard_Trigger_Number&0xFF;//����
								send(1,send_buff,11);//��ͻ��˷���	
								memset(send_buff,0,13);								
							}
							else if(Drive_control_array[9]==0x32)//�ж���ʼ��ַ������ǰӲ��������
							{
								send_buff[8]=Data_Bytes;//���������ݵ��ֽڳ���
								send_buff[9]=0X00;//����
								send_buff[10]=0X00;//����
								send(1,send_buff,11);//��ͻ��˷���	
								memset(send_buff,0,13);															
							}
						}
						else if(Data_Bytes==0x04)
						{
							if(Drive_control_array[9]==0x2C)//�ж���ʼ��ַ������ʼӲ����λ��
							{
								send_buff[8]=Data_Bytes;//���������ݵ��ֽڳ���
								send_buff[9]=(Hard_Trigger_Position>>24)&0xFF;//����
								send_buff[10]=(Hard_Trigger_Position>>16)&0xFF;//����
								send_buff[11]=(Hard_Trigger_Position>>8)&0xFF;//����
								send_buff[12]=Hard_Trigger_Position&0xFF;//����
								send(1,send_buff,13);//��ͻ��˷���	
								memset(send_buff,0,13);														
							}
							else if(Drive_control_array[9]==0x2E)//�ж���ʼ��ַ����Ӳ�������
							{
								send_buff[8]=Data_Bytes;//���������ݵ��ֽڳ���
								send_buff[9]=(Hard_Trigger_Interval>>24)&0xFF;//�Ĵ���1H-����
								send_buff[10]=(Hard_Trigger_Interval>>16)&0xFF;//�Ĵ���1L����
								send_buff[11]=(Hard_Trigger_Interval>>8)&0xFF;//�Ĵ���2H����
								send_buff[12]=Hard_Trigger_Interval&0xFF;//�Ĵ���2L����
								send(1,send_buff,13);//��ͻ��˷���	
								memset(send_buff,0,13);														
							}
						}
					}
					
					else if(Drive_control_array[8]==0x00)//�ж���ʼ��ַ
					{
						if(Data_Bytes==0x02)
						{
							if(Drive_control_array[9]==0x20)//�ж���ʼ��ַ�����ŷ��������
							{
								send_buff[8]=Data_Bytes;//���������ݵ��ֽڳ���
								send_buff[9]=0X00;//����
								send_buff[10]=0X00;//����
								send(1,send_buff,11);//��ͻ��˷���	
								memset(send_buff,0,13);							
							}
							else if(Drive_control_array[9]==0x96)//�ж���ʼ��ַ����PLC�������
							{
								send_buff[8]=Data_Bytes;//���������ݵ��ֽڳ���
								send_buff[9]=0X00;//����
								send_buff[10]=0X00;//����
								send(1,send_buff,11);//��ͻ��˷���	
								memset(send_buff,0,13);														
							}
						}
						else if(Data_Bytes==0x04)
						{
							if(Drive_control_array[9]==0x64)//�ж���ʼ��ַ���������ٶ�
							{
								send_buff[8]=Data_Bytes;//���������ݵ��ֽڳ���
								send_buff[9]=(zero_return_speed_value>>24)&0xFF;//����
								send_buff[10]=(zero_return_speed_value>>16)&0xFF;//����
								send_buff[11]=(zero_return_speed_value>>8)&0xFF;//����
								send_buff[12]=zero_return_speed_value&0xFF;//����
								send(1,send_buff,13);//��ͻ��˷���	
								memset(send_buff,0,13);																
							}
							else if(Drive_control_array[9]==0x68)//�ж���ʼ��ַ������������ٶ�
							{
								send_buff[8]=Data_Bytes;//���������ݵ��ֽڳ���
								send_buff[9]=(zero_return_creep_speed_value>>24)&0xFF;//�Ĵ���1H-����
								send_buff[10]=(zero_return_creep_speed_value>>16)&0xFF;//�Ĵ���1L-����
								send_buff[11]=(zero_return_creep_speed_value>>8)&0xFF;//�Ĵ���2H-����
								send_buff[12]=zero_return_creep_speed_value&0xFF;//�Ĵ���2L-����
								send(1,send_buff,13);//��ͻ��˷���	
								memset(send_buff,0,13);									
							}
							else if(Drive_control_array[9]==0x78)//�ж���ʼ��ַ���������ٶ�
							{		
								send_buff[8]=Data_Bytes;//���������ݵ��ֽڳ���
								send_buff[9]=(running_speed_value>>24)&0xFF;//�Ĵ���1H-����
								send_buff[10]=(running_speed_value>>16)&0xFF;//�Ĵ���1L-����
								send_buff[11]=(running_speed_value>>8)&0xFF;//�Ĵ���2H-����
								send_buff[12]=running_speed_value&0xFF;//�Ĵ���2L-����
								send(1,send_buff,13);//��ͻ��˷���	
								memset(send_buff,0,13);															
							}
							else if(Drive_control_array[9]==0x7A)//�ж���ʼ��ַ������Զ�λ����
							{
								send_buff[8]=Data_Bytes;//���������ݵ��ֽڳ���
								send_buff[9]=0X00;//�Ĵ���1H-����
								send_buff[10]=0X00;//�Ĵ���1L-����
								send_buff[11]=0X00;//�Ĵ���2H-����
								send_buff[12]=0X00;//�Ĵ���2L-����
								send(1,send_buff,13);//��ͻ��˷���	
								memset(send_buff,0,13);														
							}
							else if(Drive_control_array[9]==0x7C)//�ж���ʼ��ַ�������Զ�λλ��
							{
								send_buff[8]=Data_Bytes;//���������ݵ��ֽڳ���
								send_buff[9]=(Target_Position>>24)&0xFF;//�Ĵ���1H-����
								send_buff[10]=(Target_Position>>16)&0xFF;//�Ĵ���1L-����
								send_buff[11]=(Target_Position>>8)&0xFF;//�Ĵ���2H-����
								send_buff[12]=Target_Position&0xFF;//�Ĵ���2L-����
								send(1,send_buff,13);//��ͻ��˷���	
								memset(send_buff,0,13);														
							}		
							else if(Drive_control_array[9]==0x7E)//�ж���ʼ��ַ�����豸��������г�
							{
								send_buff[8]=Data_Bytes;//���������ݵ��ֽڳ���
								send_buff[9]=0X00;//�Ĵ���1H-����
								send_buff[10]=0X00;//�Ĵ���1L-����
								send_buff[11]=0X00;//�Ĵ���2H-����
								send_buff[12]=0X00;//�Ĵ���2L-����
								send(1,send_buff,13);//��ͻ��˷���	
								memset(send_buff,0,13);														
							}	
							else if(Drive_control_array[9]==0x80)//�ж���ʼ��ַ�������λ������
							{
								send_buff[8]=Data_Bytes;//���������ݵ��ֽڳ���
								send_buff[9]=0X00;//�Ĵ���1H-����
								send_buff[10]=0X00;//�Ĵ���1L-����
								send_buff[11]=0X00;//�Ĵ���2H-����
								send_buff[12]=0X00;//�Ĵ���2L-����
								send(1,send_buff,13);//��ͻ��˷���	
								memset(send_buff,0,13);														
							}	
						}							
					}
					break;
				
				case 0x05://д������Ȧ(����MBAP ������ �Ĵ�����ַH �Ĵ�����ַL ����H(FF/00) ����L(00))
					send_buff[4]=0X00;//���ݳ���
					send_buff[5]=0X06;//���ݳ���
					send_buff[8]=Drive_control_array[8];//�Ĵ�����ַH
					send_buff[9]=Drive_control_array[9];//�Ĵ�����ַH
					if(Drive_control_array[8]==0x20)//�ж���ʼ��ַ
					{
						switch(Drive_control_array[9])//�ж���ʼ��ַ
						{
							case 0x01://ԭ��ع�/ֹͣ
								if(Drive_control_array[10]==0xFF)//�ع�
								{
									if(Position_flag==0)//����ԭ��
									{
										Target_Position_flag=0;
										Motor_Rive(GPIO_PIN_RESET,GPIO_PIN_SET,zero_return_speed_value);//��ʼ��˳ʱ�룬ԭ��ع��ٶ�
										Sports_status_flag=1;
									}	
									send_buff[10]=0XFF;
									send_buff[11]=0X00;
								}
								else if(Drive_control_array[10]==0x00)//ֹͣ
								{
									HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//�ر�PWM���
									HAL_GPIO_WritePin(TIM8_CH1N_GPIO_Port,TIM8_CH1N_Pin,GPIO_PIN_SET);//PA7-���ʹ��������(GPIO_PIN_RESET:��ʼ��GPIO_PIN_SET:ֹͣ��								
									Sports_status_flag=0;	
									send_buff[10]=0X00;
									send_buff[11]=0X00;									
								}
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,13);	
								break;
							
							case 0x02://��ת�㶯/ֹͣ
								if(Drive_control_array[10]==0xFF)//��ʼ
								{
									Motor_Rive(GPIO_PIN_RESET,GPIO_PIN_RESET,running_speed_value);//��ʼ��˳ʱ�룬�����ٶ�
									Sports_status_flag=1;
									Position_flag=0;	
									send_buff[10]=0XFF;
									send_buff[11]=0X00;
								}
								else//ֹͣ
								{
									HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//�ر�PWM���
									HAL_GPIO_WritePin(TIM8_CH1N_GPIO_Port,TIM8_CH1N_Pin,GPIO_PIN_SET);//PA7-���ʹ��������(GPIO_PIN_RESET:��ʼ��GPIO_PIN_SET:ֹͣ��
									Sports_status_flag=0;
									send_buff[10]=0X00;
									send_buff[11]=0X00;									
								}
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,13);								
								break;
							
							case 0x03://��ת�㶯/ֹͣ
								if(Drive_control_array[10]==0xFF)//��ʼ
								{
									Motor_Rive(GPIO_PIN_RESET,GPIO_PIN_SET,running_speed_value);//��ʼ��˳ʱ�룬�����ٶ�
									Position_flag=0;
									Sports_status_flag=1;	
									send_buff[10]=0XFF;
									send_buff[11]=0X00;									
								}
								else//ֹͣ
								{
									HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//�ر�PWM���
									HAL_GPIO_WritePin(TIM8_CH1N_GPIO_Port,TIM8_CH1N_Pin,GPIO_PIN_SET);//PA7-���ʹ��������(GPIO_PIN_RESET:��ʼ��GPIO_PIN_SET:ֹͣ��	
									Sports_status_flag=0;
									send_buff[10]=0X00;
									send_buff[11]=0X00;									
								}
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,13);								
								break;							
							
							case 0x04://����˶�/ֹͣ
								if(Drive_control_array[10]==0xFF)//��ʼ
								{
									Motor_Rive(GPIO_PIN_RESET,GPIO_PIN_RESET,running_speed_value);//��ʼ��˳ʱ�룬�����ٶ�
									Position_flag=0;
									Sports_status_flag=1;
									send_buff[10]=0XFF;
									send_buff[11]=0X00;
								}
								else//ֹͣ
								{
									HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//�ر�PWM���
									HAL_GPIO_WritePin(TIM8_CH1N_GPIO_Port,TIM8_CH1N_Pin,GPIO_PIN_SET);//PA7-���ʹ��������(GPIO_PIN_RESET:��ʼ��GPIO_PIN_SET:ֹͣ��
									Sports_status_flag=0;
									send_buff[10]=0X00;
									send_buff[11]=0X00;
								}
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,13);								
								break;
							
							case 0x05://�����˶�/ֹͣ
								if(Drive_control_array[10]==0xFF)//��ʼ
								{
									Motor_Rive(GPIO_PIN_RESET,GPIO_PIN_RESET,running_speed_value);//��ʼ��˳ʱ�룬�����ٶ�
									Position_flag=0;
									Sports_status_flag=1;
									send_buff[10]=0XFF;
									send_buff[11]=0X00;
								}
								else//ֹͣ
								{
									HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//�ر�PWM���
									HAL_GPIO_WritePin(TIM8_CH1N_GPIO_Port,TIM8_CH1N_Pin,GPIO_PIN_SET);//PA7-���ʹ��������(GPIO_PIN_RESET:��ʼ��GPIO_PIN_SET:ֹͣ��
									Sports_status_flag=0;
									send_buff[10]=0X00;
									send_buff[11]=0X00;
								}
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,13);								
								break;
							
							case 0x14://ֹͣ�˶�״̬
								if(Drive_control_array[10]==0xFF)//ֹͣ
								{
									HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//�ر�PWM���
									HAL_GPIO_WritePin(TIM8_CH1N_GPIO_Port,TIM8_CH1N_Pin,GPIO_PIN_SET);//PA7-���ʹ��������(GPIO_PIN_RESET:��ʼ��GPIO_PIN_SET:ֹͣ��
									Sports_status_flag=0;
									send_buff[10]=0XFF;
									send_buff[11]=0X00;
								}
								else
								{
									send_buff[10]=0X00;
									send_buff[11]=0X00;
								}
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,13);								
								break;
							
							case 0x00://Ӳ��������/ֹͣ
								if(Drive_control_array[10]==0xFF)//��ʼ
								{
									Hard_Trigger_start_flag=1;
									send_buff[10]=0XFF;
									send_buff[11]=0X00;							
								}
								else//ֹͣ
								{
									Hard_Trigger_start_flag=0;
									send_buff[10]=0X00;
									send_buff[11]=0X00;
								}
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,13);								
								break;
							
							case 0x06://�ŷ���λ
							
								break;
							
							case 0x07://�ŷ���ͣ
								
								break;

							case 0x09://�״��ϵ�/�ϵ�
//								if()//
//								{
//									send_buff[10]=0XFF;
//									send_buff[11]=0X00;							
//								}
//								else//
//								{
//									send_buff[10]=0X00;
//									send_buff[11]=0X00;	
//								}
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,13);								
								break;
							
							case 0x0F://PLC�������
							
								break;
							
							default:
								break;													
						}
					}
					break;
				
				case 0x06://д�������ּĴ���(����MBAP ������ �Ĵ�����ַH �Ĵ�����ַL �Ĵ���ֵH �Ĵ���ֵL)
					if(Sports_status_flag==0)//δ��ʼ�˶�
					{
						send_buff[4]=0X00;//���ݳ���
						send_buff[5]=0X06;//���ݳ���
						send_buff[8]=Drive_control_array[8];//�Ĵ�����ַ
						send_buff[9]=Drive_control_array[9];//�Ĵ�����ַ
						send_buff[10]=Drive_control_array[10];//�Ĵ���ֵH
						send_buff[11]=Drive_control_array[11];//�Ĵ���ֵL
						if(Drive_control_array[8]==0x01)//�ж���ʼ��ַ
						{
							if(Drive_control_array[9]==0x4A)//�ж���ʼ��ַ��������ʱ��
							{						
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,13);													
							}
							else if(Drive_control_array[9]==0x4E)//�ж���ʼ��ַ��������ʱ��
							{
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,13);												
							}
							else if(Drive_control_array[9]==0x00)//�ж���ʼ��ַ����ʵʱλ���ϴ�����
							{
								if(Drive_control_array[10]||Drive_control_array[11])
								{
									Position_upload_flag=1;//��ʼʵʱλ���ϴ�		
								}
								else
								{
									Position_upload_flag=0;//ֹͣʵʱλ���ϴ�
								}
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,13);												
							}
							else if(Drive_control_array[9]==0x30)//�ж���ʼ��ַ����Ӳ��������
							{
								Hard_Trigger_Number=(Drive_control_array[10]<<8|Drive_control_array[11]);//��˴������ݴ�����Ӳ��������
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,13);												
							}					
						}
					}										
					break;
					
				case 0x10://д������ּĴ���(����MBAP(7λ) ������ ��ʼ��ַH ��ʼ��ַL �Ĵ�������H �Ĵ�������L �ֽڳ��� �Ĵ���ֵ)
					if(Sports_status_flag==0)//δ��ʼ�˶�
					{
						send_buff[4]=0X00;//���ݳ���
						send_buff[5]=0X06;//���ݳ���
						send_buff[8]=Drive_control_array[8];//�Ĵ�����ʼ��ַH
						send_buff[9]=Drive_control_array[9];//�Ĵ�����ʼ��ַL
						send_buff[10]=Drive_control_array[10];//�Ĵ�������H
						send_buff[11]=Drive_control_array[11];//�Ĵ�������L
						if(Drive_control_array[8]==0x00)//�ж���ʼ��ַ
						{
							if(Drive_control_array[9]==0x64)//�ж���ʼ��ַ���������ٶ�
							{
								zero_return_speed_value=(Drive_control_array[13]<<24|Drive_control_array[14]<<16|Drive_control_array[15]<<8|Drive_control_array[16]);//��˴������ݴ����������ٶ�0.001mm/s
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,13);																
							}
							else if(Drive_control_array[9]==0x68)//�ж���ʼ��ַ������������ٶ�
							{
								zero_return_creep_speed_value=(Drive_control_array[13]<<24|Drive_control_array[14]<<16|Drive_control_array[15]<<8|Drive_control_array[16]);//��˴������ݴ�������������ٶ�0.001mm/s
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,13);									
							}
							else if(Drive_control_array[9]==0x78)//�ж���ʼ��ַ���������ٶ�
							{
								running_speed_value=(Drive_control_array[13]<<24|Drive_control_array[14]<<16|Drive_control_array[15]<<8|Drive_control_array[16]);//��˴������ݴ����������ٶ�0.001mm/s
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,13);		
							}							
							else if(Drive_control_array[9]==0x7A)//�ж���ʼ��ַ������Զ�λ����
							{
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,13);														
							}
							else if(Drive_control_array[9]==0x7C)//�ж���ʼ��ַ�������Զ�λλ��
							{
								Target_Position=(Drive_control_array[13]<<24|Drive_control_array[14]<<16|Drive_control_array[15]<<8|Drive_control_array[16]);//��˴������ݴ��������Զ�λλ��0.001mm
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,13);														
							}		
							else if(Drive_control_array[9]==0x7E)//�ж���ʼ��ַ�����豸��������г�
							{
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,13);														
							}	
							else if(Drive_control_array[9]==0x80)//�ж���ʼ��ַ�������λ������
							{
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,13);														
							}	
						}
						else if(Drive_control_array[8]==0x01)//�ж���ʼ��ַ
						{
							if(Drive_control_array[9]==0x2C)//�ж���ʼ��ַ������ʼӲ����λ��
							{
								Hard_Trigger_Position=(Drive_control_array[13]<<24|Drive_control_array[14]<<16|Drive_control_array[15]<<8|Drive_control_array[16]);//��˴������ݴ�������ʼӲ����λ��0.001mm
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,13);														
							}
							else if(Drive_control_array[9]==0x2E)//�ж���ʼ��ַ����Ӳ�������
							{
								Hard_Trigger_Interval=(Drive_control_array[13]<<24|Drive_control_array[14]<<16|Drive_control_array[15]<<8|Drive_control_array[16]);//��˴������ݴ�����Ӳ�������0.001mm
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,13);														
							}				
						}	
					}						
					break;
					
				default:
					break;						
			}				
			memset(Drive_control_array,0,20);		
		}
  }
}


float sd;
uint32_t Trigger_Interval;//�������
void Encoder(void *argument)//�綯��������ͨѶ
{
//	if(Position_flag==1)//��λ��ԭ���ʼ�ɼ�������
//	{
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
		Position=0;
		Position_upload=0;
//	}
  for(;;)
  {
		osDelay(1);
		if(Encoder_Processing_flag==1)
		{
			Encoder_Processing_flag=0;
			motor1.direct = __HAL_TIM_IS_TIM_COUNTING_DOWN(&htim2);//������ϼ�������ת��������ֵΪ0�����򷵻�ֵΪ1
			int16_t pluse = COUNTERNUM - (RELOADVALUE)/2;
			motor1.totalCount = pluse+ motor1.overflowNum * (int16_t)RELOADVALUE/2;//�ӿ�ʼ�����ڵ�ǰ5ms����������	
			motor1.speed = (float)(motor1.totalCount - motor1.lastCount) / (4 * MOTOR_SPEED_RERATIO * PULSE_PRE_ROUND);//���5msת�˶���Ȧ,����4����Ϊ4��Ƶ
//			motor1.speed = Speed_Low_Filter(motor1.speed,speed_Record);//�˲�����
			motor1.lastCount = motor1.totalCount; //��¼��һ�εļ���ֵ 
			motor1.distance = One_Circle_Distance * motor1.speed;//����5ms�ƶ����루��λmm��	
			sd=motor1.distance*200;//1���ƶ��ľ���
			if(Hard_Trigger_flag==1)
			{
				Trigger_Interval=Trigger_Interval+motor1.distance*1000;
				if(Trigger_Interval>=Hard_Trigger_Interval)
				{
					Hard_Trigger_flag=0;
					Trigger_Interval=0;					
				}			
			}
			if(Position_flag==1)//ԭ��У׼
			{	
				motor1.lastCount=0;
				motor1.totalCount=0;
				motor1.overflowNum=0;
				motor1.speed=0;
				motor1.direct = 0;
				motor1.distance = 0;
				Position=0;
				Position_upload=0;
				pluse=0;
			}	
			else
			{
				Position = Position + motor1.distance;	//��ԭ�����
				Position_upload = Position*1000;//λ���ϴ����ݣ���λ0.001mm��
				if(Position_upload>=Target_Position&&Target_Position_flag==1)//������Զ�λλ��ֹͣ�˶���֮ǰ���й�����
				{
					HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//�ر�PWM���
					HAL_GPIO_WritePin(TIM8_CH1N_GPIO_Port,TIM8_CH1N_Pin,GPIO_PIN_SET);//PA7-���ʹ��������(GPIO_PIN_RESET:��ʼ��GPIO_PIN_SET:ֹͣ��
					Sports_status_flag=0;	//�˶�״̬				
				}						
			}
		}	
  }
}

void Pulse_Emission(void *argument)//���崥����TIM13ͨ����
{	
  for(;;)
  {
		osDelay(1);
		if(Hard_Trigger_start_flag==1&&Position_upload>=Hard_Trigger_Position&&Sports_status_flag==1)//Ӳ��������(Ӳ��������������Ӳ����λ�ã��˶�״̬)
		{
			if(Hard_Trigger_flag==0)
			{
				if(Hard_Trigger_Current_Number<Hard_Trigger_Number)//Ӳ���������ж�
				{		
					Hard_Trigger_Current_Number++;
					HAL_GPIO_WritePin(IO_OUT_GPIO_Port, IO_OUT_Pin, GPIO_PIN_SET);//PA2-Ӳ����
					HAL_TIM_Base_Start_IT(&htim13);//����TIM13��ʱ���жϣ�10MHZ������500�Σ�50us��
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


void Extreme_position_Drive(void *argument)//����λ�ÿ���
{
	uint8_t number_a=0xff;
  for(;;)
  {
		osDelay(1);
		if(Extreme_position_flag==1&&Sports_status_flag==1)
		{
			HAL_GPIO_WritePin(TIM8_CH1N_GPIO_Port,TIM8_CH1N_Pin,GPIO_PIN_RESET);//PA7-���ʹ��������(GPIO_PIN_RESET:��ʼ��GPIO_PIN_SET:ֹͣ��
			osDelay(1);	
			HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_7);//���Ƶ���������Ƶ������ת	
			osDelay(1);	
			while(number_a>zero_return_speed_value)
			{
				number_a--;
				HAL_TIM_Base_Start_IT(&htim8);//����TIM8��ʱ���жϣ�20KHZ������50us��
				__HAL_TIM_SET_AUTORELOAD(&htim8,number_a);//�޸��Զ���װ��ֵ
				HAL_TIM_PWM_Start_IT(&htim8,TIM_CHANNEL_1);//����TIM8_CH1��PWM��������жϣ�
			if(number_a>=0x96)
			{
				HAL_Delay(5);
			}
			else if(0x96>number_a&&number_a>=0x28)
			{
				HAL_Delay(15);	
			}	
			else
			{
				HAL_Delay(20);
			}	
			}
			number_a=0xff;
			Extreme_position_flag=0;
		}
  }
}
/* USER CODE END Application */

