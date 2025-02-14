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

	
//Motor motor1;
	
float Position=0;//��0��λ��,��λmm
uint32_t Position_upload=0;//��0������ϴ�ֵ����λ0.001mm��
uint32_t zero_return_speed_value=20000;//�����ٶȣ���λ0.001mm/s��
uint16_t pwmVal_TIM8 = 0;  //ռ�ձȴ�С(CCRx�Ĵ�С���ߵ�ƽʱ��)
uint8_t Position_flag=0;//ԭ��λ�ñ�־λ
uint8_t Sports_status_flag=0;//�˶�״̬��־λ
uint8_t Extreme_position_flag=0;//����λ�ñ�־λ
uint8_t Position_upload_flag=1;//λ���ϴ���־λ
uint8_t Encoder_Processing_flag=0;//���������ݴ����־λ
uint8_t Zero_regression_flag=0;//�Ƿ�������־λ
uint8_t Absolute_position_flag=0;//����λ�ñ�־λ
uint8_t Servo_reset_flag=0;//�ŷ���λ��־λ
float Track_Length=0;//���������λ����
uint32_t Target_Position=0;//Ŀ��λ��
uint32_t Zero_Position=0;//���λ��
uint8_t Sports_mode=0;//�˶�ģʽ
uint16_t Hard_Trigger_Number=0;//Ӳ��������
uint16_t Hard_Trigger_Current_Number=0;//Ӳ������ǰ����
uint32_t Hard_Trigger_Interval=0;//Ӳ����ʱ����
uint32_t Hard_Trigger_Position=0;//Ӳ������ʼλ��
uint8_t Hard_Trigger_start_flag=0;//Ӳ������ʼ��־λ
uint8_t Hard_Trigger_flag=0;//Ӳ������־λ
uint32_t Trigger_Interval;//�������
float Position_lastpulsec;//�ϴ�Ӳ����λ��
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


void Ethernet_Server(void *argument);//����ͨ��-�����
void Encoder(void *argument);//�綯��������ͨѶ
void Drive(void *argument);//����������
void Motor_drive(void *argument);//�綯����
void Pulse_Emission(void *argument);//���巢��
void Extreme_position_Drive(void *argument);//����λ�ÿ���
void Uart3_Communicate(void *argument);//����λ�ÿ���
void Servo_reset(void *argument);//�ŷ���λ

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
	if(HAL_GPIO_ReadPin(IN1_GPIO_Port,IN1_Pin)==GPIO_PIN_RESET)//��ԭ��
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
		osDelay(20);
  }
}


