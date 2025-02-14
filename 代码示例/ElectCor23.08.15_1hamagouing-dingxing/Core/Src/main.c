/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *2021.5.26 �����޸ģ�1.��������ļ�setting��������������FLASH���ٶ�ȡ��2.��������ŷ�������setting.c�С�3.33996������ע��
	*2021.5.27 �����޸ģ����ADC���ѹ�����г̡���ӿ����񡣴�����������δ���
  *2021.7.1 ����ټܼ��ͨ�����߽���ͨ
  *2021.9.13 �������ͨ���޸��˲��ֺ궨��
	*2021.9.15 �����bug��������������Ĵ���û�ĳɹ�
	*2021.10.11 ����Զ���ѹ�ĳ��޶��������
	*
	*
	*
  *
	*****************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */


#include "stm32f4xx_hal.h"
#include "bsp_ds1302.h"
#include "bsp_mcz33996.h"
#include "bsp_oled19264.h"
#include "bsp_hd7279.h"
#include "bsp_leds.h"
#include "bsp_max3232.h"
#include "bsp_ncv7356.h"
#include "bsp_w25q128.h"
#include "parameter_operation.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "time.h"
#include "define.h"
#include "Group_move.h"
/* test */
#include <stdio.h>
#include <stdlib.h>
#include "bsp_ParaSyn.h"
#include "Follow_AUTO_.h"
#include "wireless_control.h"
#include "bsp_485.h"
#include "bsp_oled19264.h"
#include "sensor.h"
#include "interface.h"
#include "main_logic.h"
#include "cansend.h"
#include "canreceive.h"
#include "controlded.h"
#include "set.h"
#include "drv_usart.h"
#include "controlded.h"
#include "follow.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
int Gflag_Dir=0;	//han ������

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

CAN_HandleTypeDef hcan1;
CAN_HandleTypeDef hcan2;
CAN_HandleTypeDef hcan3;

IWDG_HandleTypeDef hiwdg;

SPI_HandleTypeDef hspi2;

TIM_HandleTypeDef htim1;

UART_HandleTypeDef huart7;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
UART_HandleTypeDef huart6;
DMA_HandleTypeDef hdma_uart7_rx;
DMA_HandleTypeDef hdma_uart7_tx;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;
DMA_HandleTypeDef hdma_usart6_tx;
DMA_HandleTypeDef hdma_usart6_rx;




/* USER CODE BEGIN PV */
u8 Count_MAIN_SCTAMSTOP = 0;
u8 Count_MAIN_LOCK = 0;
u8 FLAG_MAIN_SCTAMSTOP = 0;
u8 FLAG_MAIN_LOCK = 0;


void Abrupt_Stop_function(void);










typedef uint8_t u8;



/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_CAN2_Init(void);
static void MX_CAN3_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM1_Init(void);
static void MX_UART7_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_USART6_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_CAN1_Init(void);
static void MX_IWDG_Init(void);
static void MX_SPI2_Init(void);
/* USER CODE BEGIN PFP */
/******************************���������******************************/
/* ���������� */
static TaskHandle_t AppTaskCreate_Handle = NULL;
/* ���˵������� */
static TaskHandle_t Main_Menu_Task_Handle = NULL;
/* ������������� */
static TaskHandle_t Key_Monitor_Task_Handle = NULL;
/* ���������� */
//static TaskHandle_t Trumpet_Task_Handle = NULL;

/* CAN���������� */
static TaskHandle_t CAN_Send_Task_Handle = NULL;
/*QQQQQQQQQQQQQQ���߽���������QQQQQQQQQQQQQQQQQQ */
static TaskHandle_t CAN_Bus_Receive_Task_Handle = NULL;
/*QQQQQQQQQQQQQQ����״̬������QQQQQQQQQQQQQQQQQQ */
static TaskHandle_t Fault_State_Task_Handle = NULL;
/*QQQQQQQQQQQQQQ�����Զ�������������QQQQQQQQQQQQQQQQQQ */
static TaskHandle_t Follow_Automation_Task_Handle = NULL;//�Զ�����������

static TaskHandle_t CAN_Receive_Task_Handle = NULL;

/*jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj*/
/* USART3���������� */
static TaskHandle_t Sensor_Receive_Task_Handle = NULL;
/* ����������������� */
static TaskHandle_t Idle_Display_Task_Handle = NULL;
/* ��ͣ������������� */
static TaskHandle_t Abrupt_Stop_Task_Handle = NULL;
/* ����can����������*/
static TaskHandle_t Unite_Action_Task_Handle = NULL;
/* ң��������*/
static TaskHandle_t Interface_Mian_Logic_Task_Handle = NULL;
/* ��λ������������*/
static TaskHandle_t MOVE_Control_Task_Handle = NULL;
/*jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj*/
/* LED�ƿ��������� */
static TaskHandle_t LED_Task_Handle = NULL;
/*jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj*/
/* ��ȫ���������� */
static TaskHandle_t Security_Msnagement_Task_Handle = NULL;
/******************************����������******************************/
static void AppTaskCreate(void);/* ���ڴ������� */
static void Main_Menu_Task(void* pvParameters);/* ���˵����� */
static void Key_Monitor_Task(void* pvParameters);/* ����������� */
//static void Trumpet_Task(void* pvParameters);/* ���������� */
/*QQQQQQQQQQQQQQQ���߽�������QQQQQQQQQQQQQQQQQQ*/
static void CAN_Bus_Receive_Task(void* pvParameters);/* ���߽�������*/


