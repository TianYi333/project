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
#include "cmsis_os.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"
#include "usart.h"
#include <stdio.h>
#include "w5500_spi.h"
#include "wizchip_conf.h"
#include "socket.h"
#include "w5500.h"
#include "tjc_usart_hmi.h"
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
#define LED2_Pin GPIO_PIN_1
#define LED2_GPIO_Port GPIOC
#define CH374_INT_Pin GPIO_PIN_2
#define CH374_INT_GPIO_Port GPIOC
#define TIM2_DIR_Pin GPIO_PIN_3
#define TIM2_DIR_GPIO_Port GPIOC
#define TJC_RXD_Pin GPIO_PIN_2
#define TJC_RXD_GPIO_Port GPIOA
#define TJC_TXD_Pin GPIO_PIN_3
#define TJC_TXD_GPIO_Port GPIOA
#define IO_DIR_Pin GPIO_PIN_4
#define IO_DIR_GPIO_Port GPIOA
#define IO_OUT_Pin GPIO_PIN_6
#define IO_OUT_GPIO_Port GPIOA
#define WK__RESET_Pin GPIO_PIN_4
#define WK__RESET_GPIO_Port GPIOC
#define WK__INT_Pin GPIO_PIN_5
#define WK__INT_GPIO_Port GPIOC
#define CH2N_Pin GPIO_PIN_0
#define CH2N_GPIO_Port GPIOB
#define DS1302_RST_Pin GPIO_PIN_1
#define DS1302_RST_GPIO_Port GPIOB
#define DS1302_IO_Pin GPIO_PIN_2
#define DS1302_IO_GPIO_Port GPIOB
#define DS1302_SCLK_Pin GPIO_PIN_10
#define DS1302_SCLK_GPIO_Port GPIOB
#define CH374_SCS_Pin GPIO_PIN_12
#define CH374_SCS_GPIO_Port GPIOB
#define CH374_SCK_Pin GPIO_PIN_13
#define CH374_SCK_GPIO_Port GPIOB
#define CH374_SDI_Pin GPIO_PIN_14
#define CH374_SDI_GPIO_Port GPIOB
#define CH374_SDO_Pin GPIO_PIN_15
#define CH374_SDO_GPIO_Port GPIOB
#define CH2_Pin GPIO_PIN_7
#define CH2_GPIO_Port GPIOC
#define TIM3_DIR_Pin GPIO_PIN_9
#define TIM3_DIR_GPIO_Port GPIOC
#define IN2_Pin GPIO_PIN_8
#define IN2_GPIO_Port GPIOA
#define IN2_EXTI_IRQn EXTI9_5_IRQn
#define IN1_Pin GPIO_PIN_11
#define IN1_GPIO_Port GPIOA
#define IN1_EXTI_IRQn EXTI15_10_IRQn
#define IN0_Pin GPIO_PIN_12
#define IN0_GPIO_Port GPIOA
#define IN0_EXTI_IRQn EXTI15_10_IRQn
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define TDI_Pin GPIO_PIN_15
#define TDI_GPIO_Port GPIOA
#define WK_SPI_SCL_Pin GPIO_PIN_10
#define WK_SPI_SCL_GPIO_Port GPIOC
#define WK_SPI_IO_Pin GPIO_PIN_11
#define WK_SPI_IO_GPIO_Port GPIOC
#define WK_SPI_OI_Pin GPIO_PIN_12
#define WK_SPI_OI_GPIO_Port GPIOC
#define WK_SPI_SCS_Pin GPIO_PIN_2
#define WK_SPI_SCS_GPIO_Port GPIOD
#define TDO_Pin GPIO_PIN_3
#define TDO_GPIO_Port GPIOB
#define NTRST_Pin GPIO_PIN_4
#define NTRST_GPIO_Port GPIOB
#define SP3485_RS_Pin GPIO_PIN_5
#define SP3485_RS_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define MOTOR_SPEED_RERATIO 51	//������ٱ�
#define PULSE_PRE_ROUND 1000	//һȦ���ٸ����壨1000�ߣ�
#define One_Circle_Distance 4	//һȦ���룬��λ����
//#define Track_Length 1000	//���������λ����
#define RELOADVALUE __HAL_TIM_GetAutoreload(&htim2)    //��ȡ��������ʱ���Զ�װ��ֵ,������Ϊ20000-1
#define COUNTERNUM __HAL_TIM_GetCounter(&htim2)        //��ȡ��������ʱ���еļ���ֵ

#define SPEED_RECORD_NUM 20 // �����ԣ�50Hz������ֵ�����˲���Ч���ȽϺ�

#define FRAMELENGTH 9							//������Ļ�����ֽ���

typedef struct _Motor
{
    int32_t lastCount;   //��һ�μ���ֵ
    int32_t totalCount;  //�ܼ���ֵ
    int16_t overflowNum; //�������
    float speed;         //���ת��
    uint8_t direct;      //��ת����
		float distance;    //���о���
}Motor;
extern Motor motor1;
extern float Position;//λ��
extern uint16_t pwmVal_TIM8;
extern uint16_t pwmVal_TIM13;
extern uint8_t Sports_mode;//�˶�ģʽ
extern uint8_t Position_flag;//λ�ñ�־λ
extern uint8_t Encoder_Processing_flag;//���������ݴ����־λ
extern float Track_Length;//���������λ����
extern float speed_Record[SPEED_RECORD_NUM];
float Speed_Low_Filter(float new_Spe,float *speed_Record);
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