void Drive(void *argument)//����������
{
	uint8_t	send_buff[13];
	uint8_t status=0;	
	uint32_t running_speed_value=2000;//�����ٶ�
	uint32_t zero_return_creep_speed_value=0;//��������ٶ�
	uint8_t Drive_control_array[20];	//��������������
	uint16_t Data_Bytes=0;//�����ֽ�
	uint8_t Motor_drive_send[3];//�����������
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

							case 0x01://ԭ��ع�/ֹͣ
								send_buff[4]=0X00;//���ݳ���
								send_buff[5]=0X05;//���ݳ���
								send_buff[8]=0X01;//���������ݵ��ֽڳ���
								if(Sports_status_flag==1)//��ԭ��
								{
									send_buff[9]=0X01;
								}
								else if(Sports_status_flag==0)//����ԭ��
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
						if(HAL_GPIO_ReadPin(IN2_GPIO_Port,IN2_Pin)==GPIO_PIN_RESET)//������λ
						{
							send_buff[9]=0X01;//����
							send(1,send_buff,10);//��ͻ��˷���	
							memset(send_buff,0,13);	
						}
						else
						{
							send_buff[9]=0X00;//����
							send(1,send_buff,10);//��ͻ��˷���	
							memset(send_buff,0,13);	
						}
					}						
					else if(Drive_control_array[8]==0x00&&Drive_control_array[9]==0x03)//�ж���ʼ��ַ������ת���޹��״̬
					{	
						if(HAL_GPIO_ReadPin(IN0_GPIO_Port,IN0_Pin)==GPIO_PIN_RESET)//������λ
						{
							send_buff[9]=0X01;//����
							send(1,send_buff,10);//��ͻ��˷���	
							memset(send_buff,0,13);	
						}
						else
						{
							send_buff[9]=0X00;//����
							send(1,send_buff,10);//��ͻ��˷���	
							memset(send_buff,0,13);	
						}						
					}
					else if(Drive_control_array[8]==0x00&&Drive_control_array[9]==0x04)//�ж���ʼ��ַ���������λ����״̬	
					{
						if(HAL_GPIO_ReadPin(IN1_GPIO_Port,IN1_Pin)==GPIO_PIN_RESET)//������λ
						{
							send_buff[9]=0X01;//����
							send(1,send_buff,10);//��ͻ��˷���	
							memset(send_buff,0,13);	
						}
						else
						{
							send_buff[9]=0X00;//����
							send(1,send_buff,10);//��ͻ��˷���	
							memset(send_buff,0,13);	
						}
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
							else if(Drive_control_array[9]==0x77)//�ж���ʼ��ַ�����ϵ���Ƿ�ִ�й�����
							{
								send_buff[8]=Data_Bytes;//���������ݵ��ֽڳ���
								if(Zero_regression_flag==1)
								{
									send_buff[9]=0X00;//����
									send_buff[10]=0X01;//����
									send(1,send_buff,11);//��ͻ��˷���	
									memset(send_buff,0,13);
								}
								else if(Zero_regression_flag==0)
								{
									send_buff[9]=0X00;//����
									send_buff[10]=0X00;//����
									send(1,send_buff,11);//��ͻ��˷���	
									memset(send_buff,0,13);	
								}									
							}
						}
						else if(Data_Bytes==0x04)
						{
							if(Drive_control_array[9]==0x2C)//�ж���ʼ��ַ������ʼӲ����λ��
							{
								send_buff[8]=Data_Bytes;//���������ݵ��ֽڳ���
								send_buff[11]=(Hard_Trigger_Position>>24)&0xFF;//����
								send_buff[12]=(Hard_Trigger_Position>>16)&0xFF;//����
								send_buff[9]=(Hard_Trigger_Position>>8)&0xFF;//����
								send_buff[10]=Hard_Trigger_Position&0xFF;//����
								send(1,send_buff,13);//��ͻ��˷���	
								memset(send_buff,0,13);														
							}
							else if(Drive_control_array[9]==0x2E)//�ж���ʼ��ַ����Ӳ�������
							{
								send_buff[8]=Data_Bytes;//���������ݵ��ֽڳ���
								send_buff[11]=(Hard_Trigger_Interval>>24)&0xFF;//�Ĵ���1H-����
								send_buff[12]=(Hard_Trigger_Interval>>16)&0xFF;//�Ĵ���1L����
								send_buff[9]=(Hard_Trigger_Interval>>8)&0xFF;//�Ĵ���2H����
								send_buff[10]=Hard_Trigger_Interval&0xFF;//�Ĵ���2L����
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
								send_buff[11]=(zero_return_speed_value>>24)&0xFF;//����
								send_buff[12]=(zero_return_speed_value>>16)&0xFF;//����
								send_buff[9]=(zero_return_speed_value>>8)&0xFF;//����
								send_buff[10]=zero_return_speed_value&0xFF;//����
								send(1,send_buff,13);//��ͻ��˷���	
								memset(send_buff,0,13);																
							}
							else if(Drive_control_array[9]==0x68)//�ж���ʼ��ַ������������ٶ�
							{
								send_buff[8]=Data_Bytes;//���������ݵ��ֽڳ���
								send_buff[11]=(zero_return_creep_speed_value>>24)&0xFF;//�Ĵ���1H-����
								send_buff[12]=(zero_return_creep_speed_value>>16)&0xFF;//�Ĵ���1L-����
								send_buff[9]=(zero_return_creep_speed_value>>8)&0xFF;//�Ĵ���2H-����
								send_buff[10]=zero_return_creep_speed_value&0xFF;//�Ĵ���2L-����
								send(1,send_buff,13);//��ͻ��˷���	
								memset(send_buff,0,13);									
							}
							else if(Drive_control_array[9]==0x78)//�ж���ʼ��ַ���������ٶ�
							{		
								send_buff[8]=Data_Bytes;//���������ݵ��ֽڳ���
								send_buff[11]=(running_speed_value>>24)&0xFF;//�Ĵ���1H-����
								send_buff[12]=(running_speed_value>>16)&0xFF;//�Ĵ���1L-����
								send_buff[9]=(running_speed_value>>8)&0xFF;//�Ĵ���2H-����
								send_buff[10]=running_speed_value&0xFF;//�Ĵ���2L-����
								send(1,send_buff,13);//��ͻ��˷���	
								memset(send_buff,0,13);															
							}
							else if(Drive_control_array[9]==0x7A)//�ж���ʼ��ַ������Զ�λ����
							{
								send_buff[8]=Data_Bytes;//���������ݵ��ֽڳ���
								send_buff[11]=0X00;//�Ĵ���1H-����
								send_buff[12]=0X00;//�Ĵ���1L-����
								send_buff[9]=0X00;//�Ĵ���2H-����
								send_buff[10]=0X00;//�Ĵ���2L-����
								send(1,send_buff,13);//��ͻ��˷���	
								memset(send_buff,0,13);														
							}
							else if(Drive_control_array[9]==0x7C)//�ж���ʼ��ַ�������Զ�λλ��
							{
								send_buff[8]=Data_Bytes;//���������ݵ��ֽڳ���
								send_buff[11]=(Target_Position>>24)&0xFF;//�Ĵ���1H-����
								send_buff[12]=(Target_Position>>16)&0xFF;//�Ĵ���1L-����
								send_buff[9]=(Target_Position>>8)&0xFF;//�Ĵ���2H-����
								send_buff[10]=Target_Position&0xFF;//�Ĵ���2L-����
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
								send_buff[11]=(Zero_Position>>24)&0xFF;//�Ĵ���1H-����
								send_buff[12]=(Zero_Position>>16)&0xFF;//�Ĵ���1L-����
								send_buff[9]=(Zero_Position>>8)&0xFF;//�Ĵ���2H-����
								send_buff[10]=Zero_Position&0xFF;//�Ĵ���2L-����
								send(1,send_buff,13);//��ͻ��˷���	
								memset(send_buff,0,13);														
							}	
							else if(Drive_control_array[9]==0x87)//�ж���ʼ��ַ����ʵʱλ��������ȡ
							{
								send_buff[8]=Data_Bytes;//���������ݵ��ֽڳ���
								send_buff[11]=(Position_upload>>24)&0xFF;//�Ĵ���1H-����
								send_buff[12]=(Position_upload>>16)&0xFF;//�Ĵ���1L-����
								send_buff[9]=(Position_upload>>8)&0xFF;//�Ĵ���2H-����
								send_buff[10]=Position_upload&0xFF;//�Ĵ���2L-����
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
										if(Sports_status_flag==0)//�ж��˶�״̬
										{
											send_buff[10]=0XFF;
											send_buff[11]=0X00;
											send(1,send_buff,12);//��ͻ��˷���	
											memset(send_buff,0,13);	
											Sports_status_flag=1;
											Motor_drive_send[0]=GPIO_PIN_SET;
											Motor_drive_send[1]=((One_Circle_Distance*1000000*10)/(zero_return_speed_value*Subtraction/100))>>8&0xff;
											Motor_drive_send[2]=(One_Circle_Distance*1000000*10)/(zero_return_speed_value*Subtraction/100)&0xff;
											osMessageQueuePut(Motor_driveQueueHandle,Motor_drive_send,1,0);	
										}											
									}	
								}
								else if(Drive_control_array[10]==0x00)//ֹͣ
								{
									send_buff[10]=0X00;
									send_buff[11]=0X00;
									send(1,send_buff,12);//��ͻ��˷���	
									memset(send_buff,0,13);	
									HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//�ر�PWM���
									HAL_GPIO_WritePin(TIM8_CH1N_GPIO_Port,TIM8_CH1N_Pin,GPIO_PIN_SET);//PA7-���ʹ��������(GPIO_PIN_RESET:��ʼ��GPIO_PIN_SET:ֹͣ��								
									Sports_status_flag=0;									
								}
								break;
							
							case 0x02://��ת�㶯/ֹͣ
								if(Drive_control_array[10]==0xFF)//��ʼ
								{
									send_buff[10]=0XFF;
									send_buff[11]=0X00;
									send(1,send_buff,12);//��ͻ��˷���	
									memset(send_buff,0,13);	
									Sports_status_flag=1;
									Position_flag=0;
									Motor_drive_send[0]=GPIO_PIN_RESET;
									Motor_drive_send[1]=((One_Circle_Distance*1000000*10)/(running_speed_value*Subtraction/100))>>8&0xff;
									Motor_drive_send[2]=(One_Circle_Distance*1000000*10)/(running_speed_value*Subtraction/100)&0xff;
									osMessageQueuePut(Motor_driveQueueHandle,Motor_drive_send,1,0);
								}
								else//ֹͣ
								{
									send_buff[10]=0X00;
									send_buff[11]=0X00;
									send(1,send_buff,12);//��ͻ��˷���	
									memset(send_buff,0,13);	
									HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//�ر�PWM���
									HAL_GPIO_WritePin(TIM8_CH1N_GPIO_Port,TIM8_CH1N_Pin,GPIO_PIN_SET);//PA7-���ʹ��������(GPIO_PIN_RESET:��ʼ��GPIO_PIN_SET:ֹͣ��
									Sports_status_flag=0;									
								}
								break;
							
							case 0x03://��ת�㶯/ֹͣ
								if(Drive_control_array[10]==0xFF)//��ʼ
								{
									send_buff[10]=0XFF;
									send_buff[11]=0X00;
									send(1,send_buff,12);//��ͻ��˷���	
									memset(send_buff,0,13);
									Sports_status_flag=1;
									Position_flag=0;
									Motor_drive_send[0]=GPIO_PIN_SET;
									Motor_drive_send[1]=((One_Circle_Distance*1000000*10)/(running_speed_value*Subtraction/100))>>8&0xff;
									Motor_drive_send[2]=(One_Circle_Distance*1000000*10)/(running_speed_value*Subtraction/100)&0xff;
									osMessageQueuePut(Motor_driveQueueHandle,Motor_drive_send,1,0);								
								}
								else//ֹͣ
								{
									send_buff[10]=0X00;
									send_buff[11]=0X00;
									send(1,send_buff,12);//��ͻ��˷���	
									memset(send_buff,0,13);	
									HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//�ر�PWM���
									HAL_GPIO_WritePin(TIM8_CH1N_GPIO_Port,TIM8_CH1N_Pin,GPIO_PIN_SET);//PA7-���ʹ��������(GPIO_PIN_RESET:��ʼ��GPIO_PIN_SET:ֹͣ��
									Sports_status_flag=0;								
								}							
								break;							
							
							case 0x04://����˶�/ֹͣ
								if(Drive_control_array[10]==0xFF)//��ʼ
								{
									send_buff[10]=0XFF;
									send_buff[11]=0X00;
									send(1,send_buff,12);//��ͻ��˷���	
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
								else//ֹͣ
								{
									send_buff[10]=0X00;
									send_buff[11]=0X00;
									send(1,send_buff,12);//��ͻ��˷���	
									memset(send_buff,0,13);	
									HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//�ر�PWM���
									HAL_GPIO_WritePin(TIM8_CH1N_GPIO_Port,TIM8_CH1N_Pin,GPIO_PIN_SET);//PA7-���ʹ��������(GPIO_PIN_RESET:��ʼ��GPIO_PIN_SET:ֹͣ��
									Sports_status_flag=0;
								}								
								break;
							
							case 0x05://�����˶�/ֹͣ
								if(Drive_control_array[10]==0xFF)//��ʼ
								{
									send_buff[10]=0XFF;
									send_buff[11]=0X00;
									send(1,send_buff,12);//��ͻ��˷���	
									memset(send_buff,0,13);
									if(Sports_status_flag==0&&Extreme_position_flag==0)
									{
										if(Position_upload*10<Target_Position)//��ǰλ��С��Ŀ��λ��-�����ƶ�
										{
											Motor_drive_send[0]=GPIO_PIN_RESET;
											Motor_drive_send[1]=((One_Circle_Distance*1000000*10)/(running_speed_value*Subtraction/100))>>8&0xff;
											Motor_drive_send[2]=(One_Circle_Distance*1000000*10)/(running_speed_value*Subtraction/100)&0xff;
											osMessageQueuePut(Motor_driveQueueHandle,Motor_drive_send,1,0);
										}
										else if(Position_upload*10>Target_Position&&Position_flag==0)//��ǰλ�ô���Ŀ��λ��-�����ƶ�
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
								else//ֹͣ
								{
									send_buff[10]=0X00;
									send_buff[11]=0X00;
									send(1,send_buff,12);//��ͻ��˷���	
									memset(send_buff,0,13);	
									HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//�ر�PWM���
									HAL_GPIO_WritePin(TIM8_CH1N_GPIO_Port,TIM8_CH1N_Pin,GPIO_PIN_SET);//PA7-���ʹ��������(GPIO_PIN_RESET:��ʼ��GPIO_PIN_SET:ֹͣ��
									Sports_status_flag=0;
								}							
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
								if(Drive_control_array[10]==0xFF)
								{
									send_buff[10]=0XFF;
									send_buff[11]=0X00;
									send(1,send_buff,12);//��ͻ��˷���	
									memset(send_buff,0,13);	
									Servo_reset_flag=1;
								}
								else if(Drive_control_array[10]==0x00)
								{
									send_buff[10]=0X00;
									send_buff[11]=0X00;
									send(1,send_buff,12);//��ͻ��˷���	
									memset(send_buff,0,13);	
									Servo_reset_flag=0;									
								}
								break;
							
							case 0x07://�ŷ���ͣ
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
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,13);	
								break;

							case 0x09://�״��ϵ�/�ϵ�
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
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,13);								
								break;
							
							case 0x0F://PLC�������							
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
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,13);	
								break;
							
							default:
								break;													
						}
					}
					break;
				
				case 0x06://д�������ּĴ���(����MBAP ������ �Ĵ�����ַH �Ĵ�����ַL �Ĵ���ֵH �Ĵ���ֵL)