static void CAN_Send_Task(void* pvParameters);/* CAN���������� */
static void CAN_Receive_Task(void* pvParameters);/* CAN���������� */
/*QQQQQQQQQQQQQQ����״̬����QQQQQQQQQQQQQQQQQQ */
static void Fault_State_Task(void* pvParameters);/* ����״̬���� */
/*QQQQQQQQQQQQQQ�����Զ�����������QQQQQQQQQQQQQQQQQQ */
static void Follow_Automation_Task(void* pvParameters);//�Զ���������
/*jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj*/
static void LED_Task(void* pvParameters);/* LED�ƿ��������� */
//static void USART3_Receive_Task(void* pvParameters);/* USART2���������� JJJJJJJJJJJ*/
static void Sensor_Receive_Task(void* pvParameters);/* ADC�ɼ������� JJJJJJJJJJ*/
static void Idle_Display_Task(void* pvParameters);/* ����������������� JJJJJJJJJJ*/
static void Abrupt_Stop_Task(void* pvParameters);/* ��ͣ���������� JJJJJJJJJJ*/
static void Unite_Action_Task(void* pvParameters);/*������������������ JJJJJJJJJJ*/
static void Interface_Mian_Logic_Task(void* pvParameters);/*ң�ط��������� JJJJJJJJJJ*/
static void MOVE_Control_Task(void* pvParameters);/*������������������ JJJJJJJJJJ*/
static void Security_Msnagement_Task(void* pvParameters);/*�������鰲ȫ���������� JJJJJJJJJJ*/


static void Fault_Pre_Task(void* pvParameters);/* ����״̬���� */
static void Fault_Near_Task(void* pvParameters);/* ����״̬���� */

/*jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj*/
/******************************��Ϣ����******************************/
QueueHandle_t Key_Value_ISR_Queue =NULL;/* �����ж��з��ذ���ֵ */
QueueHandle_t Key_Monitor_Start_Queue =NULL;/* ����֪ͨ�����������ʼ��ⰴ���Ƿ��ɿ� */
QueueHandle_t Key_Status_Queue =NULL;/* ���ڰ�����������з��ذ����Ѿ��ɿ���һ״̬ */
QueueHandle_t Trumpet_Start_Queue =NULL;/* ����֪ͨ������ */
QueueHandle_t CAN_Send_Queue =NULL;/* ����֪ͨCAN��ʼ���� */

QueueHandle_t CAN_Receive_ISR_Queue =NULL;


QueueHandle_t CAN2_Receive_ISR_Queue =NULL;

QueueHandle_t LED_Control_Queue =NULL;/* ����֪ͨLED�ı� */

QueueHandle_t Para_Trans_Queue =NULL;/* ������������ɱ�־λ */
QueueHandle_t Abrupt_Stop_Queue =NULL;/* ��ͣ���� */
/*jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj*/
QueueHandle_t Group_Unite_Move_Queue =NULL;
QueueHandle_t Unite_Action_Queue =NULL;
QueueHandle_t  Interface_Mian_Logic_Queue = NULL;
QueueHandle_t  MOVE_Control_Queue = NULL;
/*jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj*/
/*QQQQQQQQQQQQQQͨ�ż����Ϣ����QQQQQQQQQQQQQQQQ*/
QueueHandle_t Communication_Detection_Left_Queue = NULL;//CAN��������
QueueHandle_t Communication_Detection_Right_Queue = NULL;//CAN��������
/*QQQQQQQQQQQQQQQ���߽�������QQQQQQQQQQQQQQQQQQ*/
QueueHandle_t CAN_Bus_Receive_Queue = NULL;//���߽�����Ϣ����
/*QQQQQQQQQQQQQQ����״̬��Ϣ����QQQQQQQQQQQQQQQQ*/
QueueHandle_t Fault_State_Queue = NULL;//���߽�����Ϣ����
/*QQQQQQQQQQQQQQ�����Զ�����Ϣ����QQQQQQQQQQQQQQQQ*/
QueueHandle_t Follow_Automation_Queue = NULL;//�����Զ�����Ϣ����



/*QQQQQQQQQQQQQQ�������ͽṹ��QQQQQQQQQQQQQQQQQQ*/
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* ������ʾ�������� */


void Display_Stand_Column(void);
void Display_Front_Column(void);
void Display_Back_Column(void);
void Display_Balance_Arm(void);
void Display_Bottom(void);
void Display_Back_Protect(void);
//void Display_Flexible_Arm(void);
void Display_Front_Arm(void);
//void Display_Side_Protect(void);
void Display_Back_Arm(void);
void Display_Spile(void);
//void Display_Aback(void);
void Display_Reverse_Wash(void);

/* �жϲ���ʾѡ�е��� */
void Display_Cheak_Valve(uint8_t cheak_num);

/* ���ý�����ʾ���� */
void Display_Cheak_Commu_Info(uint8_t cheak_num);//ͨѶ��Χ
void Display_Cheak_Global_Setting(uint8_t cheak_num);//��������
void Display_Cheak_Groupe_Prohibit(uint8_t cheak_num);//�����ֹ����
void Display_Cheak_Serve(uint8_t cheak_num);//����
void Display_Cheak_Default_Para(uint8_t cheak_num);//ȱʡ����
void Display_Cheak_Sensor(uint8_t cheak_num);//������
void Display_Cheak_Follow_Para(uint8_t cheak_num);//��������

/* ���ý����������뺯�� */
uint8_t Passwd_Input_Fun(void);


/* ���ò����޸ĺ��� */
void Setting_Prar_Change(uint8_t Setting_Crosswise,uint8_t Setting_Lengthways);

/* ���ý�����ʾ���� */
uint8_t *Display_Setting_Interface(uint8_t Setting_Crosswise,uint8_t Setting_Lengthways);

/* ���ò������뺯�� */
int Setting_Prar_Change_Input(uint8_t type,uint8_t Previous_Para);

/* �������������ݺ��� */
void Para_Trans_Fun(CAN_HandleTypeDef *hcanX,uint8_t Next_ID);


