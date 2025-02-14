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

/* TIM8_CH1(PWM���)-OUT1��TIM8_CH1N(���ʹ��)-OUT2��TIM8_CH2(����������)-OUT3��TIM8_CH2N(�ŷ���λ)-OUT4��TIM8_CH3(�״��ϵ�)-OUT5��TIM8_CH4(�����������)-OUT6 */

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

	
/***********************************************variable***********************************/
float    variable_Position=0;//��0��λ��,��λmm
int32_t variable_Position_upload=0;//��0������ϴ�ֵ����λ0.001mm��
uint32_t variable_Zero_return_speed_value=10000;//�����ٶȣ���λ0.001mm/s��
uint32_t variable_Return_to_zero_parameter=0;//���������0~4:���λ�ԭ���ٶȣ�5:��ԭ��ʱֹͣ��ʽ��6~14:��ԭ���ٶȣ�15:��ԭ�㷽��
uint32_t variable_Running_speed_value=5000;//�����ٶȣ���λ0.001mm/s��
uint32_t variable_Target_position=0;//Ŀ��λ��
uint32_t variable_Zero_position=0;//���λ��	
uint32_t variable_Hard_trigger_interval=0;//Ӳ����ʱ����
uint32_t variable_Hard_trigger_position=0;//Ӳ������ʼλ��
uint32_t variable_Trigger_interval;//�������
float    variable_Position_lastpulsec;//�ϴ�Ӳ����λ��	
uint16_t variable_Register_address;//�Ĵ�����ַ

/***********************************************flag***************************************/
uint8_t flag_Motion_state=0;//�˶�״̬��־λ
uint8_t flag_Position_upload=0;//λ���ϴ���־λ
uint8_t flag_Real_time_location_query=0;//ʵʱλ��������ѯ��־λ
uint8_t flag_Encoder_processing=0;//���������ݴ����־λ
uint8_t flag_Zero_regression=0;//�Ƿ�������־λ
uint8_t flag_Absolute_position=0;//����λ�ñ�־λ
uint8_t flag_Hard_trigger_start=0;//Ӳ������ʼ��־λ
uint8_t flag_Hard_trigger=0;//Ӳ������־λ	
uint8_t flag_Motor_deceleration_stops=0;//�������ֹͣ��־λ
uint8_t flag_Resend_timeout=0;//��ʱ�ط���־λ

/***********************************************number*************************************/	
uint16_t number_Hard_trigger=0;//Ӳ��������
uint16_t number_Hard_trigger_curren=0;//Ӳ������ǰ����
uint8_t  number_Iwdg=0;//���Ź�����
int32_t  number_Motor_pulsec;//������������

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

