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
#include "iwdg.h"
#include "CRC.h"
#include "string.h"
#include "stdio.h"
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
#define IN2_Pin GPIO_PIN_0
#define IN2_GPIO_Port GPIOC
#define IN2_EXTI_IRQn EXTI0_IRQn
#define LED1_Pin GPIO_PIN_1
#define LED1_GPIO_Port GPIOC
#define IN1_Pin GPIO_PIN_2
#define IN1_GPIO_Port GPIOC
#define IN1_EXTI_IRQn EXTI2_IRQn
#define IN0_Pin GPIO_PIN_3
#define IN0_GPIO_Port GPIOC
#define IN0_EXTI_IRQn EXTI3_IRQn
#define TJC_RXD_Pin GPIO_PIN_2
#define TJC_RXD_GPIO_Port GPIOA
#define IN3_Pin GPIO_PIN_4
#define IN3_GPIO_Port GPIOA
#define IN4_Pin GPIO_PIN_5
#define IN4_GPIO_Port GPIOA
#define IN5_Pin GPIO_PIN_6
#define IN5_GPIO_Port GPIOA
#define TIM8_CH1N_Pin GPIO_PIN_7
#define TIM8_CH1N_GPIO_Port GPIOA
#define WK__RESET_Pin GPIO_PIN_4
#define WK__RESET_GPIO_Port GPIOC
#define WK__INT_Pin GPIO_PIN_5
#define WK__INT_GPIO_Port GPIOC
#define TIM8_CH2N_Pin GPIO_PIN_0
#define TIM8_CH2N_GPIO_Port GPIOB
#define DS1302_RST_Pin GPIO_PIN_1
#define DS1302_RST_GPIO_Port GPIOB
#define DS1302_IO_Pin GPIO_PIN_2
#define DS1302_IO_GPIO_Port GPIOB
#define DS1302_SCLK_Pin GPIO_PIN_10
#define DS1302_SCLK_GPIO_Port GPIOB
#define WK_SPI_SCS_Pin GPIO_PIN_12
#define WK_SPI_SCS_GPIO_Port GPIOB
#define WK_SPI_SCL_Pin GPIO_PIN_13
#define WK_SPI_SCL_GPIO_Port GPIOB
#define WK_SPI_IO_Pin GPIO_PIN_14
#define WK_SPI_IO_GPIO_Port GPIOB
#define WK_SPI_OI_Pin GPIO_PIN_15
#define WK_SPI_OI_GPIO_Port GPIOB
#define TIM8_CH2_Pin GPIO_PIN_7
#define TIM8_CH2_GPIO_Port GPIOC
#define TIM8_CH3_Pin GPIO_PIN_8
#define TIM8_CH3_GPIO_Port GPIOC
#define TIM8_CH4_Pin GPIO_PIN_9
#define TIM8_CH4_GPIO_Port GPIOC
#define SP3485_RS_Pin GPIO_PIN_8
#define SP3485_RS_GPIO_Port GPIOA
#define RS485_RXD_Pin GPIO_PIN_9
#define RS485_RXD_GPIO_Port GPIOA
#define RS485_TXD_Pin GPIO_PIN_10
#define RS485_TXD_GPIO_Port GPIOA
#define PULSE_Pin GPIO_PIN_11
#define PULSE_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define TDI_Pin GPIO_PIN_15
#define TDI_GPIO_Port GPIOA
#define USB_RXD_Pin GPIO_PIN_10
#define USB_RXD_GPIO_Port GPIOC
#define USB_TXD_Pin GPIO_PIN_11
#define USB_TXD_GPIO_Port GPIOC
#define TDO_Pin GPIO_PIN_3
#define TDO_GPIO_Port GPIOB
#define NTRST_Pin GPIO_PIN_4
#define NTRST_GPIO_Port GPIOB
#define AT24_SCL_Pin GPIO_PIN_6
#define AT24_SCL_GPIO_Port GPIOB
#define AT24_SDA_Pin GPIO_PIN_7
#define AT24_SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */
extern osMessageQueueId_t Ethernet_ReceiveHandle;
extern osMessageQueueId_t Motor_driveQueueHandle;
extern osMessageQueueId_t RS485_ReceiveQueueHandle;
extern osMessageQueueId_t RS485_TransmitQueueHandle;


#define MOTION HAL_GPIO_ReadPin(IN0_GPIO_Port,IN0_Pin)==GPIO_PIN_RESET//运动
#define CEASE  HAL_GPIO_ReadPin(IN0_GPIO_Port,IN0_Pin)==GPIO_PIN_SET//停止

//#define REACH 		HAL_GPIO_ReadPin(IN0_GPIO_Port,IN0_Pin)==GPIO_PIN_SET//到达
//#define UNREACH  	HAL_GPIO_ReadPin(IN0_GPIO_Port,IN0_Pin)==GPIO_PIN_RESET//未到达