/*QQQQQQQQQQQQQQQQQQQQQ�ϱ���Ϣ����QQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
void Report_Information(void);
void Display_Fault_Num(uint8_t num);//������ʾ����ܺź�������
/*QQQQQQQQQQQQQQQQQQQQQ������Ϣ��ʾ����QQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
uint8_t Fault_Display(uint8_t Setting_Crosswise);
char Fault_List_Display(uint8_t fault_count,uint8_t array[],char index,char* display,char number);//�����б���ʾ
void Bus_Fault_Display(void);
void Adjacent_Fault_Display(void);
void Display_Num(uint8_t page,uint8_t column,uint8_t Number);
/*QQQQQQQQQQQQQQQQQQQQQ�Զ���������QQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
void FAM_FRAME_MOVE(uint8_t temp);
void FAM_active_send(uint8_t action,uint16_t value,uint8_t extra,uint8_t count);
void FAM_apply_send(void);
void FAM_BUS_send(char choice);



int cmpfun(const void* a, const void * b)
{
	return (*(uint8_t*)a - *(uint8_t*)b);
}


uint8_t State_main_TrumpetStatus;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_CAN2_Init();
  MX_CAN3_Init();
  MX_ADC1_Init();
  MX_TIM1_Init();
  MX_UART7_Init();
  MX_USART3_UART_Init();
  MX_USART6_UART_Init();
  MX_USART2_UART_Init();
  MX_CAN1_Init();
  MX_IWDG_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */
	/*********************/
	/*********************/	
	__enable_irq();
	LEDS_LIGHT(UNKOWN_UP);

	SPI_FLASH_Init();
	MCZ33996_INIT();
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);//P-Ctrl
//	TIME3_Init(2000-1,9600-1);//���Ź���ʱ��
	OLED19264_INIT();

//	Setting_Init_Char();
//	Setting_Init_Para();
	
	
	Clear_Screen();

	Main_Logic_Init();

	hw_usart_register(&huart6);
hw_usart_register(&huart2);
	HAL_UART_Receive_DMA(&huart7,array_Angel_receive,20);

	OPEN_MCZ33996_OUT(0);
	
	


	/*********************/
	/*********************/
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		/* ����AppTaskCreate���� */
		xTaskCreate((TaskFunction_t )AppTaskCreate,  /* ������ں��� */
							(const char*    )"AppTaskCreate",/* �������� */
							(uint16_t       )512,  /* ����ջ��С */
							(void*          )NULL,/* ������ں������� */
							(UBaseType_t    )1, /* ��������ȼ� */
							(TaskHandle_t*  )&AppTaskCreate_Handle);/* ������ƿ�ָ�� */ 
		/* ����������� */           
		vTaskStartScheduler();   /* �������񣬿������� */

		
		while(1)
		{
			
		}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = ENABLE;
  hadc1.Init.NbrOfDiscConversion = 1;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 3;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = 2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = 3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief CAN1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */

  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 80;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_9TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_8TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */

  /* USER CODE END CAN1_Init 2 */

}

/**
  * @brief CAN2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN2_Init(void)
{

  /* USER CODE BEGIN CAN2_Init 0 */

  /* USER CODE END CAN2_Init 0 */

  /* USER CODE BEGIN CAN2_Init 1 */

  /* USER CODE END CAN2_Init 1 */
  hcan2.Instance = CAN2;
  hcan2.Init.Prescaler = 80;
  hcan2.Init.Mode = CAN_MODE_NORMAL;
  hcan2.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan2.Init.TimeSeg1 = CAN_BS1_9TQ;
  hcan2.Init.TimeSeg2 = CAN_BS2_8TQ;
  hcan2.Init.TimeTriggeredMode = DISABLE;
  hcan2.Init.AutoBusOff = DISABLE;
  hcan2.Init.AutoWakeUp = DISABLE;
  hcan2.Init.AutoRetransmission = DISABLE;
  hcan2.Init.ReceiveFifoLocked = DISABLE;
  hcan2.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN2_Init 2 */

  /* USER CODE END CAN2_Init 2 */

}

/**
  * @brief CAN3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN3_Init(void)
{

  /* USER CODE BEGIN CAN3_Init 0 */

  /* USER CODE END CAN3_Init 0 */

  /* USER CODE BEGIN CAN3_Init 1 */

  /* USER CODE END CAN3_Init 1 */
  hcan3.Instance = CAN3;
  hcan3.Init.Prescaler = 80;
  hcan3.Init.Mode = CAN_MODE_NORMAL;
  hcan3.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan3.Init.TimeSeg1 = CAN_BS1_9TQ;
  hcan3.Init.TimeSeg2 = CAN_BS2_8TQ;
  hcan3.Init.TimeTriggeredMode = DISABLE;
  hcan3.Init.AutoBusOff = DISABLE;
  hcan3.Init.AutoWakeUp = DISABLE;
  hcan3.Init.AutoRetransmission = DISABLE;
  hcan3.Init.ReceiveFifoLocked = DISABLE;
  hcan3.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN3_Init 2 */

  /* USER CODE END CAN3_Init 2 */

}

/**
  * @brief IWDG Initialization Function
  * @param None
  * @retval None
  */
