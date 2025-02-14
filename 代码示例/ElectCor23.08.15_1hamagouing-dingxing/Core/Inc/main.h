/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "FreeRTOS.h"
#include "queue.h"

#include "bsp_fault.h"

#include "task.h"
#include "time.h"
#include "event_groups.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
//�������궨�����

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

#define TIME                                  xTaskGetTickCount()



extern uint8_t Flag_main_STOP;
extern uint16_t ADC_Sensor_ConvertedValue[2];
extern uint8_t Device_Status;
extern uint8_t GROUP_M[8];
extern uint8_t GROUP_PUSE;
extern QueueHandle_t CAN_Bus_Receive_Queue;//���߽�����Ϣ����
extern QueueHandle_t Trumpet_Start_Queue;/* ����֪ͨ������ */
extern UART_HandleTypeDef huart6;
extern uint8_t GLOUB_REMOTE_R[10];
extern uint8_t GLOUB_REMOTE_T[20];
extern QueueHandle_t  MOVE_Control_Queue;
extern uint16_t GLOUB_ANGLE_NUM;

extern ADC_HandleTypeDef hadc1;

extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;
extern CAN_HandleTypeDef hcan3;



extern SPI_HandleTypeDef hspi2;

extern TIM_HandleTypeDef htim1;

extern UART_HandleTypeDef huart7;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart6;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern DMA_HandleTypeDef hdma_usart2_tx;
extern DMA_HandleTypeDef hdma_uart7_rx;
extern DMA_HandleTypeDef hdma_uart7_tx;
extern DMA_HandleTypeDef hdma_usart6_tx;
extern DMA_HandleTypeDef hdma_usart6_rx;

extern QueueHandle_t Unite_Action_Queue;
extern QueueHandle_t Key_Value_ISR_Queue;/* �����ж��з��ذ���ֵ */
extern QueueHandle_t Key_Monitor_Start_Queue;/* ����֪ͨ�����������ʼ��ⰴ���Ƿ��ɿ� */
extern QueueHandle_t Key_Status_Queue;/* ���ڰ�����������з��ذ����Ѿ��ɿ���һ״̬ */
extern QueueHandle_t Trumpet_Start_Queue;/* ����֪ͨ������ */
extern QueueHandle_t CAN_Send_Queue;/* ����֪ͨCAN��ʼ���� */

extern QueueHandle_t CAN_Receive_ISR_Queue;


extern QueueHandle_t CAN2_Receive_ISR_Queue;

extern QueueHandle_t LED_Control_Queue;/* ����֪ͨLED�ı� */

extern QueueHandle_t Para_Trans_Queue;/* ������������ɱ�־λ */
extern QueueHandle_t Abrupt_Stop_Queue;/* ��ͣ���� */
extern QueueHandle_t Interface_Mian_Logic_Queue;
extern QueueHandle_t Follow_Automation_Queue;
typedef struct Fault_Struct//���Ͻṹ��
{
	uint8_t fault_device_count;
	uint8_t fault_device_num[20];
}Fault_Struct_Type;



extern uint8_t State_main_TrumpetStatus;
extern uint8_t Flag_main_STOPcontitue;

extern uint8_t array_main_STOP[10];//��¼��ͣ�ܺ�
extern uint8_t variant_main_STOPnum;
extern char variant_main_STOPveiw1[4];//ת���ַ���
extern char variant_main_STOPveiw2[4];//ת���ַ���
extern char variant_main_STOPveiw3[4];//ת���ַ���
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void Main_Delay(uint32_t time);
uint16_t  ADC1_Wave_Filtering(void);
void MX_CAN1_Init(void);
void MX_CAN2_Init(void);
void MX_CAN3_Init(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */
extern IWDG_HandleTypeDef hiwdg;
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