#define COROTATION HAL_GPIO_ReadPin(TIM8_CH2_GPIO_Port,TIM8_CH2_Pin)==GPIO_PIN_RESET//正转
#define REVERSAL   HAL_GPIO_ReadPin(TIM8_CH2_GPIO_Port,TIM8_CH2_Pin)==GPIO_PIN_SET//反转

#define DO_RADAR_ON  HAL_GPIO_WritePin(TIM8_CH3_GPIO_Port,TIM8_CH3_Pin,GPIO_PIN_SET)//PC8-使雷达上电
#define DO_RADAR_OFF HAL_GPIO_WritePin(TIM8_CH3_GPIO_Port,TIM8_CH3_Pin,GPIO_PIN_RESET)//PC8-使雷达断电

#define RADAR_ON  HAL_GPIO_ReadPin(TIM8_CH3_GPIO_Port,TIM8_CH3_Pin)==GPIO_PIN_SET//雷达上电
#define RADAR_OFF HAL_GPIO_ReadPin(TIM8_CH3_GPIO_Port,TIM8_CH3_Pin)==GPIO_PIN_RESET//雷达断电

#define RS485_TX	HAL_GPIO_ReadPin(SP3485_RS_GPIO_Port,SP3485_RS_Pin)==GPIO_PIN_SET//PA8-发送
#define RS485_RX	HAL_GPIO_ReadPin(SP3485_RS_GPIO_Port,SP3485_RS_Pin)==GPIO_PIN_RESET//PA8-接收

#define DO_RS485_TX	HAL_GPIO_WritePin(SP3485_RS_GPIO_Port,SP3485_RS_Pin,GPIO_PIN_SET)//PA7-使发送
#define DO_RS485_RX	HAL_GPIO_WritePin(SP3485_RS_GPIO_Port,SP3485_RS_Pin,GPIO_PIN_RESET)//PA7-使接收



#define LISTEN_PORT_1 502//本地主机端口
#define LISTEN_PORT_2 4500//本地主机端口
#define RECEIVE_BUFF_SIZE 128
extern uint8_t receive_buff[RECEIVE_BUFF_SIZE];


#define One_Circle_Distance 5	//一圈距离，单位毫米
#define Subtraction 1000	//细分数(或输出多少个脉冲旋转一圈)

#define BUF_SIZE 20
extern uint8_t array_RS485_buffer[BUF_SIZE]; // 创建接收缓存,大小为BUF_SIZE

/***********************************************variable***********************************/
extern float variable_Position;//距0点位置,单位mm
extern int32_t variable_Position_upload;//距0点距离上传值（单位0.001mm）
extern uint32_t variable_Zero_return_speed_value;//回零速度（单位0.001mm/s）
extern uint32_t variable_Return_to_zero_parameter;//回零参数（0~4:二次回原点速度；5:回原点时停止方式；6~14:回原点速度；15:回原点方向）
extern uint32_t variable_Running_speed_value;//运行速度
extern uint32_t variable_Target_position;//目标位置
extern uint32_t variable_Zero_position;//零点位置
extern uint32_t variable_Hard_trigger_interval;//硬触发时间间隔
extern uint32_t variable_Hard_trigger_position;//硬触发开始位置
extern uint32_t variable_Trigger_interval;//触发间隔
extern float variable_Position_lastpulsec;//上次硬触发位置

/***********************************************flag***************************************/
extern uint8_t flag_First_power_on;//首次上电标志位
extern uint8_t flag_Motion_state;//运动状态标志位
extern uint8_t flag_Position_upload;//位置上传标志位
extern uint8_t flag_Real_time_location_query;//实时位置主动查询标志位
extern uint8_t flag_Encoder_processing;//编码器数据处理标志位
extern uint8_t flag_Zero_regression;//是否回零过标志位
extern uint8_t flag_Absolute_position;//绝对位置标志位
extern uint8_t flag_Hard_trigger_start;//硬触发启停标志位	
extern uint8_t flag_Hard_trigger;//硬触发标志位
extern uint8_t flag_Motor_deceleration_stops;//点击减速停止标志位

/***********************************************number*************************************/
extern uint16_t number_Hard_trigger;//硬触发个数
extern uint16_t number_Hard_trigger_curren;//硬触发当前个数
extern uint8_t number_Iwdg;//看门狗计数
extern int32_t number_Motor_pulsec;//输入电机脉冲数

/***********************************************function*************************************/
void *memset(void *s,int c,size_t n);//声明函数，清除编译器警告
int strcmp(const char *str1,const char *str2 );//声明函数，清除编译器警告
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