static void MX_IWDG_Init(void)
{

  /* USER CODE BEGIN IWDG_Init 0 */

  /* USER CODE END IWDG_Init 0 */

  /* USER CODE BEGIN IWDG_Init 1 */

  /* USER CODE END IWDG_Init 1 */
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_256;
  hiwdg.Init.Reload = 4095;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN IWDG_Init 2 */

  /* USER CODE END IWDG_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 600;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 1000;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief UART7 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART7_Init(void)
{

  /* USER CODE BEGIN UART7_Init 0 */

  /* USER CODE END UART7_Init 0 */

  /* USER CODE BEGIN UART7_Init 1 */

  /* USER CODE END UART7_Init 1 */
  huart7.Instance = UART7;
  huart7.Init.BaudRate = 9600;
  huart7.Init.WordLength = UART_WORDLENGTH_8B;
  huart7.Init.StopBits = UART_STOPBITS_1;
  huart7.Init.Parity = UART_PARITY_NONE;
  huart7.Init.Mode = UART_MODE_TX_RX;
  huart7.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart7.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart7) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART7_Init 2 */

  /* USER CODE END UART7_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief USART6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART6_UART_Init(void)
{

  /* USER CODE BEGIN USART6_Init 0 */

  /* USER CODE END USART6_Init 0 */

  /* USER CODE BEGIN USART6_Init 1 */

  /* USER CODE END USART6_Init 1 */
  huart6.Instance = USART6;
  huart6.Init.BaudRate = 9600;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART6_Init 2 */

  /* USER CODE END USART6_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
  /* DMA1_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
  /* DMA1_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);
  /* DMA2_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);
  /* DMA2_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream6_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_13
                          |GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_12, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA5 PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PC4 PC5 PC12 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PD0 PD1 PD2 PD3
                           PD4 PD7 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PG9 PG10 PG11 PG12
                           PG13 PG14 PG15 */
  GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12
                          |GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pin : PB3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB4 PB8 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  

  /*Configure GPIO pins : PB5 PB6 PB7 */
  GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI3_IRQn, 8, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

}

/* USER CODE BEGIN 4 */
void Main_Delay(uint32_t time)
{
	for(int i=0;i<time;i++);
}

