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
uint32_t variable_Position_upload=0;//��0������ϴ�ֵ����λ0.001mm��
uint32_t variable_Zero_return_speed_value=10000;//�����ٶȣ���λ0.001mm/s��
uint32_t variable_Running_speed_value=5000;//�����ٶȣ���λ0.001mm/s��
uint32_t variable_Target_position=0;//Ŀ��λ��
uint32_t variable_Zero_position=0;//���λ��	
uint32_t variable_Hard_trigger_interval=0;//Ӳ����ʱ����
uint32_t variable_Hard_trigger_position=0;//Ӳ������ʼλ��
uint32_t variable_Trigger_interval;//�������
float    variable_Position_lastpulsec;//�ϴ�Ӳ����λ��
	
/***********************************************flag***************************************/
uint8_t flag_Extreme_position=0;//����λ�ñ�־λ
uint8_t flag_Position_upload=1;//λ���ϴ���־λ
uint8_t flag_Encoder_processing=0;//���������ݴ����־λ
uint8_t flag_Zero_regression=0;//�Ƿ�������־λ
uint8_t flag_Absolute_position=0;//����λ�ñ�־λ
uint8_t flag_Hard_trigger_start=0;//Ӳ������ʼ��־λ
uint8_t flag_Hard_trigger=0;//Ӳ������־λ	
uint8_t flag_Motor_deceleration_stops=0;//�������ֹͣ��־λ

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