osThreadId_t RS485_Receive_functionTaskHandle;
const osThreadAttr_t RS485_Receive_functionTask_attributes = {
  .name = "RS485_Receive_functionTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

osThreadId_t RS485_Transmit_functionTaskHandle;
const osThreadAttr_t RS485_Transmit_functionTask_attributes = {
  .name = "RS485_Transmit_functionTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

osThreadId_t Resend_timeoutTaskHandle;
const osThreadAttr_t Resend_timeoutTask_attributes = {
  .name = "Resend_timeoutTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

osThreadId_t Position_queryTaskHandle;
const osThreadAttr_t Position_queryTask_attributes = {
  .name = "Position_queryTask",
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

osMessageQueueId_t RS485_ReceiveQueueHandle;
const osMessageQueueAttr_t RS485_ReceiveQueue_attributes = {
  .name = "RS485_ReceiveQueue"
};

osMessageQueueId_t RS485_TransmitQueueHandle;
const osMessageQueueAttr_t RS485_TransmitQueue_attributes = {
  .name = "RS485_TransmitQueue"
};


void Ethernet_Server(void *argument);//����ͨ��-�����
void Encoder(void *argument);//�綯��������ͨѶ
void Drive(void *argument);//����������
void Pulse_Emission(void *argument);//���巢��
void Uart3_Communicate(void *argument);//����λ�ÿ���
void Iwdg_function(void *argument);//���Ź�����
void RS485_Receive_function(void *argument);//RS485����
void RS485_Transmit_function(void *argument);//RS485����
void Resend_timeout(void *argument);//RS485���ͽ�����Ӧ��ʱ
void Position_query(void *argument);//λ�ò�ѯ
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
	Uart3_CommunicateTaskHandle = osThreadNew(Uart3_Communicate, NULL, &Uart3_CommunicateTask_attributes);
	Iwdg_functionTaskHandle = osThreadNew(Iwdg_function, NULL, &Iwdg_functionTask_attributes);
	RS485_Receive_functionTaskHandle = osThreadNew(RS485_Receive_function, NULL, &RS485_Receive_functionTask_attributes);
	RS485_Transmit_functionTaskHandle = osThreadNew(RS485_Transmit_function, NULL, &RS485_Transmit_functionTask_attributes);
	Resend_timeoutTaskHandle = osThreadNew(Resend_timeout, NULL, &Resend_timeoutTask_attributes);
	Position_queryTaskHandle = osThreadNew(Position_query, NULL, &Position_queryTask_attributes);
	
	Ethernet_ReceiveHandle = osMessageQueueNew (5, 20*sizeof(uint8_t), &Ethernet_Receive_attributes);
	Ethernet_SendHandle = osMessageQueueNew (5, 20*sizeof(uint8_t), &Ethernet_Send_attributes);
	Motor_driveQueueHandle = osMessageQueueNew (5, 3*sizeof(uint8_t), &Motor_driveQueue_attributes);
	RS485_ReceiveQueueHandle = osMessageQueueNew (10, 20*sizeof(uint8_t), &RS485_ReceiveQueue_attributes);
	RS485_TransmitQueueHandle = osMessageQueueNew (10, 20*sizeof(uint8_t), &RS485_TransmitQueue_attributes);
	
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
//	if(REACH)//ԭ���ж�
//	{
//		variable_Position=0;
//		flag_Zero_regression=1;
//		number_Motor_pulsec=0;
//	}
//	else 
//	{
//		number_Motor_pulsec=0xffffff;
//		variable_Position=2000;
//	}
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
  W5500Init();//w5500��ʼ��
  ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);  //�����������
  wiz_PhyConf phyconf;
  phyconf.by = PHY_CONFBY_SW;
  phyconf.duplex = PHY_DUPLEX_FULL;
  phyconf.speed = PHY_SPEED_10;
  phyconf.mode = PHY_MODE_AUTONEGO;
  ctlwizchip(CW_SET_PHYCONF, (void*)&phyconf); //����PHY����
  for(;;)
  {
		net_process_socket(1, LISTEN_PORT_1);
		net_process_socket(2, LISTEN_PORT_2);
		osDelay(10);
  }
}

void Drive(void *argument)//����������
{
	uint8_t	 array_Send_buff[13];
	uint8_t  array_Drive_control[20];	//��������������
	uint8_t  array_drive_TXbuffer[20];//�����������
	uint8_t  status=0;	
	uint32_t variable_Zero_return_creep_speed_value=0;//��������ٶ�
	uint16_t number_Data_bytes=0;//�����ֽ�
  for(;;)
  {
		osDelay(1);	
		status=osMessageQueueGet(Ethernet_ReceiveHandle,array_Drive_control,NULL,10);
		if(status==osOK)
		{		
			number_Iwdg=0;	
			array_Send_buff[0]=array_Drive_control[0];//0X19;�������ʶ
			array_Send_buff[1]=array_Drive_control[1];//0XB2;�������ʶ
			array_Send_buff[2]=0X00;//Э���ʶ����00 00ΪmodbusЭ��
			array_Send_buff[3]=0X00;//Э���ʶ����00 00ΪmodbusЭ��
			array_Send_buff[6]=array_Drive_control[6];//0XDB;�豸��ַ
			array_Send_buff[7]=array_Drive_control[7];//������
			switch(array_Drive_control[7])//�жϹ�����
			{
				case 0x01://����Ȧ(MBAP ������ ��ʼ��ַH ��ʼ��ַL ��Ȧ����H ��Ȧ����L)
					if(array_Drive_control[8]==0x20)//�ж���ʼ��ַ
					{
						switch(array_Drive_control[9])//�ж���ʼ��ַ
						{
							case 0x0A://ԭ��ع���ɱ�־

								break;
							
							case 0x64://�ŷ��˶�״̬
								array_Send_buff[4]=0X00;//���ݳ���
								array_Send_buff[5]=0X05;//���ݳ���
								array_Send_buff[8]=0X01;//���������ݵ��ֽڳ���
								if(flag_Motion_state==1)//�˶�
								{
									array_Send_buff[9]=0X01;//����						
								}
								else if(flag_Motion_state==0)//ֹͣ
								{
									array_Send_buff[9]=0X00;//����
								}
								send(1,array_Send_buff,10);//��ͻ��˷���	
								break;

							case 0x01://ԭ��ع�/ֹͣ
								array_Send_buff[4]=0X00;//���ݳ���
								array_Send_buff[5]=0X05;//���ݳ���
								array_Send_buff[8]=0X01;//���������ݵ��ֽڳ���
								if(flag_Zero_regression==1)//����
								{
									array_Send_buff[9]=0X00;
								}
								else//������
								{
									array_Send_buff[9]=0X01;
								}													
								send(1,array_Send_buff,10);//��ͻ��˷���	
								break;
							
							case 0x02://��ת�㶯/ֹͣ
								
								break;
							
							case 0x03://��ת�㶯/ֹͣ
							
								break;							
							
							case 0x04://����˶�/ֹͣ							
									
								break;
							
							case 0x05://�����˶�/ֹͣ	
								array_Send_buff[4]=0X00;//���ݳ���
								array_Send_buff[5]=0X05;//���ݳ���
								array_Send_buff[8]=0X01;//���������ݵ��ֽڳ���
								if(flag_Motion_state==1)//�˶�
								{
									array_Send_buff[9]=0X01;//����								
								}
								else if(flag_Motion_state==0)//ֹͣ
								{
									array_Send_buff[9]=0X00;//����
								}
								send(1,array_Send_buff,10);//��ͻ��˷���								
								break;
							
							case 0x00://Ӳ��������/ֹͣ
								array_Send_buff[4]=0X00;//���ݳ���
								array_Send_buff[5]=0X05;//���ݳ���
								array_Send_buff[8]=0X01;//���������ݵ��ֽڳ���						
								if(flag_Hard_trigger_start==1)//����
								{
									array_Send_buff[9]=0X01;//����						
								}
								else//ֹͣ
								{
									array_Send_buff[9]=0X00;//����
								}
								send(1,array_Send_buff,10);//��ͻ��˷���							
								break;
							
							case 0x06://�ŷ���λ
								array_Send_buff[4]=0X00;//���ݳ���
								array_Send_buff[5]=0X05;//���ݳ���
								array_Send_buff[8]=0X01;//���������ݵ��ֽڳ���
								array_Send_buff[9]=0X01;												
								send(1,array_Send_buff,10);//��ͻ��˷���	
								break;

							case 0x09://�״��ϵ�/�ϵ�
								array_Send_buff[4]=0X00;//���ݳ���
								array_Send_buff[5]=0X06;//���ݳ���
								array_Send_buff[8]=0X01;//���������ݵ��ֽڳ���					
								if(RADAR_ON)//�ϵ�
								{
									array_Send_buff[9]=0X01;//����						
								}
								else if(RADAR_OFF)//�ϵ�
								{
									array_Send_buff[9]=0X00;//����
								}
								send(1,array_Send_buff,10);//��ͻ��˷���	
								break;
						
							default:
								break;
						}
					}
					memset(array_Send_buff,0,13);		
					break;
				
				case 0x02://����ɢ������
					array_Send_buff[4]=0X00;//���ݳ���
					array_Send_buff[5]=0X04;//���ݳ���
					array_Send_buff[8]=0X01;//���������ݵ��ֽڳ���
					if(array_Drive_control[8]==0x00&&array_Drive_control[9]==0x01)//�ж���ʼ��ַ������ת���޹��״̬
					{					
						if(HAL_GPIO_ReadPin(IN2_GPIO_Port,IN2_Pin)==GPIO_PIN_RESET)//������λ
						{
							array_Send_buff[9]=0X01;//����
							send(1,array_Send_buff,10);//��ͻ��˷���	
						}
						else
						{
							array_Send_buff[9]=0X00;//����
							send(1,array_Send_buff,10);//��ͻ��˷���	
						}
					}						
					else if(array_Drive_control[8]==0x00&&array_Drive_control[9]==0x03)//�ж���ʼ��ַ������ת���޹��״̬
					{	
						if(HAL_GPIO_ReadPin(IN0_GPIO_Port,IN0_Pin)==GPIO_PIN_RESET)//������λ
						{
							array_Send_buff[9]=0X01;//����
							send(1,array_Send_buff,10);//��ͻ��˷���	
						}
						else
						{
							array_Send_buff[9]=0X00;//����
							send(1,array_Send_buff,10);//��ͻ��˷���	
						}						
					}
					else if(array_Drive_control[8]==0x00&&array_Drive_control[9]==0x04)//�ж���ʼ��ַ���������λ����״̬	
					{
						if(HAL_GPIO_ReadPin(IN1_GPIO_Port,IN1_Pin)==GPIO_PIN_RESET)//������λ
						{
							array_Send_buff[9]=0X01;//����
							send(1,array_Send_buff,10);//��ͻ��˷���	
						}
						else
						{
							array_Send_buff[9]=0X00;//����
							send(1,array_Send_buff,10);//��ͻ��˷���	
						}
					}
					else if(array_Drive_control[8]==0x00&&array_Drive_control[9]==0x05)//�ж���ʼ��ַ�����ŷ�����״̬	
					{	
	
					}
					memset(array_Send_buff,0,13);	
					break;

				case 0x03://�����ּĴ���(MBAP ������ ��ʼ��ַH ��ʼ��ַL �Ĵ�������H �Ĵ�������L)
					number_Data_bytes=(array_Drive_control[10]<<8|array_Drive_control[11])*2;//��˴������ݴ�������ȡ�����ֽ�
					array_Send_buff[4]=0X00;//���ݳ���
					array_Send_buff[5]=0x03+number_Data_bytes;//���ݳ���
					if(array_Drive_control[8]==0x01)//�ж���ʼ��ַ
					{
						if(number_Data_bytes==0x02)
						{
							if(array_Drive_control[9]==0x4A)//�ж���ʼ��ַ��������ʱ��
							{
								array_Send_buff[8]=number_Data_bytes;//���������ݵ��ֽڳ���
								array_Send_buff[9]=0X00;//����
								array_Send_buff[10]=0X00;//����
								send(1,array_Send_buff,11);//��ͻ��˷���							
							}
							else if(array_Drive_control[9]==0x4E)//�ж���ʼ��ַ��������ʱ��
							{
								array_Send_buff[8]=number_Data_bytes;//���������ݵ��ֽڳ���
								array_Send_buff[9]=0X00;//����
								array_Send_buff[10]=0X00;//����
								send(1,array_Send_buff,11);//��ͻ��˷���													
							}
							else if(array_Drive_control[9]==0x00)//�ж���ʼ��ַ����ʵʱλ���ϴ�����
							{
								array_Send_buff[8]=number_Data_bytes;//���������ݵ��ֽڳ���
								array_Send_buff[9]=(variable_Position_upload>>8)&0xFF;//����
								array_Send_buff[10]=variable_Position_upload&0xFF;//����
								send(1,array_Send_buff,11);//��ͻ��˷���															
							}
							else if(array_Drive_control[9]==0x30)//�ж���ʼ��ַ����Ӳ��������
							{
								array_Send_buff[8]=number_Data_bytes;//���������ݵ��ֽڳ���
								array_Send_buff[9]=(number_Hard_trigger>>8)&0xFF;//����
								array_Send_buff[10]=number_Hard_trigger&0xFF;//����
								send(1,array_Send_buff,11);//��ͻ��˷���								
							}
							else if(array_Drive_control[9]==0x32)//�ж���ʼ��ַ������ǰӲ��������
							{
								array_Send_buff[8]=number_Data_bytes;//���������ݵ��ֽڳ���
								array_Send_buff[9]=0X00;//����
								array_Send_buff[10]=0X00;//����
								send(1,array_Send_buff,11);//��ͻ��˷���															
							}
							else if(array_Drive_control[9]==0x77)//�ж���ʼ��ַ�����ϵ���Ƿ�ִ�й�����
							{
								array_Send_buff[8]=number_Data_bytes;//���������ݵ��ֽڳ���
								if(flag_Zero_regression==1)
								{
									array_Send_buff[9]=0X00;//����
									array_Send_buff[10]=0X01;//����
									send(1,array_Send_buff,11);//��ͻ��˷���	
								}
								else if(flag_Zero_regression==0)
								{
									array_Send_buff[9]=0X00;//����
									array_Send_buff[10]=0X00;//����
									send(1,array_Send_buff,11);//��ͻ��˷���	
								}									
							}
						}
						else if(number_Data_bytes==0x04)
						{
							if(array_Drive_control[9]==0x2C)//�ж���ʼ��ַ������ʼӲ����λ��
							{
								array_Send_buff[8]=number_Data_bytes;//���������ݵ��ֽڳ���
								array_Send_buff[11]=(variable_Hard_trigger_position>>24)&0xFF;//����
								array_Send_buff[12]=(variable_Hard_trigger_position>>16)&0xFF;//����
								array_Send_buff[9]=(variable_Hard_trigger_position>>8)&0xFF;//����
								array_Send_buff[10]=variable_Hard_trigger_position&0xFF;//����
								send(1,array_Send_buff,13);//��ͻ��˷���													
							}
							else if(array_Drive_control[9]==0x2E)//�ж���ʼ��ַ����Ӳ�������
							{
								array_Send_buff[8]=number_Data_bytes;//���������ݵ��ֽڳ���
								array_Send_buff[11]=(variable_Hard_trigger_interval>>24)&0xFF;//�Ĵ���1H-����
								array_Send_buff[12]=(variable_Hard_trigger_interval>>16)&0xFF;//�Ĵ���1L����
								array_Send_buff[9]=(variable_Hard_trigger_interval>>8)&0xFF;//�Ĵ���2H����
								array_Send_buff[10]=variable_Hard_trigger_interval&0xFF;//�Ĵ���2L����
								send(1,array_Send_buff,13);//��ͻ��˷���														
							}
						}
					}
					
					else if(array_Drive_control[8]==0x00)//�ж���ʼ��ַ
					{
						if(number_Data_bytes==0x02)
						{
							if(array_Drive_control[9]==0x20)//�ж���ʼ��ַ�����ŷ��������
							{
								array_Send_buff[8]=number_Data_bytes;//���������ݵ��ֽڳ���
								array_Send_buff[9]=0X00;//����
								array_Send_buff[10]=0X00;//����
								send(1,array_Send_buff,11);//��ͻ��˷���							
							}
							else if(array_Drive_control[9]==0x96)//�ж���ʼ��ַ����PLC�������
							{
								array_Send_buff[8]=number_Data_bytes;//���������ݵ��ֽڳ���
								array_Send_buff[9]=0X00;//����
								array_Send_buff[10]=0X00;//����
								send(1,array_Send_buff,11);//��ͻ��˷���													
							}
						}
						else if(number_Data_bytes==0x04)
						{
							if(array_Drive_control[9]==0x64)//�ж���ʼ��ַ���������ٶ�
							{
								array_Send_buff[8]=number_Data_bytes;//���������ݵ��ֽڳ���
								array_Send_buff[11]=(variable_Zero_return_speed_value>>24)&0xFF;//����
								array_Send_buff[12]=(variable_Zero_return_speed_value>>16)&0xFF;//����
								array_Send_buff[9]=(variable_Zero_return_speed_value>>8)&0xFF;//����
								array_Send_buff[10]=variable_Zero_return_speed_value&0xFF;//����
								send(1,array_Send_buff,13);//��ͻ��˷���																
							}
							else if(array_Drive_control[9]==0x68)//�ж���ʼ��ַ������������ٶ�
							{
								array_Send_buff[8]=number_Data_bytes;//���������ݵ��ֽڳ���
								array_Send_buff[11]=(variable_Zero_return_creep_speed_value>>24)&0xFF;//�Ĵ���1H-����
								array_Send_buff[12]=(variable_Zero_return_creep_speed_value>>16)&0xFF;//�Ĵ���1L-����
								array_Send_buff[9]=(variable_Zero_return_creep_speed_value>>8)&0xFF;//�Ĵ���2H-����
								array_Send_buff[10]=variable_Zero_return_creep_speed_value&0xFF;//�Ĵ���2L-����
								send(1,array_Send_buff,13);//��ͻ��˷���									
							}
							else if(array_Drive_control[9]==0x78)//�ж���ʼ��ַ���������ٶ�
							{		
								array_Send_buff[8]=number_Data_bytes;//���������ݵ��ֽڳ���
								array_Send_buff[11]=(variable_Running_speed_value>>24)&0xFF;//�Ĵ���1H-����
								array_Send_buff[12]=(variable_Running_speed_value>>16)&0xFF;//�Ĵ���1L-����
								array_Send_buff[9]=(variable_Running_speed_value>>8)&0xFF;//�Ĵ���2H-����
								array_Send_buff[10]=variable_Running_speed_value&0xFF;//�Ĵ���2L-����
								send(1,array_Send_buff,13);//��ͻ��˷���															
							}
							else if(array_Drive_control[9]==0x7A)//�ж���ʼ��ַ������Զ�λ����
							{
								array_Send_buff[8]=number_Data_bytes;//���������ݵ��ֽڳ���
								array_Send_buff[11]=0X00;//�Ĵ���1H-����
								array_Send_buff[12]=0X00;//�Ĵ���1L-����
								array_Send_buff[9]=0X00;//�Ĵ���2H-����
								array_Send_buff[10]=0X00;//�Ĵ���2L-����
								send(1,array_Send_buff,13);//��ͻ��˷���														
							}
							else if(array_Drive_control[9]==0x7C)//�ж���ʼ��ַ�������Զ�λλ��
							{
								array_Send_buff[8]=number_Data_bytes;//���������ݵ��ֽڳ���
								array_Send_buff[11]=(variable_Target_position>>24)&0xFF;//�Ĵ���1H-����
								array_Send_buff[12]=(variable_Target_position>>16)&0xFF;//�Ĵ���1L-����
								array_Send_buff[9]=(variable_Target_position>>8)&0xFF;//�Ĵ���2H-����
								array_Send_buff[10]=variable_Target_position&0xFF;//�Ĵ���2L-����
								send(1,array_Send_buff,13);//��ͻ��˷���													
							}		
							else if(array_Drive_control[9]==0x7E)//�ж���ʼ��ַ�����豸��������г�
							{
								array_Send_buff[8]=number_Data_bytes;//���������ݵ��ֽڳ���
								array_Send_buff[9]=0X00;//�Ĵ���1H-����
								array_Send_buff[10]=0X00;//�Ĵ���1L-����
								array_Send_buff[11]=0X00;//�Ĵ���2H-����
								array_Send_buff[12]=0X00;//�Ĵ���2L-����
								send(1,array_Send_buff,13);//��ͻ��˷���													
							}	
							else if(array_Drive_control[9]==0x80)//�ж���ʼ��ַ�������λ������
							{
								array_Send_buff[8]=number_Data_bytes;//���������ݵ��ֽڳ���
								array_Send_buff[11]=(variable_Zero_position>>24)&0xFF;//�Ĵ���1H-����
								array_Send_buff[12]=(variable_Zero_position>>16)&0xFF;//�Ĵ���1L-����
								array_Send_buff[9]=(variable_Zero_position>>8)&0xFF;//�Ĵ���2H-����
								array_Send_buff[10]=variable_Zero_position&0xFF;//�Ĵ���2L-����
								send(1,array_Send_buff,13);//��ͻ��˷���													
							}	
							else if(array_Drive_control[9]==0x87)//�ж���ʼ��ַ����ʵʱλ��������ȡ
							{
//								int number_wait=0;
								flag_Encoder_processing=0;
								flag_Real_time_location_query=1;
								array_drive_TXbuffer[0]=0x01;//վ��
								array_drive_TXbuffer[1]=0X03;//������
								array_drive_TXbuffer[2]=0X00;//��ַ��λ
								array_drive_TXbuffer[3]=0x04;//��ַ��λ
								array_drive_TXbuffer[4]=0x00;//���ݸ�λ
								array_drive_TXbuffer[5]=0x02;//���ݵ�λ
								osMessageQueuePut(RS485_TransmitQueueHandle,array_drive_TXbuffer,1,0);				
//								while(flag_Real_time_location_query==1&&number_wait<100)
//								{
//									number_wait++;
//									osDelay(10);
//								}
//								array_Send_buff[8]=number_Data_bytes;//���������ݵ��ֽڳ���
//								array_Send_buff[11]=(variable_Position_upload>>24)&0xFF;//�Ĵ���1H-����
//								array_Send_buff[12]=(variable_Position_upload>>16)&0xFF;//�Ĵ���1L-����
//								array_Send_buff[9]=(variable_Position_upload>>8)&0xFF;//�Ĵ���2H-����
//								array_Send_buff[10]=variable_Position_upload&0xFF;//�Ĵ���2L-����
//								send(1,array_Send_buff,13);//��ͻ��˷���							
							}	
						}							
					}
					memset(array_Send_buff,0,13);		
					break;
				
				case 0x05://д������Ȧ(����MBAP ������ �Ĵ�����ַH �Ĵ�����ַL ����H(FF/00) ����L(00))
					array_Send_buff[4]=0X00;//���ݳ���
					array_Send_buff[5]=0X06;//���ݳ���
					array_Send_buff[8]=array_Drive_control[8];//�Ĵ�����ַH
					array_Send_buff[9]=array_Drive_control[9];//�Ĵ�����ַH
					if(array_Drive_control[8]==0x20)//�ж���ʼ��ַ
					{
						switch(array_Drive_control[9])//�ж���ʼ��ַ
						{
							case 0x01://ԭ��ع�/ֹͣ
								if(array_Drive_control[10]==0xFF)//�ع�
								{
									array_Send_buff[10]=0XFF;
									array_Send_buff[11]=0X00;
									send(1,array_Send_buff,12);//��ͻ��˷���	
									flag_Zero_regression=0;
									if(variable_Position<=0)//��ͷ��-�����ƶ�
									{
										variable_Return_to_zero_parameter=((variable_Zero_return_speed_value/1000*12)&0x1FF)<<6;//����
										array_drive_TXbuffer[0]=0x01;//վ��
										array_drive_TXbuffer[1]=0X06;//������
										array_drive_TXbuffer[2]=0X00;//��ַ��λ
										array_drive_TXbuffer[3]=0xC9;//��ַ��λ
										array_drive_TXbuffer[4]=(variable_Return_to_zero_parameter>>8)&0xFF;//���ݸ�λ
										array_drive_TXbuffer[5]=(variable_Return_to_zero_parameter)&0xFF;//���ݵ�λ
										osMessageQueuePut(RS485_TransmitQueueHandle,array_drive_TXbuffer,1,0);	
										flag_Motion_state=1;
									}
									else
									{
										variable_Return_to_zero_parameter=((variable_Zero_return_speed_value/1000*12)&0x1FF)<<6|0x8000;//����
										array_drive_TXbuffer[0]=0x01;//վ��
										array_drive_TXbuffer[1]=0X06;//������
										array_drive_TXbuffer[2]=0X00;//��ַ��λ
										array_drive_TXbuffer[3]=0xC9;//��ַ��λ
										array_drive_TXbuffer[4]=(variable_Return_to_zero_parameter>>8)&0xFF;//���ݸ�λ
										array_drive_TXbuffer[5]=(variable_Return_to_zero_parameter)&0xFF;//���ݵ�λ
										osMessageQueuePut(RS485_TransmitQueueHandle,array_drive_TXbuffer,1,0);
										flag_Motion_state=1;										
									}										
								}
								else if(array_Drive_control[10]==0x00)//ֹͣ
								{
									array_Send_buff[10]=0X00;
									array_Send_buff[11]=0X00;
									send(1,array_Send_buff,12);//��ͻ��˷���	
									array_drive_TXbuffer[0]=0x01;//վ��
									array_drive_TXbuffer[1]=0X06;//������
									array_drive_TXbuffer[2]=0X00;//��ַ��λ
									array_drive_TXbuffer[3]=0xC8;//��ַ��λ
									array_drive_TXbuffer[4]=0X00;//���ݸ�λ
									array_drive_TXbuffer[5]=0X00;//���ݵ�λ
									osMessageQueuePut(RS485_TransmitQueueHandle,array_drive_TXbuffer,1,0);
									flag_Motion_state=0;									
								}
								break;
							
							case 0x02://��ת�㶯/ֹͣ
								if(array_Drive_control[10]==0xFF)//��ʼ
								{
									array_Send_buff[10]=0XFF;
									array_Send_buff[11]=0X00;
									send(1,array_Send_buff,12);//��ͻ��˷���	
									array_drive_TXbuffer[0]=0x01;//վ��
									array_drive_TXbuffer[1]=0X06;//������
									array_drive_TXbuffer[2]=0X00;//��ַ��λ
									array_drive_TXbuffer[3]=0xC8;//��ַ��λ
									array_drive_TXbuffer[4]=0X00;//���ݸ�λ
									array_drive_TXbuffer[5]=0X01;//���ݵ�λ
									osMessageQueuePut(RS485_TransmitQueueHandle,array_drive_TXbuffer,1,0);
									flag_Motion_state=1;
								}
								else//ֹͣ
								{
									array_Send_buff[10]=0X00;
									array_Send_buff[11]=0X00;
									send(1,array_Send_buff,12);//��ͻ��˷���	
									array_drive_TXbuffer[0]=0x01;//վ��
									array_drive_TXbuffer[1]=0X06;//������
									array_drive_TXbuffer[2]=0X00;//��ַ��λ
									array_drive_TXbuffer[3]=0xC8;//��ַ��λ
									array_drive_TXbuffer[4]=0X00;//���ݸ�λ
									array_drive_TXbuffer[5]=0X00;//���ݵ�λ
									osMessageQueuePut(RS485_TransmitQueueHandle,array_drive_TXbuffer,1,0);
									flag_Motion_state=0;									
								}
								break;
							
							case 0x03://��ת�㶯/ֹͣ
								if(array_Drive_control[10]==0xFF)//��ʼ
								{
									array_Send_buff[10]=0XFF;
									array_Send_buff[11]=0X00;
									send(1,array_Send_buff,12);//��ͻ��˷���	
									array_drive_TXbuffer[0]=0x01;//վ��
									array_drive_TXbuffer[1]=0X06;//������
									array_drive_TXbuffer[2]=0X00;//��ַ��λ
									array_drive_TXbuffer[3]=0xC8;//��ַ��λ
									array_drive_TXbuffer[4]=0X01;//���ݸ�λ
									array_drive_TXbuffer[5]=0X01;//���ݵ�λ
									osMessageQueuePut(RS485_TransmitQueueHandle,array_drive_TXbuffer,1,0);
									flag_Motion_state=1;									
								}
								else//ֹͣ
								{
									array_Send_buff[10]=0X00;
									array_Send_buff[11]=0X00;
									send(1,array_Send_buff,12);//��ͻ��˷���	
									array_drive_TXbuffer[0]=0x01;//վ��
									array_drive_TXbuffer[1]=0X06;//������
									array_drive_TXbuffer[2]=0X00;//��ַ��λ
									array_drive_TXbuffer[3]=0xC8;//��ַ��λ
									array_drive_TXbuffer[4]=0X00;//���ݸ�λ
									array_drive_TXbuffer[5]=0X00;//���ݵ�λ
									osMessageQueuePut(RS485_TransmitQueueHandle,array_drive_TXbuffer,1,0);
									flag_Motion_state=0;									
								}							
								break;							
							
							case 0x04://����˶�/ֹͣ
								if(array_Drive_control[10]==0xFF)//��ʼ
								{
									array_Send_buff[10]=0XFF;
									array_Send_buff[11]=0X00;
									send(1,array_Send_buff,12);//��ͻ��˷���	
									array_drive_TXbuffer[0]=0x01;//վ��
									array_drive_TXbuffer[1]=0X06;//������
									array_drive_TXbuffer[2]=0X00;//��ַ��λ
									array_drive_TXbuffer[3]=0xC8;//��ַ��λ
									array_drive_TXbuffer[4]=0X00;//���ݸ�λ
									array_drive_TXbuffer[5]=0X01;//���ݵ�λ
									osMessageQueuePut(RS485_TransmitQueueHandle,array_drive_TXbuffer,1,0);
									flag_Motion_state=1;
								}
								else//ֹͣ
								{
									array_Send_buff[10]=0X00;
									array_Send_buff[11]=0X00;
									send(1,array_Send_buff,12);//��ͻ��˷���	
									array_drive_TXbuffer[0]=0x01;//վ��
									array_drive_TXbuffer[1]=0X06;//������
									array_drive_TXbuffer[2]=0X00;//��ַ��λ
									array_drive_TXbuffer[3]=0xC8;//��ַ��λ
									array_drive_TXbuffer[4]=0X00;//���ݸ�λ
									array_drive_TXbuffer[5]=0X00;//���ݵ�λ
									osMessageQueuePut(RS485_TransmitQueueHandle,array_drive_TXbuffer,1,0);	
									flag_Motion_state=0;
								}								
								break;
							
							case 0x05://�����˶�/ֹͣ
								if(array_Drive_control[10]==0xFF)//��ʼ
								{
									array_Send_buff[10]=0XFF;
									array_Send_buff[11]=0X00;
									send(1,array_Send_buff,12);//��ͻ��˷���	
									if(flag_Absolute_position==1)//�ж��Ƿ���յ�
									{
										array_drive_TXbuffer[0]=0x01;//վ��
										array_drive_TXbuffer[1]=0X10;//������
										array_drive_TXbuffer[2]=0X00;//��ַ��λ
										array_drive_TXbuffer[3]=0xD0;//��ַ��λ----��ֹʱ
										array_drive_TXbuffer[4]=0x00;//�Ĵ���������λ
										array_drive_TXbuffer[5]=0x02;//�Ĵ���������λ
										array_drive_TXbuffer[6]=0x04;//���ֽ���
										array_drive_TXbuffer[7]=((variable_Target_position/One_Circle_Distance)>>8)&0xFF;//���ݸ�λ
										array_drive_TXbuffer[8]=(variable_Target_position/One_Circle_Distance)&0xFF;//���ݵ�λ
										array_drive_TXbuffer[9]=((variable_Target_position/One_Circle_Distance)>>24)&0xFF;//���ݸ�λ
										array_drive_TXbuffer[10]=((variable_Target_position/One_Circle_Distance)>>16)&0xFF;//���ݵ�λ
										osMessageQueuePut(RS485_TransmitQueueHandle,array_drive_TXbuffer,1,0);
										flag_Motion_state=1;									
									}
								}
								else//ֹͣ
								{
									array_Send_buff[10]=0X00;
									array_Send_buff[11]=0X00;
									send(1,array_Send_buff,12);//��ͻ��˷���	
									array_drive_TXbuffer[0]=0x01;//վ��
									array_drive_TXbuffer[1]=0X06;//������
									array_drive_TXbuffer[2]=0X00;//��ַ��λ
									array_drive_TXbuffer[3]=0xC8;//��ַ��λ
									array_drive_TXbuffer[4]=0X00;//���ݸ�λ
									array_drive_TXbuffer[5]=0X00;//���ݵ�λ
									osMessageQueuePut(RS485_TransmitQueueHandle,array_drive_TXbuffer,1,0);
									flag_Motion_state=0;									
								}							
								break;
							
							case 0x14://ֹͣ�˶�״̬
								if(array_Drive_control[10]==0xFF)//ֹͣ
								{
									array_Send_buff[10]=0XFF;
									array_Send_buff[11]=0X00;
									if(flag_Motion_state==1)
									{
										array_drive_TXbuffer[0]=0x01;//վ��
										array_drive_TXbuffer[1]=0X06;//������
										array_drive_TXbuffer[2]=0X00;//��ַ��λ
										array_drive_TXbuffer[3]=0xC8;//��ַ��λ
										array_drive_TXbuffer[4]=0X00;//���ݸ�λ
										array_drive_TXbuffer[5]=0X00;//���ݵ�λ
										osMessageQueuePut(RS485_TransmitQueueHandle,array_drive_TXbuffer,1,0);	
										flag_Motion_state=0;
									}
								}
								else
								{
									array_Send_buff[10]=0X00;
									array_Send_buff[11]=0X00;
								}
								send(1,array_Send_buff,12);//��ͻ��˷���								
								break;
							
							case 0x00://Ӳ��������/ֹͣ
								if(array_Drive_control[10]==0xFF)//��ʼ
								{
									flag_Hard_trigger_start=1;
									array_Send_buff[10]=0XFF;
									array_Send_buff[11]=0X00;							
								}
								else//ֹͣ
								{
									flag_Hard_trigger_start=0;
									array_Send_buff[10]=0X00;
									array_Send_buff[11]=0X00;
								}
								send(1,array_Send_buff,12);//��ͻ��˷���								
								break;
							
							case 0x06://�ŷ���λ
								if(array_Drive_control[10]==0xFF)
								{
									array_Send_buff[10]=0XFF;
									array_Send_buff[11]=0X00;
									send(1,array_Send_buff,12);//��ͻ��˷���	
								}
								else if(array_Drive_control[10]==0x00)
								{
									array_Send_buff[10]=0X00;
									array_Send_buff[11]=0X00;
									send(1,array_Send_buff,12);//��ͻ��˷���									
								}
								break;
							
							case 0x07://�ŷ���ͣ
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
								send(1,array_Send_buff,12);//��ͻ��˷���	
								break;

							case 0x09://�״��ϵ�/�ϵ�
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
								send(1,array_Send_buff,12);//��ͻ��˷���							
								break;
							
							case 0x0F://PLC�������							
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
								send(1,array_Send_buff,12);//��ͻ��˷���	
								break;
							
							default:
								break;													
						}
					}
					memset(array_Send_buff,0,13);	
					break;
				
				case 0x06://д�������ּĴ���(����MBAP ������ �Ĵ�����ַH �Ĵ�����ַL �Ĵ���ֵH �Ĵ���ֵL)
					array_Send_buff[4]=0X00;//���ݳ���
					array_Send_buff[5]=0X06;//���ݳ���
					array_Send_buff[8]=array_Drive_control[8];//�Ĵ�����ַ
					array_Send_buff[9]=array_Drive_control[9];//�Ĵ�����ַ
					array_Send_buff[10]=array_Drive_control[10];//�Ĵ���ֵH
					array_Send_buff[11]=array_Drive_control[11];//�Ĵ���ֵL
					if(array_Drive_control[8]==0x01)//�ж���ʼ��ַ
					{
						if(array_Drive_control[9]==0x4A)//�ж���ʼ��ַ��������ʱ��
						{						
							send(1,array_Send_buff,12);//��ͻ��˷���	
						}
						else if(array_Drive_control[9]==0x4E)//�ж���ʼ��ַ��������ʱ��
						{
							send(1,array_Send_buff,12);//��ͻ��˷���												
						}
						else if(array_Drive_control[9]==0x00)//�ж���ʼ��ַ����ʵʱλ���ϴ�����
						{
							if(array_Drive_control[10]||array_Drive_control[11])
							{
								flag_Position_upload=1;//��ʼʵʱλ���ϴ�		
							}
							else
							{
								flag_Position_upload=0;//ֹͣʵʱλ���ϴ�
							}
							send(1,array_Send_buff,12);//��ͻ��˷���											
						}
						else if(array_Drive_control[9]==0x30)//�ж���ʼ��ַ����Ӳ��������
						{
							number_Hard_trigger=(array_Drive_control[10]<<8|array_Drive_control[11]);//��˴������ݴ�����Ӳ��������
							send(1,array_Send_buff,12);//��ͻ��˷���											
						}					
					}	
					memset(array_Send_buff,0,13);					
					break;
					
				case 0x10://д������ּĴ���(����MBAP(7λ) ������ ��ʼ��ַH ��ʼ��ַL �Ĵ�������H �Ĵ�������L �ֽڳ��� �Ĵ���ֵ)
					array_Send_buff[4]=0X00;//���ݳ���
					array_Send_buff[5]=0X06;//���ݳ���
					array_Send_buff[8]=array_Drive_control[8];//�Ĵ�����ʼ��ַH
					array_Send_buff[9]=array_Drive_control[9];//�Ĵ�����ʼ��ַL
					array_Send_buff[10]=array_Drive_control[10];//�Ĵ�������H
					array_Send_buff[11]=array_Drive_control[11];//�Ĵ�������L
					if(array_Drive_control[8]==0x00)//�ж���ʼ��ַ
					{
						if(array_Drive_control[9]==0x64)//�ж���ʼ��ַ���������ٶ�
						{
							variable_Zero_return_speed_value=(array_Drive_control[13]<<8|array_Drive_control[14]|array_Drive_control[15]<<24|array_Drive_control[16]<<16);//��˴������ݴ����������ٶ�0.001mm/s
							send(1,array_Send_buff,12);//��ͻ��˷���													
						}
						else if(array_Drive_control[9]==0x68)//�ж���ʼ��ַ������������ٶ�
						{
							variable_Zero_return_creep_speed_value=(array_Drive_control[13]<<8|array_Drive_control[14]|array_Drive_control[15]<<24|array_Drive_control[16]<<16);//��˴������ݴ�������������ٶ�0.001mm/s
							send(1,array_Send_buff,12);//��ͻ��˷���							
						}
						else if(array_Drive_control[9]==0x78)//�ж���ʼ��ַ���������ٶ�
						{
//								variable_Running_speed_value=(array_Drive_control[13]<<24|array_Drive_control[14]<<16|array_Drive_control[15]<<8|array_Drive_control[16]);//��˴������ݴ����������ٶ�0.001mm/s
							variable_Running_speed_value=(array_Drive_control[13]<<8|array_Drive_control[14]|array_Drive_control[15]<<24|array_Drive_control[16]<<16);//��˴������ݴ����������ٶ�0.001mm/s
							send(1,array_Send_buff,12);//��ͻ��˷���	
							array_drive_TXbuffer[0]=0x01;//վ��
							array_drive_TXbuffer[1]=0X06;//������
							array_drive_TXbuffer[2]=0X00;//��ַ��λ
							array_drive_TXbuffer[3]=0x9A;//��ַ��λ
							array_drive_TXbuffer[4]=((variable_Running_speed_value/1000*12)>>8)&0xFF;//���ݸ�λ
							array_drive_TXbuffer[5]=(variable_Running_speed_value/1000*12)&0xFF;//���ݵ�λ
							osMessageQueuePut(RS485_TransmitQueueHandle,array_drive_TXbuffer,1,0);	
						}							
						else if(array_Drive_control[9]==0x7A)//�ж���ʼ��ַ������Զ�λ����
						{
							send(1,array_Send_buff,12);//��ͻ��˷���													
						}
						else if(array_Drive_control[9]==0x7C)//�ж���ʼ��ַ�������Զ�λλ��
						{
							flag_Absolute_position=1;
							variable_Target_position=(array_Drive_control[13]<<8|array_Drive_control[14]|array_Drive_control[15]<<24|array_Drive_control[16]<<16);//��˴������ݴ��������Զ�λλ��0.001mm
							send(1,array_Send_buff,12);//��ͻ��˷���														
						}		
						else if(array_Drive_control[9]==0x7E)//�ж���ʼ��ַ�����豸��������г�
						{
							send(1,array_Send_buff,12);//��ͻ��˷���														
						}	
						else if(array_Drive_control[9]==0x80)//�ж���ʼ��ַ�������λ������
						{
							variable_Zero_position=(array_Drive_control[13]<<8|array_Drive_control[14]|array_Drive_control[15]<<24|array_Drive_control[16]<<16);//��˴������ݴ��������λ��0.001mm
							send(1,array_Send_buff,12);//��ͻ��˷���														
						}	
					}
					else if(array_Drive_control[8]==0x01)//�ж���ʼ��ַ
					{
						if(array_Drive_control[9]==0x2C)//�ж���ʼ��ַ������ʼӲ����λ��
						{
							variable_Hard_trigger_position=(array_Drive_control[13]<<8|array_Drive_control[14]|array_Drive_control[15]<<24|array_Drive_control[16]<<16);//��˴������ݴ�������ʼӲ����λ��0.001mm
							variable_Position_lastpulsec=variable_Hard_trigger_position;
							send(1,array_Send_buff,12);//��ͻ��˷���														
						}
						else if(array_Drive_control[9]==0x2E)//�ж���ʼ��ַ����Ӳ�������
						{
							variable_Hard_trigger_interval=(array_Drive_control[13]<<8|array_Drive_control[14]|array_Drive_control[15]<<24|array_Drive_control[16]<<16);//��˴������ݴ�����Ӳ�������0.001mm
							send(1,array_Send_buff,12);//��ͻ��˷���													
						}				
					}
					memset(array_Send_buff,0,13);												
					break;
					
				default:
					break;						
			}				
			memset(array_Drive_control, 0, 20);		
		}
  }
}


void Encoder(void *argument)//�綯��������ͨѶ
{
	uint8_t array_Real_time_location[6]={0xCF,0xAE,0x00,0x00,0x00,0x00};//ʵʱλ������
	uint8_t array_Real_time_location_query[13]={0x19,0xB2,0x00,0x00,0x00,0x07,0xDB,0x03,0x04,0x00,0x00,0x00,0x00};//ʵʱλ��������ѯ����
  for(;;)
  {
		if(flag_Encoder_processing==1)
		{
			flag_Encoder_processing=0;
			if(CEASE&&number_Motor_pulsec==0)//ԭ��У׼--ֹͣ��������Ϊ0
			{	
				flag_Zero_regression=1;	
				number_Hard_trigger_curren=0;
				variable_Position=0;
				variable_Position_upload=0;
				flag_Motion_state=0;
			}	
			else
			{
				variable_Position = ((float)number_Motor_pulsec/Subtraction)*One_Circle_Distance;	//��ԭ�����
				variable_Position_upload = variable_Position*100;//λ���ϴ����ݣ���λ0.01mm��
				if(CEASE&&(variable_Target_position*0.99<=variable_Position*1000<=variable_Target_position*1.01))
				{
					flag_Motion_state=0;
				}
			}
			if(flag_Real_time_location_query==1)
			{
				flag_Real_time_location_query=0;
				array_Real_time_location_query[11]=(variable_Position_upload>>24)&0xFF;//�Ĵ���1H-����
				array_Real_time_location_query[12]=(variable_Position_upload>>16)&0xFF;//�Ĵ���1L-����
				array_Real_time_location_query[9]=(variable_Position_upload>>8)&0xFF;//�Ĵ���2H-����
				array_Real_time_location_query[10]=variable_Position_upload&0xFF;//�Ĵ���2L-����
				send(1,array_Real_time_location_query,13);//��ͻ��˷���	
			}
		}
		if(flag_Position_upload==1)//ʵʱλ���ϴ�
		{				
			array_Real_time_location[2]=variable_Position_upload&0xFF;
			array_Real_time_location[3]=(variable_Position_upload>>8)&0xFF;
			array_Real_time_location[4]=(variable_Position_upload>>16)&0xFF;
			array_Real_time_location[5]=(variable_Position_upload>>24)&0xFF;
			send(2,array_Real_time_location,6);//��ͻ��˷���	
		}	
		if(flag_Hard_trigger==1)
		{
			variable_Trigger_interval=variable_Position*1000-variable_Position_lastpulsec;
			if(variable_Trigger_interval>=variable_Hard_trigger_interval*10)//.����������������������������������������������������������������������������������������������
			{
				variable_Position_lastpulsec=variable_Position_lastpulsec+variable_Hard_trigger_interval*10;//.����������������������������������������������������������������������������������������������
				flag_Hard_trigger=0;
				variable_Trigger_interval=0;					
			}			
		}		
  }
}






void Pulse_Emission(void *argument)//���崥����TIM13ͨ����
{	
  for(;;)
  {
//		osDelay(1);
		if(flag_Hard_trigger_start==1&&variable_Position_upload>=variable_Hard_trigger_position&&MOTION)//Ӳ��������(Ӳ��������������Ӳ����λ�ã��˶�״̬)
		{
			if(flag_Hard_trigger==0)
			{
				if(number_Hard_trigger_curren<number_Hard_trigger)//Ӳ���������ж�
				{		
					number_Hard_trigger_curren++;
					HAL_GPIO_WritePin(PULSE_GPIO_Port, PULSE_Pin, GPIO_PIN_SET);//PA2-Ӳ����
					HAL_TIM_Base_Start_IT(&htim13);//����TIM13��ʱ���жϣ�1MHZ������50�Σ�50us��
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


void Uart3_Communicate(void *argument)//����ͨѶ
{
	const uint8_t str_date[12] = __DATE__; //ʾ�� "Mar 23 2022"  
	const uint8_t str_time[10] = __TIME__;
	uint8_t uart_receive_buff[3];
	uint8_t	VERSION[6];
	uint8_t y1,y2,d1,d2,t1,t2;
  for(;;)
  {
		osDelay(1);
		HAL_UART_Receive_IT(&huart3, uart_receive_buff, 3);
		VERSION[0] = 0XAA;
		VERSION[1] = 0X0C;//�����
		y1 = str_date[9] - '0';
		y2 = str_date[10] - '0';
		VERSION[2] = y1 << 4 | y2;
		switch(str_date[2])
    {
        case 'n': //Jan��jun
           VERSION[3] = str_date[1] == 'a' ? 1 : 6; //�ж�1�»���6��
            break;

        case 'b': //Feb
            VERSION[3] = 2;
            break;

        case 'r': //Mart��Apr
            VERSION[3] = str_date[1] == 'a' ? 3 : 4; //�ж�3�»���4��
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
		d1 = str_date[4] - '0'; //���� ʮλ
    d2 = str_date[5] - '0'; //���� ��λ
		VERSION[4] = d1 << 4 | d2;
		t1 = str_time[0] - '0'; //Сʱ ʮλ
    t2 = str_time[1] - '0'; //Сʱ ��λ
		VERSION[5] = t1 << 4 | t2;

		if(uart_receive_buff[0]==0XAA && uart_receive_buff[1]==0X01 && uart_receive_buff[2]==0XBB)
		{
			HAL_UART_Transmit(&huart3, VERSION, 6, 10);
		}
		memset(uart_receive_buff, 0, 3);	
  }
}


void Iwdg_function(void *argument)//���Ź�����
{
  for(;;)
  {
		osDelay(1);	
		if(number_Iwdg<40)//������40ʱ����˵��20minû����λ�������źţ���Ƭ������
		{	
			number_Iwdg++;
			HAL_IWDG_Refresh(&hiwdg);//���ι��ʱ��32s
			osDelay(30000);//30s
		}		
	}
}


void RS485_Receive_function(void *argument)//RS485����
{
	uint16_t m_crc;                 //�������crc
	uint8_t status=0;	
	uint8_t	array_RS485_RXbuffer[20];
	uint16_t number_Data_bytes=0;//�����ֽ�
  for(;;)
  {
		osDelay(1);
		status=osMessageQueueGet(RS485_ReceiveQueueHandle,array_RS485_RXbuffer,NULL,100);
		if(status==osOK)
		{
			if(array_RS485_RXbuffer[1]==0x03||array_RS485_RXbuffer[1]==0x04)//�жϹ�����
			{
				number_Data_bytes=array_RS485_RXbuffer[2];
				m_crc = CRC16(array_RS485_RXbuffer, 3+number_Data_bytes);
				if(m_crc != (array_RS485_RXbuffer[3+number_Data_bytes]<<8|array_RS485_RXbuffer[4+number_Data_bytes]))//CRC����
				{
					memset(array_RS485_RXbuffer, 0, 20);
				}
				else                    
				{
					if(variable_Register_address==0x0004)//���ʵʱλ��
					{
						number_Motor_pulsec=(array_RS485_RXbuffer[3]<<8|array_RS485_RXbuffer[4]|array_RS485_RXbuffer[5]<<24|array_RS485_RXbuffer[6]<<16);//��˴������ݴ��������ʵʱλ��������
						variable_Register_address=0x0000;
						flag_Encoder_processing=1;
						memset(array_RS485_RXbuffer, 0, 20);
					}
					else
					{
						variable_Register_address=0x0000;
						memset(array_RS485_RXbuffer, 0, 20);
					}	
				}
			}
			else if(array_RS485_RXbuffer[1]==0x06||array_RS485_RXbuffer[1]==0x10)
			{
				m_crc = CRC16(array_RS485_RXbuffer, 6);
				if(m_crc != (array_RS485_RXbuffer[6]<<8|array_RS485_RXbuffer[7]))//CRC����
				{
					memset(array_RS485_RXbuffer, 0, 20);
				} 
				else                    
				{
					variable_Register_address=0x0000;
					memset(array_RS485_RXbuffer, 0, 20);
				}				
			}
		}
	}
}




void RS485_Transmit_function(void *argument)//RS485����
{
	uint16_t m_crc;                 //�������crc
	uint8_t status=0;	
	uint8_t	array_RS485_TXbuffer[20];
	uint8_t number_Data_bytes=0;//�����ֽ�
  for(;;)
  {
		osDelay(1);
		status=osMessageQueueGet(RS485_TransmitQueueHandle,array_RS485_TXbuffer,NULL,100);
		if(status==osOK)
		{
			if(array_RS485_TXbuffer[1]==0x03||array_RS485_TXbuffer[1]==0x04||array_RS485_TXbuffer[1]==0x06)//�жϹ�����
			{
				variable_Register_address=(array_RS485_TXbuffer[2]<<8|array_RS485_TXbuffer[3]);
				m_crc = CRC16(array_RS485_TXbuffer, 6);
				array_RS485_TXbuffer[6]=(m_crc>>8)&0xFF;
				array_RS485_TXbuffer[7]=m_crc&0xFF;
				DO_RS485_TX;
				if(variable_Register_address==0x0004)
				{
					HAL_UART_Transmit(&huart1,array_RS485_TXbuffer,8,1);
					DO_RS485_RX;
				}
				else
				{
					flag_Resend_timeout=1;
					HAL_UART_Transmit(&huart1,array_RS485_TXbuffer,8,1);
					osDelay(50);
					HAL_UART_Transmit(&huart1,array_RS485_TXbuffer,8,1);
				}
			}
			else if(array_RS485_TXbuffer[1]==0x10)
			{
				variable_Register_address=(array_RS485_TXbuffer[2]<<8|array_RS485_TXbuffer[3]);
				number_Data_bytes=array_RS485_TXbuffer[6];
				m_crc = CRC16(array_RS485_TXbuffer, 7+number_Data_bytes);
				array_RS485_TXbuffer[7+number_Data_bytes]=(m_crc>>8)&0xFF;
				array_RS485_TXbuffer[8+number_Data_bytes]=m_crc&0xFF;
				DO_RS485_TX;
				flag_Resend_timeout=1;
				HAL_UART_Transmit(&huart1,array_RS485_TXbuffer,9+number_Data_bytes,1);
				osDelay(50);
				HAL_UART_Transmit(&huart1,array_RS485_TXbuffer,9+number_Data_bytes,1);
				osDelay(50);
				HAL_UART_Transmit(&huart1,array_RS485_TXbuffer,9+number_Data_bytes,1);
			}
			memset(array_RS485_TXbuffer, 0, 20);			
//			DO_RS485_RX;
//			HAL_UARTEx_ReceiveToIdle_DMA(&huart1,array_RS485_buffer,BUF_SIZE);//��������1�����жϺ�DMA,
		}
	}
}

void Resend_timeout(void *argument)//RS485���ͽ�����Ӧ��ʱ
{
	uint8_t count_Resend_timeout=0;	
  for(;;)
  {
		osDelay(1);
		if(flag_Resend_timeout==1)
		{
			if(count_Resend_timeout<10)
			{
				osDelay(10);	
				count_Resend_timeout++;
			}
			else if(count_Resend_timeout>=1)
			{
				count_Resend_timeout=0;
				flag_Resend_timeout=0;
			}
		}
		else
		{
			count_Resend_timeout=0;	
		}		
	}
}


void Position_query(void *argument)//λ�ò�ѯ
{
	uint8_t count_Position_query=0;	
	uint8_t	array_Position_Query_buffer[8]={0X01,0X03,0X00,0X04,0X00,0X02,0X85,0XCA};
  for(;;)
  {
		osDelay(1);
		if(flag_Resend_timeout==0)
		{
			if(count_Position_query<1)
			{
				osDelay(100);
				count_Position_query++;
			}
			else
			{
				osMessageQueuePut(RS485_TransmitQueueHandle,array_Position_Query_buffer,1,0);
				count_Position_query=0;
			}
		}
	}

}

/* USER CODE END Application */