static void AppTaskCreate(void)
{

  taskENTER_CRITICAL();           //�����ٽ���
  
	Key_Value_ISR_Queue = xQueueCreate(1,1);
	Key_Monitor_Start_Queue = xQueueCreate(1,1);
	Key_Status_Queue = xQueueCreate(1,1);
	Trumpet_Start_Queue = xQueueCreate(5,3);
	
	CAN_Send_Queue = xQueueCreate(10,8);//֪ͨCAN����������CAN��Ϣ����Ϣ���д�СΪ8�ֽ�
	
	CAN_Receive_ISR_Queue = xQueueCreate(100,8);//��CAN�жϽ��յ�����Ϣ���͵�CAN��Ϣ�����������Ϣ����
	CAN2_Receive_ISR_Queue = xQueueCreate(100,8);
	Abrupt_Stop_Queue = xQueueCreate(5,8);
	LED_Control_Queue = xQueueCreate(1,1);
	
	Para_Trans_Queue = xQueueCreate(1,1);
	Group_Unite_Move_Queue = xQueueCreate(1,1);

	Unite_Action_Queue = xQueueCreate(1,8);
	Interface_Mian_Logic_Queue = xQueueCreate(2,8);
	MOVE_Control_Queue = xQueueCreate(2,16);

		/* QQQQQQQQQQQQQQͨ�ż�����Ϣ����QQQQQQQQQQQQQQQQQQQQQ */
	CAN_Bus_Receive_Queue = xQueueCreate(20,8);//��CAN�����жϽ��յ�����Ϣ���͵�������Ϣ�����������Ϣ����
	/* QQQQQQQQQQQQQQͨ�ż�����Ϣ����QQQQQQQQQQQQQQQQQQQQQ */
	Communication_Detection_Left_Queue = xQueueCreate(2,1);//����������յ���Ϣ�����ڼ�ͨ�ż�⺯��
	Communication_Detection_Right_Queue = xQueueCreate(2,1);//����������յ���Ϣ�����ڼ�ͨ�ż�⺯��
	Fault_State_Queue = xQueueCreate(8,1);//���չ���״̬��Ϣ
	Follow_Automation_Queue = xQueueCreate(20,8);//�����Զ�����Ϣ����
	  
	
  /* �������˵����� */
  xTaskCreate((TaskFunction_t )Main_Menu_Task, /* ������ں��� */
                        (const char*    )"Main_Menu_Task",/* �������� */
                        (uint16_t       )256,   /* ����ջ��С */
                        (void*          )NULL,	/* ������ں������� */
                        (UBaseType_t    )15,	    /* ��������ȼ� */
                        (TaskHandle_t*  )&Main_Menu_Task_Handle);/* ������ƿ�ָ�� */
												
//	/* ��������������� */
//  xTaskCreate((TaskFunction_t )Key_Monitor_Task, /* ������ں��� */
//                        (const char*    )"Key_Monitor_Task",/* �������� */
//                        (uint16_t       )128,   /* ����ջ��С */
//                        (void*          )NULL,	/* ������ں������� */
//                        (UBaseType_t    )3,	    /* ��������ȼ� */
//                        (TaskHandle_t*  )&Key_Monitor_Task_Handle);/* ������ƿ�ָ�� */
//		
	/* �������������� */
  xTaskCreate((TaskFunction_t )CAN_Send_Task,
                        (const char*    )"CAN_Send_Task",
                        (uint16_t       )128,   
                        (void*          )NULL,
                        (UBaseType_t    )13,	  
                        (TaskHandle_t*  )&CAN_Send_Task_Handle);
												

						
	/* ����CAN�������� */
  xTaskCreate((TaskFunction_t )CAN_Receive_Task,
                        (const char*    )"CAN_Receive_Task",
                        (uint16_t       )128,   
                        (void*          )NULL,
                        (UBaseType_t    )14,	  
                        (TaskHandle_t*  )&CAN_Receive_Task_Handle);
												
	/* ����LED���� */
  xTaskCreate((TaskFunction_t )LED_Task,
                        (const char*    )"LED_Task",
                        (uint16_t       )128,   
                        (void*          )NULL,
                        (UBaseType_t    )6,	  
                        (TaskHandle_t*  )&LED_Task_Handle);
//JJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJ
  xTaskCreate((TaskFunction_t )Sensor_Receive_Task,
                        (const char*    )"Sensor_Receive_Task",
                        (uint16_t       )128,   
                        (void*          )NULL,
                        (UBaseType_t    )8,	  
                        (TaskHandle_t*  )&Sensor_Receive_Task_Handle);
										
												


  xTaskCreate((TaskFunction_t )Abrupt_Stop_Task	,
                        (const char*    )"Abrupt_Stop_Task",
                        (uint16_t       )128,   
                        (void*          )NULL,
                        (UBaseType_t    )20,	  
                        (TaskHandle_t*  )&Abrupt_Stop_Task_Handle);
												


	
												
	xTaskCreate((TaskFunction_t )Unite_Action_Task	,
                        (const char*    )"Unite_Action_Task",
                        (uint16_t       )128,   
                        (void*          )NULL,
                        (UBaseType_t    )9,	  
                        (TaskHandle_t*  )&Unite_Action_Task_Handle );
												
	xTaskCreate((TaskFunction_t )Fault_State_Task	,
                        (const char*    )"Fault_State_Task",
                        (uint16_t       )128,   
                        (void*          )NULL,
                        (UBaseType_t    )17,	  
                        (TaskHandle_t*  )&Fault_State_Task_Handle );												
								
//JJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJ	
	/*QQQQQQQQQQQQQQQQQQQQQQQQQQ����ͨ�Ž��պ���QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
	xTaskCreate((TaskFunction_t )CAN_Bus_Receive_Task,
                        (const char*    )"CAN_Bus_Receive_Task",
                        (uint16_t       )128,   
                        (void*          )NULL,
                        (UBaseType_t    )16,	  
                        (TaskHandle_t*  )&CAN_Bus_Receive_Task_Handle );	
	/*QQQQQQQQQQQQQQQQQQQQQQQQQQ�����Զ�������QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
	xTaskCreate((TaskFunction_t )Follow_Automation_Task,
                        (const char*    )"Follow_Automation_Task",
                        (uint16_t       )128,   
                        (void*          )NULL,
                        (UBaseType_t    )11,	  
                        (TaskHandle_t*  )&Follow_Automation_Task_Handle );	//���ȼ�����
	/*QQQQQQQQQQQQQQQQQQQQQQQQQQ�����Զ�������QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
	xTaskCreate((TaskFunction_t )Interface_Mian_Logic_Task,
                        (const char*    )"Interface_Mian_Logic_Task",
                        (uint16_t       )128,   
                        (void*          )NULL,
                        (UBaseType_t    )11,	  
                        (TaskHandle_t*  )&Interface_Mian_Logic_Task_Handle );	//���ȼ�����
	/*QQQQQQQQQQQQQQQQQQQQQQQQQQ�����Զ�������QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
	xTaskCreate((TaskFunction_t )MOVE_Control_Task,
                        (const char*    )"MOVE_Control_Task",
                        (uint16_t       )128,   
                        (void*          )NULL,
                        (UBaseType_t    )12,	  
                        (TaskHandle_t*  )&MOVE_Control_Task_Handle );	//���ȼ�����
												
	xTaskCreate((TaskFunction_t )Security_Msnagement_Task,
                        (const char*    )"Security_Msnagement_Task",
                        (uint16_t       )128,   
                        (void*          )NULL,
                        (UBaseType_t    )10,	  
                        (TaskHandle_t*  )&Security_Msnagement_Task_Handle );	//���ȼ�����Security_Msnagement
                        
//  xTaskCreate((TaskFunction_t )Fault_Near_Task	,
//                        (const char*    )"Fault_Near_Task",
//                        (uint16_t       )32,   
//                        (void*          )NULL,
//                        (UBaseType_t    )7,	  
//                        (TaskHandle_t*  )NULL );	
//                        
//	xTaskCreate((TaskFunction_t )Fault_Pre_Task	,
//                        (const char*    )"Fault_Pre_Task",
//                        (uint16_t       )32,   
//                        (void*          )NULL,
//                        (UBaseType_t    )7,	  
//                        (TaskHandle_t*  )NULL );	 
												
  vTaskDelete(AppTaskCreate_Handle); //ɾ��AppTaskCreate����		

		Init_Set_Vlaue();				
		CAN_Config();					
		CH455_init();
	Main_logic_STATEchose();						

	HAL_NVIC_EnableIRQ(EXTI3_IRQn);
	HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
	HAL_NVIC_EnableIRQ(CAN3_RX0_IRQn);
	HAL_NVIC_EnableIRQ(USART3_IRQn);
	HAL_NVIC_EnableIRQ(CAN2_RX1_IRQn);
						variant_Service_DeviceNum = Structset_Service_DeviceNum1_1.Setting_Para;
						DeviceNum[0] = (variant_Service_DeviceNum/100)+48;
						DeviceNum[1] = ((variant_Service_DeviceNum%100)/10)+48;
						DeviceNum[2] = (variant_Service_DeviceNum%10)+48;
						DeviceNum[3] = '\0';
							Display_context_show(default_page,12,0,0);

 




							srand(Structset_Service_DeviceNum1_1.Setting_Para);
							
							// ���������
							P_random = rand();
							
						min_random = P_random%20;
						P_random= min_random+240;






						Structset_Service_RemoteCortrol3_2.Setting_Para = 0;
						
  taskEXIT_CRITICAL();            //�˳��ٽ���
						
						
}



static void Main_Menu_Task(void* parameter)
{



	while (1)
	{
			Main_logic();
	}	
	
}

static void Key_Monitor_Task(void* pvParameters)
{
	uint8_t temp;

	while(1)
	{
//		xQueueReceive(Key_Monitor_Start_Queue,&temp,portMAX_DELAY);
//		while(HD7279_KEY_READ() != 0xff)
//		{
//			vTaskDelay(20);
//		}
////		xQueueReset(Key_Monitor_Start_Queue);
//		xQueueSend(Key_Status_Queue,&temp,0);

	}
	
}

static void CAN_Send_Task(void* pvParameters)
{

	while(1)
	{
		xQueueReceive(CAN_Send_Queue,&array_Cansend_commandR,portMAX_DELAY);
		Cansendfaction();
		

		
	}
}
static void LED_Task(void* pvParameters)
{
	/*LED_Status
	 *0.����UNKOWN_UP
	 *1.����KEY28��KEY27�ŵ�
	
	 */
	uint8_t  array_STATE[3] = {0};
	uint8_t  Count_Trumpet_i=0;
	while(1)
	{
		vTaskDelay(50);
		if(State_main_TrumpetStatus == 0)
		{

			 if(HAL_GPIO_ReadPin(BEEP_PORT,BEEP_PIN) == GPIO_PIN_SET)
			 {
				BEEP_OFF;
			 }
		}
		else if(State_main_TrumpetStatus == 1)
	    {
		 
			BEEP_ON;
			Count_Trumpet_i++; 
			if(Count_Trumpet_i  >=  5)
			{
				BEEP_OFF;
				State_main_TrumpetStatus = 0;
				Count_Trumpet_i  = 0;
			
			}
		}
		else if(State_main_TrumpetStatus == 2)
	    {

			Count_Trumpet_i++; 
			if(Count_Trumpet_i  >=  6)
			{
				HAL_GPIO_TogglePin(BEEP_PORT,BEEP_PIN);
				Count_Trumpet_i = 0;
			}	
		}
			
		if(variant_Canreceive_STATE !=_variant_Mainlogic_statement.PRESENTstate_value)//������״̬�ϴ�   ����������� �������2023/12/24
		{
		
			variant_Canreceive_STATE =_variant_Mainlogic_statement.PRESENTstate_value;
			array_STATE[0] = BUS_SEND_STATE;
			array_STATE[1] = Structset_Service_DeviceNum1_1.Setting_Para;

			switch(variant_Canreceive_STATE)
			{
				case 0x01:											
				case 0x03:				
				case 0x04:
				case 0x06:						
				case 0x0a:
					
					array_STATE[2] = variant_Canreceive_STATE;
					CANx_SendNormalData(&BUS_CAN, PCID, array_STATE, 3);	
					break;
				default:				
					break; 
			}
		}
	}	
}


