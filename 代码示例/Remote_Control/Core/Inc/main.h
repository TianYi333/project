/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
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

#include "cmsis_os.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"
#include "adc.h"

#include "string.h"
#include "stdio.h"


#include "Bsp_UserTask.h"
#include "Bsp_Analyze.h"
#include "Bsp_Display.h"
#include "Bsp_KeyBoard.h"
#include "Bsp_Action.h"
#include "Bsp_ReceiveInformation.h"
#include "Bsp_Battery.h"
#include "Bsp_UartTransmit.h"

#include "CRC.h"

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
#define SPI1_CS_Pin GPIO_PIN_4
#define SPI1_CS_GPIO_Port GPIOA
#define KEY1_Pin GPIO_PIN_11
#define KEY1_GPIO_Port GPIOA
#define KEY2_Pin GPIO_PIN_12
#define KEY2_GPIO_Port GPIOA
#define KEY3_Pin GPIO_PIN_10
#define KEY3_GPIO_Port GPIOC
#define KEY4_Pin GPIO_PIN_11
#define KEY4_GPIO_Port GPIOC
#define KEY5_Pin GPIO_PIN_12
#define KEY5_GPIO_Port GPIOC
#define KEY6_Pin GPIO_PIN_0
#define KEY6_GPIO_Port GPIOD
#define KEY7_Pin GPIO_PIN_1
#define KEY7_GPIO_Port GPIOD
#define KEY8_Pin GPIO_PIN_2
#define KEY8_GPIO_Port GPIOD
#define KEY9_Pin GPIO_PIN_3
#define KEY9_GPIO_Port GPIOD
#define KEY10_Pin GPIO_PIN_4
#define KEY10_GPIO_Port GPIOD
#define KEY11_Pin GPIO_PIN_5
#define KEY11_GPIO_Port GPIOD
#define KEY12_Pin GPIO_PIN_6
#define KEY12_GPIO_Port GPIOD
#define KEY13_Pin GPIO_PIN_7
#define KEY13_GPIO_Port GPIOD
#define KEY14_Pin GPIO_PIN_9
#define KEY14_GPIO_Port GPIOG
#define KEY15_Pin GPIO_PIN_10
#define KEY15_GPIO_Port GPIOG
#define KEY16_Pin GPIO_PIN_11
#define KEY16_GPIO_Port GPIOG
#define KEY17_Pin GPIO_PIN_12
#define KEY17_GPIO_Port GPIOG
#define KEY18_Pin GPIO_PIN_13
#define KEY18_GPIO_Port GPIOG
#define KEY19_Pin GPIO_PIN_14
#define KEY19_GPIO_Port GPIOG
#define KEY20_Pin GPIO_PIN_15
#define KEY20_GPIO_Port GPIOG
#define KEY21_Pin GPIO_PIN_3
#define KEY21_GPIO_Port GPIOB
#define KEY22_Pin GPIO_PIN_4
#define KEY22_GPIO_Port GPIOB
#define KEY23_Pin GPIO_PIN_5
#define KEY23_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
