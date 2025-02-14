/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
extern UART_HandleTypeDef huart2;
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */
extern uint32_t IC_TIMES;  		// �����������λ1ms
extern uint8_t IC_START_FLAG;  // ����ʼ��־��1���Ѳ��񵽸ߵ�ƽ��0����û�в��񵽸ߵ�ƽ
extern uint8_t IC_DONE_FLAG;  	// ������ɱ�־��1�������һ�θߵ�ƽ����
extern uint16_t IC_VALUE;  		// ���벶��Ĳ���ֵ
extern uint32_t Pulse_count_total;	// ���벶���������������
extern uint32_t Pulse_count;	// ���벶���һ������������
extern uint16_t Pulse_Cycle_Unqualified;  //�������ڲ��ϸ���
extern uint16_t Starting_angle;//��ʼ�Ƕ�
extern uint16_t End_angle;//�����Ƕ�
extern uint16_t Angle;//��ת�Ƕ�
extern uint8_t Angular_velocity;//���ٶ�
extern uint8_t Pulse_frame_rate;//����֡��
extern uint8_t Pulse_standard_frame_rate;//�����׼֡��
extern uint8_t Unqualified_pulse_frame_rate;//���ϸ�����֡��
extern uint8_t START_FLAG;//��ʼ�����־λ
extern uint32_t Standard_pulse_count;//��׼����������
extern uint32_t One_cycle_angle;//һ��������ת�Ƕ�
/* USER CODE END PTD */
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