static void CAN_Receive_Task(void* pvParameters)
{

		
//	uint8_t effective_time;
	while(1)
	{
		xQueueReceive(CAN_Receive_ISR_Queue,array_Canreceive_CanLRcmdR,portMAX_DELAY);
		LRcan_Reveice_Fuction();
	}
}



/*ADC�ɼ�����*/
//JJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJ
static void Sensor_Receive_Task(void* pvParameters)
{
		
		while(1)
		{
			vTaskDelay(200);
			Sensor_Receive_rfuc();
		}
}

static void Idle_Display_Task(void* pvParameters)
{
	while(1)
	{
		vTaskDelay(1000);
	}
}
//��ͣ�ͱ���



static void Abrupt_Stop_Task(void* pvParameters)
{




	while(1)
	{
		Abrupt_Stop_function();
		
	}

}


	uint8_t  instacy_count = 0;
	uint8_t  lock_count = 0;

uint8_t G_Instancy_NState   = 0; //��ͣ��ǰ״̬
uint8_t G_Lock_NState       = 0; //������ǰ״̬

uint8_t Flag_main_STOP = 0;
uint8_t Flag_main_STOPcontitue = 0;
uint32_t Time_main_stop;
#define G_H4_Check()			 G_Instancy_NState = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1)
#define G_H5_Check()    	 		G_Lock_NState = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0)

uint8_t array_main_STOPnum[3] = {0};//��¼��ͣ�ܺ�
uint8_t variant_main_STOPnum;//���ռ�ͣ��

