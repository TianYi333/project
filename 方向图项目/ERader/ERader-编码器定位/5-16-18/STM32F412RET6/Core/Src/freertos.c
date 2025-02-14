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

Motor motor1;
float Position;//��0��λ��,��λmm
uint16_t pwmVal_TIM8 = 0;  //ռ�ձȴ�С(CCRx�Ĵ�С���ߵ�ƽʱ��)
uint8_t Position_flag=0;//λ�ñ�־λ
uint8_t Encoder_Processing_flag=0;//���������ݴ����־λ
float  Track_Length;//���������λ����
uint16_t pwmVal_TIM13 = 0;  //ռ�ձȴ�С(CCRx�Ĵ�С���ߵ�ƽʱ��)
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

void Ethernet(void *argument);//����ͨ��
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
void Ethernet(void *argument)//W5500ͨѶ
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
	uint8_t Ethernet_Receive_array[5];	//��λ����������
  for(;;)
  {
    osDelay(500);
		HAL_GPIO_TogglePin(GPIOC, LED2_Pin);
		osMessageQueuePut(Ethernet_ReceiveHandle,Ethernet_Receive_array,1,0);		
  }

}


void Encoder(void *argument)//�綯��������ͨѶ
{
	if(Position_flag==1)
	{
		HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);      //������������ʱ��
		__HAL_TIM_ENABLE_IT(&htim2,TIM_IT_UPDATE);          //������������ʱ�������ж�,���������
		HAL_TIM_Base_Start_IT(&htim3);                    //����100ms��ʱ���ж�
		__HAL_TIM_SET_COUNTER(&htim2, 10000);               //��������ʱ����ʼֵ�趨Ϊ10000
		motor1.lastCount = 0;                               //�ṹ�����ݳ�ʼ��
		motor1.totalCount = 0;
		motor1.overflowNum = 0;                                  
		motor1.speed = 0;
		motor1.direct = 0;
		motor1.distance = 0;
		Position=0;
	}
  for(;;)
  {
		if(Encoder_Processing_flag==1)
		{
			Encoder_Processing_flag=0;
			motor1.direct = __HAL_TIM_IS_TIM_COUNTING_DOWN(&htim2);//������ϼ�������ת��������ֵΪ0�����򷵻�ֵΪ1
			int16_t pluse = COUNTERNUM - RELOADVALUE/2;
			motor1.totalCount = pluse+ motor1.overflowNum * RELOADVALUE/2;//�ӿ�ʼ�����ڵ�ǰ100ms����������	
			motor1.speed = (float)(motor1.totalCount - motor1.lastCount) / (4 * MOTOR_SPEED_RERATIO * PULSE_PRE_ROUND);//���100msת�˶���Ȧ,����4����Ϊ4��Ƶ
			motor1.speed = Speed_Low_Filter(motor1.speed,speed_Record);//�˲�����
			motor1.lastCount = motor1.totalCount; //��¼��һ�εļ���ֵ 
			motor1.distance = One_Circle_Distance * motor1.speed;//�����ƶ�����
			switch(Position_flag)
			{
				case 1:					
					Position = Position + motor1.distance;
					break;
				
				case 2:					
					Track_Length=Position+motor1.distance;//�ó��������	
					break;
				
				case 3:
					if(motor1.direct==1)//��ת
					{
						Position = Position - motor1.distance;	//��������
					}
					else//��ת
					{
						Position = Position + motor1.distance;	//��������		
					}
					break;
					
				case 4:
					Position=0.0f-(Track_Length/2);
					Position_flag=3;
					break;
				
				case 5:
					Position=0.0f;;
					Position_flag=3;
					break;
				
				case 6:
					Position=Track_Length/2;
					Position_flag=3;
					break;
				
				default:
					break;
			}			
		}	
    osDelay(1);
  }

}


void Drive(void *argument)//����������
{
	uint8_t status=0;	
	uint8_t Drive_control_array[5];	//��������������
	HAL_TIM_Base_Start_IT(&htim8);//����TIM8��ʱ���жϣ�200KHZ������5us��
	HAL_TIM_PWM_Start_IT(&htim8,TIM_CHANNEL_1);//����TIM8_CH1��PWM��������жϣ�
  for(;;)
  {
		status=osMessageQueueGet(Ethernet_ReceiveHandle,Drive_control_array,NULL,10);
		if(status!=osOK)	//һ��ʱ�䣨500ms��δ�յ�����
		{		
//		__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_1,pwmVal_TIM8);//�޸�ռ�ձȴ�С
//		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);//���Ƶ���������Ƶ������װ
//		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,GPIO_PIN_SET);//���Ƶ���������Ƶ����ͣ
		}
		osDelay(1);
  }
}


void Pulse_Emission(void *argument)//���巢�䣨TIM13_CH1ͨ����
{	
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);//����DIR���Ÿߵ�ƽ����ƽת����A->B
	HAL_TIM_Base_Start_IT(&htim13);//����TIM13��ʱ���ж�
	HAL_TIM_PWM_Start_IT(&htim13,TIM_CHANNEL_1);//����TIM13_CH1��PWM��������жϣ�
  for(;;)
  {
//		__HAL_TIM_SetCompare(&htim13,TIM_CHANNEL_1,pwmVal_TIM13);//�޸�ռ�ձȴ�С
    osDelay(1);
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