void Ethernet_Server(void *argument);//����ͨ��-�����
void Encoder(void *argument);//�綯��������ͨѶ
void Drive(void *argument);//����������
void Motor_drive(void *argument);//�����������
void Pulse_Emission(void *argument);//���巢��
void Extreme_position_Drive(void *argument);//����λ�ÿ���
void Uart3_Communicate(void *argument);//����λ�ÿ���
void Iwdg_function(void *argument);//���Ź�����
void Motor_deceleration_stops(void *argument);//�������ֹͣ
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
	if(REACH)//ԭ���ж�
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
	uint8_t  array_Motor_drive_send[3];//�����������
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
								if(MOTION)//�˶�
								{
									array_Send_buff[9]=0X01;//����						
								}
								else if(CEASE)//ֹͣ
								{
									array_Send_buff[9]=0X00;//����
								}
								send(1,array_Send_buff,10);//��ͻ��˷���	
								break;

							case 0x01://ԭ��ع�/ֹͣ
								array_Send_buff[4]=0X00;//���ݳ���
								array_Send_buff[5]=0X05;//���ݳ���
								array_Send_buff[8]=0X01;//���������ݵ��ֽڳ���
								if(CEASE&&REACH)//����
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
								if(MOTION)//�˶�
								{
									array_Send_buff[9]=0X01;//����								
								}
								else if(CEASE)//ֹͣ
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
								array_Send_buff[8]=number_Data_bytes;//���������ݵ��ֽڳ���
								array_Send_buff[11]=(variable_Position_upload>>24)&0xFF;//�Ĵ���1H-����
								array_Send_buff[12]=(variable_Position_upload>>16)&0xFF;//�Ĵ���1L-����
								array_Send_buff[9]=(variable_Position_upload>>8)&0xFF;//�Ĵ���2H-����
								array_Send_buff[10]=variable_Position_upload&0xFF;//�Ĵ���2L-����
								send(1,array_Send_buff,13);//��ͻ��˷���															
							}	
						}							
					}
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
									if(AWAY&&CEASE)//����ԭ����ֹͣ�˶�
									{
										array_Send_buff[10]=0XFF;
										array_Send_buff[11]=0X00;
										send(1,array_Send_buff,12);//��ͻ��˷���	
										flag_Extreme_position=0;
										flag_Zero_regression=0;
										variable_Target_position=0;//���Զ�λλ������Ϊ0
										if(HAL_GPIO_ReadPin(IN0_GPIO_Port,IN0_Pin)==GPIO_PIN_RESET)//��ͷ��-�����ƶ�
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
								else if(array_Drive_control[10]==0x00)//ֹͣ
								{
									array_Send_buff[10]=0X00;
									array_Send_buff[11]=0X00;
									send(1,array_Send_buff,12);//��ͻ��˷���		
									DO_CEASE;//PA7-���ʹ��
									HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//�ر�PWM���														
								}
								break;
							
							case 0x02://��ת�㶯/ֹͣ
								if(array_Drive_control[10]==0xFF)//��ʼ
								{
									array_Send_buff[10]=0XFF;
									array_Send_buff[11]=0X00;
									send(1,array_Send_buff,12);//��ͻ��˷���		
									array_Motor_drive_send[0]=GPIO_PIN_RESET;
									array_Motor_drive_send[1]=(variable_Running_speed_value/1000)>>8&0xff;
									array_Motor_drive_send[2]=variable_Running_speed_value/1000&0xff;
									osMessageQueuePut(Motor_driveQueueHandle,array_Motor_drive_send,1,0);
								}
								else//ֹͣ
								{
									array_Send_buff[10]=0X00;
									array_Send_buff[11]=0X00;
									send(1,array_Send_buff,12);//��ͻ��˷���		
//									DO_CEASE;//PA7-���ʹ��
//									HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//�ر�PWM���
									flag_Motor_deceleration_stops=3;									
								}
								break;
							
							case 0x03://��ת�㶯/ֹͣ
								if(array_Drive_control[10]==0xFF)//��ʼ
								{
									array_Send_buff[10]=0XFF;
									array_Send_buff[11]=0X00;
									send(1,array_Send_buff,12);//��ͻ��˷���	
									array_Motor_drive_send[0]=GPIO_PIN_SET;
									array_Motor_drive_send[1]=(variable_Running_speed_value/1000)>>8&0xff;
									array_Motor_drive_send[2]=variable_Running_speed_value/1000&0xff;
									osMessageQueuePut(Motor_driveQueueHandle,array_Motor_drive_send,1,0);								
								}
								else//ֹͣ
								{
									array_Send_buff[10]=0X00;
									array_Send_buff[11]=0X00;
									send(1,array_Send_buff,12);//��ͻ��˷���		
//									DO_CEASE;//PA7-���ʹ��
//									HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//�ر�PWM���	
									flag_Motor_deceleration_stops=3;									
								}							
								break;							
							
							case 0x04://����˶�/ֹͣ
								if(array_Drive_control[10]==0xFF)//��ʼ
								{
									array_Send_buff[10]=0XFF;
									array_Send_buff[11]=0X00;
									send(1,array_Send_buff,12);//��ͻ��˷���		
									if(CEASE)
									{
										array_Motor_drive_send[0]=GPIO_PIN_RESET;
										array_Motor_drive_send[1]=(variable_Running_speed_value/1000)>>8&0xff;
										array_Motor_drive_send[2]=variable_Running_speed_value/1000&0xff;
										osMessageQueuePut(Motor_driveQueueHandle,array_Motor_drive_send,1,0);
									}
								}
								else//ֹͣ
								{
									array_Send_buff[10]=0X00;
									array_Send_buff[11]=0X00;
									send(1,array_Send_buff,12);//��ͻ��˷���	
//									DO_CEASE;//PA7-���ʹ��
//									HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//�ر�PWM���
									flag_Motor_deceleration_stops=3;
								}								
								break;
							
							case 0x05://�����˶�/ֹͣ
								if(array_Drive_control[10]==0xFF)//��ʼ
								{
									array_Send_buff[10]=0XFF;
									array_Send_buff[11]=0X00;
									send(1,array_Send_buff,12);//��ͻ��˷���	
									if(CEASE&&flag_Extreme_position==0)
									{
										if(variable_Position_upload*10<variable_Target_position||variable_Position<0)//��ǰλ��С��Ŀ��λ�û�ǰλ��С��0-�����ƶ�
										{
											array_Motor_drive_send[0]=GPIO_PIN_RESET;
											array_Motor_drive_send[1]=(variable_Running_speed_value/1000)>>8&0xff;
											array_Motor_drive_send[2]=variable_Running_speed_value/1000&0xff;
											osMessageQueuePut(Motor_driveQueueHandle,array_Motor_drive_send,1,0);
										}
										else if(variable_Position_upload*10>variable_Target_position&&AWAY)//��ǰλ�ô���Ŀ��λ���Ҳ���ԭ��-�����ƶ�
										{
											array_Motor_drive_send[0]=GPIO_PIN_SET;
											array_Motor_drive_send[1]=(variable_Running_speed_value/1000)>>8&0xff;
											array_Motor_drive_send[2]=variable_Running_speed_value/1000&0xff;
											osMessageQueuePut(Motor_driveQueueHandle,array_Motor_drive_send,1,0);
										}
									}
								}
								else//ֹͣ
								{
									array_Send_buff[10]=0X00;
									array_Send_buff[11]=0X00;
									send(1,array_Send_buff,12);//��ͻ��˷���		
//									DO_CEASE;//PA7-���ʹ��
//									HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//�ر�PWM���
									flag_Motor_deceleration_stops=3;
								}							
								break;
							
							case 0x14://ֹͣ�˶�״̬
								if(array_Drive_control[10]==0xFF)//ֹͣ
								{
//									DO_CEASE;//PA7-���ʹ��
//									HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//�ر�PWM���
									flag_Motor_deceleration_stops=3;
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
					break;
				
				case 0x06://д�������ּĴ���(����MBAP ������ �Ĵ�����ַH �Ĵ�����ַL �Ĵ���ֵH �Ĵ���ֵL)
//					if(CEASE)//δ��ʼ�˶�
//					{
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
//					}										
					break;
					
				case 0x10://д������ּĴ���(����MBAP(7λ) ������ ��ʼ��ַH ��ʼ��ַL �Ĵ�������H �Ĵ�������L �ֽڳ��� �Ĵ���ֵ)
//				if(CEASE)//δ��ʼ�˶�
//				{
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
							}							
							else if(array_Drive_control[9]==0x7A)//�ж���ʼ��ַ������Զ�λ����
							{
								send(1,array_Send_buff,12);//��ͻ��˷���															
							}
							else if(array_Drive_control[9]==0x7C)//�ж���ʼ��ַ�������Զ�λλ��
							{
								flag_Absolute_position=1;
								variable_Target_position=(array_Drive_control[13]<<8|array_Drive_control[14]|array_Drive_control[15]<<24|array_Drive_control[16]<<16);//��˴������ݴ��������Զ�λλ��0.001mm
								if(variable_Target_position==0x00)
								{
									flag_Extreme_position=0;
									flag_Zero_regression=0;
								}
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


void Motor_drive(void *argument)//�����������
{
	uint8_t  status=0;	
	uint16_t number_v=0;
	uint16_t number_Reloading;
	uint16_t number_speed;
	uint8_t  array_Motor_drive_receive[3];//�����������
	for(;;)
	{    
		__HAL_TIM_CLEAR_IT(&htim8, TIM_IT_UPDATE);
		HAL_TIM_Base_Start_IT(&htim8);//����TIM8��ʱ���жϣ�20KHZ������50us��
		status=osMessageQueueGet(Motor_driveQueueHandle,array_Motor_drive_receive,NULL,0);
		if(status==osOK)
		{	
			number_speed=array_Motor_drive_receive[1]<<8|array_Motor_drive_receive[2];
			DO_MOTION;//PA7-���ʹ��
			osDelay(1);	
			HAL_GPIO_WritePin(TIM8_CH2_GPIO_Port,TIM8_CH2_Pin,(GPIO_PinState)array_Motor_drive_receive[0]);//PC7-����������(GPIO_PIN_RESET:˳ʱ�룬GPIO_PIN_SET:��ʱ�룩
			osDelay(1);	
			while(number_speed>number_v&&MOTION)
			{
				number_v++;
				number_Reloading=(One_Circle_Distance*1000)/number_v;//�Զ���װ��ֵn=(One_Circle_Distance*1000000*1000)/(variable_Running_speed_value*Subtraction)
				__HAL_TIM_SET_AUTORELOAD(&htim8,number_Reloading-1);//�޸��Զ���װ��ֵ
				__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, (number_Reloading/2)-1);//�޸�ռ�ձ�
				HAL_TIM_PWM_Start_IT(&htim8,TIM_CHANNEL_1);//����TIM8_CH1��PWM��������жϣ�
				osDelay(1);
			}
			number_v=0;	
		}
		memset(array_Motor_drive_receive,0,3);			
	}
}


void Encoder(void *argument)//�綯��������ͨѶ
{
	uint8_t Real_time_location_array[6]={0xCF,0xAE,0x00,0x00,0x00,0x00};//ʵʱλ������
  for(;;)
  {
		if(flag_Encoder_processing==1)
		{
			flag_Encoder_processing=0;
			if(REACH)//ԭ��У׼
			{	
				variable_Position=0;
				variable_Position_upload=0;
			}	
			else if(AWAY)
			{
				variable_Position = ((float)number_Motor_pulsec/Subtraction)*One_Circle_Distance;	//��ԭ�����
				variable_Position_upload = variable_Position*100;//λ���ϴ����ݣ���λ0.01mm��
				if(flag_Zero_regression==1&&flag_Absolute_position==1)//֮ǰ�ع����ҽ��յ�����λ��
				{
					if(COROTATION)//�����ƶ�
					{
						if(flag_Motor_deceleration_stops==0&&variable_Position_upload*10<variable_Target_position&&variable_Position_upload*10>=variable_Target_position-25000)
						{
							flag_Motor_deceleration_stops=1;
						}
						else if(flag_Motor_deceleration_stops==2&&variable_Position_upload*10>=variable_Target_position)
						{
							DO_CEASE;//PA7-���ʹ��
							HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//�ر�PWM���
							flag_Absolute_position=0;	
							flag_Motor_deceleration_stops=0;
						}	
					}
					else if(REVERSAL)//�����ƶ�
					{
						if(flag_Motor_deceleration_stops==0&&variable_Position_upload*10>variable_Target_position&&variable_Position_upload*10<=variable_Target_position+25000)
						{
							flag_Motor_deceleration_stops=1;
						}
						else if(flag_Motor_deceleration_stops==2&&variable_Position_upload*10<=variable_Target_position)
						{
							DO_CEASE;//PA7-���ʹ��
							HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//�ر�PWM���
							flag_Absolute_position=0;	
							flag_Motor_deceleration_stops=0;
						}	
					}					
				}	
			}
			if(flag_Position_upload==1)//ʵʱλ���ϴ�
			{				
				Real_time_location_array[2]=variable_Position_upload&0xFF;
				Real_time_location_array[3]=(variable_Position_upload>>8)&0xFF;
				Real_time_location_array[4]=(variable_Position_upload>>16)&0xFF;
				Real_time_location_array[5]=(variable_Position_upload>>24)&0xFF;
				send(2,Real_time_location_array,6);//��ͻ��˷���	
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
					__HAL_TIM_CLEAR_IT(&htim13, TIM_IT_UPDATE);//**********************************��������ж�����λ,��ֹ�����жϾͽ����ж�*******************************
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


void Extreme_position_Drive(void *argument)//����λ�ÿ���
{
  for(;;)
  {
		osDelay(1);
		if(flag_Extreme_position==1)
		{
			DO_CEASE;//PA7-���ʹ��
			HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//�ر�PWM���
		}
  }
}


void Uart3_Communicate(void *argument)//����ͨѶ
{
	const uint8_t str_date[12] = __DATE__; //ʾ�� "Mar 23 2022"  
	const uint8_t str_time[10] = __TIME__;
	uint8_t uart_receive_buff[3];
	uint8_t	VERSION[7];
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
		VERSION[6] =0xBB;

		if(uart_receive_buff[0]==0XAA && uart_receive_buff[1]==0X01 && uart_receive_buff[2]==0XBB)
		{
			HAL_UART_Transmit(&huart3, VERSION, 7, 10);
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


void Motor_deceleration_stops(void *argument)//�������ֹͣ
{
  for(;;)
  {
		uint16_t number_Reloading;
		uint16_t number_v=0;
		osDelay(1);	
		if(flag_Motor_deceleration_stops==1)//�����ʼ����ֹͣ
		{	
			flag_Motor_deceleration_stops=2;
			number_v=variable_Running_speed_value/1000;
			while(number_v>10&&MOTION)
			{
				number_v--;
				number_Reloading=5000/number_v;
				__HAL_TIM_SET_AUTORELOAD(&htim8,number_Reloading-1);//�޸��Զ���װ��ֵ
				__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, (number_Reloading/2)-1);//�޸�ռ�ձ�
				HAL_TIM_PWM_Start_IT(&htim8,TIM_CHANNEL_1);//����TIM8_CH1��PWM��������жϣ�
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
				__HAL_TIM_SET_AUTORELOAD(&htim8,number_Reloading-1);//�޸��Զ���װ��ֵ
				__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, (number_Reloading/2)-1);//�޸�ռ�ձ�
				HAL_TIM_PWM_Start_IT(&htim8,TIM_CHANNEL_1);//����TIM8_CH1��PWM��������жϣ�
				osDelay(1);
			}	
			DO_CEASE;//PA7-���ʹ��
			HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//�ر�PWM���
			flag_Motor_deceleration_stops=0;
		}
	}
}

/* USER CODE END Application */