//					if(Sports_status_flag==0)//δ��ʼ�˶�
//					{
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
//					}										
					break;
					
				case 0x10://д������ּĴ���(����MBAP(7λ) ������ ��ʼ��ַH ��ʼ��ַL �Ĵ�������H �Ĵ�������L �ֽڳ��� �Ĵ���ֵ)
//				if(Sports_status_flag==0)//δ��ʼ�˶�
//				{
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
								zero_return_speed_value=(Drive_control_array[13]<<8|Drive_control_array[14]|Drive_control_array[15]<<24|Drive_control_array[16]<<16);//��˴������ݴ����������ٶ�0.001mm/s
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,13);																
							}
							else if(Drive_control_array[9]==0x68)//�ж���ʼ��ַ������������ٶ�
							{
								zero_return_creep_speed_value=(Drive_control_array[13]<<8|Drive_control_array[14]|Drive_control_array[15]<<24|Drive_control_array[16]<<16);//��˴������ݴ�������������ٶ�0.001mm/s
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,13);									
							}
							else if(Drive_control_array[9]==0x78)//�ж���ʼ��ַ���������ٶ�
							{
//								running_speed_value=(Drive_control_array[13]<<24|Drive_control_array[14]<<16|Drive_control_array[15]<<8|Drive_control_array[16]);//��˴������ݴ����������ٶ�0.001mm/s
								running_speed_value=(Drive_control_array[13]<<8|Drive_control_array[14]|Drive_control_array[15]<<24|Drive_control_array[16]<<16);//��˴������ݴ����������ٶ�0.001mm/s
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
								Absolute_position_flag=1;
								Target_Position=(Drive_control_array[13]<<8|Drive_control_array[14]|Drive_control_array[15]<<24|Drive_control_array[16]<<16);//��˴������ݴ��������Զ�λλ��0.001mm
								if(Target_Position==0x00)
								{
									Extreme_position_flag=0;
								}
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
								Zero_Position=(Drive_control_array[13]<<8|Drive_control_array[14]|Drive_control_array[15]<<24|Drive_control_array[16]<<16);//��˴������ݴ��������λ��0.001mm
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,13);														
							}	
						}
						else if(Drive_control_array[8]==0x01)//�ж���ʼ��ַ
						{
							if(Drive_control_array[9]==0x2C)//�ж���ʼ��ַ������ʼӲ����λ��
							{
								Hard_Trigger_Position=(Drive_control_array[13]<<8|Drive_control_array[14]|Drive_control_array[15]<<24|Drive_control_array[16]<<16);//��˴������ݴ�������ʼӲ����λ��0.001mm
								Position_lastpulsec=Hard_Trigger_Position;
								send(1,send_buff,12);//��ͻ��˷���	
								memset(send_buff,0,13);														
							}
							else if(Drive_control_array[9]==0x2E)//�ж���ʼ��ַ����Ӳ�������
							{
								Hard_Trigger_Interval=(Drive_control_array[13]<<8|Drive_control_array[14]|Drive_control_array[15]<<24|Drive_control_array[16]<<16);//��˴������ݴ�����Ӳ�������0.001mm
								send(1,send_buff,12);//��ͻ��˷���	
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


void Motor_drive(void *argument)//�綯����
{
	uint8_t status=0;	
//	uint16_t number_i=10000;
	uint16_t number;
	uint8_t Motor_drive_receive[3];//�����������
	for(;;)
	{    
		osDelay(1);
		status=osMessageQueueGet(Motor_driveQueueHandle,Motor_drive_receive,NULL,10);
		if(status==osOK)
		{	
			if(Sports_status_flag==1)
			{
				number=Motor_drive_receive[1]<<8|Motor_drive_receive[2];
				HAL_GPIO_WritePin(TIM8_CH1N_GPIO_Port,TIM8_CH1N_Pin,GPIO_PIN_RESET);//PA7-���ʹ��������(GPIO_PIN_RESET:��ʼ��GPIO_PIN_SET:ֹͣ��
				osDelay(1);	
				HAL_GPIO_WritePin(TIM8_CH2_GPIO_Port,TIM8_CH2_Pin,(GPIO_PinState)Motor_drive_receive[0]);//PC7-����������(GPIO_PIN_RESET:˳ʱ�룬GPIO_PIN_SET:��ʱ�룩
				osDelay(1);	
				HAL_TIM_Base_Start_IT(&htim8);//����TIM8��ʱ���жϣ�20KHZ������50us��
				__HAL_TIM_SET_AUTORELOAD(&htim8,number-1);//�޸��Զ���װ��ֵ
				HAL_TIM_PWM_Start_IT(&htim8,TIM_CHANNEL_1);//����TIM8_CH1��PWM��������жϣ�
//				if(number>=100)
//				{
//					while(number_i>number&&HAL_GPIO_ReadPin(TIM8_CH1N_GPIO_Port,TIM8_CH1N_Pin)==GPIO_PIN_RESET)
//					{
//						__HAL_TIM_SET_AUTORELOAD(&htim8,number_i-1);//�޸��Զ���װ��ֵ
//						HAL_TIM_PWM_Start_IT(&htim8,TIM_CHANNEL_1);//����TIM8_CH1��PWM��������жϣ�
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


void Encoder(void *argument)//�綯��������ͨѶ
{
	uint8_t Real_time_location_array[6]={0xCF,0xAE,0x00,0x00,0x00,0x00};//ʵʱλ������
  for(;;)
  {
		if(Encoder_Processing_flag==1)
		{
			Encoder_Processing_flag=0;
			if(Position_flag==1||HAL_GPIO_ReadPin(IN1_GPIO_Port,IN1_Pin)==GPIO_PIN_RESET)//ԭ��У׼
			{	
				Position=0;
				Position_upload=0;
			}	
			else if(Position_flag==0)
			{
				Position = ((float)Motor_pulsec_ount/Subtraction)*One_Circle_Distance;	//��ԭ�����
				Position_upload = Position*100;//λ���ϴ����ݣ���λ0.01mm��
				if(Zero_regression_flag==1&&Absolute_position_flag==1)//֮ǰ�ع����ҽ��յ�����λ��
				{
					if(HAL_GPIO_ReadPin(TIM8_CH2_GPIO_Port,TIM8_CH2_Pin)==GPIO_PIN_RESET)//�����ƶ�
					{
						if(Position_upload*10>Target_Position)
						{
							HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//�ر�PWM���
							HAL_GPIO_WritePin(TIM8_CH1N_GPIO_Port,TIM8_CH1N_Pin,GPIO_PIN_SET);//PA7-���ʹ��������(GPIO_PIN_RESET:��ʼ��GPIO_PIN_SET:ֹͣ��
							Sports_status_flag=0;	//�˶�״̬	
							Absolute_position_flag=0;	
						}		
					}
					else if(HAL_GPIO_ReadPin(TIM8_CH2_GPIO_Port,TIM8_CH2_Pin)==GPIO_PIN_SET)//�����ƶ�
					{
						if(Position_upload*10<Target_Position)
						{
							HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//�ر�PWM���
							HAL_GPIO_WritePin(TIM8_CH1N_GPIO_Port,TIM8_CH1N_Pin,GPIO_PIN_SET);//PA7-���ʹ��������(GPIO_PIN_RESET:��ʼ��GPIO_PIN_SET:ֹͣ��
							Sports_status_flag=0;	//�˶�״̬	
							Absolute_position_flag=0;	
						}
					}					
				}	
			}
			if(Position_upload_flag==1)//ʵʱλ���ϴ�
			{				
				Real_time_location_array[2]=Position_upload&0xFF;
				Real_time_location_array[3]=(Position_upload>>8)&0xFF;
				Real_time_location_array[4]=(Position_upload>>16)&0xFF;
				Real_time_location_array[5]=(Position_upload>>24)&0xFF;
				send(2,Real_time_location_array,6);//��ͻ��˷���	
			}	
			if(Hard_Trigger_flag==1)
			{
				Trigger_Interval=Position*1000-Position_lastpulsec;
				if(Trigger_Interval>=Hard_Trigger_Interval*10)//.����������������������������������������������������������������������������������������������
				{
					Position_lastpulsec=Position*1000;
					Hard_Trigger_flag=0;
					Trigger_Interval=0;					
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
		if(Hard_Trigger_start_flag==1&&Position_upload>=Hard_Trigger_Position&&Sports_status_flag==1)//Ӳ��������(Ӳ��������������Ӳ����λ�ã��˶�״̬)
		{
			if(Hard_Trigger_flag==0)
			{
				if(Hard_Trigger_Current_Number<Hard_Trigger_Number)//Ӳ���������ж�
				{		
					Hard_Trigger_Current_Number++;
					HAL_GPIO_WritePin(IO_OUT_GPIO_Port, IO_OUT_Pin, GPIO_PIN_SET);//PA2-Ӳ����
					HAL_TIM_Base_Start_IT(&htim13);//����TIM13��ʱ���жϣ�1MHZ������50�Σ�50us��
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


void Extreme_position_Drive(void *argument)//����λ�ÿ���
{
  for(;;)
  {
		osDelay(1);
		if(Extreme_position_flag==1)
		{
				HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//�ر�PWM���
				HAL_GPIO_WritePin(TIM8_CH1N_GPIO_Port,TIM8_CH1N_Pin,GPIO_PIN_SET);//PA7-���ʹ��������(GPIO_PIN_RESET:��ʼ��GPIO_PIN_SET:ֹͣ��
				Sports_status_flag=0;
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
		VERSION[1] = 0X0A;//�����
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
		memset(uart_receive_buff,0,3);	
  }
}

void Servo_reset(void *argument)//�ŷ���λ
{
  for(;;)
  {
		if(Servo_reset_flag==1)
		{
			HAL_TIM_PWM_Stop_IT(&htim8,TIM_CHANNEL_1);//�ر�PWM���
			HAL_GPIO_WritePin(TIM8_CH1N_GPIO_Port,TIM8_CH1N_Pin,GPIO_PIN_SET);//PA7-���ʹ��������(GPIO_PIN_RESET:��ʼ��GPIO_PIN_SET:ֹͣ��
			Sports_status_flag=0;
			HAL_GPIO_WritePin(TIM8_CH2N_GPIO_Port,TIM8_CH2N_Pin,GPIO_PIN_RESET);	
			osDelay(500);
			HAL_GPIO_WritePin(TIM8_CH2N_GPIO_Port,TIM8_CH2N_Pin,GPIO_PIN_SET);
			Servo_reset_flag=0;
		}	
	}
}
/* USER CODE END Application */

