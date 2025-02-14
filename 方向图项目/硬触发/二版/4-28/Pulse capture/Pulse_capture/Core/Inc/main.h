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
extern uint32_t IC_TIMES;  		// 捕获次数，单位1ms
extern uint8_t IC_START_FLAG;  // 捕获开始标志，1：已捕获到高电平；0：还没有捕获到高电平
extern uint8_t IC_DONE_FLAG;  	// 捕获完成标志，1：已完成一次高电平捕获
extern uint16_t IC_VALUE;  		// 输入捕获的捕获值
extern uint32_t Pulse_count_total;	// 输入捕获的总上升沿数量
extern uint32_t Pulse_count;	// 输入捕获的一段上升沿数量
extern uint16_t Pulse_Cycle_Unqualified;  //脉冲周期不合格数
extern uint16_t Starting_angle;//开始角度
extern uint16_t End_angle;//结束角度
extern uint16_t Angle;//旋转角度
extern uint8_t Angular_velocity;//角速度
extern uint8_t Pulse_frame_rate;//脉冲帧数
extern uint8_t Pulse_standard_frame_rate;//脉冲标准帧数
extern uint8_t Unqualified_pulse_frame_rate;//不合格脉冲帧数
extern uint8_t START_FLAG;//开始捕获标志位
extern uint32_t Standard_pulse_count;//标准上升沿数量
extern uint32_t One_cycle_angle;//一周期内旋转角度
/* USER CODE END PTD */
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