char variant_main_STOPveiw1[4];//ת���ַ���
char variant_main_STOPveiw2[4];//ת���ַ���
char variant_main_STOPveiw3[4];//ת���ַ���
void Abrupt_Stop_function(void)
{
//	uint8_t const_Mainlogic_InterfaceFlag = 1;
		uint8_t  time_delay = 20;//���20��
		uint8_t array_BUFF[8];
		uint8_t i=0;
		uint8_t STop_record=0;
		
	instacy_count = 0;
	lock_count= 0;
		do{
				G_H4_Check();//��⼱ͣ״̬
				G_H5_Check();//������״̬
				
				if(!G_Instancy_NState)
				{
					instacy_count ++ ;
				}
				if(!G_Lock_NState)
				{
					lock_count++;
				}
				vTaskDelay(20);
				time_delay -- ;
		}while(time_delay);//���ʱ�䵽
		
		
		
		if((instacy_count > 17) && (lock_count > 17))//ȷ�ϼ�ͣ������ H4 H5��û��
		{
			if(_variant_Mainlogic_statement.PRESENTstate_value != URGENCY_STOP )
			{
				Time_main_stop=TIME;
				Clear_Screen();
				_variant_Mainlogic_statement.PRESENTstate_value = URGENCY_STOP;
				_variant_Interface_disp.type = TYPE_URGENCY_STOP;
					xQueueSend(Interface_Mian_Logic_Queue,&const_Mainlogic_InterfaceFlag,0);
				state_Controled_MoveFlag = CONTROLED_MOVE_OFF;
				state_Controled_ContiueFlag = CONTROLED_MOVE_STOP;
				FLAG_drvusart_romatecontrol	= 0;//ȡ��ң��
				vTaskSuspend(Unite_Action_Task_Handle);
				
				array_BUFF[0] =BUS_STOP_LOCK;
				array_BUFF[1] = 0x01;
				array_BUFF[2] = Structset_Service_DeviceNum1_1.Setting_Para;
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);
				CANx_SendNormalData(&BUS_CAN,0xff,array_BUFF,3);//
			
				

				Display_String(page_Second_Line,column_1_Line,"      ���ܼ�ͣ   ");
				
			}
			else if((_variant_Mainlogic_statement.PRESENTstate_value == URGENCY_STOP)&&(TIME-Time_main_stop>200))
			{
				Time_main_stop = TIME;
				array_BUFF[0] = BUS_STOP_LOCK;
				array_BUFF[1] = 0x01;
				array_BUFF[2] = Structset_Service_DeviceNum1_1.Setting_Para;
				CANx_SendNormalData(&BUS_CAN,0xff,array_BUFF,3);//
				Display_String(page_Second_Line,column_1_Line,"      ���ܼ�ͣ   ");
				

			}
			
				
			//�ж������Դֹͣ����������ֹͣ���鶯����ֹͣ��������
		}
		else if((instacy_count > 17 && lock_count < 5 ))//ȷ�ϱ��������� H4�е� H5û��
		{
			if(_variant_Mainlogic_statement.PRESENTstate_value != URGENCY_LOCK)
			{
				_variant_Mainlogic_statement.PRESENTstate_value = URGENCY_LOCK;
				_variant_Interface_disp.type = TYPE_DEFAULT;
				state_Controled_MoveFlag = CONTROLED_MOVE_OFF;
				state_Controled_ContiueFlag = CONTROLED_MOVE_STOP;
				vTaskSuspend(Unite_Action_Task_Handle);
				FLAG_drvusart_romatecontrol	= 0;//ȡ��ң��			
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);
				xQueueSend(Interface_Mian_Logic_Queue,&const_Mainlogic_InterfaceFlag,0);
				vTaskDelay(20);
				xQueueSend(Interface_Mian_Logic_Queue,&const_Mainlogic_InterfaceFlag,0);
			}
			

		}
		else if ((instacy_count < 5 && lock_count < 5))//��� H4��H5���е�
		{
			
			if(Flag_main_STOP== 0)
			{
				if(_variant_Mainlogic_statement.PRESENTstate_value == URGENCY_STOP)
				{
					_variant_Mainlogic_statement.PRESENTstate_value = State_Mainlogic_DWFAULTstate;
					_variant_Interface_disp.type = TYPE_DEFAULT;
					xQueueReset(Unite_Action_Queue);//���ö�����Ϣ����
					vTaskResume(Unite_Action_Task_Handle);
					HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);
					array_BUFF[0] = BUS_STOP_LOCK;
					array_BUFF[1] = 0x02;
					CANx_SendNormalData(&BUS_CAN,0xff,array_BUFF,2);//
					xQueueSend(Interface_Mian_Logic_Queue,&const_Mainlogic_InterfaceFlag,0);
					State_main_TrumpetStatus = 0;					
				}
				else if(_variant_Mainlogic_statement.PRESENTstate_value == URGENCY_LOCK)
				{
					_variant_Mainlogic_statement.PRESENTstate_value = State_Mainlogic_DWFAULTstate;
					_variant_Interface_disp.type = TYPE_DEFAULT;
					xQueueReset(Unite_Action_Queue);//���ö�����Ϣ����
					vTaskResume(Unite_Action_Task_Handle);
					HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);
					xQueueSend(Interface_Mian_Logic_Queue,&const_Mainlogic_InterfaceFlag,0);
					State_main_TrumpetStatus = 0;					
				}
			}	
			else if(Flag_main_STOP== 1)		
			{
				if(_variant_Mainlogic_statement.PRESENTstate_value != URGENCY_STOP )    
				{
					FLAG_drvusart_romatecontrol	= 0;//ȡ��ң��
					Time_main_stop=TIME;
					Clear_Screen();
					_variant_Mainlogic_statement.PRESENTstate_value = URGENCY_STOP;
					_variant_Interface_disp.type = TYPE_URGENCY_STOP;
					state_Controled_MoveFlag = CONTROLED_MOVE_OFF;
					state_Controled_ContiueFlag = CONTROLED_MOVE_STOP;
					vTaskSuspend(Unite_Action_Task_Handle);
					HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);
					xQueueSend(Interface_Mian_Logic_Queue,&const_Mainlogic_InterfaceFlag,0);
					

				}

					
				if(TIME-Time_main_stop>500)	
				{
					if(Flag_main_STOPcontitue == 1)
					{
							Flag_main_STOPcontitue = 0;
					}
					else if(Flag_main_STOPcontitue == 0)
					{
							Flag_main_STOP= 2;
					}
					memset(array_main_STOPnum,0,4);
				
				}


				if(variant_main_STOPnum != 0)//���¼ܺ�
				{
					STop_record = 0; 
					for(i=0;i<3;i++)
					{
						if(array_main_STOPnum[i] == variant_main_STOPnum)
						{
							STop_record = 1;
							i=4;
						}
						else if((array_main_STOPnum[i] == 0)&&(STop_record == 0))//û��ͬ�ż�ͣ��¼
						{
							array_main_STOPnum[i] = variant_main_STOPnum;
							i=12;					
						}

							
						
					}
					if(i>=12)
					{
						if(array_main_STOPnum[0] != 0)
						{
								variant_main_STOPveiw1[0] = (array_main_STOPnum[0]%1000)/100+48;
								variant_main_STOPveiw1[1] =(array_main_STOPnum[0]%100)/10+48;
								variant_main_STOPveiw1[2] =array_main_STOPnum[0]%10+48;
								variant_main_STOPveiw1[3] ='\0';
								Display_String(page_Second_Line,column_3_Line,variant_main_STOPveiw1);
								Display_String(page_Second_Line,column_6_Line," �ż�ͣ  ");
						}
						else
						{
							Display_String(page_Second_Line,column_1_Line,"                ");
						
						}	
						
						if(array_main_STOPnum[1] != 0)
						{
								variant_main_STOPveiw2[0] = (array_main_STOPnum[1]%1000)/100+48;
								variant_main_STOPveiw2[1] =(array_main_STOPnum[1]%100)/10+48;
								variant_main_STOPveiw2[2] =array_main_STOPnum[1]%10+48;
								variant_main_STOPveiw2[3] ='\0';
							Display_String(page_Third_Line,column_3_Line,variant_main_STOPveiw2);
								Display_String(page_Third_Line,column_6_Line," �ż�ͣ  ");
						}
						else
						{
							Display_String(page_Third_Line,column_1_Line,"                ");
						
						}
						
						if(array_main_STOPnum[2] != 0)
						{
								variant_main_STOPveiw3[0] = (array_main_STOPnum[2]%1000)/100+48;
								variant_main_STOPveiw3[1] =(array_main_STOPnum[2]%100)/10+48;
								variant_main_STOPveiw3[2] =array_main_STOPnum[2]%10+48;
								variant_main_STOPveiw3[3] ='\0';
								Display_String(page_Fouth_Line,column_3_Line,variant_main_STOPveiw2);
								Display_String(page_Fouth_Line,column_6_Line," �ż�ͣ  ");
						}
						else
						{
							Display_String(page_Fouth_Line,column_1_Line,"                ");
						
						}
						

						
					}
					variant_main_STOPnum = 0;
										
					
				}
				
			}
			else if(Flag_main_STOP== 2)
			{
				Flag_main_STOP = 0;
				_variant_Mainlogic_statement.PRESENTstate_value = State_Mainlogic_DWFAULTstate;
				_variant_Interface_disp.type = TYPE_DEFAULT;
				xQueueReset(Unite_Action_Queue);//���ö�����Ϣ����
				vTaskResume(Unite_Action_Task_Handle);
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);
				xQueueSend(Interface_Mian_Logic_Queue,&const_Mainlogic_InterfaceFlag,0);
				State_main_TrumpetStatus = 0;				
			}	
			
				
		}
		
}





