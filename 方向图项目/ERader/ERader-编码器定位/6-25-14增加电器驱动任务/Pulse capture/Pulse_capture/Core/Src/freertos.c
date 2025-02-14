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
#include "tjc_usart_hmi.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
uint32_t IC_TIMES;  		// �����������λ1ms
uint8_t IC_START_FLAG;  // ����ʼ��־��1���Ѳ��񵽸ߵ�ƽ��0����û�в��񵽸ߵ�ƽ
uint8_t IC_DONE_FLAG;  	// ������ɱ�־��1�������һ�θߵ�ƽ����
uint16_t IC_VALUE;  		// ���벶��Ĳ���ֵ

uint32_t Pulse_count_total=0;	// ���벶���������������
uint32_t Pulse_count=0;	// ���벶���һ������������
uint16_t Starting_angle=0;//��ʼ�Ƕ�
uint16_t End_angle=0;//�����Ƕ�
uint16_t Angle=0;//��ת�Ƕ�
uint8_t Angular_velocity=0;//���ٶ�
uint8_t Pulse_frame_rate=0;//����֡��
uint8_t Pulse_standard_frame_rate=0;//�����׼֡��
uint8_t Unqualified_pulse_frame_rate=0;//���ϸ�����֡��
uint8_t START_FLAG=0;//��ʼ�����־λ
uint32_t Standard_pulse_count=0;//��׼����������
uint32_t One_cycle_angle=0;//һ��������ת�Ƕ�
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define FRAMELENGTH 9							//������Ļ�����ֽ���
#define Pulse_Filtering 50   		//�����˲�us
#define Interval 500 							//һ֡������ms
#define Standard_Cycle_angle 0.2*1000000	//��׼���ڼ���Ƕȡ�
#define Error_Criteria_angle 0.01*1000000	//��׼���Ƕ�
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
/* Definitions for Pulse_acquisitionTask */
osThreadId_t Pulse_acquisitionTaskHandle;
const osThreadAttr_t Pulse_acquisitionTask_attributes = {
  .name = "Pulse_acquisitionTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for ScreenTask */
osThreadId_t ScreenTaskHandle;
const osThreadAttr_t ScreenTask_attributes = {
  .name = "ScreenTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Screen_TransferTask */
osThreadId_t Screen_TransferTaskHandle;
const osThreadAttr_t Screen_TransferTask_attributes = {
  .name = "Screen_TransferTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Pulse_Reception_Queue */
osMessageQueueId_t Pulse_Reception_QueueHandle;
const osMessageQueueAttr_t Pulse_Reception_Queue_attributes = {
  .name = "Pulse_Reception_Queue"
};
/* Definitions for Pulse_Sending_Queue */
osMessageQueueId_t Pulse_Sending_QueueHandle;
const osMessageQueueAttr_t Pulse_Sending_Queue_attributes = {
  .name = "Pulse_Sending_Queue"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void Pulse_acquisition(void *argument);
void Screen(void *argument);
void Screen_Transfer(void *argument);

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
  /* creation of Pulse_Reception_Queue */
  Pulse_Reception_QueueHandle = osMessageQueueNew (5, 6*sizeof(uint8_t), &Pulse_Reception_Queue_attributes);

  /* creation of Pulse_Sending_Queue */
  Pulse_Sending_QueueHandle = osMessageQueueNew (5, 6*sizeof(uint8_t), &Pulse_Sending_Queue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of Pulse_acquisitionTask */
  Pulse_acquisitionTaskHandle = osThreadNew(Pulse_acquisition, NULL, &Pulse_acquisitionTask_attributes);

  /* creation of ScreenTask */
  ScreenTaskHandle = osThreadNew(Screen, NULL, &ScreenTask_attributes);

  /* creation of Screen_TransferTask */
  Screen_TransferTaskHandle = osThreadNew(Screen_Transfer, NULL, &Screen_TransferTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
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
		
    osDelay(500);
		HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_Pulse_acquisition */
/**
* @brief Function implementing the Pulse_acquisitionTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Pulse_acquisition */
void Pulse_acquisition(void *argument)
{
  /* USER CODE BEGIN Pulse_acquisition */
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);//����DIR���ŵ͵�ƽ����ƽת����B->A
	uint32_t Pulse_Cycle= 0;//��������
	uint8_t Pulse_Cycle_array[6];	//������������
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
		if(IC_DONE_FLAG == 1)	// ������һ�θߵ�ƽ����
		{
			IC_DONE_FLAG = 0;	//��־����
			Pulse_Cycle = IC_TIMES * 1000;	//����ʱ��Ϊ������� * 1000us
			Pulse_Cycle += IC_VALUE;	 // ���ϲ���ʱ�䣨С��1ms�Ĳ���)
			if(Pulse_Cycle<Pulse_Filtering*1.2&&Pulse_Cycle>Pulse_Filtering*0.8)	//�˲�
			{
				Pulse_Cycle_array[0]=0x55;
				Pulse_Cycle_array[1]=Pulse_Cycle&0xff;
				Pulse_Cycle_array[2]=(Pulse_Cycle>>8)&0xff;
				Pulse_Cycle_array[3]=(Pulse_Cycle>>16)&0xff;
				Pulse_Cycle_array[4]=(Pulse_Cycle>>24)&0xff;
				Pulse_Cycle_array[5]=0xff;
				osMessageQueuePut(Pulse_Reception_QueueHandle,Pulse_Cycle_array,1,0);
			}
		}
  }
  /* USER CODE END Pulse_acquisition */
}

/* USER CODE BEGIN Header_Screen */
/**
* @brief Function implementing the ScreenTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Screen */
void Screen(void *argument)
{
  /* USER CODE BEGIN Screen */
	uint32_t Pulse_Cycle_Send=0;//�������ڷ�����
	uint8_t status=0;
	uint8_t Pulse_Cycle_Send_array[6];	//�������ڷ�������
	TJCPrintf("gather.t7.txt=\"%d\"",0);	//��ʾ����������0
	TJCPrintf("gather.t9.txt=\"%d\"",0);	//��ʾһ������������������0
	TJCPrintf("gather.t11.txt=\"%d\"",0);	//��ʾ������������������0
	TJCPrintf("gather.t12.txt=\"%d\"",0);	//��ʾ����֡�����ϸ�����0
	TJCPrintf("gather.t13.txt=\"%d\"",0);	//��ʾ����֡����0
	TJCPrintf("set.n1.val=%d",0);	//��ʾ��ʼ�Ƕ���0
	TJCPrintf("set.n2.val=%d",0);	//��ʾ�����Ƕ���0
	TJCPrintf("set.n3.val=%d",0);	//��ʾ���ٶ���0
	TJCPrintf("set.n4.val=%d",0);	//��ʾ����֡����0
	TJCPrintf("page set");	//��ʾ������ҳ��
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
		if(START_FLAG==1)//�ж��Ƿ���ܵ���ʼ��������
		{
			status=osMessageQueueGet(Pulse_Reception_QueueHandle,Pulse_Cycle_Send_array,NULL,Interval);
			if(status!=osOK)	//һ��ʱ�䣨500ms��δ�յ�����
			{	
				if(Pulse_count!=0)
				{
					if(Pulse_frame_rate<Pulse_standard_frame_rate)//�ж�֡��
					{
						Pulse_frame_rate++;	//����֡������
						if(Pulse_count!=Standard_pulse_count)//һ֡���岨δ�����׼���岨��
						{
							Unqualified_pulse_frame_rate++;
						}
						Pulse_count=0;	//��ǰ������������������0
						TJCPrintf("gather.t7.txt=\"%d\"",0);	//��ʾ�ߵ�ƽʱ����0
						TJCPrintf("gather.t9.txt=\"%d\"",0);	//��ʾһ������ߵ�ƽ������0
						TJCPrintf("gather.t12.txt=\"%d\"",Unqualified_pulse_frame_rate);	//��ʾ����֡�����ϸ���
						TJCPrintf("gather.t13.txt=\"%d\"",Pulse_frame_rate);	//��ʾ����֡��
					}
					else
					{
						Pulse_frame_rate=0;
						START_FLAG=0;	//���岶��ر�
					}
				}				
			}
			else
			{
				if(Pulse_Cycle_Send_array[0]==0x55&&Pulse_Cycle_Send_array[5]==0xff)
				{
					Pulse_Cycle_Send=Pulse_Cycle_Send_array[1]|(Pulse_Cycle_Send_array[2]<<8)|(Pulse_Cycle_Send_array[3]<<16)|(Pulse_Cycle_Send_array[4]<<24);
					TJCPrintf("gather.t7.txt=\"%d\"",Pulse_Cycle_Send);	//��ʾ�ߵ�ƽʱ��
					Pulse_count_total++;	//����ߵ�ƽ����
					Pulse_count++;	//һ֡����ߵ�ƽ����	
					TJCPrintf("gather.t9.txt=\"%d\"",Pulse_count);	//��ʾһ������ߵ�ƽ����
					TJCPrintf("gather.t11.txt=\"%d\"",Pulse_count_total);	//��ʾ����ߵ�ƽ������	
				}
			}	
		}
  }
  /* USER CODE END Screen */
}

/* USER CODE BEGIN Header_Screen_Transfer */

/**
* @brief Function implementing the Screen_TransferTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Screen_Transfer */
void Screen_Transfer(void *argument)
{
  /* USER CODE BEGIN Screen_Transfer */
  /* Infinite loop */
  for(;;)
  {
		osDelay(1);
		while(usize >= FRAMELENGTH)//���ڻ�������������6
	  {
		  //У��֡ͷ֡β�Ƿ�ƥ��
		  if(u(0) != 0x55 || u(6) != 0xff || u(7) != 0xff || u(8) != 0xff)
		  {
			  //��ƥ��ɾ��1�ֽ�
			  udelete(1);
		  }else
		  {
			  //ƥ�䣬����ѭ��
			  break;
		  }
	  }	
	  //���н���
	  if(usize >= FRAMELENGTH && u(0) == 0x55 && u(6) == 0xff && u(7) == 0xff && u(8) == 0xff)
	  {
			switch(u(1))
			{
				case 0XEE://�˳����β���
					Pulse_count=0;	//��ǰ��һ֡����������������0
					Standard_pulse_count=0;//��׼һ֡������������0
					Pulse_count_total=0;	//����������������0					
					Unqualified_pulse_frame_rate=0;	//����֡�����ϸ�����0
					Pulse_frame_rate=0;//������֡����0					
					Starting_angle=0;//�ϴο�ʼ�ǶȲ�����0
					End_angle=0;//�ϴν����ǶȲ�����0
					Angular_velocity=0;//�ϴν��ٶȲ�����0
					Pulse_standard_frame_rate=0;//�ϴ������׼֡��������0
					START_FLAG=0;	//���岶��ر�				
					break;
				
				case 0XA1://��ʼ�Ƕ�
					Starting_angle=(u(5)<<24)|(u(4)<<16)|(u(3)<<8)|(u(2)<<0);//��Ϊ��Ļ����ΪС�˴��䣬��������λ����
					break;
				
				case 0XA2://�����Ƕ�
					End_angle=(u(5)<<24)|(u(4)<<16)|(u(3)<<8)|(u(2)<<0);
					break;
				
				case 0XA3://���ٶ�
					Angular_velocity=(u(5)<<24)|(u(4)<<16)|(u(3)<<8)|(u(2)<<0);
					break;
				
				case 0XA4://�����׼֡��
					Pulse_standard_frame_rate=(u(5)<<24)|(u(4)<<16)|(u(3)<<8)|(u(2)<<0);
					break;
				
				case 0XAA://��ʼ����
					START_FLAG=1;
					Angle=End_angle-Starting_angle;//��ת�Ƕȼ���
					Standard_pulse_count=Angle/0.2+1;//��׼һ֡����������
					TJCPrintf("gather.t10.txt=\"%d\"",Standard_pulse_count);	//��ʾ��׼һ֡����������	
					break;
				
				default:
					break;
			}
		  udelete(FRAMELENGTH);//ɾ�����ڻ���������Ӧ���ȵ�����
	  }		

  }
  /* USER CODE END Screen_Transfer */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

