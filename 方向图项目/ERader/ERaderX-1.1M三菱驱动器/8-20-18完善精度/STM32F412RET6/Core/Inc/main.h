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
#define IO_OUT_Pin GPIO_PIN_2
#define IO_OUT_GPIO_Port GPIOA
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
#define SP3485_RS_Pin GPIO_PIN_8
#define SP3485_RS_GPIO_Port GPIOA
#define RS485_RXD_Pin GPIO_PIN_9
#define RS485_RXD_GPIO_Port GPIOA
#define RS485_TXD_Pin GPIO_PIN_10
#define RS485_TXD_GPIO_Port GPIOA
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
extern osMessageQueueId_t Ethernet_SendHandle;
extern osMessageQueueId_t Motor_driveQueueHandle;

#define LISTEN_PORT_1 502//���������˿�
#define LISTEN_PORT_2 4500//���������˿�
#define RECEIVE_BUFF_SIZE 128
extern uint8_t receive_buff[RECEIVE_BUFF_SIZE];


//#define MOTOR_SPEED_RERATIO 10	//������ٱ�
//#define MOTOR_SPEED_RERATIO 51	//������ٱ�
//#define PULSE_PRE_ROUND 1000	//һȦ���ٱ����������壨100�ߣ�
#define One_Circle_Distance 2	//һȦ���룬��λ����
#define Subtraction 1000	//ϸ����(��������ٸ�������תһȦ)
//#define Track_Length 1000	//���������λ����
//#define RELOADVALUE __HAL_TIM_GetAutoreload(&htim2)    //��ȡ��������ʱ���Զ�װ��ֵ,������Ϊ20000
//#define COUNTERNUM __HAL_TIM_GetCounter(&htim2)        //��ȡ��������ʱ���еļ���ֵ

#define SPEED_RECORD_NUM 20 // �����ԣ�50Hz������ֵ�����˲���Ч���ȽϺ�

#define FRAMELENGTH 9							//������Ļ�����ֽ���



//typedef struct _Motor
//{
//    int32_t lastCount;   //��һ�μ���ֵ
//    int32_t totalCount;  //�ܼ���ֵ
//    int16_t overflowNum; //�������
//    float speed;         //���ת��
//    uint8_t direct;      //��ת����
//		float distance;    //���о���
//}Motor;


//extern Motor motor1;
extern float Position;//��0��λ��,��λmm
extern uint32_t Position_upload;//��0������ϴ�ֵ����λ0.001mm��
extern uint32_t zero_return_speed_value;//�����ٶȣ���λ0.001mm/s��
extern uint16_t pwmVal_TIM8;
extern uint8_t Sports_mode;//�˶�ģʽ
extern uint8_t Position_flag;//λ�ñ�־λ
extern uint8_t Sports_status_flag;//�˶�״̬��־λ
extern uint8_t Extreme_position_flag;//����λ�ñ�־λ
extern uint8_t Position_upload_flag;//λ���ϴ���־λ
extern uint8_t Encoder_Processing_flag;//���������ݴ����־λ
extern uint8_t Zero_regression_flag;//�Ƿ�������־λ
extern uint8_t Absolute_position_flag;//����λ�ñ�־λ
extern uint8_t Servo_reset_flag;//�ŷ���λ��־λ
extern float Track_Length;//���������λ����
extern uint32_t Target_Position;//Ŀ��λ��
extern uint32_t Zero_Position;//���λ��
extern uint16_t Hard_Trigger_Number;//Ӳ��������
extern uint16_t Hard_Trigger_Current_Number;//Ӳ������ǰ����
extern uint32_t Hard_Trigger_Interval;//Ӳ����ʱ����
extern uint32_t Hard_Trigger_Position;//Ӳ������ʼλ��
extern uint8_t Hard_Trigger_start_flag;//Ӳ������ͣ��־λ	
extern uint8_t Hard_Trigger_flag;//Ӳ������־λ
extern float speed_Record[SPEED_RECORD_NUM];
extern int32_t Motor_pulsec_ount;//������������
extern uint32_t Trigger_Interval;//�������
extern float Position_lastpulsec;//�ϴ�Ӳ����λ��
extern uint8_t Iwdg_Number;//���Ź�����

void *memset(void *s,int c,size_t n);//�����������������������
int strcmp(const char *str1,const char *str2 );//�����������������������
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