static void Unite_Action_Task(void* pvParameters)
{

		
		while(1)
		{
			CMD_Control_Faction();
    
		}
}
static void Security_Msnagement_Task(void* pvParameters)
{

	while(1)
	{
	
			vTaskDelay(20);	
			if(_variant_Mainlogic_statement.PRESENTstate_value == State_Mainlogic_DWFAULTstate)
			{

			}
			if(_variant_Mainlogic_statement.PRESENTstate_value == CONTROLED)
			{
					Security_controled_Single();
			}

	}
}



/* ѡ��������ʾ���� */


/* ��ʾ����ѡ���ĸ��� */









static void CAN_Bus_Receive_Task(void* pvParameters)/* ���߽�������*/
{
	while(1)
	{
		BUScan_Reveice_Fuction();
		 HAL_IWDG_Refresh(&hiwdg); 
	}
}

static void Fault_State_Task(void* pvParameters)/* ���߽�������*/
{

	while(1)
	{
		vTaskDelay(1000);
		if(Flag_ncv7356_cansendeorr == 1)
		{
		
			MX_CAN2_Init();
			MX_CAN3_Init();
			MX_CAN1_Init();
			CAN_Config();
			Flag_ncv7356_cansendeorr =0;			
		}
	}
}





static void Follow_Automation_Task(void* pvParameters)/* �Զ���������*/
{

	while(1)
	{
        Follow_move();
	}
}


static void MOVE_Control_Task(void* pvParameters)
{
	
	while(1)
	{
		vTaskDelay(50);
//		if((Structset_Service_RemoteCortrol3_2.Setting_Para == 1)||(array_drvusart_bufR[1]==DEVICE_CONNECT))
//		{
			Romate_control_reception();
//		}
				
	}
}	


static void Interface_Mian_Logic_Task(void* pvParameters)
{
	
	while(1)
	{
		
		if((_variant_Mainlogic_statement.PRESENTstate_value == State_Mainlogic_DWFAULTstate)||(_variant_Mainlogic_statement.PRESENTstate_value == URGENCY_LOCK))
		{
					vTaskDelay(50);
					if(variant_Service_DeviceNum != Structset_Service_DeviceNum1_1.Setting_Para)
					{
						variant_Service_DeviceNum = Structset_Service_DeviceNum1_1.Setting_Para;
						DeviceNum[0] = (variant_Service_DeviceNum/100)+48;
						DeviceNum[1] = ((variant_Service_DeviceNum%100)/10)+48;
						DeviceNum[2] = (variant_Service_DeviceNum%10)+48;
						DeviceNum[3] = '\0';
						Main_logic_STATEchose();
						Display_context_show(default_page,12,0,0);
						variant_interface_Time = xTaskGetTickCount();
					}
//					if(State_main_TrumpetStatus == 2)
//					{
//						State_main_TrumpetStatus = 0;
//					}
					
					
					if((xTaskGetTickCount() - variant_interface_Time) > 5000)
					{
						Main_logic_STATEchose();
						Display_context_show(default_page,12,0,0);
						variant_interface_Time = xTaskGetTickCount();
						
						Count_interface_Refresh++;
						if(Count_interface_Refresh == 200)
						{
							OLED19264_INIT();
							Count_interface_Refresh = 0;
							Display_context_show(default_page,12,0,0);
						
						}
						
					}

		}
		else 
		{
				xQueueReceive(Interface_Mian_Logic_Queue,variant_interface_viewMes,portMAX_DELAY);
				Interface_Mian_Logic_View();
//				xQueueReset(Interface_Mian_Logic_Queue);			
		}
	}
}

static void Fault_Near_Task(void* pvParameters)/* ���߽�������*/
{

	while(1)
	{
		Fault_Near_Function();
	}
}

static void Fault_Pre_Task(void* pvParameters)/* ���߽�������*/
{

	while(1)
	{
		Fault_Pre_Function();
	}
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
